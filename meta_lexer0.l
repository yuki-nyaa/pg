%option fast unicode nodefault noline
%option outfile="meta_lexer0.cpp" header-file="meta_lexer0.h"
%option namespace="yuki::pg" prefix="yuki_pg_meta0" lexer="Meta_Lexer0" lex="lex" token-type="int"

%code_cpptop{
#include"cconfig"
#include<unordered_map>
#include"common.hpp"
namespace yuki::pg{
static void write_token(FILE* const,const Options&,yuki::Vector<Token_Data>&,yuki::Vector<Token_Data>&,std::unordered_map<std::string,size_t>&,const std::unordered_map<std::string,std::string>&);
}
}

%code_htop{
#include<unordered_map>
#include"common.hpp"
#include"cmd.hpp"
namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t>
class Meta_Lexer1;
}
}

%code_class{
  public:
    template<std::unsigned_integral Token_Kind_t>
    friend class Meta_Lexer1;

    #ifndef YUKI_PG_TOKEN_HTABLE_BUCKET
    #define YUKI_PG_TOKEN_HTABLE_BUCKET 1024
    #endif
    Meta_Lexer0(yuki::pg::Cmd_Data&& cmd_data_p,std::ostream& os=std::cout) noexcept :
        AbstractBaseLexer(reflex::Input(cmd_data_p.fp_in),os),
        cmd_data(std::move(cmd_data_p)),
        vtoken_types(YUKI_PG_TOKEN_HTABLE_BUCKET),
        token_htable(YUKI_PG_TOKEN_HTABLE_BUCKET),
        code_htable(8)
    {
        #ifndef YUKI_PG_CODE_RESERVE
        #define YUKI_PG_CODE_RESERVE 2048
        #endif
        str_temp1.reserve(YUKI_PG_CODE_RESERVE);
    }
  public:
    struct Token_Coordinate{
        bool is_term;
        size_t idx;
    };

    yuki::pg::Cmd_Data cmd_data;

    yuki::pg::Options options;

    yuki::Vector<Token_Data> nterms{yuki::reserve_tag,YUKI_PG_TOKEN_HTABLE_BUCKET};
    yuki::Vector<Token_Data> terms{yuki::reserve_tag,YUKI_PG_TOKEN_HTABLE_BUCKET};

    std::unordered_map<std::string,size_t> vtoken_types;

    std::unordered_map<std::string,Token_Coordinate> token_htable;

    std::unordered_map<std::string,std::string> code_htable;

    unsigned errors=0;

    unsigned brace_level=0;

    bool token_declared = false;

    bool is_term = true;

    bool assoc_has_args = false;

    prec_t current_prec = 0;
    Assoc current_assoc;
    Assoc assoc_default = Assoc::RIGHT;

    std::string str_temp1;
    std::string code_qualifier;

    void clear_temps(){
        std::unordered_map<std::string,size_t> umap;
        using std::swap;
        swap(vtoken_types,umap);
        str_temp1.clear();
        code_qualifier.clear();
    }

    yuki::Vector<Token_Data>& get_token_table(bool is_term_p){
        return is_term_p ? terms : nterms;
    }
    Token_Data& get_token_data(Token_Coordinate co){
        return (co.is_term ? terms : nterms)[co.idx];
    }
    Token_Data& get_token_data(const std::string& s){
        return get_token_data(token_htable[s]);
    }
}

%x TERM TOKEN_DEF ALLOC0 ALLOC1 PREC ASSOC CODE0 CODE1 O_SPTOKEN O_NSPACE O_PARSER O_TS O_LEXER O_PT

keychar    \p{Space}|\{|\}|\%|:|\||;|\$|\"
non_keychar    [^{keychar}]
any    .|\R
id [A-Za-z_]\p{Word}*

%%
<INITIAL>{
\%\%\p{Space}*    {
    terms.emplace_back("EOF_",assoc_default);
    if(options.parser_tables.empty())
        options.parser_tables.append(options.parser).append("_Tables");
    write_token(cmd_data.fp_out_token,options,nterms,terms,vtoken_types,code_htable);
    clear_temps();
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

\%default_left\p{Space}*    {options.assoc0 = assoc_default = Assoc::LEFT;}
\%default_right\p{Space}*    {options.assoc0 = assoc_default = Assoc::RIGHT;}

\%code\p{Space}*    {start(CODE0);}

\%simple_token\p{Space}*    {
    options.token_impl_type=Options::Token_Impl_Type::SIMPLE;
    str_temp1.clear();
    start(O_SPTOKEN);
}

\%tuple_token    {options.token_impl_type=Options::Token_Impl_Type::TUPLE;}

\%namespace\p{Space}*    {start(O_NSPACE);}
\%parser\p{Space}*    {start(O_PARSER);}
\%token_settings\p{Space}*    {start(O_TS);}
\%lexer\p{Space}*    {str_temp1.clear();start(O_LEXER);}
\%parser_tables\p{Space}*    {str_temp1.clear();start(O_PT);}

\%no_final_function    {options.no_final_function = true;}
\%no_final_class    {options.no_final_class = true;}
\%no_default_ctor    {options.no_default_ctor = true;}

\%action_array    {options.is_switch = false;}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.
}



<TERM>{
\".*\"    {
    if(!token_declared){
        fprintf(stderr,"Error: Alias declaration precedes token name declaration: %s\n",text());
        ++errors;
    }else{
        str_temp1 = str();
        get_token_table(is_term).back().alias = str_temp1;
        token_htable.insert_or_assign(std::move(str_temp1),Token_Coordinate{is_term,get_token_table(is_term).size()-1});
    }
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

{id}    {
    token_declared = true;
    str_temp1 = str();
    token_htable.insert_or_assign(str_temp1,Token_Coordinate{is_term,get_token_table(is_term).size()});
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
    if(options.token_impl_type==Options::Token_Impl_Type::TUPLE){
        yuki::trim_trailing_spaces(str_temp1);
        get_token_table(is_term).back().names.emplace_back(std::move(str_temp1));
    }else if(!str_temp1.empty())
        fprintf(stderr,"Warning: Token member name (\"%s\" in token \"%s\") is effective only with tuple-token!\n",str_temp1.c_str(),get_token_table(is_term).back().name.c_str());
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
    fprintf(stderr,"Fatal Error: %% encountered while scanning token allocator number of \"%s\". Did you forget the ; ?\n",get_token_table(is_term).back().name.c_str());
    ++errors;
    std::exit(EXIT_FAILURE);
}

;    {
    switch(options.token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            const auto& types = get_token_table(is_term).back().types;
            if(!types.empty())
                vtoken_types.try_emplace(types[0],0);
            break;
        }
        case Options::Token_Impl_Type::SIMPLE: break;
        case Options::Token_Impl_Type::TUPLE:{
            std::string& a = get_token_table(is_term).back().alloc;
            if(a.empty()){
                a.assign("yuki::Array_Allocator<Token::");
                a.append(get_token_table(is_term).back().name).push_back('>');
            }
            break;
        }
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
    fprintf(stderr,"Fatal Error: %%%% encountered while scanning token allocator name of \"%s\". Did you forget the ; ?\n",get_token_table(is_term).back().name.c_str());
    ++errors;
    std::exit(EXIT_FAILURE);
}
}



<PREC>{
(\"[^\"]*\")|({id})    {
    str_temp1 = str();
    try{
        Token_Coordinate co = token_htable.at(str_temp1);
        if(!co.is_term)
            fprintf(stderr,"Warning: %%prec for non-terminals makes no sense: %s\n",str_temp1.c_str());
        else
            get_token_data(co).prec = current_prec;
    }catch(const std::out_of_range&){
        fprintf(stderr,"Error: Unknown token name encountered while parsing %%prec declaration: %s\n",str_temp1.c_str());
        ++errors;
    }
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.

(?=\%)    {start(INITIAL);}
}



<ASSOC>{
(\"[^\"]*\")|({id})    {
    str_temp1 = str();
    try{
        Token_Coordinate co = token_htable.at(str_temp1);
        if(!co.is_term)
            fprintf(stderr,"Warning: %%left/%%right for non-terminals makes no sense: %s\n",str_temp1.c_str());
        else{
            Token_Data& td = get_token_data(co);
            td.prec = current_prec;
            assoc_has_args = true;
            td.assoc = current_assoc;
        }
    }catch(const std::out_of_range&){
        fprintf(stderr,"Error: Unknown token name encountered while parsing %%left/%%right declaration: %s\n",str_temp1.c_str());
        ++errors;
    }
}

\p{Space}+    {} // Spaces as well as newlines are ignored.

\/\/.*\R    {} // Comments.

(?=\%)    {
    if(assoc_has_args==false){
        options.assoc0 = current_assoc;
        --current_prec;
    }
    start(INITIAL);
}
}



<CODE0>{
{id}    {code_qualifier=str();code_htable.insert_or_assign(code_qualifier,std::string{});}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\{    {
    brace_level=0;
    str_temp1.clear();
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
        code_htable.at(code_qualifier) = std::move(str_temp1);
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
        options.sp_token = "void";
    else{
        yuki::trim_trailing_spaces(str_temp1);
        options.sp_token = std::move(str_temp1);
    }
    start(INITIAL);
}
\R    {str_temp1.push_back(' ');}
.    {str_temp1.push_back(chr());}
}



<O_NSPACE>{
[a-zA-Z_:][0-9a-zA-Z_:]*   {options.nspace.assign(text(),size());start(INITIAL);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}

<O_PARSER>{
{id}    {options.parser.assign(text(),size());start(INITIAL);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}

<O_TS>{
{id}    {options.ts.assign(text(),size());start(INITIAL);}
\p{Space}+    {} // Spaces as well as newlines are ignored.
\/\/.*\R    {} // Comments.
}



<O_LEXER>{
\/\/.*\R    {} // Comments.

\\\%    {str_temp1.push_back('%');}

(?=\%)    {
    yuki::trim_trailing_spaces(str_temp1);
    options.lexer = std::move(str_temp1);
    start(INITIAL);
}
\R    {str_temp1.push_back(' ');}
.    {str_temp1.push_back(chr());}
}

<O_PT>{
\/\/.*\R    {} // Comments.

\\\%    {str_temp1.push_back('%');}

(?=\%)    {
    yuki::trim_trailing_spaces(str_temp1);
    options.parser_tables = std::move(str_temp1);
    start(INITIAL);
}
\R    {str_temp1.push_back(' ');}
.    {str_temp1.push_back(chr());}
}

%%
namespace yuki::pg{
static void write_traits_alloc(FILE* const,const char* const,const yuki::Vector<Token_Data>&,const yuki::Vector<Token_Data>&);
static void write_token_index_table(FILE* const,const char* const,yuki::Vector<Token_Data>&,yuki::Vector<Token_Data>&,const std::unordered_map<std::string,size_t>&);

static void write_token(FILE* const out_token,const Options& options,yuki::Vector<Token_Data>& nterms_nc,yuki::Vector<Token_Data>& terms_nc,std::unordered_map<std::string,size_t>& vtoken_types,const std::unordered_map<std::string,std::string>& code_htable){
    #define IND YUKI_PG_IND
    #define IND2 IND IND
    #define IND3 IND IND IND
    #define IND4 IND IND IND IND

    const Options::Token_Impl_Type token_impl_type = options.token_impl_type;
    const yuki::Vector<Token_Data>& nterms = nterms_nc;
    const yuki::Vector<Token_Data>& terms = terms_nc;

    fprintf(out_token,"#pragma once\n");
    try{
        fprintf(out_token,"%s\n",code_htable.at("token_top").c_str());
    }catch(const std::out_of_range&){}
    fprintf(out_token,
        "#include<cassert>\n"
        "#include<yuki/tmp.hpp>\n"
        "#include<yuki/pg/core.hpp>\n"
    );
    switch(token_impl_type){
        case Options::Token_Impl_Type::VARIANT: fprintf(out_token,"#include<yuki/pg/vtoken.hpp>\n\n");break;
        case Options::Token_Impl_Type::SIMPLE: fprintf(out_token,"#include<yuki/pg/stoken.hpp>\n\n");break;
        case Options::Token_Impl_Type::TUPLE: fprintf(out_token,"#include<yuki/pg/ttoken.hpp>\n#include<yuki/allocator.hpp>\n\n");break;
    }
    if(!options.nspace.empty())
        fprintf(out_token,"namespace %s{\n",options.nspace.c_str());
    fprintf(out_token,"struct %s{\n",options.ts.c_str());
    fprintf(out_token,IND "static constexpr yuki::pg::Token_Impl_Type token_impl_type = yuki::pg::Token_Impl_Type::");
    switch(token_impl_type){
        case Options::Token_Impl_Type::VARIANT: fprintf(out_token,"VARIANT;\n\n");break;
        case Options::Token_Impl_Type::SIMPLE: fprintf(out_token,"SIMPLE;\n\n");break;
        case Options::Token_Impl_Type::TUPLE: fprintf(out_token,"TUPLE;\n\n");break;
    }
    fprintf(out_token,
        IND "static constexpr size_t token_total = %zu;\n"
        IND "typedef yuki::uint_auto_t<token_total> Token_Kind_t;\n",
        nterms.size()+terms.size()
    );
    fprintf(out_token,
        IND "struct Token_Kind{\n"
        IND2 "enum enum_t : Token_Kind_t {"
    );
    for(const Token_Data& td : nterms)
        fprintf(out_token,"%s,",td.name.c_str());
    for(const Token_Data& td : terms)
        fprintf(out_token,"%s,",td.name.c_str());
    fprintf(out_token,
        "};\n"
        IND "};\n"
    );
    fprintf(out_token,IND "static constexpr const char* token_name[token_total] = {");
    for(const Token_Data& td : nterms){
        if(td.alias.empty())
            fprintf(out_token,"\"%s\",",td.name.c_str());
        else
            fprintf(out_token,"%s,",td.alias.c_str());
    }
    for(const Token_Data& td : terms){
        if(td.alias.empty())
            fprintf(out_token,"\"%s\",",td.name.c_str());
        else
            fprintf(out_token,"%s,",td.alias.c_str());
    }
    fprintf(out_token,"};\n\n");
    fprintf(out_token,
        IND "static constexpr Token_Kind_t nterminal_first = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t nterminal_last = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t terminal_first = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t terminal_last = Token_Kind::EOF_;\n"
        IND "static constexpr Token_Kind_t eof_ = Token_Kind::EOF_;\n"
        "\n",
        nterms[0].name.c_str(), nterms.back().name.c_str(), terms[0].name.c_str()
    );
    fprintf(out_token,
        IND "static constexpr size_t nterminal_total = %zu;\n"
        IND "static constexpr size_t terminal_total = %zu;\n"
        "\n",
        nterms.size(), terms.size()
    );
    fprintf(out_token,
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
    switch(token_impl_type){
        case Options::Token_Impl_Type::VARIANT:{
            fprintf(out_token,
                IND "struct Token_Index_Table;\n\n"
                IND "typedef yuki::pg::VToken<Token_Kind_t,Token_Index_Table"
            );
            size_t i = 1;
            for(std::pair<const std::string,size_t>& pair : vtoken_types){
                fprintf(out_token,",%s",pair.first.c_str());
                pair.second = i++;
            }
            fprintf(out_token,
                "> Token_t;\n"
                IND "typedef void Token;\n"
                "}; // struct %s\n",
                options.ts.c_str()
            );
            write_token_index_table(out_token,options.ts.c_str(),nterms_nc,terms_nc,vtoken_types);
            if(!options.nspace.empty())
                fprintf(out_token,"} // namespace %s\n",options.nspace.c_str());
            break;
        }
        case Options::Token_Impl_Type::SIMPLE:{
            fprintf(out_token,
                IND "typedef yuki::pg::SToken<Token_Kind_t,%s> Token_t;\n"
                IND "typedef void Token;\n"
                "}; // struct %s\n",
                options.sp_token.c_str(),
                options.ts.c_str()
            );
            if(!options.nspace.empty())
                fprintf(out_token,"} // namespace %s\n",options.nspace.c_str());
            break;
        }
        case Options::Token_Impl_Type::TUPLE:{
            fprintf(out_token,IND "typedef yuki::pg::Any_TToken<%s> Token_t;\n\n",options.ts.c_str());
            fprintf(out_token,
                IND "template<Token_Kind_t kind_p,typename... Ts>\n"
                IND "using TToken_Tp = yuki::pg::TToken<Token_Kind_t,kind_p,Ts...>;\n"
                "\n"
                IND "struct Token{\n"
            );
            for(const Token_Data& td : nterms){
                fprintf(out_token,IND2 "using %s = TToken_Tp<Token_Kind::%s",td.name.c_str(),td.name.c_str());
                for(const std::string& type : td.types)
                    fprintf(out_token,",%s",type.c_str());
                fprintf(out_token,">;\n");
            }
            for(const Token_Data& td : terms){
                fprintf(out_token,IND2 "using %s = TToken_Tp<Token_Kind::%s",td.name.c_str(),td.name.c_str());
                for(const std::string& type : td.types)
                    fprintf(out_token,",%s",type.c_str());
                fprintf(out_token,">;\n");
            }
            fprintf(out_token,IND "}; // struct Token\n\n");
            write_traits_alloc(out_token,options.ts.c_str(),nterms,terms);
            if(!options.nspace.empty())
                fprintf(out_token,"} // namespace %s\n",options.nspace.c_str());
            break;
        }
    }
    try{
        fprintf(out_token,"%s\n",code_htable.at("token_end").c_str());
    }catch(const std::out_of_range&){}
} // write_token


static void write_token_index_table(FILE* const out_token,const char* const ts_cstr,yuki::Vector<Token_Data>& nterms,yuki::Vector<Token_Data>& terms,const std::unordered_map<std::string,size_t>& vtoken_types){
    fprintf(out_token,
        "\n"
        "struct %s::Token_Index_Table{\n"
        "static constexpr %s::Token_Kind_t token_index_table[%zu] = {\n",
        ts_cstr,
        ts_cstr,nterms.size()+terms.size()
    );

    size_t token_count = 0;
    #ifndef YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM
    #define YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM 16
    #endif
    auto print_ind = [out_token,&token_count](){
        if(token_count % YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM == 0)
            fprintf(out_token,IND);
    };
    auto print_break = [out_token,&token_count](){
        if(token_count % YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM == 0)
            fprintf(out_token,"\n");
    };

    for(Token_Data& td : nterms){
        print_ind();
        fprintf(out_token,"%zu,", td.types.empty() ? 0 : td.type_index = vtoken_types.at(td.types[0]));
        ++token_count;
        print_break();
    }
    for(Token_Data& td : terms){
        print_ind();
        fprintf(out_token,"%zu,", td.types.empty() ? 0 : td.type_index = vtoken_types.at(td.types[0]));
        ++token_count;
        print_break();
    }
    fprintf(out_token,
        "%s}; // token_index_table\n"
        "}; // struct %s::Token_Index_Table\n",
        token_count%YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM==0 ? "" : "\n",
        ts_cstr
    );
}


static void write_traits_alloc(FILE* const out_token,const char* const ts_cstr,const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms){
    fprintf(out_token,
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
        fprintf(out_token,"template<> struct %s::is_nterminal<%s::Token::%s> : std::true_type {};\n",ts_cstr,ts_cstr,td.name.c_str());
    fprintf(out_token,"\n");
    for(const Token_Data& td : terms)
        fprintf(out_token,"template<> struct %s::is_terminal<%s::Token::%s> : std::true_type {};\n",ts_cstr,ts_cstr,td.name.c_str());
    fprintf(out_token,"\n");
    for(const Token_Data& td : nterms)
        fprintf(out_token,"template<> struct %s::kind_to_type<%s::Token_Kind::%s> {typedef %s::Token::%s type;};\n",
            ts_cstr, ts_cstr, td.name.c_str(),
            ts_cstr, td.name.c_str()
        );
    for(const Token_Data& td : terms)
        fprintf(out_token,"template<> struct %s::kind_to_type<%s::Token_Kind::%s> {typedef %s::Token::%s type;};\n",
            ts_cstr, ts_cstr, td.name.c_str(),
            ts_cstr, td.name.c_str()
        );

    fprintf(out_token,"\n");

    fprintf(out_token,"struct %s::Allocator :\n",ts_cstr);
    for(const Token_Data& td : nterms)
        fprintf(out_token,IND "private type_to_alloc_t<Token::%s>,\n",td.name.c_str());
    for(size_t i = 0;i!=terms.size();++i){
        fprintf(out_token,IND "private type_to_alloc_t<Token::%s>",terms[i].name.c_str());
        if(i+1!=terms.size())
            fprintf(out_token,",");
        fprintf(out_token,"\n");
    }
    fprintf(out_token,
        "{\n"
        IND "template<typename T>\n"
        IND "auto allocate(size_t n=1) -> typename type_to_alloc_t<T>::pointer {return type_to_alloc_t<T>::allocate(n);}\n"
        "\n"
        IND "template<typename T>\n"
        IND "void deallocate(const yuki::pg::TToken_Base* p,size_t n=1) {type_to_alloc_t<T>::deallocate(static_cast<const T*>(p),n);}\n"
        "\n"
        IND "template<typename T>\n"
        IND "void destroy_deallocate(const yuki::pg::TToken_Base* p){\n"
        IND2 "if constexpr(!std::is_trivially_destructible_v<T>)\n"
        IND3 "static_cast<const T*>(p) -> ~T();\n"
        IND2 "type_to_alloc_t<T>::deallocate(static_cast<const T*>(p));\n"
        IND "}\n"
        "\n"
        IND "void dynamic_destroy_deallocate(Token_Kind_t k,const yuki::pg::TToken_Base* p){\n"
        IND2 "using enum Token_Kind::enum_t;\n"
        IND2 "switch(k){\n"
    );
    for(const Token_Data& td : nterms)
        fprintf(out_token,IND3 "case %s : {destroy_deallocate<Token::%s>(p);break;}\n",td.name.c_str(),td.name.c_str());
    for(const Token_Data& td : terms)
        fprintf(out_token,IND3 "case %s : {destroy_deallocate<Token::%s>(p);break;}\n",td.name.c_str(),td.name.c_str());
    fprintf(out_token,IND2 "}\n" IND "}\n}; // struct %s::Allocator\n",ts_cstr);
} // write_traits_alloc
} // namespace yuki::pg