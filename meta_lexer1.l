%option fast unicode nodefault noline
%option outfile="meta_lexer1.cpp" header_file="meta_lexer1.h"
%option namespace="yuki::pg" prefix="yuki_pg_meta1" lexer="Meta_Lexer1" lex="lex" token-type="int" templated_lexer="Meta_Lexer1<Token_Kind_t>"

%code_htop{
    #include"cconfig"
    #include"cmd.hpp"
    #include"meta_lexer0.h"
}

%code_hafter{
    namespace yuki::pg{
        extern template struct Meta_Lexer1<unsigned char>;
        extern template struct Meta_Lexer1<unsigned short>;
        extern template struct Meta_Lexer1<unsigned>;
    }
}

%code_templateclass{
    template<std::unsigned_integral Token_Kind_t>
}

%code_class{
  public:
    Meta_Lexer1(Meta_Lexer0&& ml0,std::ostream& os=std::cout) noexcept :
        AbstractBaseLexer(reflex::Input(ml0.cmd_data.fp_in),os),
        cmd_data(std::move(ml0.cmd_data)),
        options(std::move(ml0.options)),
        nterms(std::move(ml0.nterms)),
        terms(std::move(ml0.terms)),
        token_htable(std::move(ml0.token_htable)),
        code_htable(std::move(ml0.code_htable))
    {
        #ifndef YUKI_PG_RULE_CODE_RESERVE
        #define YUKI_PG_RULE_CODE_RESERVE 2048
        #endif
        str_temp1.reserve(YUKI_PG_RULE_CODE_RESERVE);
    }
  public:
    static constexpr const char* ordinal[11] = {"zeroth","first","second","third","fourth","fifth","sixth","seventh","eighth","ninth","tenth"};
    static constexpr size_t ordinal_max = 2;
    static_assert(ordinal_max < sizeof(ordinal)/sizeof(const char*));

    typedef Meta_Lexer0::Token_Coordinate Token_Coordinate;

    const yuki::pg::Cmd_Data cmd_data;

    const yuki::pg::Options options;

    unsigned brace_level=0;
    unsigned rule_num = 0;

    const yuki::Vector<Token_Data> nterms;
    const yuki::Vector<Token_Data> terms;

    const std::unordered_map<std::string,Token_Coordinate> token_htable;

    std::unordered_map<std::string,std::string> code_htable;

    Rule<Token_Kind_t> rule;
    Rule_Set<Token_Kind_t> rs;

    Token_Kind_t left_current = -1;
    Token_Kind_t token_current = -1;

    bool init_encountered = false;

    decltype(INITIAL) previous_state;

    std::string str_temp1;
    std::string str_temp2;

    const yuki::Vector<Token_Data>& get_token_table(bool is_term_p) const {
        return is_term_p ? terms : nterms;
    }
    const Token_Data& get_token_data(Token_Coordinate co) const {
        return (co.is_term ? terms : nterms)[co.idx];
    }
    const Token_Data& get_token_data(const std::string& s) const {
        return get_token_data(token_htable.at(s));
    }
    const Token_Data& get_token_data(Token_Kind_t k) const {
        static Token_Data td_goal_("Goal_",Assoc::RIGHT);
        if(k==(Token_Kind_t)-1)
            return td_goal_;
        return k>=nterms.size() ? terms[k-nterms.size()] : nterms[k];
    }
    Token_Kind_t to_token_num(Token_Coordinate co){
        return !co.is_term ? co.idx : nterms.size()+co.idx ;
    }
}

%x SEC1 RIGHTS INIT_BRACE INIT_BRACKET CODE TOKEN0 TOKEN1 RULE_PREC RULE_RR SEC2
keychar    \R|\p{Zs}|\{|\}|%|:|\||;|\$|\"
non_keychar    [^{keychar}]
any    .|\R
id [A-Za-z_]\p{Word}*

%%
<INITIAL>{
\%\%\p{Space}*    {start(SEC1);}
{any}    {}
}

<SEC1>{
Goal_    {
    rule.clear();
    left_current = -1;
}
(\".*\")|({id}+)    {
    str_temp1 = str();
    try{
        Token_Coordinate co = token_htable.at(str_temp1);
        if(co.is_term){
            fprintf(stderr,"Fatal Error: Terminal name encountered when parsing lhs of a rule: %s\n",str_temp1.c_str());
            std::exit(EXIT_FAILURE);
        }else{
            rule.clear();
            left_current = to_token_num(co);
        }
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing lhs of a rule: %s\n",str_temp1.c_str());
        std::exit(EXIT_FAILURE);
    }
}
:    {start(RIGHTS);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
\%\%\p{Space}*    {
    // Make sure that the goal symbol is the first non-terminal, so that the accepting state has number 1.
    typename Rule_Set<Token_Kind_t>::const_iterator it = rs.first_equiv_greater({(Token_Kind_t)-1,{},0});
    if(it==rs.end()){
        fprintf(stderr,"Warning: No goal symbol specified. Defaulted to the first non-terminal - %s - .\n",nterms[0].name.c_str());
        rs.emplace((Token_Kind_t)-1,std::initializer_list<Token_Kind_t>{0},(Token_Kind_t)0);
    }else{
        Token_Kind_t goal = (it->rights)[0];
        if(goal!=0){
            fprintf(stderr,"Fatal Error: Right hand side of goal production - %s - is not the same as the first non-terminal - %s - !\n",get_token_data(goal).name.c_str(),nterms[0].name.c_str());
            std::exit(EXIT_FAILURE);
        }
    }
    // End.

    start(SEC2);
}
}



<RIGHTS>{
(\"[^\"]*\")|({id}+)    {
    str_temp1 = str();
    try{
        Token_Coordinate co = token_htable.at(str_temp1);
        init_encountered = false;
        rule.left = left_current;
        rule.rights.push_back(to_token_num(co));
        if(co.is_term)
            rule.prec_sr = terms[co.idx].prec;
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing rhs of a rule: %s\n",str_temp1.c_str());
        std::exit(EXIT_FAILURE);
    }
}
\%empty    {}
\{    {
    str_temp1.clear();
    brace_level = 0;
    if(!init_encountered){
        init_encountered = true;
        if(options.is_tuple())
            str_temp1.push_back('{');
        start(INIT_BRACE);
    }else{
        start(CODE);
    }
}
\(    {
    str_temp1.clear();
    brace_level = 0;
    init_encountered = true;
    if(options.is_tuple())
        str_temp1.push_back('(');
    start(INIT_BRACKET);
}
\|    {
    rule.num = left_current!=(Token_Kind_t)-1 ? ++rule_num : 0;
    rs.emplace(std::move(rule));
    rule.clear();
    init_encountered = false;
}
;    {
    rule.num = left_current!=(Token_Kind_t)-1 ? ++rule_num : 0;
    rs.emplace(std::move(rule));
    rule.clear();
    init_encountered = false;
    left_current = -1;
    start(SEC1);
}
\%prec    {start(RULE_PREC);}
%rr    {start(RULE_RR);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}



<INIT_BRACE>{
\{    {
    ++brace_level;
    str_temp1.push_back('{');
}

\}    {
    if(brace_level!=0){
        --brace_level;
        str_temp1.push_back('}');
    }else{
        if(options.is_tuple())
            str_temp1.push_back('}');
        rule.init = std::move(str_temp1);
        start(RIGHTS);
    }
}

\$\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            if(get_token_data(left_current).types.empty())
                fprintf(stderr,"Error: Token with no semantic value is named: %s\n",get_token_data(left_current).name.c_str());
            str_temp1.append("token_target_");
            break;
        }
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token_target_.value"); break;
        case Options::Token_Impl_Type::TUPLE:{
            token_current = left_current;
            previous_state = INIT_BRACE;
            str_temp1.append("token_target_");
            start(TOKEN0);
            break;
        }
    }
}

\$[0-9]+    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            try{
                const Token_Kind_t k = rule.rights.at(strtoull(text()+1,nullptr,10));
                if(get_token_data(k).types.empty())
                    fprintf(stderr,"Error: Token with no semantic value is named: %s\n",get_token_data(k).name.c_str());
                str_temp1.append("token").append(text()+1,size()-1).push_back('_');
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
                std::exit(EXIT_FAILURE);
            }
        }
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token").append(text()+1,size()-1).append("_.value"); break;
        case Options::Token_Impl_Type::TUPLE:{
            try{
                token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
                previous_state = INIT_BRACE;
                str_temp1.append("token").append(text()+1,size()-1).push_back('_');
                start(TOKEN0);
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
                std::exit(EXIT_FAILURE);
            }
        }
    }
}

\$!\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT: str_temp1.append("(token_target_complete_.location_range())");break;
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token_target_.location_range"); break;
        case Options::Token_Impl_Type::TUPLE: str_temp1.append("loc_target_"); break;
    }
}

\$![0-9]+    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT: str_temp1.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token").append(text()+2,size()-2).append("_.location_range");break;
        case Options::Token_Impl_Type::TUPLE: str_temp1.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
    }
}

{any}    {str_temp1.push_back(chr());}
}



<INIT_BRACKET>{
\(    {
    ++brace_level;
    str_temp1.push_back('(');
}

\)    {
    if(brace_level!=0){
        --brace_level;
        str_temp1.push_back(')');
    }else{
        if(options.is_tuple())
            str_temp1.push_back(')');
        rule.init = std::move(str_temp1);
        start(RIGHTS);
    }
}

\$\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            if(get_token_data(left_current).types.empty())
                fprintf(stderr,"Error: Token with no semantic value is named: %s\n",get_token_data(left_current).name.c_str());
            str_temp1.append("token_target_");
            break;
        }
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token_target_.value"); break;
        case Options::Token_Impl_Type::TUPLE:{
            token_current = left_current;
            previous_state = INIT_BRACKET;
            str_temp1.append("token_target_");
            start(TOKEN0);
            break;
        }
    }
}

\$[0-9]+    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            try{
                const Token_Kind_t k = rule.rights.at(strtoull(text()+1,nullptr,10));
                if(get_token_data(k).types.empty())
                    fprintf(stderr,"Error: Token with no semantic value is named: %s\n",get_token_data(k).name.c_str());
                str_temp1.append("token").append(text()+1,size()-1).push_back('_');
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
                std::exit(EXIT_FAILURE);
            }
        }
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token").append(text()+1,size()-1).append("_.value"); break;
        case Options::Token_Impl_Type::TUPLE:{
            try{
                token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
                previous_state = INIT_BRACKET;
                str_temp1.append("token").append(text()+1,size()-1).push_back('_');
                start(TOKEN0);
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
                std::exit(EXIT_FAILURE);
            }
        }
    }
}

\$!\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT: str_temp1.append("(token_target_complete_.location_range())");break;
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token_target_.location_range"); break;
        case Options::Token_Impl_Type::TUPLE: str_temp1.append("loc_target_"); break;
    }
}

\$![0-9]+    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT: str_temp1.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token").append(text()+2,size()-2).append("_.location_range");break;
        case Options::Token_Impl_Type::TUPLE: str_temp1.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
    }
}

{any}    {str_temp1.push_back(chr());}
}



<CODE>{
\{    {
    ++brace_level;
    str_temp1.push_back('{');
}

\}    {
    if(brace_level!=0){
        --brace_level;
        str_temp1.push_back('}');
    }else{
        yuki::trim_trailing_spaces(str_temp1);
        rule.code = std::move(str_temp1);
        start(RIGHTS);
    }
}

\$\$    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            if(get_token_data(left_current).types.empty())
                fprintf(stderr,"Error: Token with no semantic value is named: %s\n",get_token_data(left_current).name.c_str());
            str_temp1.append("token_target_");
            break;
        }
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token_target_.value"); break;
        case Options::Token_Impl_Type::TUPLE:{
            token_current = left_current;
            previous_state = CODE;
            str_temp1.append("token_target_");
            start(TOKEN0);
            break;
        }
    }
}

\$[0-9]+    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            try{
                const Token_Kind_t k = rule.rights.at(strtoull(text()+1,nullptr,10));
                if(get_token_data(k).types.empty())
                    fprintf(stderr,"Error: Token with no semantic value is named: %s\n",get_token_data(k).name.c_str());
                str_temp1.append("token").append(text()+1,size()-1).push_back('_');
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
                std::exit(EXIT_FAILURE);
            }
        }
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token").append(text()+1,size()-1).append("_.value"); break;
        case Options::Token_Impl_Type::TUPLE:{
            try{
                token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
                previous_state = CODE;
                str_temp1.append("token").append(text()+1,size()-1).push_back('_');
                start(TOKEN0);
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
                std::exit(EXIT_FAILURE);
            }
        }
    }
}

\$!\$    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT: str_temp1.append("(token_target_complete_.location_range())");break;
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token_target_.location_range"); break;
        case Options::Token_Impl_Type::TUPLE: str_temp1.append("loc_target_"); break;
    }
}

\$![0-9]+    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT: str_temp1.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
        case Options::Token_Impl_Type::SIMPLE: str_temp1.append("token").append(text()+2,size()-2).append("_.location_range");break;
        case Options::Token_Impl_Type::TUPLE: str_temp1.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
    }
}

{any}    {str_temp1.push_back(chr());}
}




<TOKEN0>{
\[    {str_temp2.clear();start(TOKEN1);}

(?=[^\[])    {start(previous_state);}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
}




<TOKEN1>{
{id}+    {str_temp2=str();}

\]    {
    size_t i = 0;
    const auto& names = get_token_data(token_current).names;
    for(;i!=names.size();++i){
        if(str_temp2==names[i])
            goto suc;
    }
    fprintf(stderr,"Fatal Error: Unknown token member name: \"%s\" in token \"%s\"\n",str_temp2.c_str(),get_token_data(token_current).name.c_str());
    std::exit(EXIT_FAILURE);
    suc:
    str_temp1.push_back('.');
    if(i<ordinal_max)
        str_temp1.append(ordinal[i]);
    else
        str_temp1.append("get<").append(std::to_string(i)).append(">()");
    start(previous_state);
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
}



<RULE_PREC>{
[0-9]+    {rule.prec_sr = strtoull(text(),nullptr,10);start(RIGHTS);}

(\"[^\"]*\")|({id}+)    {
    str_temp1 = str();
    try{
        Token_Coordinate co = token_htable.at(str_temp1);
        rule.prec_sr = get_token_data(co).prec;
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing %%prec of a rule: \"%s\" with lhs \"%s\"\n",str_temp1.c_str(),get_token_data(left_current).name.c_str());
        std::exit(EXIT_FAILURE);
    }
    start(RIGHTS);
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
}



<RULE_RR>{
[0-9]+    {rule.prec_rr = strtoull(text(),nullptr,10);start(RIGHTS);}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
}


<SEC2>{
{any}+    {code_htable.insert_or_assign("SEC2_",str());}
}


%%
namespace yuki::pg{
template struct Meta_Lexer1<unsigned char>;
template struct Meta_Lexer1<unsigned short>;
template struct Meta_Lexer1<unsigned>;
}