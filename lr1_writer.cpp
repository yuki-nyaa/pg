#include"lr1_writer.h"
#include"debug.hpp"
namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t>
size_t LR1_Writer<Token_Kind_t>::write_table(
    const std::vector<Token_Data>& nterms,const std::vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules,
    const Assoc assoc0,
    FILE* fp_file,FILE* fp_goto,FILE* fp_err,FILE* fp_log)
{
    assert(fp_file); assert(fp_err); // Log-out is optional.

    First_Table<Token_Kind_t> first_table(nterms,terms,rules);
    std::vector<LR1_Item_Set> cc;
    #ifndef YUKI_PG_LR1_ITEMS_RESERVE
    #define YUKI_PG_LR1_ITEMS_RESERVE 65536
    #endif
    cc.reserve(YUKI_PG_LR1_ITEMS_RESERVE);
    size_t cc_next=1;
    cc.push_back(generate_initial_items(rules,first_table));

    LR1_Item_Set cc_candidate{};

    const size_t term_total = terms.size();
    const size_t nterm_total = nterms.size();
    const size_t token_total = term_total + nterm_total;

    Action_Candidates* action_row = new Action_Candidates[term_total];
    std::memset(action_row,0, term_total * sizeof(Action_Candidates) );

    size_t sr_conflict_total = 0;
    size_t rr_conflict_total = 0;

    for(size_t cc_current=0; cc_current<cc_next && !(cc[cc_current].processed); ++cc_current){
        cc[cc_current].processed=true;
        typename LR1_Item_Set::const_iterator it = cc[cc_current].begin();
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
            if(cursored_current>=nterm_total && cursored_current<token_total){ // `cursored_current` is terminal.
                action_row[cursored_current-nterm_total].shift=cc_candidate_number;
                // The write of "action(cursored_current(,cc_current)) = shift cc_candidate_number" is postponed until all items in the current item-set are considered.
            }else if(cursored_current<nterm_total){ // `cursored_current` is nterminal.
                fmt::print(fp_goto,"{{{},{},{}}},",cc_current,cursored_current,cc_candidate_number);
                // Since "goto" cannot have conflicts, we can immediately write "goto(cc_current,symbol)=cc_candidate_number".
            }
            yuki::try_print(fp_log,"GOTO({},{}) = {}\n",cc_current,cursored_current,cc_candidate_number);

            if(it.is_end()){
                for(Token_Kind_t i=nterm_total;i<token_total;++i){
                    if(action_row[i-nterm_total].shift!=0)
                        fmt::print(fp_file,"{{{},{},{{1,{}}}}},",cc_current,i,action_row[i-nterm_total].shift);
                }
                std::memset(action_row,0, term_total * sizeof(Action_Candidates) );
                goto loop_1_end;
            }else{
                cursored_current=it.cursored();
            }
        }

        for(;!it.is_end();++it){ // Handles "reduce".
            auto rule_it = rules.first_equal({it.left(),it.rights(),0});
            assert(rule_it!=rules.end());
            if(Action_Candidates& action_this = action_row[it.lookahead()-nterm_total] ; action_this.reduce.rule_num!=0){
                yuki::try_print(fp_log,"RR-conflict encountered. action({},{}) = reduce {}({}) / {}({}). ",
                    cc_current,
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
                    fmt::print(fp_file,"{{{},{},{{1,{}}}}},",cc_current,i,action_this.shift);
                    std::memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
                case Action_Candidates::State::R : {
                    fmt::print(fp_file,"{{{},{},{{2,{}}}}},",cc_current,i,action_this.reduce.rule_num);
                    std::memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
                case Action_Candidates::State::SR : {
                    yuki::try_print(fp_log,"SR-conflict encountered. action({},{}) = shift {} / reduce {}({}). Prec(lookahead)={}. Assoc(lookahead)={}. ",
                        cc_current,terms[i-nterm_total].name, action_this.shift,
                        action_this.reduce.rule_num, action_this.reduce.prec_sr,
                        terms[i-nterm_total].prec, terms[i-nterm_total].prec==0 ? assoc0 : terms[i-nterm_total].assoc
                    );
                    switch(resolve_sr_conflict(nterms,terms,assoc0,action_this,i)){
                        case Action_Kind::SHIFT : {
                            yuki::try_print(fp_log,"Resolved to shift {}.\n",action_this.shift);
                            fmt::print(fp_file,"{{{},{},{{1,{}}}}},",cc_current,i,action_this.shift);
                            break;
                        }
                        case Action_Kind::REDUCE : {
                            yuki::try_print(fp_log,"Resolved to reduce {}.\n",action_this.reduce.rule_num);
                            fmt::print(fp_file,"{{{},{},{{2,{}}}}},",cc_current,i,action_this.reduce.rule_num);
                            break;
                        }
                    }
                    ++sr_conflict_total;
                    std::memset(action_row+(i-nterm_total),0,sizeof(Action_Candidates));
                    break;
                }
            }
        }
        loop_1_end:;
        #ifdef YUKI_PG_DBG
        for(Token_Kind_t i=nterm_total;i<token_total;++i){
            if(!action_row[i-nterm_total].empty())
                dbgout("non-empty action_candidates at ({},{}).\n",cc_current,i);
        }
        std::memset(action_row,0, term_total * sizeof(Action_Candidates) );
        #endif
    }

    // Write the accept action.
    // The accept item-set might contain reduce-items with lookahead EOF_. Those will be overwritten, WITHOUT diagnostics. Such conflicts should be rare, however. The production-set that entails such conflicts should obviously wrong, like "A -> A".
    fmt::print(fp_file,"{{1,{},{{2,0}}}},",token_total-1);

    if(sr_conflict_total || rr_conflict_total)
        fmt::print(fp_err,"{} SR-conflict(s), {} RR-conflict(s) in total. See log for details.\n",sr_conflict_total,rr_conflict_total);

    delete[] action_row;

    return cc.size();
}

template<std::unsigned_integral Token_Kind_t>
void LR1_Writer<Token_Kind_t>::write(
    const Cmd_Data& cmd_data,
    const std::unordered_map<std::string,std::string> code_htable,
    const std::vector<Token_Data>& nterms,const std::vector<Token_Data>& terms,
    const Rule_Set<Token_Kind_t>& rules,
    const Assoc assoc0)
{
    #define IND YUKI_PG_IND
    #define IND2 IND IND
    #define IND3 IND IND IND
    #define IND4 IND IND IND IND
    #define IND5 IND IND IND IND IND
    #define IND6 IND IND IND IND IND IND

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
        "#include<yuki/pg/lr1.hpp>\n"
        "#include\"%s\"\n"
        "constinit %s%s%s::Action_Table %s%s%s::action_table = {\n",
        cmd_data.out_h.c_str(),
        cmd_data.nspace.c_str(), cmd_data.nspace.empty() ? "" : "::", cmd_data.parser.c_str(),
        cmd_data.nspace.c_str(), cmd_data.nspace.empty() ? "" : "::", cmd_data.parser.c_str()
    );
    const size_t state_size = write_table(nterms,terms,rules,assoc0,cmd_data.fp_out_cpp,cmd_data.fp_goto,stderr,cmd_data.fp_out_log);
    fprintf(out,
        "\n"
        "};\n"
        "\n"
    );
    fprintf(out,
        "constinit %s%s%s::Goto_Table %s%s%s::goto_table = {\n",
        cmd_data.nspace.c_str(), cmd_data.nspace.empty() ? "" : "::", cmd_data.parser.c_str(),
        cmd_data.nspace.c_str(), cmd_data.nspace.empty() ? "" : "::", cmd_data.parser.c_str()
    );
    freopen(cmd_data.tmp_goto,"r",cmd_data.fp_goto);
    yuki::concat_file(out,cmd_data.fp_goto);
    fprintf(out,
        "\n"
        "};\n"
        "\n"
    );
    fprintf(out,
        "int %s::%s::parse(){\n"
        IND "namespace ypg = yuki::pg;\n\n"
        IND "reset();\n\n"
        IND "Any_Token token_ahead_ = lexer_->lex();\n\n"
        IND "while(true){\n"
        IND2 "switch(action_table(state_,token_ahead_.kind%s).kind()){\n"
        IND3 "case ypg::LR1_Action_Kind::S : {\n"
        IND4 "state_ = action_table(state_,token_ahead_.kind%s).state();\n"
        IND4 "stack_.push_back({std::move(token_ahead_),state_});\n"
        IND4 "token_ahead_ = lexer_->lex();\n"
        IND4 "goto loop_end_;\n"
        IND3 "}\n"
        IND3 "case ypg::LR1_Action_Kind::R : {\n"
        IND4 "switch(action_table(state_,token_ahead_.kind%s).rule()){\n",
        cmd_data.nspace.c_str(), cmd_data.parser.c_str(),
        sp_token ? "" : "()", sp_token ? "" : "()", sp_token ? "" : "()"
    );

    for(const Rule<Token_Kind_t>& rule : rules){
        fprintf(out, IND5 "case %zu : { // ",rule.num);
        print_rule(out,rule);
        fprintf(out,
            "\n"
            IND6 "assert(stack_.size()>=%zu);\n"
            IND6 "size_t start_ = stack_.size()-%zu;\n"
            IND6 "size_t end_ = stack_.size()-1;\n",
            rule.rights.size(), rule.rights.size()
        );
        if(!sp_token){
            for(size_t i = 0;i<rule.rights.size();++i)
                fprintf(out,
                    IND6 "Token::%s& token%zu_ = stack_[start_+%zu].token.get<Token::%s>();\n",
                    get_token_data(rule.rights[i]).name.c_str(), i, i, get_token_data(rule.rights[i]).name.c_str()
                );
            fprintf(out,"\n");
            if(rule.left!=(Token_Kind_t)-1)
                fprintf(out,
                    IND6 "auto p_token_target_ = Any_Token::alloc.template allocate<Token::%s>();\n"
                    IND6 "YUKI_CONSTRUCT(p_token_target_,%s);\n"
                    IND6 "Token::%s& token_target_ = *p_token_target_;\n"
                    "\n",
                    get_token_data(rule.left).name.c_str(), rule.init.c_str(),  get_token_data(rule.left).name.c_str()
                );
        }else{
            for(size_t i = 0;i<rule.rights.size();++i)
                fprintf(out,IND6 "Any_Token& token%zu_ = stack_[start_+%zu].token;\n", i,i);
            fprintf(out,"\n");
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
            for(size_t i = rule.rights.size();i!=0;--i)
                fprintf(out,IND6 "stack_.pop_back();\n");
        }
        fprintf(out,"\n");
        if(rule.left==(Token_Kind_t)-1){
            if(!sp_token)
                fprintf(out,IND6 "token_ahead_.static_destroy_deallocate<Token::EOF_>();\n\n");
            fprintf(out,IND6 "return 0;\n" IND5 "}\n");
        }else{
            fprintf(out,
                IND6 "if(!stack_.empty()){\n"
                IND6 IND "state_ = goto_table(stack_.back().state,Token_Kind::%s);\n"
                IND6 "}else{\n"
                IND6 IND "state_ = goto_table(0,Token_Kind::%s);\n"
                IND6 "}\n"
                "\n",
                get_token_data(rule.left).name.c_str(), get_token_data(rule.left).name.c_str()
            );
            if(!sp_token)
                fprintf(out,IND6 "stack_.push_back({Any_Token(p_token_target_),state_});\n\n");
            else
                fprintf(out,IND6 "stack_.push_back({std::move(token_target_),state_});\n\n");
            fprintf(out,IND6 "goto loop_end_;\n" IND5 "}\n");
        }
    }
    fprintf(out,
        IND5 "default : return 2;\n"
        IND4 "}\n"
        IND3 "}\n"
        IND3 "case ypg::LR1_Action_Kind::ERR : {\n"
        IND4 "yuki::print_error(stderr,\"Syntax Error!\\n\");\n"
        IND4 "reset();\n"
        IND4 "yuki::print_error(stderr,\"Stack Clear!\\n\");\n"
        IND4 "return 1;\n"
        IND3 "}\n"
        IND2 "}\n"
        IND2 "loop_end_:;\n"
        IND "}\n"
        "}\n"
    );
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
        "struct %s %s : yuki::pg::AbsLR1Parser<%s,%s,%zu,%zu>{\n"
        IND "typedef yuki::pg::AbsLR1Parser<%s,%s,%zu,%zu> Base_Parser;\n"
        "\n"
        IND "static constinit Action_Table action_table;\n"
        IND "static constinit Goto_Table goto_table;\n"
        "\n"
        "%s"
        "\n"
        IND "virtual int parse() override %s;\n"
        "\n",
        cmd_data.parser.c_str(), cmd_data.if_final_class ? "final" : "",
        cmd_data.ts.c_str(), cmd_data.lexer.c_str(), state_size, rules.size(),
        cmd_data.ts.c_str(), cmd_data.lexer.c_str(), state_size, rules.size(),
        !cmd_data.no_default_ctor ? IND "using Base_Parser::Base_Parser;\n" : "",
        cmd_data.if_final_function ? "final" : ""
    );
    try{
        fprintf(out_h,"%s\n",code_htable.at("class").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out_h,"\n};\n");
    if(!cmd_data.nspace.empty())
        fprintf(out_h,"} // namespace %s\n",cmd_data.nspace.c_str());
    try{
        fprintf(out_h,"%s\n",code_htable.at("h_end").c_str());
    }catch(const std::out_of_range&){}
}
template struct LR1_Writer<unsigned char>;
template struct LR1_Writer<unsigned short>;
template struct LR1_Writer<unsigned>;
template struct LR1_Writer<unsigned long>;
template struct LR1_Writer<unsigned long long>;
} // namespace yuki::pg