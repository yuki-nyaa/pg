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

#ifndef YUKI_PG_LR1_TABLE_MAX_LINE_ITEM
#define YUKI_PG_LR1_TABLE_MAX_LINE_ITEM 8
#endif

template<>
struct fmt::formatter<yuki::pg::Assoc,char> : yuki::simple_formatter<yuki::pg::Assoc,char> {
    template<typename OutputIt>
    auto format(const yuki::pg::Assoc assoc,fmt::basic_format_context<OutputIt,char>& ctx) -> typename fmt::basic_format_context<OutputIt,char>::iterator {
        switch(assoc){
            case yuki::pg::Assoc::LEFT : {return fmt::format_to(ctx.out(),"{}","LEFT");}
            default : {return fmt::format_to(ctx.out(),"{}","RIGHT");}
        }
    }
};

namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t>
size_t LR1_Writer<Token_Kind_t>::write_table(
    const bool is_switch,
    const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules,
    const Assoc assoc0,
    FILE* const fp_file,FILE* const fp_goto,FILE* const fp_err,FILE* const fp_log)
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
    memset(action_row,0, term_total * sizeof(Action_Candidates) );

    size_t sr_conflict_total = 0;
    size_t rr_conflict_total = 0;

    size_t action_count = 0;
    size_t goto_count = 0;
    auto print_action_ind = [fp_file,&action_count](){
        if(action_count % YUKI_PG_LR1_TABLE_MAX_LINE_ITEM == 0)
            fmt::print(fp_file,IND);
    };
    auto print_action_break = [fp_file,&action_count](){
        if(action_count % YUKI_PG_LR1_TABLE_MAX_LINE_ITEM == 0)
            fmt::print(fp_file,"\n");
    };
    auto print_goto_ind = [fp_goto,&goto_count](){
        if(goto_count % YUKI_PG_LR1_TABLE_MAX_LINE_ITEM == 0)
            fmt::print(fp_goto,IND);
    };
    auto print_goto_break = [fp_goto,&goto_count](){
        if(goto_count % YUKI_PG_LR1_TABLE_MAX_LINE_ITEM == 0)
            fmt::print(fp_goto,"\n");
    };

    while(!worklist.empty()){
        action_count = 0;
        goto_count = 0;
        const typename LR1_Item_Set_Set::const_iterator front = worklist.pop_front_v();
        const LR1_Item_Set& items = front->key;
        const size_t items_num = front->mapped;

        typename LR1_Item_Set::const_iterator it = items.begin();

        if(is_switch)
            fmt::print(fp_file,IND "case {}: CC{}: switch(t_.kind()){{\n",items_num,items_num);

        for(Token_Kind_t cursored_current=it.cursored(); cursored_current!=Token_Kind_t(-1); cursored_current=it.cursored()){ // Handles "shift" and "goto".
            assert(items_pending.empty());
            for(;!it.is_end() && it.cursored()==cursored_current; ++it)
                items_pending.emplace(it.left(),it.rights(),it.lookahead(),it.cursor()+1);
            items_pending.closure(rules,first_table);

            const yuki::IB_Pair<typename LR1_Item_Set_Set::const_iterator> insert_result= cc.insert(std::move(items_pending));
            if(insert_result.has_inserted)
                worklist.emplace_back(insert_result.iterator);
            const size_t items_pending_number = insert_result.iterator->mapped;

            if(cursored_current>=nterm_total && cursored_current<token_total){ // `cursored_current` is terminal.
                action_row[cursored_current-nterm_total].shift=items_pending_number;
                // The write of "action(cursored_current(,items_num)) = shift items_pending_number" is postponed until all items in the items_num item-set are considered.
            }else if(cursored_current<nterm_total){ // `cursored_current` is nterminal.
                print_goto_ind();
                fmt::print(fp_goto,"{{{},{},{}}},",items_num,cursored_current,items_pending_number);
                ++goto_count;
                print_goto_break();
                // Since "goto" cannot have conflicts, we can immediately write "goto(items_num,symbol)=items_pending_number".
            }
            yuki::try_print(fp_log,"GOTO({},{}) = {}\n",items_num,cursored_current,items_pending_number);

            if(it.is_end()){
                for(Token_Kind_t i=nterm_total;i<token_total;++i){
                    if(action_row[i-nterm_total].shift!=0){
                        if(is_switch){
                            fmt::print(fp_file,IND2 "case {}: p_.SHIFT_(std::move(t_),{}); t_=l_.lex(); goto CC{};\n",terms[i-nterm_total].name,action_row[i-nterm_total].shift,action_row[i-nterm_total].shift);
                        }else{
                            print_action_ind();
                            fmt::print(fp_file,"{{{},{},{{1,{}}}}},",items_num,i,action_row[i-nterm_total].shift);
                            ++action_count;
                            print_action_break();
                        }
                    }
                }
                memset(action_row,0, term_total * sizeof(Action_Candidates) );
                goto loop_1_end;
            }
        }

        for(;!it.is_end();++it){ // Handles "reduce".
            const auto rule_it = rules.find({it.left(),it.rights(),0});
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
                    if(is_switch){
                        fmt::print(fp_file,IND2 "case {}: p_.SHIFT_(std::move(t_),{}); t_=l_.lex(); goto CC{};\n",terms[i-nterm_total].name,action_this.shift,action_this.shift);
                    }else{
                        print_action_ind();
                        fmt::print(fp_file,"{{{},{},{{1,{}}}}},",items_num,i,action_this.shift);
                        ++action_count;
                        print_action_break();
                    }
                    memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
                case Action_Candidates::State::R : {
                    if(is_switch){
                        fmt::print(fp_file,IND2 "case {}: p_.SET_STATE({}); return {{{},{}}};\n",terms[i-nterm_total].name,items_num,terms[i-nterm_total].name,action_this.reduce.rule_num);
                    }else{
                        print_action_ind();
                        fmt::print(fp_file,"{{{},{},{{2,{}}}}},",items_num,i,action_this.reduce.rule_num);
                        ++action_count;
                        print_action_break();
                    }
                    memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
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
                            if(is_switch){
                                fmt::print(fp_file,IND2 "case {}: p_.SHIFT_(std::move(t_),{}); t_=l_.lex(); goto CC{};\n",terms[i-nterm_total].name,action_this.shift,action_this.shift);
                            }else{
                                print_action_ind();
                                fmt::print(fp_file,"{{{},{},{{1,{}}}}},",items_num,i,action_this.shift);
                                ++action_count;
                                print_action_break();
                            }
                            break;
                        }
                        case Action_Kind::REDUCE : {
                            yuki::try_print(fp_log,"Resolved to reduce {}.\n",action_this.reduce.rule_num);
                            if(is_switch){
                                fmt::print(fp_file,IND2 "case {}: p_.SET_STATE({}); return {{{},{}}};\n",terms[i-nterm_total].name,items_num,terms[i-nterm_total].name,action_this.reduce.rule_num);
                            }else{
                                print_action_ind();
                                fmt::print(fp_file,"{{{},{},{{2,{}}}}},",items_num,i,action_this.reduce.rule_num);
                                ++action_count;
                                print_action_break();
                            }
                            break;
                        }
                    }
                    ++sr_conflict_total;
                    memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
            }
        }
        loop_1_end:
        // Write the accept action.
        // The accept item-set might contain reduce-items with lookahead EOF_. Those will be overwritten, WITHOUT diagnostics. Such conflicts should be rare, however. The production-set that entails such conflicts should be obviously wrong, like "A -> A".
        if(is_switch){
            if(items_num==1)
                fmt::print(fp_file,IND2 "case EOF_: p_.SET_STATE(1); return {{EOF_,0}};\n");
            fmt::print(fp_file,IND2 "default : return {{(size_t)-1,(size_t)-1}};\n" IND "}}\n");
        }else{
            if(items_num==1){
                print_action_ind();
                fmt::print(fp_file,"{{1,{},{{2,0}}}},",token_total-1);
                ++action_count;
                print_action_break();
            }
            if(action_count % YUKI_PG_LR1_TABLE_MAX_LINE_ITEM != 0)
                fmt::print(fp_file,"\n");
        }
        if(goto_count % YUKI_PG_LR1_TABLE_MAX_LINE_ITEM != 0)
            fmt::print(fp_goto,"\n");
        #ifdef YUKI_PG_DBG
        for(Token_Kind_t i=nterm_total;i<token_total;++i){
            if(!action_row[i-nterm_total].empty())
                YUKI_PG_DBGO("non-empty action_candidates at ({},{}).\n",items_num,i);
        }
        memset(action_row,0, term_total * sizeof(Action_Candidates) );
        #endif
    } // while(!worklist.empty())

    // Write the accept action.
    // The accept item-set might contain reduce-items with lookahead EOF_. Those will be overwritten, WITHOUT diagnostics. Such conflicts should be rare, however. The production-set that entails such conflicts should be obviously wrong, like "A -> A".
    //fmt::print(fp_file,IND "{{1,{},{{2,0}}}},\n",token_total-1);

    if(sr_conflict_total || rr_conflict_total)
        fmt::print(fp_err,"{} SR-conflict(s), {} RR-conflict(s) in total. See log for details.\n",sr_conflict_total,rr_conflict_total);

    delete[] action_row;

    fflush(fp_file);
    fflush(fp_goto);
    fflush(fp_err);
    fflush(fp_log);

    return cc.size();
} // size_t LR1_Writer<Token_Kind_t>::write_table

template<std::unsigned_integral Token_Kind_t>
void LR1_Writer<Token_Kind_t>::write_parse_array(
    FILE* const out,const Options& options,
    const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules)
{
    auto get_token_data = [&nterms,&terms](const Token_Kind_t k) -> const Token_Data& {
        assert(k!=(Token_Kind_t)-1);
        return k>=nterms.size() ? terms[k-nterms.size()] : nterms[k];
    };

    auto print_rule = [get_token_data](FILE* const fp,const Rule<Token_Kind_t>& r) -> void {
        fprintf(fp,"%s ->",
            r.left!=(Token_Kind_t)-1 ? get_token_data(r.left).name_or_alias().c_str() : "Goal_"
        );
        for(const Token_Kind_t k : r.rights)
            fprintf(fp," %s",get_token_data(k).name_or_alias().c_str());
    };

    auto print_rule_escaped = [get_token_data](FILE* const fp,const Rule<Token_Kind_t>& r) -> void {
        static std::string buf;

        if(r.left!=(Token_Kind_t)-1){
            const std::string_view noa = get_token_data(r.left).name_or_alias();
            for(const char c : noa){
                switch(c){
                    case '\'': buf.append("\\\'");break;
                    case '\"': buf.append("\\\"");break;
                    case '\\': buf.append("\\\\");break;
                    default: buf.push_back(c);break;
                }
            }
            fprintf(fp,"%s ->",buf.c_str());
            buf.clear();
        }else
            fprintf(fp,"Goal_ ->");

        for(const Token_Kind_t k : r.rights){
            const std::string_view noa = get_token_data(k).name_or_alias();
            for(const char c : noa){
                switch(c){
                    case '\'': buf.append("\\\'");break;
                    case '\"': buf.append("\\\"");break;
                    case '\\': buf.append("\\\\");break;
                    default: buf.push_back(c);break;
                }
            }
            fprintf(fp," %s",buf.c_str());
            buf.clear();
        }
    };

    fprintf(out,
        "int %s::parse(%s& l_){\n"
        IND "namespace ypg = yuki::pg;\n"
        "\n"
        IND "reset();\n"
        IND "stack_.reserve(%zu);\n"
        "\n"
        IND "Token_t t_ = l_.lex();\n"
        "\n"
        IND "while(true){\n"
        IND2 "switch(action_table(state_,t_.kind()).kind()){\n"
        IND3 "case ypg::LR1_Action_Kind::S:{\n"
        IND4 "state_ = action_table(state_,t_.kind()).state();\n"
        IND4 "stack_.emplace_back(std::move(t_),state_);\n"
        IND4 "t_ = l_.lex();\n"
        IND4 "goto loop_end_;\n"
        IND3 "}\n"
        IND3 "case ypg::LR1_Action_Kind::R:{\n"
        IND4 "switch(action_table(state_,t_.kind()).rule()){\n",
        options.parser.c_str(), options.lexer.c_str(),
        options.lr1_stack_reserve
    );

    for(const Rule<Token_Kind_t>& rule : rules){
        fprintf(out, IND5 "case %zu:{ // ",rule.num);
        print_rule(out,rule);
        fprintf(out,
            "\n"
            IND6 "assert(stack_.size()>=%zu);\n"
            IND6 "const size_t start_ = stack_.size()-%zu;\n",
            rule.rights.size(), rule.rights.size()
        );
        switch(options.token_impl_type){
            case Options::Token_Impl_Type::VARIANT:{
                for(size_t i = 0;i<rule.rights.size();++i){
                    const Token_Data& td = get_token_data(rule.rights[i]);
                    if(!td.types.empty())
                        fprintf(out,
                            IND6 "%s& token%zu_ = stack_[start_+%zu].token.get<%zu>();\n",
                            td.types[0].c_str(),i,i,td.type_index
                        );
                }
                if(rule.left!=(Token_Kind_t)-1){
                    const Token_Data& td = get_token_data(rule.left);
                    fprintf(out,IND6 "Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::%s>,",td.name.c_str());
                    if(rule.rights.size()!=0)
                        fprintf(out,"{stack_[start_].token.location_range().begin,stack_[start_+%zu].token.location_range().end}",rule.rights.size()-1);
                    else
                        fprintf(out,"{}");
                    fprintf(out,"%s%s);\n", rule.init.empty() ? "" : ",", rule.init.c_str());
                    if(!td.types.empty())
                        fprintf(out,IND6 "%s& token_target_ = token_target_complete_.get<%zu>();\n\n",td.types[0].c_str(),td.type_index);
                }
                break;
            }
            case Options::Token_Impl_Type::SIMPLE:{
                for(size_t i = 0;i<rule.rights.size();++i)
                    fprintf(out,IND6 "Token_t& token%zu_ = stack_[start_+%zu].token;\n", i,i);
                if(rule.left!=(Token_Kind_t)-1){
                    fprintf(out,
                        IND6 "Token_t token_target_{Token_Kind::%s,%s",
                        get_token_data(rule.left).name.c_str(),rule.init.c_str()
                    );
                    if(rule.rights.size()!=0)
                        fprintf(out,",{token0_.location_range.begin,token%zu_.location_range.end}};\n\n", rule.rights.size()-1);
                    else
                        fprintf(out,"};\n\n");
                }
                break;
            }
            case Options::Token_Impl_Type::TUPLE:{
                for(size_t i = 0;i<rule.rights.size();++i)
                    fprintf(out,
                        IND6 "Token::%s& token%zu_ = stack_[start_+%zu].token.get<Token::%s>();\n",
                        get_token_data(rule.rights[i]).name.c_str(), i, i, get_token_data(rule.rights[i]).name.c_str()
                    );
                if(rule.left!=(Token_Kind_t)-1){
                    fprintf(out,
                        IND6 "auto p_token_target_ = Token_t::alloc.template allocate<Token::%s>();\n"
                        IND6 "yuki::pg::Location_Range loc_target_ = ",
                        get_token_data(rule.left).name.c_str()
                    );
                    if(rule.rights.size()!=0)
                        fprintf(out,"{stack_[start_].token.location_range().begin,stack_[start_+%zu].token.location_range().end};\n", rule.rights.size()-1);
                    else
                        fprintf(out,"{};\n");
                    fprintf(out,
                        IND6 "YUKI_CONSTRUCT_BRACE(p_token_target_,%s);\n"
                        IND6 "Token::%s& token_target_ = *p_token_target_;\n\n",
                        rule.init.c_str(),  get_token_data(rule.left).name.c_str()
                    );
                }
                break;
            }
        }
        fprintf(out,
            IND6 "{%s\n"
            IND6 "}\n"
            "\n",
            rule.code.c_str()
        );
        if(options.is_tuple()){
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
            if(options.is_tuple())
                fprintf(out,IND6 "t_.static_destroy_deallocate<Token::EOF_>();\n");
            fprintf(out,IND6 "return 0;\n" IND5 "}\n");
        }else{
            fprintf(out,
                IND6 "state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::%s);\n",
                get_token_data(rule.left).name.c_str()
            );
            switch(options.token_impl_type){
                case Options::Token_Impl_Type::VARIANT: fprintf(out,IND6 "stack_.emplace_back(std::move(token_target_complete_),state_);\n"); break;
                case Options::Token_Impl_Type::SIMPLE: fprintf(out,IND6 "stack_.emplace_back(std::move(token_target_),state_);\n"); break;
                case Options::Token_Impl_Type::TUPLE: fprintf(out,IND6 "stack_.emplace_back(Token_t(p_token_target_,loc_target_),state_);\n"); break;
            }
            fprintf(out,IND6 "YUKI_PG_TARGET_DBGO(\"REDUCE ");
            print_rule_escaped(out,rule);
            fprintf(out,
                "\\n\");\n"
                IND6 "break;\n"
                IND5 "} // case %zu //",
                rule.num
            );
            print_rule(out,rule);
            fprintf(out,"\n");
        }
    } // for(const Rule<Token_Kind_t>& rule : rules)
    fprintf(out,
        IND5 "default:return 2;\n"
        IND4 "} // switch(action_table(state_,t_.kind()).rule())\n"
        IND3 "} // case ypg::LR1_Action_Kind::R\n"
        IND3 "case ypg::LR1_Action_Kind::ERR:{\n"
        IND4 "yuki::print_error(stderr,\"Syntax Error!\\n\");\n"
        IND4 "reset();\n"
        IND4 "yuki::print_error(stderr,\"Stack Clear!\\n\");\n"
        IND4 "return 1;\n"
        IND3 "}\n"
        IND2 "} // switch(action_table(state_,t_.kind()).kind)\n"
        IND2 "loop_end_:;\n"
        IND "} // while(true)\n"
        "} // int %s::parse(%s&)\n",
        options.parser.c_str(),options.lexer.c_str()
    );
} // void LR1_Writer<Token_Kind_t>::write_parse_array

template<std::unsigned_integral Token_Kind_t>
void LR1_Writer<Token_Kind_t>::write_parse_switch(
    FILE* const out,const Options& options,
    const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules)
{
    auto get_token_data = [&nterms,&terms](const Token_Kind_t k) -> const Token_Data& {
        assert(k!=(Token_Kind_t)-1);
        return k>=nterms.size() ? terms[k-nterms.size()] : nterms[k];
    };

    auto print_rule = [get_token_data](FILE* const fp,const Rule<Token_Kind_t>& r) -> void {
        fprintf(fp,"%s ->",
            r.left!=(Token_Kind_t)-1 ? get_token_data(r.left).name_or_alias().c_str() : "Goal_"
        );
        for(const Token_Kind_t k : r.rights)
            fprintf(fp," %s",get_token_data(k).name_or_alias().c_str());
    };

    auto print_rule_escaped = [get_token_data](FILE* const fp,const Rule<Token_Kind_t>& r) -> void {
        static std::string buf;

        if(r.left!=(Token_Kind_t)-1){
            const std::string_view noa = get_token_data(r.left).name_or_alias();
            for(const char c : noa){
                switch(c){
                    case '\'': buf.append("\\\'");break;
                    case '\"': buf.append("\\\"");break;
                    case '\\': buf.append("\\\\");break;
                    default: buf.push_back(c);break;
                }
            }
            fprintf(fp,"%s ->",buf.c_str());
            buf.clear();
        }else
            fprintf(fp,"Goal_ ->");

        for(const Token_Kind_t k : r.rights){
            const std::string_view noa = get_token_data(k).name_or_alias();
            for(const char c : noa){
                switch(c){
                    case '\'': buf.append("\\\'");break;
                    case '\"': buf.append("\\\"");break;
                    case '\\': buf.append("\\\\");break;
                    default: buf.push_back(c);break;
                }
            }
            fprintf(fp," %s",buf.c_str());
            buf.clear();
        }
    };

    fprintf(out,
        "int %s::parse(%s& l_){\n"
        IND "reset();\n"
        IND "stack_.reserve(%zu);\n"
        IND "Action_Table action_table(l_);\n"
        "\n"
        IND "while(true){\n"
        IND2 "yuki::pg::lr1_action_return_type art_ = action_table(*this,l_);\n"
        IND2 "switch(art_.rule){\n",
        options.parser.c_str(), options.lexer.c_str(),
        options.lr1_stack_reserve
    );

    for(const Rule<Token_Kind_t>& rule : rules){
        fprintf(out, IND3 "case %zu:{ // ",rule.num);
        print_rule(out,rule);
        fprintf(out,
            "\n"
            IND4 "assert(stack_.size()>=%zu);\n"
            IND4 "const size_t start_ = stack_.size()-%zu;\n",
            rule.rights.size(), rule.rights.size()
        );
        switch(options.token_impl_type){
            case Options::Token_Impl_Type::VARIANT:{
                for(size_t i = 0;i<rule.rights.size();++i){
                    const Token_Data& td = get_token_data(rule.rights[i]);
                    if(!td.types.empty())
                        fprintf(out,
                            IND4 "%s& token%zu_ = stack_[start_+%zu].token.get<%zu>();\n",
                            td.types[0].c_str(),i,i,td.type_index
                        );
                }
                if(rule.left!=(Token_Kind_t)-1){
                    const Token_Data& td = get_token_data(rule.left);
                    fprintf(out,IND4 "Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::%s>,",td.name.c_str());
                    if(rule.rights.size()!=0)
                        fprintf(out,"{stack_[start_].token.location_range().begin,stack_[start_+%zu].token.location_range().end}",rule.rights.size()-1);
                    else
                        fprintf(out,"{}");
                    fprintf(out,"%s%s);\n", rule.init.empty() ? "" : ",", rule.init.c_str());
                    if(!td.types.empty())
                        fprintf(out,IND4 "%s& token_target_ = token_target_complete_.get<%zu>();\n\n",td.types[0].c_str(),td.type_index);
                }
                break;
            }
            case Options::Token_Impl_Type::SIMPLE:{
                for(size_t i = 0;i<rule.rights.size();++i)
                    fprintf(out,IND4 "Token_t& token%zu_ = stack_[start_+%zu].token;\n", i,i);
                if(rule.left!=(Token_Kind_t)-1){
                    fprintf(out,
                        IND4 "Token_t token_target_{Token_Kind::%s,%s",
                        get_token_data(rule.left).name.c_str(),rule.init.c_str()
                    );
                    if(rule.rights.size()!=0)
                        fprintf(out,",{token0_.location_range.begin,token%zu_.location_range.end}};\n\n", rule.rights.size()-1);
                    else
                        fprintf(out,"};\n\n");
                }
                break;
            }
            case Options::Token_Impl_Type::TUPLE:{
                for(size_t i = 0;i<rule.rights.size();++i)
                    fprintf(out,
                        IND4 "Token::%s& token%zu_ = stack_[start_+%zu].token.get<Token::%s>();\n",
                        get_token_data(rule.rights[i]).name.c_str(), i, i, get_token_data(rule.rights[i]).name.c_str()
                    );
                if(rule.left!=(Token_Kind_t)-1){
                    fprintf(out,
                        IND4 "auto p_token_target_ = Token_t::alloc.template allocate<Token::%s>();\n"
                        IND4 "yuki::pg::Location_Range loc_target_ = ",
                        get_token_data(rule.left).name.c_str()
                    );
                    if(rule.rights.size()!=0)
                        fprintf(out,"{stack_[start_].token.location_range().begin,stack_[start_+%zu].token.location_range().end};\n", rule.rights.size()-1);
                    else
                        fprintf(out,"{};\n");
                    fprintf(out,
                        IND4 "YUKI_CONSTRUCT_BRACE(p_token_target_,%s);\n"
                        IND4 "Token::%s& token_target_ = *p_token_target_;\n\n",
                        rule.init.c_str(),  get_token_data(rule.left).name.c_str()
                    );
                }
                break;
            }
        }
        fprintf(out,
            IND4 "{%s\n"
            IND4 "}\n"
            "\n",
            rule.code.c_str()
        );
        if(options.is_tuple()){
            for(size_t i = rule.rights.size();i!=0;--i){
                fprintf(out, IND4 "stack_[start_+%zu].token.static_destroy_deallocate<Token::%s>();\n",
                    i-1, get_token_data(rule.rights[i-1]).name.c_str()
                );
                fprintf(out,IND4 "stack_.pop_back();\n");
            }
        }else{
            fprintf(out,IND4 "stack_.pop_back(%zu);\n",rule.rights.size());
        }
        if(rule.left==(Token_Kind_t)-1){
            if(options.is_tuple())
                fprintf(out,IND4 "action_table.static_destroy_deallocate<Token::EOF_>();\n");
            fprintf(out,IND4 "return 0;\n" IND3 "}\n");
        }else{
            fprintf(out,
                IND4 "state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::%s);\n",
                get_token_data(rule.left).name.c_str()
            );
            switch(options.token_impl_type){
                case Options::Token_Impl_Type::VARIANT: fprintf(out,IND4 "stack_.emplace_back(std::move(token_target_complete_),state_);\n"); break;
                case Options::Token_Impl_Type::SIMPLE: fprintf(out,IND4 "stack_.emplace_back(std::move(token_target_),state_);\n"); break;
                case Options::Token_Impl_Type::TUPLE: fprintf(out,IND4 "stack_.emplace_back(Token_t(p_token_target_,loc_target_),state_);\n"); break;
            }
            fprintf(out,IND4 "YUKI_PG_TARGET_DBGO(\"REDUCE ");
            print_rule_escaped(out,rule);
            fprintf(out,
                "\\n\");\n"
                IND4 "break;\n"
                IND3 "} // case %zu //",
                rule.num
            );
            print_rule(out,rule);
            fprintf(out,"\n");
        }
    } // for(const Rule<Token_Kind_t>& rule : rules)
    fprintf(out,
        IND3 "default:{\n"
        IND4 "yuki::print_error(stderr,\"Syntax Error!\\n\");\n"
        IND4 "reset();\n"
        IND4 "yuki::print_error(stderr,\"Stack Clear!\\n\");\n"
        IND4 "return 1;\n"
        IND3 "}\n"
        IND2 "} // switch(art_.rule)\n"
        IND "} // while(true)\n"
        "} // int %s::parse(%s&)\n",
        options.parser.c_str(),options.lexer.c_str()
    );
} // void LR1_Writer<Token_Kind_t>::write_parse_switch

template<std::unsigned_integral Token_Kind_t>
void LR1_Writer<Token_Kind_t>::write(
    const Cmd_Data& cmd_data,const Options& options,
    const std::unordered_map<std::string,std::string> code_htable,
    const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules)
{
    FILE* const out = cmd_data.fp_out_cpp;
    const bool is_switch = options.is_switch;

    try{
        fprintf(out,"%s\n",code_htable.at("cpp_top").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out,
        "#include<yuki/print.hpp>\n"
        "#include<yuki/pg/lr1.hpp>\n"
        "#include\"%s\"\n",
        cmd_data.out_h.c_str()
    );
    if(!options.nspace.empty())
        fprintf(out,"namespace %s{\n",options.nspace.c_str());
    if(is_switch){
        fprintf(out,
            "yuki::pg::lr1_action_return_type %s::Action_Table::operator()(yuki::pg::AbsLR1Parser<%s>& p_,%s& l_){\n"
            IND "using enum %s::Token_Kind::enum_t;\n"
            IND "switch(p_.state()){\n",
            options.parser_tables.c_str(),options.ts.c_str(),options.lexer.c_str(),
            options.ts.c_str()
        );
    }else{
        fprintf(out,"constinit %s::Action_Table %s::action_table = {\n",options.parser_tables.c_str(),options.parser_tables.c_str());
    }
    const size_t state_size = write_table(is_switch,nterms,terms,rules,options.assoc0,cmd_data.fp_out_cpp,cmd_data.fp_goto,stderr,cmd_data.fp_out_log);
    if(is_switch){
        fprintf(out,
            IND "default: return {(size_t)-1,(size_t)-1};\n"
            IND "} // switch(p_.state())\n"
            "} // yuki::pg::lr1_action_return_type %s::Action_Table::operator()(yuki::pg::AbsLR1Parser<%s>& p_,%s& l_)\n"
            "\n",
            options.parser_tables.c_str(),options.ts.c_str(),options.lexer.c_str()
        );
    }else{
        fprintf(out,
            "}; // constinit %s::Action_Table %s::action_table\n"
            "\n",
            options.parser_tables.c_str(),options.parser_tables.c_str()
        );
    }
    fprintf(out,"constinit %s::Goto_Table %s::goto_table = {\n",options.parser_tables.c_str(),options.parser_tables.c_str());
    rewind(cmd_data.fp_goto);
    yuki::concat_file(out,cmd_data.fp_goto);
    fprintf(out,
        "}; // constinit %s::Goto_Table %s::goto_table\n"
        "\n",
        options.parser_tables.c_str(),options.parser_tables.c_str()
    );
    is_switch ? write_parse_switch(out,options,nterms,terms,rules) : write_parse_array(out,options,nterms,terms,rules);
    if(!options.nspace.empty())
        fprintf(out,"} // namespace %s\n",options.nspace.c_str());
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
    if(!options.nspace.empty())
        fprintf(out_h,"namespace %s{\n",options.nspace.c_str());
    if(is_switch){
        fprintf(out_h,
            "struct %s{\n"
            IND "struct Action_Table{\n"
            IND YUKI_PG_HIND "private:\n"
            IND2 "%s::Token_t t_;\n"
            IND YUKI_PG_HIND "public:\n"
            IND2 "Action_Table(%s& l_) noexcept : t_(l_.lex()) {}\n"
            IND2 "yuki::pg::lr1_action_return_type operator()(yuki::pg::AbsLR1Parser<%s>&,%s&);\n",
            options.parser_tables.c_str(),
            options.ts.c_str(),
            options.lexer.c_str(),
            options.ts.c_str(),options.lexer.c_str()
        );
        if(options.is_tuple())
            fprintf(out_h,
                "\n"
                IND2 "template<typename T> requires %s::is_token_v<T>\n"
                IND "void static_destroy_deallocate() noexcept {t_.static_destroy_deallocate<T>();}\n",
                options.ts.c_str()
            );
        fprintf(out_h,
            IND "};\n"
            IND "typedef yuki::pg::LR1_Goto_Table<%s,%zu> Goto_Table;\n"
            IND "static constinit Goto_Table goto_table;\n"
            "};\n"
            "\n",
            options.ts.c_str(), state_size
        );
        fprintf(out_h,
            "struct %s %s: yuki::pg::AbsLR1Parser<%s>, private %s {\n"
            IND "%s* lexer;\n"
            "\n"
            IND "using %s::Action_Table;\n"
            IND "using %s::Goto_Table;\n"
            IND "using %s::goto_table;\n"
            "\n",
            options.parser.c_str(), options.no_final_class ? "" : "final ", options.ts.c_str(), options.parser_tables.c_str(),
            options.lexer.c_str(),
            options.parser_tables.c_str(),
            options.parser_tables.c_str(),
            options.parser_tables.c_str()
        );
    }else{
        fprintf(out_h,
            "struct %s{\n"
            IND "typedef yuki::pg::LR1_Action_Table<%s,%zu,%zu> Action_Table;\n"
            IND "typedef yuki::pg::LR1_Goto_Table<%s,%zu> Goto_Table;\n"
            IND "static constinit Action_Table action_table;\n"
            IND "static constinit Goto_Table goto_table;\n"
            "};\n"
            "\n",
            options.parser_tables.c_str(),
            options.ts.c_str(), state_size, rules.size(),
            options.ts.c_str(), state_size
        );
        fprintf(out_h,
            "struct %s %s: yuki::pg::AbsLR1Parser<%s>, private %s {\n"
            IND "%s* lexer;\n"
            "\n"
            IND "using %s::Action_Table;\n"
            IND "using %s::Goto_Table;\n"
            IND "using %s::action_table;\n"
            IND "using %s::goto_table;\n"
            "\n",
            options.parser.c_str(), options.no_final_class ? "" : "final ", options.ts.c_str(), options.parser_tables.c_str(),
            options.lexer.c_str(),
            options.parser_tables.c_str(),
            options.parser_tables.c_str(),
            options.parser_tables.c_str(),
            options.parser_tables.c_str()
        );
    }
    if(!options.no_default_ctor){
        fprintf(out_h,IND "constexpr %s(%s* const l=nullptr) noexcept : lexer(l) {}\n\n",options.parser.c_str(),options.lexer.c_str());
    }
    fprintf(out_h,
        IND "int parse(%s&);\n"
        "\n"
        IND "virtual int parse() override %s{assert(lexer); return parse(*lexer);}\n"
        "\n",
        options.lexer.c_str(),
        options.no_final_function ? "" : "final "
    );
    try{
        fprintf(out_h,"%s\n",code_htable.at("class").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out_h,"\n}; // struct %s\n",options.parser.c_str());
    if(!options.nspace.empty())
        fprintf(out_h,"} // namespace %s\n",options.nspace.c_str());
    try{
        fprintf(out_h,"%s\n",code_htable.at("h_end").c_str());
    }catch(const std::out_of_range&){}
} // void LR1_Writer<Token_Kind_t>::write

template struct LR1_Writer<unsigned char>;
template struct LR1_Writer<unsigned short>;
template struct LR1_Writer<unsigned>;
} // namespace yuki::pg