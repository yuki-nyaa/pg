#include"cconfig"
#include"cmd.hpp"
#include"Meta_Lexer0.hpp"
#include"Meta_Lexer1.h"

namespace yuki::pg{
void write_traits_alloc(FILE* const out_token,const char* const ts_cstr,const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms);

void write_token(FILE* const out_token,Sec0_Data& sec0_data){
    #define IND YUKI_PG_IND
    #define IND2 IND IND
    #define IND3 IND IND IND
    #define IND4 IND IND IND IND

    const Sec0_Data::Token_Impl_Type token_impl_type = sec0_data.token_impl_type;
    yuki::Vector<Token_Data>& nterms = sec0_data.token_datas.nterms;
    yuki::Vector<Token_Data>& terms = sec0_data.token_datas.terms;

    fprintf(out_token,"#pragma once\n");
    for(const Sec0_Data::Code& code : sec0_data.codes){
        if(code.qualifier=="token_top"){
            fprintf(out_token,"%s\n",code.contents.c_str());
            break;
        }
    }
    fprintf(out_token,
        "#include<cassert>\n"
        "#include<yuki/tmp.hpp>\n"
        "#include<yuki/pg/core.hpp>\n"
    );
    switch(token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: fprintf(out_token,"#include<yuki/pg/VToken.hpp>\n\n");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: fprintf(out_token,"#include<yuki/pg/SToken.hpp>\n\n");break;
        case Sec0_Data::Token_Impl_Type::TUPLE: fprintf(out_token,"#include<yuki/pg/TToken.hpp>\n#include<yuki/Allocator.hpp>\n\n");break;
    }
    if(!sec0_data.nspace.empty())
        fprintf(out_token,"namespace %s{\n",sec0_data.nspace.c_str());
    fprintf(out_token,"struct %s{\n",sec0_data.ts.c_str());
    fprintf(out_token,IND "static constexpr yuki::pg::Token_Impl_Type token_impl_type = yuki::pg::Token_Impl_Type::");
    switch(token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: fprintf(out_token,"VARIANT;\n\n");break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: fprintf(out_token,"SIMPLE;\n\n");break;
        case Sec0_Data::Token_Impl_Type::TUPLE: fprintf(out_token,"TUPLE;\n\n");break;
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
    // Finalize `sec0_data` while printing token names.
    const Assoc assoc0 = sec0_data.assoc0;
    yuki::unordered_map_str<std::string,size_t> vtoken_types;
    std::string alloc_temp;
    const char** const vtoken_types_linear = new const char*[nterms.size()+terms.size()];
    auto td_finalize = [token_impl_type,assoc0,&vtoken_types,&alloc_temp,vtoken_types_linear](Token_Data& td){
        if(td.assoc==Assoc::DEFAULT)
            td.assoc=assoc0;
        switch(token_impl_type){
            case Sec0_Data::Token_Impl_Type::VARIANT:
                if(!td.types.empty()){
                    if(const auto pair=vtoken_types.try_emplace(td.types.front(),vtoken_types.size()+1); pair.second){
                        td.vtoken_index = vtoken_types.size();
                        vtoken_types_linear[vtoken_types.size()-1] = pair.first->first.c_str();
                    }else
                        td.vtoken_index = pair.first->second;
                }
                break;
            case Sec0_Data::Token_Impl_Type::TUPLE:
                if(td.alloc.empty()){
                    td.alloc="yuki::Array_Allocator<Token::";
                    td.alloc.append(td.name).push_back('>');
                }else if(isdigit(static_cast<unsigned char>(td.alloc.front()))){
                    char* end=nullptr;
                    strtoull(td.alloc.c_str(),&end,0);
                    alloc_temp.assign(td.alloc.c_str(),static_cast<const char*>(end));
                    td.alloc="yuki::Array_Allocator<Token::";
                    td.alloc.append(td.name).append(",").append(alloc_temp).push_back('>');
                }
                break;
        }
    };
    for(Token_Data& td : nterms){
        td_finalize(td);
        fprintf(out_token,"%s,",td.name.c_str());
    }
    for(Token_Data& td : terms){
        td_finalize(td);
        fprintf(out_token,"%s,",td.name.c_str());
    }
    //< Finalize `sec0_data` while printing token names.
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
        IND "static constexpr bool is_nterminal_f(const Token_Kind_t kind) {return kind>=nterminal_first && kind<=nterminal_last;}\n"
        IND "static constexpr bool is_terminal_f(const Token_Kind_t kind) {return kind>=terminal_first && kind<=terminal_last;}\n"
        "\n"
        IND "static constexpr size_t nterminal_kind_to_index(const Token_Kind_t kind){\n"
        IND2 "assert(is_nterminal_f(kind));\n"
        IND2 "return kind-nterminal_first;\n"
        IND "}\n"
        "\n"
        IND "static constexpr size_t terminal_kind_to_index(const Token_Kind_t kind){\n"
        IND2 "assert(is_terminal_f(kind));\n"
        IND2 "return kind-terminal_first;\n"
        IND "}\n"
        "\n"
    );
    switch(token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT:{
            fprintf(out_token,IND "static constexpr Token_Kind_t token_index_table[%zu] = {\n",nterms.size()+terms.size());
            #ifndef YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM
            #define YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM 32
            #endif
            size_t token_count = 0;
            for(const Token_Data& td : nterms){
                if(token_count==0)
                    fprintf(out_token,IND2);
                fprintf(out_token,"%zu, ",td.vtoken_index);
                if(token_count==YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM-1){
                    fprintf(out_token,"\n");
                    token_count=0;
                }else
                    ++token_count;
            }
            for(const Token_Data& td : terms){
                if(token_count==0)
                    fprintf(out_token,IND2);
                fprintf(out_token,"%zu, ",td.vtoken_index);
                if(token_count==YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM-1){
                    fprintf(out_token,"\n");
                    token_count=0;
                }else
                    ++token_count;
            }
            fprintf(out_token,
                "%s"
                IND "}; // token_index_table\n"
                "\n"
                IND "typedef yuki::pg::VToken<Token_Kind_t,token_index_table\n",
                token_count==0 ? "" : "\n"
            );
            token_count=0;
            #ifndef YUKI_PG_VTOKEN_MAX_LINE_ITEM
            #define YUKI_PG_VTOKEN_MAX_LINE_ITEM 16
            #endif
            const size_t vtoken_total = vtoken_types.size();
            while(token_count<vtoken_total){
                if(token_count%YUKI_PG_VTOKEN_MAX_LINE_ITEM==0)
                    fprintf(out_token,IND2);
                fprintf(out_token,", %s",vtoken_types_linear[token_count]);
                ++token_count;
                if(token_count%YUKI_PG_VTOKEN_MAX_LINE_ITEM==0)
                    fprintf(out_token,"\n");
            }
            fprintf(out_token,
                "%s"
                IND "> Token_t;\n"
                IND "typedef void Token;\n"
                "}; // struct %s\n",
                token_count%YUKI_PG_VTOKEN_MAX_LINE_ITEM==0 ? "" : "\n",
                sec0_data.ts.c_str()
            );
            if(!sec0_data.nspace.empty())
                fprintf(out_token,"} // namespace %s\n",sec0_data.nspace.c_str());
            break;
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE:{
            fprintf(out_token,
                IND "typedef yuki::pg::SToken<Token_Kind_t,%s> Token_t;\n"
                IND "typedef void Token;\n"
                "}; // struct %s\n",
                sec0_data.sp_token.c_str(),
                sec0_data.ts.c_str()
            );
            if(!sec0_data.nspace.empty())
                fprintf(out_token,"} // namespace %s\n",sec0_data.nspace.c_str());
            break;
        }
        case Sec0_Data::Token_Impl_Type::TUPLE:{
            fprintf(out_token,IND "typedef yuki::pg::Any_TToken<%s> Token_t;\n\n",sec0_data.ts.c_str());
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
            write_traits_alloc(out_token,sec0_data.ts.c_str(),nterms,terms);
            if(!sec0_data.nspace.empty())
                fprintf(out_token,"} // namespace %s\n",sec0_data.nspace.c_str());
            break;
        }
    }
    for(const Sec0_Data::Code& code : sec0_data.codes){
        if(code.qualifier=="token_bottom"){
            fprintf(out_token,"%s\n",code.contents.c_str());
            break;
        }
    }
    delete[] vtoken_types_linear;
} // write_token


void write_traits_alloc(FILE* const out_token,const char* const ts_cstr,const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms){
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
        IND "auto allocate(const size_t n=1) -> typename type_to_alloc_t<T>::pointer {return type_to_alloc_t<T>::allocate(n);}\n"
        "\n"
        IND "template<typename T>\n"
        IND "void deallocate(const yuki::pg::TToken_Base* const p,const size_t n=1) {type_to_alloc_t<T>::deallocate(static_cast<const T*>(p),n);}\n"
        "\n"
        IND "template<typename T>\n"
        IND "void destroy_deallocate(const yuki::pg::TToken_Base* const p){\n"
        IND2 "if constexpr(!std::is_trivially_destructible_v<T>)\n"
        IND3 "static_cast<const T*>(p) -> ~T();\n"
        IND2 "type_to_alloc_t<T>::deallocate(static_cast<const T*>(p));\n"
        IND "}\n"
        "\n"
        IND "void dynamic_destroy_deallocate(const Token_Kind_t k,const yuki::pg::TToken_Base* const p){\n"
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




namespace yuki::pg::lr{
template<std::unsigned_integral Token_Kind_t>
void write(const Cmd_Data& cmd_data,const Sec0_Data& sec0_data,const Rule_Set<Token_Kind_t>& rules);
}




int main(const int argc,const char*const*const argv){
    if(argc<=1){
        yuki::pg::cmd_impl::version(stdout);
        yuki::cmd_help(stdout,yuki::pg::coarr);
        return EXIT_SUCCESS;
    }
    yuki::pg::Cmd_Data cmd_data;
    if(yuki::Cmd_Option_Table<yuki::pg::coarr>{}.parse(cmd_data,argc,argv)!=0)
        return EXIT_SUCCESS;
    if(!cmd_data.post_process())
        exit(EXIT_FAILURE);

    yuki::pg::Sec0_Data sec0_data = yuki::pg::parse_sec0(cmd_data.fp_in,cmd_data.in.data());
    if(sec0_data.token_datas.nterms.empty()){
        fprintf(stderr,"Fatal Error: No non-terminals are declared!\n");
        fprintf(stderr,"%u errors encountered.\n",sec0_data.errors+1);
        return EXIT_FAILURE;
    }
    sec0_data.token_datas.terms.emplace_back("EOF_");
    sec0_data.token_datas.terms.back().assoc=yuki::pg::Assoc::LEFT;
    if(sec0_data.parser_tables.empty())
        (sec0_data.parser_tables=sec0_data.parser).append("_Tables");
    yuki::pg::write_token(cmd_data.fp_out_token,sec0_data);
    fclose(cmd_data.fp_out_token);
    cmd_data.fp_out_token=nullptr;

    unsigned total_errors=0;

    try{
        switch(yuki::uint_auto_f(sec0_data.token_datas.nterms.size()+sec0_data.token_datas.terms.size()+1)){
            case yuki::uint_enum::UCHAR:{
                yuki::pg::Meta_Lexer1<unsigned char> meta_lexer1(cmd_data.fp_in,std::move(sec0_data));
                meta_lexer1.lex();
                yuki::pg::lr::write<unsigned char>(cmd_data,meta_lexer1.sec0_data,meta_lexer1.rs);
                total_errors=meta_lexer1.sec0_data.errors;
                break;
            }
            case yuki::uint_enum::USHORT:{
                yuki::pg::Meta_Lexer1<unsigned short> meta_lexer1(cmd_data.fp_in,std::move(sec0_data));
                meta_lexer1.lex();
                yuki::pg::lr::write<unsigned short>(cmd_data,meta_lexer1.sec0_data,meta_lexer1.rs);
                total_errors=meta_lexer1.sec0_data.errors;
                break;
            }
            case yuki::uint_enum::UINT:{
                yuki::pg::Meta_Lexer1<unsigned> meta_lexer1(cmd_data.fp_in,std::move(sec0_data));
                meta_lexer1.lex();
                yuki::pg::lr::write<unsigned>(cmd_data,meta_lexer1.sec0_data,meta_lexer1.rs);
                total_errors=meta_lexer1.sec0_data.errors;
                break;
            }
            default:
                fprintf(stderr,"Fatal Error: Token count exceeds implementation limit %u!\n",std::numeric_limits<unsigned>::max());
                fprintf(stderr,"%u errors encountered.\n",sec0_data.errors+1);
                return EXIT_FAILURE;
        }
        if(total_errors!=0){
            fprintf(stderr,"%u errors encountered.\n",total_errors);
            return EXIT_FAILURE;
        }else
            return EXIT_SUCCESS;
    }catch(const std::runtime_error& e){
        fprintf(stderr,"ICE: %s\n",e.what());
        return EXIT_FAILURE;
    }
}
