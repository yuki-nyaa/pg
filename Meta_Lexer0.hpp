#pragma once
#include<unordered_map>
#include<yuki/CHashTable_Str.hpp>
#include<yuki/uchar.hpp>
#include<yuki/unicode/binary_properties.h>
#include"common.hpp"

namespace yuki::pg{

namespace sec0_impl{
    inline void debug(Sec0_Data&,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){
        if(arg_num==0)
            fprintf(stderr,"%s - %%__debug\n",filename);
        fprintf(stderr,"(%zu)--%zu:%zu %s\n",arg_num,lineno,colno,arg.data());
    }

    #define YUKI_PG_META_ML0_ZERO_PARAM(name,...) \
    inline void name(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){ \
        if(arg_num>0){ \
            print_loc(stderr,lineno,colno,filename); \
            fprintf(stderr,"Warning: %" #name "with extraneous argument: %s\n",arg.data()); \
        } \
        __VA_ARGS__ \
    }

    YUKI_PG_META_ML0_ZERO_PARAM(variant_token,data.token_impl_type=Sec0_Data::Token_Impl_Type::VARIANT;)
    //YUKI_PG_META_ML0_ZERO_PARAM(tuple_token,data.token_impl_type=Sec0_Data::Token_Impl_Type::TUPLE;)

    YUKI_PG_META_ML0_ZERO_PARAM(plr1,data.alg_type=Sec0_Data::Alg_Type::PLR1;)
    YUKI_PG_META_ML0_ZERO_PARAM(clr1,data.alg_type=Sec0_Data::Alg_Type::CLR1;)
    YUKI_PG_META_ML0_ZERO_PARAM(lalr1,data.alg_type=Sec0_Data::Alg_Type::LALR1;)

    YUKI_PG_META_ML0_ZERO_PARAM(no_default_ctor,data.no_default_ctor=true;)

    YUKI_PG_META_ML0_ZERO_PARAM(default_left,data.assoc0=Assoc::LEFT;)
    YUKI_PG_META_ML0_ZERO_PARAM(default_right,data.assoc0=Assoc::RIGHT;)

    #define YUKI_PG_META_ML0_ONE_PARAM(name,...) \
    inline void name(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){ \
        if(arg_num>1){ \
            print_loc(stderr,lineno,colno,filename); \
            fprintf(stderr,"Warning: %" #name "with extraneous argument: %s\n",arg.data()); \
            return; \
        } \
        __VA_ARGS__ \
    }

    YUKI_PG_META_ML0_ONE_PARAM(nspace,data.nspace=arg;)
    YUKI_PG_META_ML0_ONE_PARAM(parser,data.parser=arg;)
    YUKI_PG_META_ML0_ONE_PARAM(token_settings,data.ts=arg;)
    YUKI_PG_META_ML0_ONE_PARAM(lexer,data.lexer=arg;)
    YUKI_PG_META_ML0_ONE_PARAM(debug_prefix,data.debug_prefix=arg;)

    YUKI_PG_META_ML0_ONE_PARAM(simple_token, data.sp_token=arg; data.token_impl_type=Sec0_Data::Token_Impl_Type::SIMPLE;)

    template<Assoc assoc>
    void right_or_left(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){
        if(arg_num==0)
            ++data.current_prec;
        if(const auto it=data.token_datas.token_htable.find(arg); it!=data.token_datas.token_htable.end()){
            const Token_Datas::Coordinate co = it->second;
            if(!co.is_term){
                print_loc(stderr,lineno,colno,filename);
                fprintf(stderr,"Warning: %%left/%%right for non-terminals makes no sense: %s\n",arg.data());
            }else{
                Token_Data& td = data.token_datas[co];
                td.prec = data.current_prec;
                td.assoc = assoc;
            }
        }else{
            print_loc(stderr,lineno,colno,filename);
            fprintf(stderr,"Error: Unknown token name encountered while parsing %%left/%%right declaration: %s\n",arg.data());
            data.advance_errors();
        }
    }

    void prec(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){
        if(arg_num==0)
            ++data.current_prec;
        if(const auto it=data.token_datas.token_htable.find(arg); it!=data.token_datas.token_htable.end()){
            const Token_Datas::Coordinate co = it->second;
            if(!co.is_term){
                print_loc(stderr,lineno,colno,filename);
                fprintf(stderr,"Warning: %%prec for non-terminals makes no sense: %s\n",arg.data());
            }else{
                data.token_datas[co].prec = data.current_prec;
            }
        }else{
            print_loc(stderr,lineno,colno,filename);
            fprintf(stderr,"Error: Unknown token name encountered while parsing %%prec declaration: %s\n",arg.data());
            data.advance_errors();
        }
    }

    template<typename Ts,typename Ns>
    void parse_token_def(Ts& types,Ns& names,const std::string_view def){
        std::string_view::const_iterator b = def.begin();
        const std::string_view::const_iterator e = def.end();
        std::string_view::const_iterator i=b;

        std::string_view type;
        while(i!=e){
            switch(*i){
                case ':':
                    if(i+1!=e && *(i+1)==':'){
                        i+=2;
                    }else{
                        type = std::string_view(b,yuki::remove_trailing_u8(b,i,yuki::unicode::is_WSpace<yuki::U8Char>));
                        ++i;
                        while(i!=e && yuki::unicode::is_WSpace(yuki::U8Char(i)))
                            i+=yuki::u8_length_byte(*i);
                        b=i;
                    }
                    continue;
                case ';':
                    types.emplace_back(type);
                    names.emplace_back(b,yuki::remove_trailing_u8(b,i,yuki::unicode::is_WSpace<yuki::U8Char>));
                    ++i;
                    while(i!=e && yuki::unicode::is_WSpace(yuki::U8Char(i)))
                        i+=yuki::u8_length_byte(*i);
                    b=i;
                    continue;
                default:
                    i+=yuki::u8_length_byte(*i);
                    continue;
            }
        }
    }

    template<bool is_term>
    void term(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){
        auto& terms = is_term ? data.token_datas.terms : data.token_datas.nterms;

        switch(arg_num){
            case 0:
                if(arg.front()=='\"' || arg.front()=='\''){
                    print_loc(stderr,lineno,colno,filename);
                    fprintf(stderr,"Warning: Token name begins with quote: %s\n",arg.data());
                }
                if(const auto it = data.token_datas.token_htable.find(arg); it==data.token_datas.token_htable.end()){
                    data.token_datas.token_htable.emplace(arg,Token_Datas::Coordinate{is_term,terms.size()});
                    data.term_valid=true;
                    terms.emplace_back(arg);
                }else{
                    print_loc(stderr,lineno,colno,filename);
                    fprintf(stderr,"Error: Redefinition of token: %s\n",arg.data());
                    data.advance_errors();
                    data.term_valid=false;
                }
                return;
            case 1:
                if(arg.front()=='\"' || arg.front()=='\''){
                    if(data.term_valid){
                        assert(!terms.empty());
                        const auto it = data.token_datas.token_htable.find(arg);
                        if(it==data.token_datas.token_htable.end()){
                            data.token_datas.token_htable.emplace(arg,Token_Datas::Coordinate{is_term,terms.size()-1});
                            terms.back().alias = arg;
                        }else{
                            print_loc(stderr,lineno,colno,filename);
                            fprintf(stderr,"Error: Duplicate token alias %s!\n",arg.data());
                            fprintf(stderr,"--Note: The alias is already used by: %s\n",data.token_datas[it->second].name.c_str());
                            data.advance_errors();
                        }
                    }else{
                        const auto it = data.token_datas.token_htable.find(arg);
                        if(it!=data.token_datas.token_htable.end()){
                            print_loc(stderr,lineno,colno,filename);
                            fprintf(stderr,"Error: Duplicate token alias %s!\n",arg.data());
                            fprintf(stderr,"--Note: The alias is already used by: %s\n",data.token_datas[it->second].name.c_str());
                            data.advance_errors();
                        }
                    }
                }else{
                    if(data.term_valid){
                        assert(!terms.empty());
                        assert(terms.back().type.empty());
                        terms.back().type = arg;
                    }
                }
                return;
            case 2:
                if(arg.front()=='\"' || arg.front()=='\''){
                    print_loc(stderr,lineno,colno,filename);
                    fprintf(stderr,"Warning: Token value type begins with quote: %s\n",arg.data());
                }
                if(data.term_valid){
                    assert(!terms.empty());
                    if(terms.back().type.empty())
                        terms.back().type = arg;
                    else{
                        print_loc(stderr,lineno,colno,filename);
                        fprintf(stderr,"Error: Token value type redeclared for token: %s\n",terms.back().name.c_str());
                        fprintf(stderr,"--Note: Previously declared type: %s . Currently declared type: %s\n",terms.back().type.c_str(),arg.data());
                        data.advance_errors();
                    }
                }
                return;
            default:
                print_loc(stderr,lineno,colno,filename);
                fprintf(stderr,"Warning: %%(n)term with extraneous argument: %s\n",arg.data());
                return;
        }
    } // term

    template<bool is_term>
    void terms(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){
        auto& terms = is_term ? data.token_datas.terms : data.token_datas.nterms;

        if(arg_num==0)
            data.term_valid=false;

        if(arg.front()=='\"' || arg.front()=='\''){
            if(data.term_valid){
                assert(!terms.empty());
                const auto it = data.token_datas.token_htable.find(arg);
                if(it==data.token_datas.token_htable.end()){
                    data.token_datas.token_htable.emplace(arg,Token_Datas::Coordinate{is_term,terms.size()-1});
                    if(terms.back().alias.empty())
                        terms.back().alias = arg;
                    else{
                        print_loc(stderr,lineno,colno,filename);
                        fprintf(stderr,"Error: Token alias redeclared for token: %s\n",terms.back().name.c_str());
                        fprintf(stderr,"--Note: Previously declared alias: %s. Currently declared alias: %s.\n",terms.back().alias.c_str(),arg.data());
                        data.advance_errors();
                    }
                }else{
                    print_loc(stderr,lineno,colno,filename);
                    fprintf(stderr,"Error: Duplicate token alias %s!\n",arg.data());
                    fprintf(stderr,"--Note: The alias is already used by: %s\n",data.token_datas[it->second].name.c_str());
                    data.advance_errors();
                }
            }else{
                const auto it = data.token_datas.token_htable.find(arg);
                if(it!=data.token_datas.token_htable.end()){
                    print_loc(stderr,lineno,colno,filename);
                    fprintf(stderr,"Error: Duplicate token alias %s!\n",arg.data());
                    fprintf(stderr,"--Note: The alias is already used by: %s\n",data.token_datas[it->second].name.c_str());
                    data.advance_errors();
                }
            }
        }else{
            const auto it = data.token_datas.token_htable.find(arg);
            if(it==data.token_datas.token_htable.end()){
                data.token_datas.token_htable.emplace(arg,Token_Datas::Coordinate{is_term,terms.size()});
                data.term_valid=true;
                terms.emplace_back(arg);
            }else{
                print_loc(stderr,lineno,colno,filename);
                fprintf(stderr,"Error: Redefinition of token: %s\n",arg.data());
                data.advance_errors();
                data.term_valid=false;
            }
        }
    } // terms

    inline void code(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){
        switch(arg_num){
            case 0:
                for(const Sec0_Data::QC_Pair pair : data.code_qualifiers){
                    if(arg==pair.qualifier){
                        data.current_code=pair.cat;
                        return;
                    }
                }
                data.current_code=Sec0_Data::Code_Cat::NIL_;
                print_loc(stderr,lineno,colno,filename);
                fputs("Error: Unknown code qualifier",stderr);
                data.advance_errors();
                if(arg.size()<=32){
                    fputc(static_cast<unsigned char>('\"'),stderr);
                    fputs(arg.data(),stderr);
                    fputc(static_cast<unsigned char>('\"'),stderr);
                }
                fputs("!\n",stderr);
                return;
            case 1:
                if(data.current_code!=Sec0_Data::Code_Cat::NIL_){
                    data.codes[data.current_code].append(arg);
                    data.current_code=Sec0_Data::Code_Cat::NIL_;
                }else{
                    print_loc(stderr,lineno,colno,filename);
                    fputs("Error: Missing code qualifier!\n",stderr);
                    data.advance_errors();
                }
                if(arg.empty()){
                    print_loc(stderr,lineno,colno,filename);
                    fputs("Warning: Empty %code section!\n",stderr);
                }
                return;
            default:
                print_loc(stderr,lineno,colno,filename);
                fprintf(stderr,"Warning: %%code with extraneous argument: %s\n",arg.data());
                return;
        }
    }

    struct Sec0_Entry{
        std::string_view key;
        void (*action)(Sec0_Data&,size_t,std::string_view,size_t,size_t,const char*);
        bool arg_required;

        struct SV{
            static constexpr std::string_view operator()(const Sec0_Entry se) {return se.key;}
        };

        struct Nil{
            static constexpr bool operator()(const Sec0_Entry se) {return !se.action;}
        };
    };
} // namespace sec0_impl




constexpr yuki::CHashTable_Str<sec0_impl::Sec0_Entry,sec0_impl::Sec0_Entry::SV,sec0_impl::Sec0_Entry::Nil,128> sec0_actions = {
    {"plr1",sec0_impl::plr1,false},
    {"clr1",sec0_impl::clr1,false},
    {"lalr1",sec0_impl::lalr1,false},
    {"variant_token",sec0_impl::variant_token,false},
    //{"tuple_token",sec0_impl::tuple_token},
    {"no_default_ctor",sec0_impl::no_default_ctor,false},
    {"namespace",sec0_impl::nspace,true},
    {"parser",sec0_impl::parser,true},
    {"token_settings",sec0_impl::token_settings,true},
    {"lexer",sec0_impl::lexer,true},
    {"debug_prefix",sec0_impl::debug_prefix,true},
    {"simple_token",sec0_impl::simple_token,true},
    {"default_left",sec0_impl::default_left,false},
    {"default_right",sec0_impl::default_right,false},
    {"left",sec0_impl::right_or_left<Assoc::LEFT>,true},
    {"right",sec0_impl::right_or_left<Assoc::RIGHT>,true},
    {"prec",sec0_impl::prec,true},
    {"term",sec0_impl::term<true>,true},
    {"nterm",sec0_impl::term<false>,true},
    {"terms",sec0_impl::terms<true>,true},
    {"nterms",sec0_impl::terms<false>,true},
    {"code",sec0_impl::code,true},
    {"__debug",sec0_impl::debug,false},
};


inline Sec0_Data parse_sec0(FILE* const in,const char* const filename,const unsigned max_errors){
    Sec0_Data data;
    data.max_errors=max_errors;

    yuki::U8Char u8c;

    using namespace yuki::literals;

  skip_spaces_first:
    do{
        u8c=data.input.get(in);
    }while(yuki::unicode::is_WSpace(u8c));
    switch(u8c.raw()){
        case yuki::EOF_U8.raw():
            eof_error(data.errors()+1);
        case '/'_u8.raw():
            switch(data.input.try_skip_comment(in)){
                case EOF:
                    eof_error(data.errors()+1);
                case 0:
                    print_loc(stderr,data.input.lineno_orig,data.input.colno_orig,filename);
                    fputs("Error: Section 0 does not begin with \'%\'!\n",stderr);
                    data.advance_errors();
                    break;
                case static_cast<unsigned char>('\n'):
                    goto skip_spaces_first;
            }
            break;
        case '%'_u8.raw():
            break;
        default:
            print_loc(stderr,data.input.lineno_orig,data.input.colno_orig,filename);
            fputs("Error: Section 0 does not begin with \'%\'!\n",stderr);
            data.advance_errors();
            break;
    }

    std::string arg;

  head:
    const size_t head_lineno = data.input.lineno_orig;
    const size_t head_colno = data.input.colno_orig;
    u8c=data.input.get(in);
    if(u8c=='%'_u8)
        return data;
    for(; !yuki::unicode::is_WSpace(u8c); u8c.write_to(arg),u8c=data.input.get(in)){
        if(u8c=='/'_u8){
            switch(data.input.try_skip_comment(in)){
                case EOF: eof_error(data.errors()+1);
                case 0: break;
                case static_cast<unsigned char>('\n'): goto head_done;
            }
        }else if(u8c==yuki::EOF_U8)
            eof_error(data.errors()+1);
    }
  head_done:
    const sec0_impl::Sec0_Entry* const sep=sec0_actions.find(arg);
    const sec0_impl::Sec0_Entry se = sep ? *sep : sec0_impl::Sec0_Entry{"",nullptr,false};
    if(!sep){
        print_loc(stderr,head_lineno,head_colno,filename);
        fprintf(stderr,"Error: Unknown section 0 directive \"%s\"!\n",arg.c_str());
        data.advance_errors();
    }
    arg.clear();
    size_t arg_current=0;
  skip_spaces:
    do{
        u8c=data.input.get(in);
    }while(yuki::unicode::is_WSpace(u8c));
    if(u8c=='/'_u8){
        switch(data.input.try_skip_comment(in)){
            case EOF: eof_error(data.errors()+1);
            case 0: break;
            case static_cast<unsigned char>('\n'): goto skip_spaces;
        }
    }else if(u8c==yuki::EOF_U8)
        eof_error(data.errors()+1);
  parse_arg:
    const size_t lineno = data.input.lineno_orig;
    const size_t colno = data.input.colno_orig;
    switch(u8c.raw()){
        case '\"'_u8.raw():
            switch(data.input.parse_quoted<'\"'>(in,arg,filename)){
                case EOF: data.advance_errors(); eof_error(data.errors()+1);
                case '\"'_uc: u8c=data.input.get(in); goto shipout_1_arg;
                default: assert(false); std::unreachable();
            }
        case '\''_u8.raw():
            switch(data.input.parse_quoted<'\''>(in,arg,filename)){
                case EOF: data.advance_errors(); eof_error(data.errors()+1);
                case '\''_uc: u8c=data.input.get(in); goto shipout_1_arg;
                default: assert(false); std::unreachable();
            }
        case '{'_u8.raw():{
            unsigned brace_level=1;
            do{
                u8c=data.input.get(in);
            }while(yuki::unicode::is_WSpace(u8c));
            while(1){
                switch(u8c.raw()){
                    case yuki::EOF_U8.raw():
                        print_loc(stderr,data.input.lineno,data.input.colno,filename);
                        fputs("Error: Missing closing brace!\n",stderr);
                        eof_error(data.errors()+1);
                    case '}'_u8.raw():
                        u8c=data.input.get(in);
                        if(--brace_level==0)
                            goto shipout_1_arg;
                        else{
                            arg.push_back('}');
                            break;
                        }
                    default:
                        u8c.write_to(arg);
                        u8c=data.input.get(in);
                        break;
                }
            }
        }
        case '%'_u8.raw(): goto shipout;
        default:
            assert(arg.empty());
            while(1){
                u8c.write_to(arg);
                u8c=data.input.get(in);
                switch(u8c.raw()){
                    case yuki::EOF_U8.raw(): eof_error(data.errors()+1);
                    case '/'_u8.raw():
                        switch(data.input.try_skip_comment(in)){
                            case EOF: eof_error(data.errors()+1);
                            case 0: break;
                            case static_cast<unsigned char>('\n'): goto shipout_1_arg;
                        }
                        break;
                    default:
                        if(yuki::unicode::is_WSpace(u8c)){
                            goto shipout_1_arg;
                        }else
                            break;
                    case '{'_u8.raw():case '\"'_u8.raw():case '\''_u8.raw(): goto shipout_1_arg;
                    case '%'_u8.raw(): goto shipout;
                }
            }
    } // switch(u8c.raw())
    assert(false);
  shipout_1_arg:
    if(sep)
        se.action(data,arg_current,arg,lineno,colno,filename);
    arg.clear();
    ++arg_current;
    if(yuki::unicode::is_WSpace(u8c))
        goto skip_spaces;
    else
        goto parse_arg;
  shipout:
    if(arg_current==0 && sep && se.arg_required){
        print_loc(stderr,head_lineno,head_colno,filename);
        fprintf(stderr,"Warning: %%%s without argument(s)!\n",se.key.data());
    }
    goto head;
}
} // namespace yuki::pg
