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
        extern template struct Meta_Lexer1<unsigned long>;
        extern template struct Meta_Lexer1<unsigned long long>;
    }
}

%code_templateclass{
    template<std::unsigned_integral Token_Kind_t>
}

%code_class{
  public:
    Meta_Lexer1(Meta_Lexer0&& ml0,std::ostream& os=std::cout) noexcept :
        AbstractBaseLexer(reflex::Input(ml0.cmd_data->fp_in),os),
        cmd_data(ml0.cmd_data),
        nterm_table(std::move(ml0.nterm_table)),
        term_table(std::move(ml0.term_table)),
        token_htable(std::move(ml0.token_htable)),
        code_htable(std::move(ml0.code_htable))
    {}
  public:
    static constexpr const char* ordinal[11] = {"zeroth","first","second","third","fourth","fifth","sixth","seventh","eighth","ninth","tenth"};
    static constexpr size_t ordinal_max = 2;
    static_assert(ordinal_max <= sizeof(ordinal)/sizeof(const char*));

    const yuki::pg::Cmd_Data* cmd_data;

    unsigned brace_level=0;
    unsigned rule_num = 0;

    std::vector<Token_Data> nterm_table;
    std::vector<Token_Data> term_table;

    std::unordered_map<std::string,yuki::Pair<bool,size_t>> token_htable;

    std::unordered_map<std::string,std::string> code_htable;

    Token_Kind_t left_current = -1;
    Token_Kind_t token_current = -1;

    bool init_encountered = false;

    long long previous_state;

    Rule<Token_Kind_t> rule;
    Rule_Set<Token_Kind_t> rs;

    std::string str_temp1;
    std::string str_temp2;

    std::vector<Token_Data>& get_token_table(bool is_term_p){
        return is_term_p ? term_table : nterm_table;
    }
    Token_Data& get_token_data(bool is_term_p,size_t idx){
        return (is_term_p ? term_table : nterm_table)[idx];
    }
    Token_Data& get_token_data(const std::string& s){
        yuki::Pair<bool,size_t> p = token_htable[s];
        return get_token_data(p.zeroth,p.first);
    }
    Token_Data& get_token_data(Token_Kind_t k){
        static Token_Data td_goal_("Goal_");
        if(k==(Token_Kind_t)-1)
            return td_goal_;
        return k>=nterm_table.size() ? term_table[k-nterm_table.size()] : nterm_table[k];
    }
    Token_Kind_t get_token_num(bool is_term_p,size_t idx){
        return !is_term_p ? idx : nterm_table.size()+idx ;
    }
}

%x SEC1 RIGHTS INIT_BRACE INIT_BRACKET CODE TOKEN0 TOKEN1 RULE_PREC RULE_RR SEC2
keychar    \R|\p{Zs}|\{|\}|%|:|\||;|\$|\"
non_keychar    [^{keychar}]
any    .|\R
id [A-Za-z_]\p{Word}*

%%
<INITIAL>{
\%\%    {start(SEC1);}
{any}    {}
}

<SEC1>{
(\".*\")|({id}+)    {
    str_temp1 = str();
    try{
        yuki::Pair<bool,size_t> pair = token_htable.at(str_temp1);
        if(pair.zeroth){
            fprintf(stderr,"Fatal Error: Terminal name encountered when parsing lhs of a rule: %s\n",str_temp1.c_str());
            std::exit(EXIT_FAILURE);
        }else{
            rule.clear();
            left_current = get_token_num(pair.zeroth,pair.first);
        }
    }catch(const std::out_of_range&){
        if(str_temp1!="Goal_"){
            fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing lhs of a rule: %s\n",str_temp1.c_str());
            std::exit(EXIT_FAILURE);
        }else{
            rule.clear();
            left_current = -1;
        }
    }
}
:    {start(RIGHTS);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*    {} // Comments.
\%\%\p{Space}*    {start(SEC2);}
}



<RIGHTS>{
(\"[^\"]*\")|({id}+)    {
    str_temp1 = str();
    try{
        yuki::Pair<bool,size_t> pair = token_htable.at(str_temp1);
        init_encountered = false;
        rule.left = left_current;
        rule.rights.push_back(get_token_num(pair.zeroth,pair.first));
    }catch(const std::out_of_range&){
            fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing rhs of a rule: %s\n",str_temp1.c_str());
            std::exit(EXIT_FAILURE);
    }
}
\{    {
    str_temp1.clear();
    brace_level = 0;
    if(!init_encountered){
        init_encountered = true;
        str_temp1.push_back('{');
        start(INIT_BRACE);
    }else{
        #ifndef YUKI_PG_RULE_CODE_RESERVE
        #define YUKI_PG_RULE_CODE_RESERVE 2048
        #endif
        str_temp1.reserve(YUKI_PG_RULE_CODE_RESERVE);
        start(CODE);
    }
}
\(    {
    str_temp1.clear();
    brace_level = 0;
    init_encountered = true;
    str_temp1.push_back('(');
    start(INIT_BRACKET);
}
\|    {
    rule.num = left_current!=(Token_Kind_t)-1 ? ++rule_num : 0;
    rs.insert_equal(std::move(rule));
    rule.clear();
    init_encountered = false;
}
;    {
    rule.num = left_current!=(Token_Kind_t)-1 ? ++rule_num : 0;
    rs.insert_equal(std::move(rule));
    rule.clear();
    init_encountered = false;
    left_current = -1;
    start(SEC1);
}
\%prec    {start(RULE_PREC);}
%rr    {start(RULE_RR);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*    {} // Comments.
}



<INIT_BRACE>{
\{    {
    ++brace_level;
    str_temp1.push_back('{');
}

\}    {
    str_temp1.push_back('}');
    if(brace_level!=0)
        --brace_level;
    else{
        rule.init = std::move(str_temp1);
        start(RIGHTS);
    }
}

\$\$    {
    token_current = left_current;
    previous_state = INIT_BRACE;
    str_temp1.append("token_target_");
    start(TOKEN0);
}
\$[0-9]+    {
    try{
        token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
        previous_state = INIT_BRACE;
        str_temp1.append("token").append(text()+1,size()-1).push_back('_');
        start(TOKEN0);
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
        std::exit(EXIT_FAILURE);
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
    str_temp1.push_back(')');
    if(brace_level!=0)
        --brace_level;
    else{
        rule.init = std::move(str_temp1);
        start(RIGHTS);
    }
}

\$\$    {
    token_current = left_current;
    previous_state = INIT_BRACKET;
    str_temp1.append("token_target_");
    start(TOKEN0);
}
\$[0-9]+    {
    try{
        token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
        previous_state = INIT_BRACKET;
        str_temp1.append("token").append(text()+1,size()-1).push_back('_');
        start(TOKEN0);
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
        std::exit(EXIT_FAILURE);
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
        rule.code = std::move(str_temp1);
        start(RIGHTS);
    }
}

\$\$    {
    token_current = left_current;
    previous_state = CODE;
    str_temp1.append("token_target_");
    start(TOKEN0);
}
\$[0-9]+    {
    try{
        token_current = rule.rights.at(strtoull(text()+1,nullptr,10));
        previous_state = CODE;
        str_temp1.append("token").append(text()+1,size()-1).push_back('_');
        start(TOKEN0);
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Token index out of range: %s with %s.rights.size()=%zu\n",text()+1,get_token_data(left_current).name.c_str(),rule.rights.size());
        std::exit(EXIT_FAILURE);
    }
}

{any}    {str_temp1.push_back(chr());}
}




<TOKEN0>{
\[    {str_temp2.clear();start(TOKEN1);}

(?=[^\[])    {start(previous_state);}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*    {} // Comments.
}




<TOKEN1>{
{id}+    {str_temp2=str();}

\]    {
    size_t i = 0;
    const std::vector<std::string>& vec = get_token_data(token_current).names;
    for(;i!=vec.size();++i){
        if(str_temp2==vec[i])
            goto suc;
    }
    fprintf(stderr,"Fatal Error: Unknown token member name: %s in token %s\n",str_temp2.c_str(),get_token_data(token_current).name.c_str());
    std::exit(EXIT_FAILURE);
    suc:
    str_temp1.push_back('.');
    if(i<=ordinal_max)
        str_temp1.append(ordinal[i]);
    else
        str_temp1.append("get<").append(std::to_string(i)).append(">()");
    start(previous_state);
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*    {} // Comments.
}



<RULE_PREC>{
[0-9]+    {rule.prec_sr = strtoull(text(),nullptr,10);start(RIGHTS);}

(\"[^\"]*\")|({id}+)    {
    str_temp1 = str();
    try{
        yuki::Pair<bool,size_t> pair = token_htable.at(str_temp1);
        rule.prec_sr = get_token_data(pair.zeroth,pair.first).prec;
    }catch(const std::out_of_range&){
        fprintf(stderr,"Fatal Error: Unknown token name encountered when parsing %%prec of a rule: %s with lhs %s\n",str_temp1.c_str(),get_token_data(left_current).name.c_str());
        std::exit(EXIT_FAILURE);
    }
    start(RIGHTS);
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*    {} // Comments.
}



<RULE_RR>{
[0-9]+    {rule.prec_rr = strtoull(text(),nullptr,10);start(RIGHTS);}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*    {} // Comments.
}


<SEC2>{
{any}+    {
    #ifndef YUKI_PG_SEC2_RESERVE
    #define YUKI_PG_SEC2_RESERVE 4096
    #endif
    code_htable.insert_or_assign("SEC2_",str());
}
}


%%
namespace yuki::pg{
template struct Meta_Lexer1<unsigned char>;
template struct Meta_Lexer1<unsigned short>;
template struct Meta_Lexer1<unsigned>;
template struct Meta_Lexer1<unsigned long>;
template struct Meta_Lexer1<unsigned long long>;
}