#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/ring_queue.hpp>
#include"cmd.hpp"
#include"lr1_writer.h"
#include"debug.hpp"

#define IND YUKI_PG_IND
#define IND2 IND IND
#define IND3 IND IND IND
#define IND4 IND IND IND IND
#define IND5 IND IND IND IND IND
#define IND6 IND IND IND IND IND IND

#ifndef YUKI_PG_TABLE_MAX_LINE_ITEM
#define YUKI_PG_TABLE_MAX_LINE_ITEM 8
#endif

template<>
struct fmt::formatter<yuki::pg::Assoc,char> : yuki::simple_formatter<yuki::pg::Assoc,char> {
    template<typename OutputIt>
    auto format(yuki::pg::Assoc assoc,fmt::basic_format_context<OutputIt,char>& ctx) -> typename fmt::basic_format_context<OutputIt,char>::iterator {
        switch(assoc){
            case yuki::pg::Assoc::LEFT : {return fmt::format_to(ctx.out(),"{}","LEFT");}
            default : {return fmt::format_to(ctx.out(),"{}","RIGHT");}
        }
    }
};

namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t>
size_t LR1_Writer<Token_Kind_t>::write_table(
    const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules,
    const Assoc assoc0,
    FILE* fp_file,FILE* fp_goto,FILE* fp_err,FILE* fp_log)
{
    assert(fp_file); assert(fp_err); // Log-out is optional.

    First_Table<Token_Kind_t> first_table(nterms,terms,rules);
    LR1_Item_Set_Set cc;
    cc.insert(LR1_Item_Set(rules,first_table));

    #ifndef YUKI_PG_CC_WORKLIST_RESERVE
    #define YUKI_PG_CC_WORKLIST_RESERVE 65535
    #endif
    yuki::RingQueue<typename LR1_Item_Set_Set::const_iterator> worklist(yuki::reserve_tag,YUKI_PG_CC_WORKLIST_RESERVE);
    worklist.emplace_back(cc.begin());

    LR1_Item_Set items_pending;

    const size_t term_total = terms.size();
    const size_t nterm_total = nterms.size();
    const size_t token_total = term_total + nterm_total;

    Action_Candidates* action_row = new Action_Candidates[term_total];
    std::memset(action_row,0, term_total * sizeof(Action_Candidates) );

    size_t sr_conflict_total = 0;
    size_t rr_conflict_total = 0;

    size_t action_count = 0;
    size_t goto_count = 0;
    auto print_action_ind = [fp_file,&action_count](){
        if(action_count % YUKI_PG_TABLE_MAX_LINE_ITEM == 0)
            fprintf(fp_file,IND);
    };
    auto reset_action_ind = [fp_file,&action_count](){
        if(action_count % YUKI_PG_TABLE_MAX_LINE_ITEM == 0)
            fprintf(fp_file,"\n");
    };
    auto print_goto_ind = [fp_goto,&goto_count](){
        if(goto_count % YUKI_PG_TABLE_MAX_LINE_ITEM == 0)
            fprintf(fp_goto,IND);
    };
    auto reset_goto_ind = [fp_goto,&goto_count](){
        if(goto_count % YUKI_PG_TABLE_MAX_LINE_ITEM == 0)
            fprintf(fp_goto,"\n");
    };

    while(!worklist.empty()){
        action_count = 0;
        goto_count = 0;
        const LR1_Item_Set& items = worklist.front()->key;
        const size_t items_num = worklist.front()->mapped;

        typename LR1_Item_Set::const_iterator it = items.begin();
        Token_Kind_t cursored_current=it.cursored();

        while(cursored_current!=Token_Kind_t(-1)){ // Handles "shift" and "goto".
            assert(items_pending.empty());
            for(;!it.is_end() && it.cursored()==cursored_current; ++it)
                items_pending.emplace(it.left(),it.rights(),it.lookahead(),it.cursor()+1);
            items_pending.closure(rules,first_table);

            yuki::IB_Pair<typename LR1_Item_Set_Set::const_iterator> insert_result= cc.insert(std::move(items_pending));
            if(insert_result.has_inserted)
                worklist.emplace_back(insert_result.iterator);
            size_t items_pending_number = insert_result.iterator->mapped;

            if(cursored_current>=nterm_total && cursored_current<token_total){ // `cursored_current` is terminal.
                action_row[cursored_current-nterm_total].shift=items_pending_number;
                // The write of "action(cursored_current(,items_num)) = shift items_pending_number" is postponed until all items in the items_num item-set are considered.
            }else if(cursored_current<nterm_total){ // `cursored_current` is nterminal.
                print_goto_ind();
                fmt::print(fp_goto,"{{{},{},{}}},",items_num,cursored_current,items_pending_number);
                ++goto_count;
                reset_goto_ind();
                // Since "goto" cannot have conflicts, we can immediately write "goto(items_num,symbol)=items_pending_number".
            }
            yuki::try_print(fp_log,"GOTO({},{}) = {}\n",items_num,cursored_current,items_pending_number);

            if(it.is_end()){
                for(Token_Kind_t i=nterm_total;i<token_total;++i){
                    if(action_row[i-nterm_total].shift!=0){
                        print_action_ind();
                        fmt::print(fp_file,"{{{},{},{{1,{}}}}},",items_num,i,action_row[i-nterm_total].shift);
                        ++action_count;
                        reset_action_ind();
                    }
                }
                std::memset(action_row,0, term_total * sizeof(Action_Candidates) );
                goto loop_1_end;
            }else{
                cursored_current=it.cursored();
            }
        }

        for(;!it.is_end();++it){ // Handles "reduce".
            auto rule_it = rules.find({it.left(),it.rights(),0});
            assert(rule_it!=rules.end());
            if(Action_Candidates& action_this = action_row[it.lookahead()-nterm_total] ; action_this.reduce.rule_num!=0){
                yuki::try_print(fp_log,"RR-conflict encountered. action({},{}) = reduce {}({}) / {}({}). ",
                    items_num,
                    terms[it.lookahead()-nterm_total].name,
                    action_this.reduce.rule_num,
                    action_this.reduce.prec_rr,
                    rule_it->num,
                    rule_it->prec_rr
                );
                resolve_rr_conflict(action_this,rule_it->num,rule_it->prec_sr,rule_it->prec_rr);
                ++rr_conflict_total;
                yuki::try_print(fp_log,"Resolved to rule {}.",action_this.reduce.rule_num);
            }else{
                action_this.reduce={rule_it->num,rule_it->prec_sr,rule_it->prec_rr};
            }
        }

        // SR-conflict resolution
        for(Token_Kind_t i=nterm_total;i<token_total;++i){
            switch(Action_Candidates& action_this=action_row[i-nterm_total] ; action_this.state()){
                case Action_Candidates::State::EMPTY : {
                    continue;
                }
                case Action_Candidates::State::S : {
                    print_action_ind();
                    fmt::print(fp_file,"{{{},{},{{1,{}}}}},",items_num,i,action_this.shift);
                    ++action_count;
                    reset_action_ind();
                    std::memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
                case Action_Candidates::State::R : {
                    print_action_ind();
                    fmt::print(fp_file,"{{{},{},{{2,{}}}}},",items_num,i,action_this.reduce.rule_num);
                    ++action_count;
                    reset_action_ind();
                    std::memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
                case Action_Candidates::State::SR : {
                    yuki::try_print(fp_log,"SR-conflict encountered. action({},{}) = shift {} / reduce {}({}). Prec(lookahead)={}. Assoc(lookahead)={}. ",
                        items_num,terms[i-nterm_total].name, action_this.shift,
                        action_this.reduce.rule_num, action_this.reduce.prec_sr,
                        terms[i-nterm_total].prec, terms[i-nterm_total].prec==0 ? assoc0 : terms[i-nterm_total].assoc
                    );
                    switch(resolve_sr_conflict(nterms,terms,assoc0,action_this,i)){
                        case Action_Kind::SHIFT : {
                            yuki::try_print(fp_log,"Resolved to shift {}.\n",action_this.shift);
                            print_action_ind();
                            fmt::print(fp_file,"{{{},{},{{1,{}}}}},",items_num,i,action_this.shift);
                            ++action_count;
                            reset_action_ind();
                            break;
                        }
                        case Action_Kind::REDUCE : {
                            yuki::try_print(fp_log,"Resolved to reduce {}.\n",action_this.reduce.rule_num);
                            print_action_ind();
                            fmt::print(fp_file,"{{{},{},{{2,{}}}}},",items_num,i,action_this.reduce.rule_num);
                            ++action_count;
                            reset_action_ind();
                            break;
                        }
                    }
                    ++sr_conflict_total;
                    std::memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
            }
        }
        loop_1_end:
        worklist.pop_front();
        if(action_count % YUKI_PG_TABLE_MAX_LINE_ITEM != 0)
            fprintf(fp_file,"\n");
        if(goto_count % YUKI_PG_TABLE_MAX_LINE_ITEM != 0)
            fprintf(fp_goto,"\n");
        #ifdef YUKI_PG_DBG
        for(Token_Kind_t i=nterm_total;i<token_total;++i){
            if(!action_row[i-nterm_total].empty())
                YUKI_PG_DBGO("non-empty action_candidates at ({},{}).\n",items_num,i);
        }
        std::memset(action_row,0, term_total * sizeof(Action_Candidates) );
        #endif
    } // while(!worklist.empty())

    // Write the accept action.
    // The accept item-set might contain reduce-items with lookahead EOF_. Those will be overwritten, WITHOUT diagnostics. Such conflicts should be rare, however. The production-set that entails such conflicts should obviously wrong, like "A -> A".
    fmt::print(fp_file,IND "{{1,{},{{2,0}}}},\n",token_total-1);

    if(sr_conflict_total || rr_conflict_total)
        fmt::print(fp_err,"{} SR-conflict(s), {} RR-conflict(s) in total. See log for details.\n",sr_conflict_total,rr_conflict_total);

    delete[] action_row;

    return cc.size();
} // size_t LR1_Writer<Token_Kind_t>::write_table

template<std::unsigned_integral Token_Kind_t>
void LR1_Writer<Token_Kind_t>::write(
    const Cmd_Data& cmd_data,
    const std::unordered_map<std::string,std::string> code_htable,
    const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules,
    const Assoc assoc0)
{
    auto get_token_data = [&nterms,&terms](Token_Kind_t k) -> const Token_Data& {
        assert(k!=(Token_Kind_t)-1);
        return k>=nterms.size() ? terms[k-nterms.size()] : nterms[k];
    };
    auto print_rule = [get_token_data](FILE* fp,const Rule<Token_Kind_t>& r) -> void {
        fprintf(fp,"%s ->",
            r.left!=(Token_Kind_t)-1 ? get_token_data(r.left).name_or_alias().c_str() : "Goal_"
        );
        for(const Token_Kind_t k : r.rights)
            fprintf(fp," %s",get_token_data(k).name_or_alias().c_str());
    };

    FILE* const out = cmd_data.fp_out_cpp;
    const bool sp_token = !cmd_data.sp_token.empty();

    try{
        fprintf(out,"%s\n",code_htable.at("cpp_top").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out,
        "#include<yuki/print.hpp>\n"
        "#include<yuki/pg/lr1.hpp>\n"
        "#include\"%s\"\n",
        cmd_data.out_h.c_str()
    );
    if(!cmd_data.nspace.empty())
        fprintf(out,"namespace %s{\n",cmd_data.nspace.c_str());
    fprintf(out,"constinit %s::Action_Table %s::action_table = {\n",cmd_data.parser.c_str(),cmd_data.parser.c_str());
    const size_t state_size = write_table(nterms,terms,rules,assoc0,cmd_data.fp_out_cpp,cmd_data.fp_goto,stderr,cmd_data.fp_out_log);
    fprintf(out,
        "}; // constinit %s::Action_Table %s::action_table\n"
        "\n",
        cmd_data.parser.c_str(),cmd_data.parser.c_str()
    );
    fprintf(out,"constinit %s::Goto_Table %s::goto_table = {\n",cmd_data.parser.c_str(),cmd_data.parser.c_str());
    freopen(cmd_data.tmp_goto,"r",cmd_data.fp_goto);
    yuki::concat_file(out,cmd_data.fp_goto);
    fprintf(out,
        "}; // constinit %s::Goto_Table %s::goto_table\n"
        "\n",
        cmd_data.parser.c_str(),cmd_data.parser.c_str()
    );
    fprintf(out,
        "int %s::parse(%s& lexer_p_){\n"
        IND "namespace ypg = yuki::pg;\n"
        "\n"
        IND "reset();\n"
        IND "stack_.reserve(%zu);\n"
        "\n"
        IND "Any_Token token_ahead_ = lexer_p_.lex();\n"
        "\n"
        IND "while(true){\n"
        IND2 "switch(action_table(state_,token_ahead_.kind%s).kind()){\n"
        IND3 "case ypg::LR1_Action_Kind::S:{\n"
        IND4 "state_ = action_table(state_,token_ahead_.kind%s).state();\n"
        IND4 "stack_.emplace_back(std::move(token_ahead_),state_);\n"
        IND4 "token_ahead_ = lexer_p_.lex();\n"
        IND4 "goto loop_end_;\n"
        IND3 "}\n"
        IND3 "case ypg::LR1_Action_Kind::R:{\n"
        IND4 "switch(action_table(state_,token_ahead_.kind%s).rule()){\n",
        cmd_data.parser.c_str(), cmd_data.lexer.c_str(),
        cmd_data.lr1_stack_reserve,
        sp_token ? "" : "()",
        sp_token ? "" : "()",
        sp_token ? "" : "()"
    );

    for(const Rule<Token_Kind_t>& rule : rules){
        fprintf(out, IND5 "case %zu:{ // ",rule.num);
        print_rule(out,rule);
        fprintf(out,
            "\n"
            IND6 "assert(stack_.size()>=%zu);\n"
            IND6 "size_t start_ = stack_.size()-%zu;\n",
            rule.rights.size(), rule.rights.size()
        );
        if(!sp_token){
            for(size_t i = 0;i<rule.rights.size();++i)
                fprintf(out,
                    IND6 "Token::%s& token%zu_ = stack_[start_+%zu].token.get<Token::%s>();\n",
                    get_token_data(rule.rights[i]).name.c_str(), i, i, get_token_data(rule.rights[i]).name.c_str()
                );
            if(rule.left!=(Token_Kind_t)-1)
                fprintf(out,
                    IND6 "auto p_token_target_ = Any_Token::alloc.template allocate<Token::%s>();\n"
                    IND6 "YUKI_CONSTRUCT_BRACE(p_token_target_,%s);\n"
                    IND6 "Token::%s& token_target_ = *p_token_target_;\n"
                    "\n",
                    get_token_data(rule.left).name.c_str(), rule.init.c_str(),  get_token_data(rule.left).name.c_str()
                );
        }else{
            for(size_t i = 0;i<rule.rights.size();++i)
                fprintf(out,IND6 "Any_Token& token%zu_ = stack_[start_+%zu].token;\n", i,i);
            if(rule.left!=(Token_Kind_t)-1)
                fprintf(out,
                    IND6 "Any_Token token_target_{Token_Kind::%s,%s};\n"
                    "\n",
                    get_token_data(rule.left).name.c_str(), rule.init.c_str()
                );
        }
        fprintf(out,
            IND6 "{%s\n"
            IND6 "}\n"
            "\n",
            rule.code.c_str()
        );
        if(!sp_token){
            for(size_t i = rule.rights.size();i!=0;--i){
                fprintf(out, IND6 "stack_[start_+%zu].token.static_destroy_deallocate<Token::%s>();\n",
                    i-1, get_token_data(rule.rights[i-1]).name.c_str()
                );
                fprintf(out,IND6 "stack_.pop_back();\n");
            }
        }else{
            fprintf(out,IND6 "stack_.pop_back(%zu);\n",rule.rights.size());
        }
        if(rule.left==(Token_Kind_t)-1){
            if(!sp_token)
                fprintf(out,IND6 "token_ahead_.static_destroy_deallocate<Token::EOF_>();\n");
            fprintf(out,IND6 "return 0;\n" IND5 "}\n");
        }else{
            fprintf(out,
                IND6 "state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::%s);\n",
                get_token_data(rule.left).name.c_str()
            );
            if(!sp_token)
                fprintf(out,IND6 "stack_.emplace_back(Any_Token(p_token_target_),state_);\n");
            else
                fprintf(out,IND6 "stack_.emplace_back(std::move(token_target_),state_);\n");
            fprintf(out,
                IND6 "goto loop_end_;\n"
                IND5 "} // case %zu // ",
                rule.num
            );
            print_rule(out,rule);
            fprintf(out,"\n");
        }
    } // for(const Rule<Token_Kind_t>& rule : rules)
    fprintf(out,
        IND5 "default:return 2;\n"
        IND4 "} // switch(action_table(state_,token_ahead_.kind%s).rule())\n"
        IND3 "} // case ypg::LR1_Action_Kind::R\n"
        IND3 "case ypg::LR1_Action_Kind::ERR:{\n"
        IND4 "yuki::print_error(stderr,\"Syntax Error!\\n\");\n"
        IND4 "reset();\n"
        IND4 "yuki::print_error(stderr,\"Stack Clear!\\n\");\n"
        IND4 "return 1;\n"
        IND3 "}\n"
        IND2 "} // switch(action_table(state_,token_ahead_.kind%s).kind)\n"
        IND2 "loop_end_:;\n"
        IND "} // while(true)\n"
        "} // int %s::parse(%s&)\n",
        sp_token ? "" : "()",
        sp_token ? "" : "()",
        cmd_data.parser.c_str(),cmd_data.lexer.c_str()
    );
    if(!cmd_data.nspace.empty())
        fprintf(out,"} // namespace %s\n",cmd_data.nspace.c_str());
    fprintf(out,"\n");
    try{
        fprintf(out,"%s\n",code_htable.at("SEC2_").c_str());
    }catch(const std::out_of_range&){}


    FILE* const out_h = cmd_data.fp_out_h;
    fprintf(out_h,"#pragma once\n");
    try{
        fprintf(out_h,"%s\n",code_htable.at("h_top").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out_h,
        "#include<yuki/pg/lr1.hpp>\n"
        "#include\"%s\"\n",
        cmd_data.out_token.c_str()
    );
    if(!cmd_data.nspace.empty())
        fprintf(out_h,"namespace %s{\n",cmd_data.nspace.c_str());
    fprintf(out_h,
        "struct %s %s: yuki::pg::AbsLR1Parser<%s> {\n"
        IND "%s* lexer;\n"
        "\n"
        IND "typedef yuki::pg::LR1_Action_Table<%s,%zu,%zu> Action_Table;\n"
        IND "typedef yuki::pg::LR1_Goto_Table<%s,%zu> Goto_Table;\n"
        IND "static constinit Action_Table action_table;\n"
        IND "static constinit Goto_Table goto_table;\n"
        "\n",
        cmd_data.parser.c_str(), cmd_data.no_final_class ? "" : "final ", cmd_data.ts.c_str(),
        cmd_data.lexer.c_str(),
        cmd_data.ts.c_str(), state_size, rules.size(),
        cmd_data.ts.c_str(), state_size
    );
    if(!cmd_data.no_default_ctor){
        fprintf(out_h,IND "constexpr %s(%s* l=nullptr) noexcept : lexer(l) {}\n\n",cmd_data.parser.c_str(),cmd_data.lexer.c_str());
    }
    fprintf(out_h,
        IND "int parse(%s&);\n"
        "\n"
        IND "virtual int parse() override %s{assert(lexer); return parse(*lexer);}\n"
        "\n",
        cmd_data.lexer.c_str(),
        cmd_data.no_final_function ? "" : "final "
    );
    try{
        fprintf(out_h,"%s\n",code_htable.at("class").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out_h,"\n}; // struct %s\n",cmd_data.parser.c_str());
    if(!cmd_data.nspace.empty())
        fprintf(out_h,"} // namespace %s\n",cmd_data.nspace.c_str());
    try{
        fprintf(out_h,"%s\n",code_htable.at("h_end").c_str());
    }catch(const std::out_of_range&){}
} // void LR1_Writer<Token_Kind_t>::write

template struct LR1_Writer<unsigned char>;
template struct LR1_Writer<unsigned short>;
template struct LR1_Writer<unsigned>;
} // namespace yuki::pg