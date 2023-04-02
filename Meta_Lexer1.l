%option fast unicode nodefault noline
%option outfile="Meta_Lexer1.cpp" header_file="Meta_Lexer1.h"
%option namespace="yuki::pg" prefix="yuki_pg_meta1" lexer="Meta_Lexer1" lex="lex" token-type="int" templated_lexer="Meta_Lexer1<Token_Kind_t>"

%code_cpptop{
#include"cconfig"
#include<yuki/unicode/binary_properties.h>
#include<yuki/uchar.hpp>
}

%code_htop{
#include"common.hpp"
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
    Meta_Lexer1(FILE* const in,Sec0_Data&& sec0_data_p,std::ostream& os=std::cout) noexcept :
        AbstractBaseLexer(reflex::Input(in),os),
        sec0_data(std::move(sec0_data_p))
    {}
  public:
    static constexpr const char* ordinal[11] = {"zeroth","first","second","third","fourth","fifth","sixth","seventh","eighth","ninth","tenth"};
    static constexpr size_t ordinal_max = 2;
    static_assert(ordinal_max < sizeof(ordinal)/sizeof(const char*));

    typedef Sec0_Data::Token_Coordinate Token_Coordinate;

    Sec0_Data sec0_data;

    unsigned brace_level=0;
    unsigned rule_num = 0;

    Rule<Token_Kind_t> rule;
    Rule_Set<Token_Kind_t> rs;

    Token_Kind_t left_current = -1;
    Token_Kind_t token_current = -1;

    bool init_encountered = false;

    decltype(INITIAL) previous_state;

    std::string str_temp0;
    std::string str_temp1;
}

%x RIGHTS INIT_BRACE INIT_BRACKET CODE TOKEN0 TOKEN1 RULE_PREC RULE_RR SEC2
keychar    \R|\p{Zs}|\{|\}|%|:|\||;|\$|\"
non_keychar    [^{keychar}]
any    .|\R
id [A-Za-z_]\p{Word}*

%%
<INITIAL>{
Goal_    {
    rule.clear();
    left_current = -1;
}
(\".*\")|({id}+)    {
    str_temp0 = str();
    try{
        const Token_Coordinate co = sec0_data.token_htable.at(str_temp0);
        if(co.is_term){
            fprintf(stderr,"Fatal Error: Terminal name encountered when parsing lhs of a rule: %s\n",str_temp0.c_str());
            ++sec0_data.errors;
            exit(EXIT_FAILURE);
        }else{
            rule.clear();
            left_current = sec0_data.to_token_num<Token_Kind_t>(co);
        }
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing lhs of a rule: %s\n",str_temp0.c_str());
        ++sec0_data.errors;
        exit(EXIT_FAILURE);
    }
}
:    {start(RIGHTS);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
\%\%\p{Space}*    {
    // Make sure that the goal symbol is the first non-terminal, so that the accepting state has number 1.
    const typename Rule_Set<Token_Kind_t>::const_iterator it = rs.first_equiv_greater({(Token_Kind_t)-1,{},0});
    if(it==rs.end()){
        fprintf(stderr,"Warning: No goal symbol specified. Defaulted to the first non-terminal - %s - .\n",sec0_data.nterms[0].name.c_str());
        rs.emplace((Token_Kind_t)-1,std::initializer_list<Token_Kind_t>{0},(Token_Kind_t)0);
    }else{
        Token_Kind_t goal = (it->rights)[0];
        if(goal!=0){
            fprintf(stderr,"Fatal Error: Right hand side of goal production - %s - is not the same as the first non-terminal - %s - !\n",sec0_data.get_token_data(goal).name.c_str(),sec0_data.nterms[0].name.c_str());
            ++sec0_data.errors;
            exit(EXIT_FAILURE);
        }
    }
    // End.

    start(SEC2);
}
}



<RIGHTS>{
(\"[^\"]*\")|({id}+)    {
    str_temp0 = str();
    try{
        const Token_Coordinate co = sec0_data.token_htable.at(str_temp0);
        init_encountered = false;
        rule.left = left_current;
        rule.rights.push_back(sec0_data.to_token_num<Token_Kind_t>(co));
        if(co.is_term)
            rule.prec_sr = sec0_data.terms[co.idx].prec;
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing rhs of a rule: %s\n",str_temp0.c_str());
        ++sec0_data.errors;
        exit(EXIT_FAILURE);
    }
}
\%empty    {}
\{    {
    str_temp0.clear();
    brace_level = 0;
    if(!init_encountered){
        init_encountered = true;
        if(sec0_data.is_tuple())
            str_temp0.push_back('{');
        start(INIT_BRACE);
    }else{
        start(CODE);
    }
}
\(    {
    str_temp0.clear();
    brace_level = 0;
    init_encountered = true;
    if(sec0_data.is_tuple())
        str_temp0.push_back('(');
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
    start(INITIAL);
}
\%prec    {start(RULE_PREC);}
%rr    {start(RULE_RR);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}



<INIT_BRACE>{
\{    {
    ++brace_level;
    str_temp0.push_back('{');
}

\}    {
    if(brace_level!=0){
        --brace_level;
        str_temp0.push_back('}');
    }else{
        if(sec0_data.is_tuple())
            str_temp0.push_back('}');
        rule.init = std::move(str_temp0);
        start(RIGHTS);
    }
}

\$\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    ++sec0_data.errors;
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT:{
            if(sec0_data.get_token_data(left_current).types.empty()){
                fprintf(stderr,"Error: Token with no semantic value is named: %s\n",sec0_data.get_token_data(left_current).name.c_str());
                ++sec0_data.errors;
            }
            str_temp0.append("token_target_");
            break;
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token_target_.value"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE:{
            token_current = left_current;
            previous_state = INIT_BRACE;
            str_temp0.append("token_target_");
            start(TOKEN0);
            break;
        }
    }
}

\$[0-9]+    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT:{
            try{
                const Token_Kind_t k = rule.rights.at(strtoull(text()+1,nullptr,10));
                if(sec0_data.get_token_data(k).types.empty()){
                    fprintf(stderr,"Error: Token with no semantic value is named: %s\n",sec0_data.get_token_data(k).name.c_str());
                    ++sec0_data.errors;
                }
                str_temp0.append("token").append(text()+1,size()-1).push_back('_');
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,sec0_data.get_token_data(left_current).name.c_str(),rule.rights.size());
                ++sec0_data.errors;
                exit(EXIT_FAILURE);
            }
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token").append(text()+1,size()-1).append("_.value"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE:{
            try{
                token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
                previous_state = INIT_BRACE;
                str_temp0.append("token").append(text()+1,size()-1).push_back('_');
                start(TOKEN0);
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,sec0_data.get_token_data(left_current).name.c_str(),rule.rights.size());
                ++sec0_data.errors;
                exit(EXIT_FAILURE);
            }
        }
    }
}

\$!\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    ++sec0_data.errors;
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: str_temp0.append("(token_target_complete_.location_range())");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token_target_.location_range"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE: str_temp0.append("loc_target_"); break;
    }
}

\$![0-9]+    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: str_temp0.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token").append(text()+2,size()-2).append("_.location_range");break;
        case Sec0_Data::Token_Impl_Type::TUPLE: str_temp0.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
    }
}

{any}    {str_temp0.push_back(chr());}
}



<INIT_BRACKET>{
\(    {
    ++brace_level;
    str_temp0.push_back('(');
}

\)    {
    if(brace_level!=0){
        --brace_level;
        str_temp0.push_back(')');
    }else{
        if(sec0_data.is_tuple())
            str_temp0.push_back(')');
        rule.init = std::move(str_temp0);
        start(RIGHTS);
    }
}

\$\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    ++sec0_data.errors;
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT:{
            if(sec0_data.get_token_data(left_current).types.empty()){
                fprintf(stderr,"Error: Token with no semantic value is named: %s\n",sec0_data.get_token_data(left_current).name.c_str());
                ++sec0_data.errors;
            }
            str_temp0.append("token_target_");
            break;
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token_target_.value"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE:{
            token_current = left_current;
            previous_state = INIT_BRACKET;
            str_temp0.append("token_target_");
            start(TOKEN0);
            break;
        }
    }
}

\$[0-9]+    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT:{
            try{
                const Token_Kind_t k = rule.rights.at(strtoull(text()+1,nullptr,10));
                if(sec0_data.get_token_data(k).types.empty()){
                    fprintf(stderr,"Error: Token with no semantic value is named: %s\n",sec0_data.get_token_data(k).name.c_str());
                    ++sec0_data.errors;
                }
                str_temp0.append("token").append(text()+1,size()-1).push_back('_');
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,sec0_data.get_token_data(left_current).name.c_str(),rule.rights.size());
                ++sec0_data.errors;
                exit(EXIT_FAILURE);
            }
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token").append(text()+1,size()-1).append("_.value"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE:{
            try{
                token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
                previous_state = INIT_BRACKET;
                str_temp0.append("token").append(text()+1,size()-1).push_back('_');
                start(TOKEN0);
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,sec0_data.get_token_data(left_current).name.c_str(),rule.rights.size());
                ++sec0_data.errors;
                exit(EXIT_FAILURE);
            }
        }
    }
}

\$!\$    {
    fprintf(stderr,"Error: Target token being referred to in the target token initializer!\n");
    ++sec0_data.errors;
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: str_temp0.append("(token_target_complete_.location_range())");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token_target_.location_range"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE: str_temp0.append("loc_target_"); break;
    }
}

\$![0-9]+    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: str_temp0.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token").append(text()+2,size()-2).append("_.location_range");break;
        case Sec0_Data::Token_Impl_Type::TUPLE: str_temp0.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
    }
}

{any}    {str_temp0.push_back(chr());}
}



<CODE>{
\{    {
    ++brace_level;
    str_temp0.push_back('{');
}

\}    {
    if(brace_level!=0){
        --brace_level;
        str_temp0.push_back('}');
    }else{
        str_temp0.resize(yuki::remove_trailing_u8(str_temp0.begin(),str_temp0.end(),yuki::unicode::is_WSpace<yuki::U8Char>));
        rule.code = std::move(str_temp0);
        start(RIGHTS);
    }
}

\$\$    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT:{
            if(sec0_data.get_token_data(left_current).types.empty()){
                fprintf(stderr,"Error: Token with no semantic value is named: %s\n",sec0_data.get_token_data(left_current).name.c_str());
                ++sec0_data.errors;
            }
            str_temp0.append("token_target_");
            break;
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token_target_.value"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE:{
            token_current = left_current;
            previous_state = CODE;
            str_temp0.append("token_target_");
            start(TOKEN0);
            break;
        }
    }
}

\$[0-9]+    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT:{
            try{
                const Token_Kind_t k = rule.rights.at(strtoull(text()+1,nullptr,10));
                if(sec0_data.get_token_data(k).types.empty()){
                    fprintf(stderr,"Error: Token with no semantic value is named: %s\n",sec0_data.get_token_data(k).name.c_str());
                    ++sec0_data.errors;
                }
                str_temp0.append("token").append(text()+1,size()-1).push_back('_');
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,sec0_data.get_token_data(left_current).name.c_str(),rule.rights.size());
                ++sec0_data.errors;
                exit(EXIT_FAILURE);
            }
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token").append(text()+1,size()-1).append("_.value"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE:{
            try{
                token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
                previous_state = CODE;
                str_temp0.append("token").append(text()+1,size()-1).push_back('_');
                start(TOKEN0);
                break;
            }catch(const std::out_of_range&){
                fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,sec0_data.get_token_data(left_current).name.c_str(),rule.rights.size());
                ++sec0_data.errors;
                exit(EXIT_FAILURE);
            }
        }
    }
}

\$!\$    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: str_temp0.append("(token_target_complete_.location_range())");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token_target_.location_range"); break;
        case Sec0_Data::Token_Impl_Type::TUPLE: str_temp0.append("loc_target_"); break;
    }
}

\$![0-9]+    {
    switch(sec0_data.token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: str_temp0.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: str_temp0.append("token").append(text()+2,size()-2).append("_.location_range");break;
        case Sec0_Data::Token_Impl_Type::TUPLE: str_temp0.append("(stack_[start_+").append(text()+2,size()-2).append("].token.location_range())");break;
    }
}

{any}    {str_temp0.push_back(chr());}
}




<TOKEN0>{
\[    {str_temp1.clear();start(TOKEN1);}

(?=[^\[])    {start(previous_state);}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
}




<TOKEN1>{
{id}+    {str_temp1=str();}

\]    {
    size_t i = 0;
    const auto& names = sec0_data.get_token_data(token_current).names;
    for(;i!=names.size();++i){
        if(str_temp1==names[i])
            goto suc;
    }
    fprintf(stderr,"Fatal Error: Unknown token member name: \"%s\" in token \"%s\"\n",str_temp1.c_str(),sec0_data.get_token_data(token_current).name.c_str());
    ++sec0_data.errors;
    exit(EXIT_FAILURE);
    suc:
    str_temp0.push_back('.');
    if(i<ordinal_max)
        str_temp0.append(ordinal[i]);
    else
        str_temp0.append("get<").append(std::to_string(i)).append(">()");
    start(previous_state);
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
}



<RULE_PREC>{
[0-9]+    {rule.prec_sr = strtoull(text(),nullptr,10);start(RIGHTS);}

(\"[^\"]*\")|({id}+)    {
    str_temp0 = str();
    try{
        const Token_Coordinate co = sec0_data.token_htable.at(str_temp0);
        rule.prec_sr = sec0_data.get_token_data(co).prec;
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing %%prec of a rule: \"%s\" with lhs \"%s\"\n",str_temp0.c_str(),sec0_data.get_token_data(left_current).name.c_str());
        ++sec0_data.errors;
        exit(EXIT_FAILURE);
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
{any}+    {
    for(Sec0_Data::Code& code : sec0_data.codes)
        if(code.qualifier=="SEC2_")
            code.contents.append(text());
}
}


%%
namespace yuki::pg{
template struct Meta_Lexer1<unsigned char>;
template struct Meta_Lexer1<unsigned short>;
template struct Meta_Lexer1<unsigned>;
}
