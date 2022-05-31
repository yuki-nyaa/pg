%option fast unicode nodefault noline
%option outfile="meta_lexer0.cpp" header-file="meta_lexer0.h"
%option namespace="yuki::pg" prefix="yuki_pg_meta0" lexer="Meta_Lexer0" lex="lex" token-type="int"

%code_htop{
    #include"cconfig"
    #include"common.hpp"
    #include"cmd.hpp"

    class Meta_Lexer1;
    namespace yuki::pg{
        void write_token(const Cmd_Data&,const yuki::Vector<Token_Data>&,const yuki::Vector<Token_Data>&,const std::unordered_map<std::string,std::string>&);
    }
}

%code_cpptop{
    #include<fmt/core.h>
    #include<yuki/string_split.hpp>
}

%code_class{
  public:
    friend Meta_Lexer1;

    #ifndef YUKI_PG_TOKEN_HTABLE_BUCKET
    #define YUKI_PG_TOKEN_HTABLE_BUCKET 1024
    #endif
    Meta_Lexer0(yuki::pg::Cmd_Data& cmd_data_p,std::ostream& os=std::cout) noexcept :
        AbstractBaseLexer(reflex::Input(cmd_data_p.fp_in),os),
        cmd_data(&cmd_data_p),
        token_htable(YUKI_PG_TOKEN_HTABLE_BUCKET),
        code_htable(8)
    {
        token_htable.max_load_factor(std::numeric_limits<float>::max()); // Inhibits rehashing anyway.
        code_htable.max_load_factor(std::numeric_limits<float>::max());
    }
  public:
    yuki::pg::Cmd_Data* cmd_data;

    unsigned brace_level=0;

    bool token_declared = false;

    bool is_term = true;

    bool assoc_has_args = false;

    yuki::Vector<Token_Data> nterms{yuki::reserve_tag,YUKI_PG_TOKEN_HTABLE_BUCKET};
    yuki::Vector<Token_Data> terms{yuki::reserve_tag,YUKI_PG_TOKEN_HTABLE_BUCKET};

    std::unordered_map<std::string,yuki::Pair<bool,size_t>> token_htable;

    std::string str_temp1;

    prec_t current_prec = 0;
    Assoc current_assoc;
    Assoc assoc0 = Assoc::RIGHT;
    Assoc assoc_default = Assoc::RIGHT;

    std::unordered_map<std::string,std::string> code_htable;
    std::string code_qualifier;

    yuki::Vector<Token_Data>& get_token_table(bool is_term_p){
        return is_term_p ? terms : nterms;
    }
    Token_Data& get_token_data(bool is_term_p,size_t idx){
        return (is_term_p ? terms : nterms)[idx];
    }
    Token_Data& get_token_data(const std::string& s){
        yuki::Pair<bool,size_t> p = token_htable[s];
        return get_token_data(p.zeroth,p.first);
    }
}

%x TERM TOKEN_DEF ALLOC0 ALLOC1 PREC ASSOC CODE0 CODE1 O_SPTOKEN O_NSPACE O_PARSER O_TS O_LEXER

keychar    \p{Space}|\{|\}|\%|:|\||;|\$|\"
non_keychar    [^{keychar}]
any    .|\R
id [A-Za-z_]\p{Word}*

%%
<INITIAL>{
\%\%\p{Space}*    {
    terms.emplace_back("EOF_",assoc_default);
    write_token(*cmd_data,nterms,terms,code_htable);
    return 1;
}

\%nterm\p{Space}*    {token_declared = false; is_term = false; start(TERM);}

\%term\p{Space}*    {token_declared = false; is_term = true; start(TERM);}

\%prec\p{Space}*    {++current_prec; start(PREC);}

\%left\p{Space}*    {
    ++current_prec;
    current_assoc = Assoc::LEFT;
    assoc_has_args = false;
    start(ASSOC);
}

\%right\p{Space}*    {
    ++current_prec;
    current_assoc = Assoc::RIGHT;
    assoc_has_args = false;
    start(ASSOC);
}

\%default_left\p{Space}*    {
    assoc0 = assoc_default = Assoc::LEFT;
}

\%default_right\p{Space}*    {
    assoc0 = assoc_default = Assoc::RIGHT;
}

\%code\p{Space}*    {
    start(CODE0);
}

\%simple_token\p{Space}*    {
    str_temp1.clear();
    start(O_SPTOKEN);
}

\%namespace\p{Space}*    {
    start(O_NSPACE);
}

\%parser\p{Space}*    {
    start(O_PARSER);
}

\%token_settings\p{Space}*    {
    start(O_TS);
}

\%lexer\p{Space}*    {
    str_temp1.clear();
    start(O_LEXER);
}

\%no_final_function    {cmd_data->no_final_function = true;}
\%no_final_class    {cmd_data->no_final_class = true;}
\%no_default_ctor    {cmd_data->no_default_ctor = true;}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
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
    get_token_table(is_term).emplace_back(std::move(str_temp1),assoc_default);
}

\{\p{Space}*    {brace_level=0;str_temp1.clear();start(TOKEN_DEF);}

\/\/.*\R    {} // Comments.
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

:\p{Space}*    {
    yuki::trim_trailing_spaces(str_temp1);
    get_token_table(is_term).back().types.emplace_back(std::move(str_temp1));
}

;\p{Space}*    {
    yuki::trim_trailing_spaces(str_temp1);
    get_token_table(is_term).back().names.emplace_back(std::move(str_temp1));
}

\R    {str_temp1.push_back(' ');}

\p{Space}    {
    if(!str_temp1.empty())
        str_temp1.push_back(chr());
}

{any}    {str_temp1.push_back(chr());}

::    {str_temp1.push_back(':');str_temp1.push_back(':');}

\/\/.*\R    {} // Comments.
}




<ALLOC0>{
[0-9]+    {
    std::string& a = get_token_table(is_term).back().alloc;
    a.assign("yuki::Array_Allocator<Token::");
    a.append(get_token_table(is_term).back().name).push_back(',');
    a.append(text(),size()).push_back('>');
}

\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.

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
\R    {
    get_token_table(is_term).back().alloc.push_back(' ');
}
;    {
    std::string& a = get_token_table(is_term).back().alloc;
    if(a.empty()){
        a.assign("yuki::Array_Allocator<Token::");
        a.append(get_token_table(is_term).back().name).push_back('>');
    }else
        yuki::trim_trailing_spaces(a);
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
(\"[^\"]*\")|({id}+)    {
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

\/\/.*\R    {} // Comments.

(?=\%)    {start(INITIAL);}
}



<ASSOC>{
(\"[^\"]*\")|({id}+)    {
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

\/\/.*\R    {} // Comments.

(?=\%)    {
    if(assoc_has_args==false){
        assoc0 = current_assoc;
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
\{    {++brace_level;str_temp1.push_back('{');}
\}    {
    if(brace_level!=0){
        --brace_level;
        str_temp1.push_back('}');
    }else{
        yuki::trim_trailing_spaces(str_temp1);
        code_htable[code_qualifier] = std::move(str_temp1);
        start(INITIAL);
    }
}

{any}    {str_temp1.push_back(chr());}

}



<O_SPTOKEN>{
\/\/.*\R    {} // Comments.

\\\%    {str_temp1.push_back('%');}

(?=\%)    {
    if(str_temp1.empty())
        cmd_data->sp_token = "void";
    else{
        yuki::trim_trailing_spaces(str_temp1);
        cmd_data->sp_token = std::move(str_temp1);
    }
    start(INITIAL);
}
\R    {str_temp1.push_back(' ');}
.    {str_temp1.push_back(chr());}
}



<O_NSPACE>{
{id}+    {cmd_data->nspace.assign(text(),size());start(INITIAL);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}

<O_PARSER>{
{id}+    {cmd_data->parser.assign(text(),size());start(INITIAL);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}

<O_TS>{
{id}+    {cmd_data->ts.assign(text(),size());start(INITIAL);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}



<O_LEXER>{
\/\/.*\R    {} // Comments.

\\\%    {str_temp1.push_back('%');}

(?=\%)    {
    yuki::trim_trailing_spaces(str_temp1);
    cmd_data->lexer = std::move(str_temp1);
    start(INITIAL);
}
\R    {str_temp1.push_back(' ');}
.    {str_temp1.push_back(chr());}
}

%%
namespace yuki::pg{
static void write_token_non_simple(const Cmd_Data&,const yuki::Vector<Token_Data>&,const yuki::Vector<Token_Data>&);

void write_token(const Cmd_Data& cmd_data,const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,const std::unordered_map<std::string,std::string>& code_htable){
    #define IND YUKI_PG_IND
    #define IND2 IND IND
    #define IND3 IND IND IND
    #define IND4 IND IND IND IND
    FILE* const out = cmd_data.fp_out_token;
    fprintf(out,"#pragma once\n");
    try{
        fprintf(out,"%s\n",code_htable.at("token_top").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out,
        "#include<type_traits>\n"
        "#include<cassert>\n"
        "#include<yuki/tmp.hpp>\n"
        "#include<yuki/pg/core.hpp>\n"
        "%s"
        "\n",
        cmd_data.sp_token.empty() ? "#include<yuki/allocator.hpp>\n" : ""
    );
    if(!cmd_data.nspace.empty())
        fprintf(out,"namespace %s{\n",cmd_data.nspace.c_str());
    fprintf(out,"struct %s{\n",cmd_data.ts.c_str());
    fprintf(out,
        IND "static constexpr bool is_simple_token = %s;\n"
        "\n"
        IND "static constexpr size_t token_total = %zu;\n"
        IND "typedef yuki::uint_auto_t<token_total> Token_Kind_t;\n",
        cmd_data.sp_token.empty() ? "false" : "true",
        nterms.size()+terms.size()
    );
    fprintf(out,
        IND "struct Token_Kind{\n"
        IND2 "enum enum_t : Token_Kind_t {"
    );
    for(const Token_Data& td : nterms)
        fprintf(out,"%s,",td.name.c_str());
    for(const Token_Data& td : terms)
        fprintf(out,"%s,",td.name.c_str());
    fprintf(out,
        "};\n"
        IND "};\n"
    );
    fprintf(out,IND "const char* token_name[token_total] = {");
    for(const Token_Data& td : nterms){
        if(td.alias.empty())
            fprintf(out,"\"%s\",",td.name.c_str());
        else
            fprintf(out,"%s,",td.alias.c_str());
    }
    for(const Token_Data& td : terms){
        if(td.alias.empty())
            fprintf(out,"\"%s\",",td.name.c_str());
        else
            fprintf(out,"%s,",td.alias.c_str());
    }
    fprintf(out,"};\n\n");
    fprintf(out,
        IND "static constexpr Token_Kind_t nterminal_first = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t nterminal_last = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t terminal_first = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t terminal_last = Token_Kind::EOF_;\n"
        IND "static constexpr Token_Kind_t eof_ = Token_Kind::EOF_;\n"
        "\n",
        nterms[0].name.c_str(), nterms.back().name.c_str(), terms[0].name.c_str()
    );
    fprintf(out,
        IND "static constexpr size_t nterminal_total = %zu;\n"
        IND "static constexpr size_t terminal_total = %zu;\n"
        "\n",
        nterms.size(), terms.size()
    );
    fprintf(out,
        IND "static constexpr bool is_nterminal_f(Token_Kind_t kind) {return kind>=nterminal_first && kind<=nterminal_last;}\n"
        IND "static constexpr bool is_terminal_f(Token_Kind_t kind) {return kind>=terminal_first && kind<=terminal_last;}\n"
        "\n"
        IND "static constexpr size_t nterminal_kind_to_index(Token_Kind_t kind){\n"
        IND2 "assert(is_nterminal_f(kind));\n"
        IND2 "return kind-nterminal_first;\n"
        IND "}\n"
        "\n"
        IND "static constexpr size_t terminal_kind_to_index(Token_Kind_t kind){\n"
        IND2 "assert(is_terminal_f(kind));\n"
        IND2 "return kind-terminal_first;\n"
        IND "}\n"
        "\n"
    );
    if(!cmd_data.sp_token.empty()){
        fprintf(out,
            IND "using value_type = %s;\n"
            IND "typedef void Token;\n"
            "}; // struct %s\n",
            cmd_data.sp_token.c_str(),
            cmd_data.ts.c_str()
        );
        if(!cmd_data.nspace.empty())
            fprintf(out,"} // namespace %s\n",cmd_data.nspace.c_str());
    }else
        write_token_non_simple(cmd_data,nterms,terms);
    try{
        fprintf(out,"%s\n",code_htable.at("token_end").c_str());
    }catch(const std::out_of_range&){}
} // write_token


static void write_token_non_simple(const Cmd_Data& cmd_data,const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms){
    FILE* const out = cmd_data.fp_out_token;
    const char* const ts_cstr = cmd_data.ts.c_str();
    fprintf(out,
        IND "template<Token_Kind_t kind_p,typename... Ts>\n"
        IND "using Token_Tp = yuki::pg::Token<Token_Kind_t,kind_p,Ts...>;\n"
        "\n"
        IND "struct Token{\n"
    );
    for(const Token_Data& td : nterms){
        fprintf(out,IND2 "using %s = Token_Tp<Token_Kind::%s",td.name.c_str(),td.name.c_str());
        for(const std::string& type : td.types)
            fprintf(out,",%s",type.c_str());
        fprintf(out,">;\n");
    }
    for(const Token_Data& td : terms){
        fprintf(out,IND2 "using %s = Token_Tp<Token_Kind::%s",td.name.c_str(),td.name.c_str());
        for(const std::string& type : td.types)
            fprintf(out,",%s",type.c_str());
        fprintf(out,">;\n");
    }
    fprintf(out,IND "};\n\n");
    fprintf(out,
        IND "template<typename T> struct is_nterminal : std::false_type {};\n"
        IND "template<typename T> static constexpr bool is_nterminal_v = is_nterminal<T>::value;\n"
        IND "template<typename T> struct is_terminal : std::false_type {};\n"
        IND "template<typename T> static constexpr bool is_terminal_v = is_terminal<T>::value;\n"
        IND "template<typename T>\n"
        IND "struct is_token : std::conditional_t<is_nterminal_v<T> || is_terminal_v<T>,std::true_type,std::false_type> {};\n"
        IND "template<typename T>\n"
        IND "static constexpr bool is_token_v = is_nterminal_v<T> || is_terminal_v<T>;\n"
        "\n"
        IND "template<Token_Kind_t kind_p> struct kind_to_type {};\n"
        IND "template<Token_Kind_t kind_p> using kind_to_type_t = typename kind_to_type<kind_p>::type;\n"
        "\n"
        IND "template<typename T> struct type_to_alloc {typedef yuki::Array_Allocator<T> type;};\n"
        IND "template<typename T> using type_to_alloc_t = typename type_to_alloc<T>::type;\n"
        IND "struct Allocator;\n"
        "}; // struct %s\n"
        "\n",
        ts_cstr
    );
    for(const Token_Data& td : nterms)
        fprintf(out,"template<> struct %s::is_nterminal<%s::Token::%s> : std::true_type {};\n",ts_cstr,ts_cstr,td.name.c_str());
    fprintf(out,"\n");
    for(const Token_Data& td : terms)
        fprintf(out,"template<> struct %s::is_terminal<%s::Token::%s> : std::true_type {};\n",ts_cstr,ts_cstr,td.name.c_str());
    fprintf(out,"\n");
    for(const Token_Data& td : nterms)
        fprintf(out,"template<> struct %s::kind_to_type<%s::Token_Kind::%s> {typedef %s::Token::%s type;};\n",
            ts_cstr, ts_cstr, td.name.c_str(),
            ts_cstr, td.name.c_str()
        );
    for(const Token_Data& td : terms)
        fprintf(out,"template<> struct %s::kind_to_type<%s::Token_Kind::%s> {typedef %s::Token::%s type;};\n",
            ts_cstr, ts_cstr, td.name.c_str(),
            ts_cstr, td.name.c_str()
        );
    fprintf(out,"\n");
    fprintf(out,"struct %s::Allocator :\n",ts_cstr);
    for(const Token_Data& td : nterms)
        fprintf(out,IND "private type_to_alloc_t<Token::%s>,\n",td.name.c_str());
    for(size_t i = 0;i!=terms.size();++i){
        fprintf(out,IND "private type_to_alloc_t<Token::%s>",terms[i].name.c_str());
        if(i+1!=terms.size())
            fprintf(out,",");
        fprintf(out,"\n");
    }
    fprintf(out,
        "{\n"
        IND "template<typename T>\n"
        IND "auto allocate(size_t n=1) -> typename type_to_alloc_t<T>::pointer {return type_to_alloc_t<T>::allocate(n);}\n"
        "\n"
        IND "template<typename T>\n"
        IND "void deallocate(const yuki::pg::Token_Base* p,size_t n=1) {type_to_alloc_t<T>::deallocate(static_cast<const T*>(p),n);}\n"
        "\n"
        IND "template<typename T>\n"
        IND "void destroy_deallocate(const yuki::pg::Token_Base* p){\n"
        IND2 "if constexpr(!std::is_trivially_destructible_v<T>)\n"
        IND3 "static_cast<const T*>(p) -> ~T();\n"
        IND2 "type_to_alloc_t<T>::deallocate(static_cast<const T*>(p));\n"
        IND "}\n"
        "\n"
        IND "void dynamic_destroy_deallocate(Token_Kind_t k,const yuki::pg::Token_Base* p){\n"
        IND2 "using enum Token_Kind::enum_t;\n"
        IND2 "switch(k){\n"
    );
    for(const Token_Data& td : nterms)
        fprintf(out,IND3 "case %s : {destroy_deallocate<Token::%s>(p);break;}\n",td.name.c_str(),td.name.c_str());
    for(const Token_Data& td : terms)
        fprintf(out,IND3 "case %s : {destroy_deallocate<Token::%s>(p);break;}\n",td.name.c_str(),td.name.c_str());
    fprintf(out,IND2 "}\n" IND "}\n}; // struct %s::Allocator\n",ts_cstr);
    if(!cmd_data.nspace.empty())
        fprintf(out,"} // namespace %s\n",cmd_data.nspace.c_str());
} // write_token_non_simple
} // namespace yuki::pg