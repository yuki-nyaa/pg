#define YUKI_PG_DBG
#include"debug.hpp"
#include<fmt/core.h>
#include"lr1_gen.hpp"
namespace yuki::pg::lr1{
template<std::unsigned_integral Token_Kind_t>
size_t lr1_table_generate(
    const Token_Info& token_info,
    const Rule_Set<Token_Kind_t>& rules,
    const Prec_Table& prec_table, const Assoc_Table& assoc_table,
    FILE* fp_file,FILE* fp_err,FILE* fp_log)
{
    assert(fp_file); assert(fp_err); // Log-out is optional.
    First_Table<Token_Kind_t> first_table(token_info,rules);
    std::vector<LR1_Item_Set<Token_Kind_t>> cc;
    cc.reserve(100000); // MNumber
    size_t cc_next=1;
    cc.push_back(generate_initial_items(rules,first_table));

    LR1_Item_Set<Token_Kind_t> cc_candidate{};

    Action_Candidates* action_row = new Action_Candidates[token_info.terminal_total()];

    size_t sr_conflict_total = 0;
    size_t rr_conflict_total = 0;

    for(size_t cc_current=0; cc_current<cc_next && !(cc[cc_current].processed); ++cc_current){
        cc[cc_current].processed=true;
        typename LR1_Item_Set<Token_Kind_t>::const_iterator it = cc[cc_current].begin();
        Token_Kind_t cursored_current=it.cursored();

        while(cursored_current!=Token_Kind_t(-1)){ // Handles "shift" and "goto".
            assert(cc_candidate.empty());
            for(;!it.is_end() && it.cursored()==cursored_current; ++it){
                cc_candidate.insert({it.left(),it.rights(),it.lookahead(),it.cursor()+1});
            }
            closure(cc_candidate,rules,first_table);

            size_t cc_candidate_number=0;
            for(;cc_candidate_number!=cc_next;++cc_candidate_number){
                if(cc[cc_candidate_number]==cc_candidate){
                    cc_candidate.clear();
                    goto cc_candidate_already_exists;
                }
            }
            cc.emplace_back(std::move(cc_candidate));
            ++cc_next;
            cc_candidate_already_exists:
            if(is_terminal(token_info,cursored_current)){
                action_row[terminal_kind_to_index(token_info,cursored_current)].shift=cc_candidate_number;
                // The write of "action(cursored_current(,cc_current)) = shift cc_candidate_number" is postponed until all items in the current item-set are considered.
            }else if(is_nterminal(token_info,cursored_current)){
                fmt::print(fp_file,"goto({},{}) = {}\n",cc_current,cursored_current,cc_candidate_number);
                // Since "goto" cannot have conflicts, we can immediately write "goto(cc_current,symbol)=cc_candidate_number".
            }
            YUKI_PG_DBGO("cgoto({},{}) = {}\n",cc_current,cursored_current,cc_candidate_number);

            if(it.is_end()){
                for(Token_Kind_t i=token_info.terminal_first;i<=token_info.terminal_last;++i){
                    if(action_row[terminal_kind_to_index(token_info,i)].shift!=0)
                    fmt::print(fp_file,"action({},{}) = shift {}\n",cc_current,i,(action_row[terminal_kind_to_index(token_info,i)].shift));
                }
                std::memset(action_row,0, token_info.terminal_total() * sizeof(Action_Candidates) );
                goto loop_1_end;
            }else{
                cursored_current=it.cursored();
            }
        }

        for(;!it.is_end();++it){ // Handles "reduce".
            auto rule_it = rules.first_equal({it.left(),it.rights(),0});
            assert(rule_it!=rules.end());
            if(Action_Candidates& action_this = action_row[terminal_kind_to_index(token_info,it.lookahead())] ; action_this.reduce.rule_num!=0){
                yuki::try_print(fp_log,"RR-conflict encountered. action({},{}) = reduce {}({}) / {}({}). ",cc_current,token_info.name_table[it.lookahead()],action_this.reduce.rule_num,action_this.reduce.prec_rr,rule_it->num,rule_it->prec_rr);
                resolve_rr_conflict(action_this,rule_it->num,rule_it->prec_sr,rule_it->prec_rr);
                ++rr_conflict_total;
                yuki::try_print(fp_log,"Resolved to rule {}.",action_this.reduce.rule_num);
            }else{
                action_this.reduce={rule_it->num,rule_it->prec_sr,rule_it->prec_rr};
            }
        }

        // SR-conflict resolution
        for(Token_Kind_t i=token_info.terminal_first;i<=token_info.terminal_last;++i){
            switch(Action_Candidates& action_this=action_row[terminal_kind_to_index(token_info,i)] ; action_this.state()){
                case Action_Candidates::State::EMPTY : {
                    continue;
                }
                case Action_Candidates::State::S : {
                    fmt::print(fp_file,"action({},{}) = shift {}\n",cc_current,i,action_this.shift);
                    std::memset(action_row+terminal_kind_to_index(token_info,i),0,sizeof(Action_Candidates));
                    break;
                }
                case Action_Candidates::State::R : {
                    fmt::print(fp_file,"action({},{}) = reduce {}\n",cc_current,i,action_this.reduce.rule_num);
                    std::memset(action_row+terminal_kind_to_index(token_info,i),0,sizeof(Action_Candidates));
                    break;
                }
                case Action_Candidates::State::SR : {
                    yuki::try_print(fp_log,"SR-conflict encountered. action({},{}) = shift {} / reduce {}({}). Prec(lookahead)={}. Assoc(lookahead)={}. ",cc_current,token_info.name_table[i],action_this.shift,action_this.reduce.rule_num,action_this.reduce.prec_sr,prec_table[i],assoc_table[i]);
                    switch(resolve_sr_conflict(prec_table,assoc_table,action_this,i)){
                        case Action_Kind::SHIFT : {
                            yuki::try_print(fp_log,"Resolved to shift {}.\n",action_this.shift);
                            fmt::print(fp_file,"action({},{}) = shift {}\n",cc_current,i,action_this.shift);
                            break;
                        }
                        case Action_Kind::REDUCE : {
                            yuki::try_print(fp_log,"Resolved to reduce {}.\n",action_this.reduce.rule_num);
                            fmt::print(fp_file,"action({},{}) = reduce {}\n",cc_current,i,action_this.reduce.rule_num);
                            break;
                        }
                    }
                    ++sr_conflict_total;
                    std::memset(action_row+terminal_kind_to_index(token_info,i),0,sizeof(Action_Candidates));
                    break;
                }
            }
        }
        loop_1_end:
        #ifdef YUKI_PG_DBG
        for(size_t i=token_info.terminal_first;i<=token_info.terminal_last;++i){
            if(!action_row[terminal_kind_to_index(token_info,i)].empty())
                dbgout("non-empty action_candidates at ({},{}).\n",cc_current,i);
        }
        std::memset(action_row,0, token_info.terminal_total() * sizeof(Action_Candidates) );
        #endif
    }

    // Write the accept action.
    // The accept item-set might contain reduce-items with lookahead EOF_. Those will be overwritten, WITHOUT diagnostics. Such conflicts should be rare, however. The production-set that entails such conflicts should obviously wrong, like "A -> A".
    assert(token_info.nterminal_last<token_info.terminal_first); // The accept state has number 1 only if non-terminals are placed BEFORE terminals.
    fmt::print(fp_file,"action({},{}) = reduce {}\n",1,token_info.EOF_,0);

    if(sr_conflict_total || rr_conflict_total)
        fmt::print(fp_err,"{} SR-conflict(s), {} RR-conflict(s) in total. See log for details.\n",sr_conflict_total,rr_conflict_total);

    delete[] action_row;

    return cc.size();
}
}

#include"cmd.hpp"
#include"meta_lexer0.h"
#include"meta_lexer1.h"
int main(int argc,char** argv){
    namespace ypg=yuki::pg;
    ypg::Cmd_Data cmd_data;
    yuki::Cmd_Lexer<ypg::Cmd_Data> cmd_lexer(std::move(ypg::opt_table));
    cmd_lexer.process(cmd_data,argc,argv);
    if(!cmd_data.post_process())
        std::exit(EXIT_FAILURE);

    ypg::Meta_Lexer0 meta_lexer0(cmd_data);
    meta_lexer0.lex();

    cmd_data.fp_in = freopen(cmd_data.in.c_str(),"r",cmd_data.fp_in);

    switch(yuki::uint_auto_f(meta_lexer0.nterm_table.size()+meta_lexer0.term_table.size())){
        case yuki::uint_enum::UCHAR : {
            ypg::Meta_Lexer1<unsigned char> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            printf("%zu",meta_lexer1.rs.size());
            break;
        }
        case yuki::uint_enum::USHORT : {
            ypg::Meta_Lexer1<unsigned short> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            printf("%zu",meta_lexer1.rs.size());
            break;
        }
        case yuki::uint_enum::UINT : {
            ypg::Meta_Lexer1<unsigned> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            printf("%zu",meta_lexer1.rs.size());
            break;
        }
        case yuki::uint_enum::ULONG : {
            ypg::Meta_Lexer1<unsigned long> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            printf("%zu",meta_lexer1.rs.size());
            break;
        }
        case yuki::uint_enum::ULLONG : {
            ypg::Meta_Lexer1<unsigned long long> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            printf("%zu",meta_lexer1.rs.size());
            break;
        }
    }
    // Tokens: {(=0,),EOF_  List=3, Pair, epsilon_=5}
    /* Rules:
        Goal:
            List ;
        list:
            List Pair
            Pair ;
        Pair:
            ( Pair )
            ( )
    */
    //yuki::pg::Token_Name_Table name_table={"List","Pair","(",")","EOF"};
    //yuki::pg::Token_Info t_info = {0,1,2,4,std::move(name_table)};
    //yuki::pg::Rule_Set<unsigned short> rs;
    //rs.insert_equal({0,{0,1},1});
    //rs.insert_equal({0,{1},2});

    //rs.insert_equal({1,{2,1,3},3});
    //rs.insert_equal({1,{2,3},4});

    //yuki::pg::First_Table<unsigned short> ft(t_info,rs);

    //yuki::pg::lr1::Prec_Table prec_table(t_info);
    //yuki::pg::lr1::Assoc_Table assoc_table{};

    //yuki::pg::lr1::lr1_table_generate(t_info,rs,prec_table,assoc_table,ypg::cmd_data.fp_out_cpp,stderr,ypg::cmd_data.fp_out_log);
}