%option fast unicode nodefault noline
%option outfile="meta_lexer0.cpp" header-file="meta_lexer0.h"
%option namespace="yuki::pg" prefix="yuki_pg_meta0" lexer="Meta_Lexer0" lex="lex" token-type="int"

%code_htop{
    #include"cconfig"
    #include"common.hpp"
    #include"cmd.hpp"

    class Meta_Lexer1;
    namespace yuki::pg{

    }
}

%code_cpptop{
    #include<fmt/core.h>
}

%code_class{
  public:
    friend Meta_Lexer1;

    #ifndef YUKI_PG_TOKEN_HTABLE_BUCKET
    #define YUKI_PG_TOKEN_HTABLE_BUCKET 1024
    #endif
    Meta_Lexer0(const yuki::pg::Cmd_Data& cmd_data_p,std::ostream& os=std::cout) noexcept :
        AbstractBaseLexer(reflex::Input(cmd_data_p.fp_in),os),
        cmd_data(&cmd_data_p),
        token_htable(YUKI_PG_TOKEN_HTABLE_BUCKET),
        code_htable(8)
    {
        nterm_table.reserve(YUKI_PG_TOKEN_HTABLE_BUCKET);
        term_table.reserve(YUKI_PG_TOKEN_HTABLE_BUCKET);
        token_htable.max_load_factor(std::numeric_limits<float>::max()); // Inhibits rehashing anyway.
        code_htable.max_load_factor(std::numeric_limits<float>::max());
    }
  public:
    const yuki::pg::Cmd_Data* cmd_data;

    unsigned brace_level=0;

    bool token_declared = false;

    bool is_term = true;

    bool assoc_has_args = false;

    std::vector<Token_Data> nterm_table;
    std::vector<Token_Data> term_table;

    std::unordered_map<std::string,yuki::Pair<bool,size_t>> token_htable;

    std::string str_temp1;

    prec_t current_prec = 0;
    Assoc current_assoc = Assoc::RIGHT;
    Assoc assoc_zero = Assoc::RIGHT;

    std::unordered_map<std::string,std::string> code_htable;
    std::string code_qualifier;

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
}

%x TERM TOKEN_DEF ALLOC0 ALLOC1 PREC ASSOC CODE0 CODE1

keychar    \p{Space}|\{|\}|\%|:|\||;|\$|\"
non_keychar    [^{keychar}]
any    .|\R
id [A-Za-z_]\p{Word}*

%%
<INITIAL>{
\%\%    {/*write_parser_common(cmd_data,token_table);*/return 1;}

\%nterm    {token_declared = false; is_term = false; start(TERM);}

\%term    {token_declared = false; is_term = true; start(TERM);}

\%prec    {++current_prec; start(PREC);}

\%left    {
    ++current_prec;
    current_assoc = Assoc::LEFT;
    assoc_has_args = false;
    start(ASSOC);
}

\%right    {
    ++current_prec;
    current_assoc = Assoc::RIGHT;
    assoc_has_args = false;
    start(ASSOC);
}

\%code    {
    start(CODE0);
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*    {} // Comments.
}



<TERM>{
\".*\"    {
    if(!token_declared)
        fprintf(stderr,"Error: Alias declaration precedes token name declaration: %s\n",text());
    else{
        str_temp1 = str();
        get_token_table(is_term).back().alias = str_temp1;
        token_htable.insert_or_assign(std::move(str_temp1),yuki::Pair<bool,size_t>{is_term,get_token_table(is_term).size()-1});
    }
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

{id}+    {
    token_declared = true;
    str_temp1 = str();
    token_htable.insert_or_assign(str_temp1,yuki::Pair<bool,size_t>{is_term,get_token_table(is_term).size()});
    get_token_table(is_term).emplace_back(std::move(str_temp1));
}

\{    {brace_level=0;str_temp1.clear();start(TOKEN_DEF);}

\/\/.*    {} // Comments.
}



<TOKEN_DEF>{
\{    {++brace_level;str_temp1.push_back('{');}

\}    {
    if(brace_level!=0){
        --brace_level;
        str_temp1.push_back('}');
    }else{
        start(ALLOC0);
    }
}

:    {get_token_table(is_term).back().types.push_back(std::move(str_temp1));}

;    {get_token_table(is_term).back().names.push_back(std::move(str_temp1));}

\p{Space}+    {} // Spaces as well as newlines are ignored.

{any}    {str_temp1.push_back(chr());}

::    {str_temp1.push_back(':');str_temp1.push_back(':');}
}




<ALLOC0>{
[0-9]+    {
    std::string& a = get_token_table(is_term).back().alloc;
    a.assign("yuki::Array_Allocator<Token::");
    a.append(get_token_table(is_term).back().name).push_back(',');
    a.append(text(),size()).push_back('>');
}

\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*    {} // Comments.

\%    {
    fprintf(stderr,"Fatal Error: %% encountered while scanning token allocator number of %s. Did you forget the ; ?\n",get_token_table(is_term).back().name.c_str());
    std::exit(EXIT_FAILURE);
}

;    {
    std::string& a = get_token_table(is_term).back().alloc;
    if(a.empty()){
        a.assign("yuki::Array_Allocator<Token::");
        a.append(get_token_table(is_term).back().name).push_back('>');
    }
    start(INITIAL);
}

[^0-9]    {
    get_token_table(is_term).back().alloc.push_back(chr());
    start(ALLOC1);
}
}



<ALLOC1>{
\n    {
    fprintf(stderr,"Warning: Newline encountered while scanning token allocator name of %s. Did you forget the ; ?\n",get_token_table(is_term).back().name.c_str());
    get_token_table(is_term).back().alloc.push_back(chr());
}
;    {
    std::string& a = get_token_table(is_term).back().alloc;
    if(a.empty()){
        a.assign("yuki::Array_Allocator<Token::");
        a.append(get_token_table(is_term).back().name).push_back('>');
    }
    start(INITIAL);
}
.    {
    get_token_table(is_term).back().alloc.push_back(chr());
}
\%\%    {
    fprintf(stderr,"Fatal Error: %%%% encountered while scanning token allocator name of %s. Did you forget the ; ?\n",get_token_table(is_term).back().name.c_str());
    std::exit(EXIT_FAILURE);
}
}



<PREC>{
(\".*\")|({id}+)    {
    str_temp1 = str();
    try{
        yuki::Pair<bool,size_t> pair = token_htable.at(str_temp1);
        if(!pair.zeroth)
            fprintf(stderr,"Warning: %%prec for non-terminals makes no sense: %s\n",str_temp1.c_str());
        else
            get_token_data(pair.zeroth,pair.first).prec = current_prec;
    }catch(const std::out_of_range&){
        fprintf(stderr,"Error: Unknown token name encountered while parsing %%prec declaration: %s\n",str_temp1.c_str());
    }
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*    {} // Comments.

(?=%)    {start(INITIAL);}
}



<ASSOC>{
(\".*\")|({id}+)    {
    str_temp1 = str();
    try{
        yuki::Pair<bool,size_t> pair = token_htable.at(str_temp1);
        if(!pair.zeroth)
            fprintf(stderr,"Warning: %%left/%%right for non-terminals makes no sense: %s\n",str_temp1.c_str());
        else{
            Token_Data& td = get_token_data(pair.zeroth,pair.first);
            td.prec = current_prec;
            assoc_has_args = true;
            td.assoc = current_assoc;
        }
    }catch(const std::out_of_range&){
        fprintf(stderr,"Error: Unknown token name encountered while parsing %%left/%%right declaration: %s\n",str_temp1.c_str());
    }
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*    {} // Comments.

(?=%)    {
    if(assoc_has_args==false){
        assoc_zero = current_assoc;
        --current_prec;
    }
    start(INITIAL);
}
}



<CODE0>{
{id}+    {code_qualifier=str();code_htable.insert_or_assign(code_qualifier,std::string{});}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\{    {
    brace_level=0;
    str_temp1.clear();
    #ifndef YUKI_PG_CODE_RESERVE
    #define YUKI_PG_CODE_RESERVE 2048
    #endif
    str_temp1.reserve(YUKI_PG_CODE_RESERVE);
    start(CODE1);
}
}




<CODE1>{
\}    {
    if(brace_level!=0){
        --brace_level;
        str_temp1.push_back('}');
    }else{
        code_htable[code_qualifier] = std::move(str_temp1);
        start(INITIAL);
    }
}

{any}    {str_temp1.push_back(chr());}

}

%%
namespace yuki::pg{
    /*void write_parser_common(FILE* fp,const std::vector<Token_Data>& nterms,const std::vector<Token_Data>& terms){

    }*/
}