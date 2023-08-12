#pragma once
#include<unordered_map>
#include<yuki/CHashTable_Str.hpp>
#include<yuki/uchar.hpp>
#include<yuki/unicode/binary_properties.h>
#include"common.hpp"

namespace yuki::pg{

namespace sec0_impl{
    constexpr void noop(Sec0_Data&,Str_Loc*,unsigned,size_t,size_t,const char*) {}

    inline void debug(Sec0_Data&,Str_Loc* args,unsigned args_size,const size_t lineno,const size_t colno,const char* const filename){
        fprintf(stderr,"%zu:%zu:%u - %s - %%__debug\n",lineno,colno,args_size,filename);
        for(;args_size!=0;++args,--args_size)
            fprintf(stderr,"--%zu:%zu %s\n",args->lineno,args->colno,args->str.c_str());
    }

    #define YUKI_PG_META_ML0_ZERO_PARAM(name,...) \
    inline void name(Sec0_Data& data,Str_Loc* const,unsigned const args_size,const size_t lineno,const size_t colno,const char* const filename){ \
        if(args_size!=0){ \
            fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename); \
            fputs("Warning: %" #name " does not expect any arguments!\n",stderr); \
        } \
        __VA_ARGS__ \
    }

    YUKI_PG_META_ML0_ZERO_PARAM(variant_token,data.token_impl_type=Sec0_Data::Token_Impl_Type::VARIANT;)
    YUKI_PG_META_ML0_ZERO_PARAM(tuple_token,data.token_impl_type=Sec0_Data::Token_Impl_Type::TUPLE;)

    YUKI_PG_META_ML0_ZERO_PARAM(plr1,data.alg_type=Sec0_Data::Alg_Type::PLR1;)
    YUKI_PG_META_ML0_ZERO_PARAM(clr1,data.alg_type=Sec0_Data::Alg_Type::CLR1;)
    YUKI_PG_META_ML0_ZERO_PARAM(lalr1,data.alg_type=Sec0_Data::Alg_Type::LALR1;)

    YUKI_PG_META_ML0_ZERO_PARAM(action_switch,data.is_switch=true;)
    YUKI_PG_META_ML0_ZERO_PARAM(action_array,data.is_switch=false;)

    YUKI_PG_META_ML0_ZERO_PARAM(no_final_function,data.no_final_function=true;)
    YUKI_PG_META_ML0_ZERO_PARAM(no_final_class,data.no_final_class=true;)
    YUKI_PG_META_ML0_ZERO_PARAM(no_default_ctor,data.no_default_ctor=true;)

    YUKI_PG_META_ML0_ZERO_PARAM(default_left,data.assoc0=Assoc::LEFT;)
    YUKI_PG_META_ML0_ZERO_PARAM(default_right,data.assoc0=Assoc::RIGHT;)

    #define YUKI_PG_META_ML0_ONE_PARAM(name,...) \
    inline void name(Sec0_Data& data,Str_Loc* const args,unsigned const args_size,const size_t lineno,const size_t colno,const char* const filename){ \
        switch(args_size){ \
            case 0: \
                fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename); \
                fputs("Warning: %" #name "without argument!\n",stderr); \
                return; \
            default: \
                fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename); \
                fputs("Error: %" #name "with more than one arguments! (Note: Did you mistype some spaces?)\n",stderr); \
                ++data.errors; \
                [[fallthrough]]; \
            case 1: \
                __VA_ARGS__ \
                return; \
        } \
    }

    YUKI_PG_META_ML0_ONE_PARAM(nspace,data.nspace=std::move(args->str);)
    YUKI_PG_META_ML0_ONE_PARAM(parser,data.parser=std::move(args->str);)
    YUKI_PG_META_ML0_ONE_PARAM(ts,data.ts=std::move(args->str);)
    YUKI_PG_META_ML0_ONE_PARAM(lexer,data.lexer=std::move(args->str);)
    YUKI_PG_META_ML0_ONE_PARAM(pt,data.parser_tables=std::move(args->str);)

    YUKI_PG_META_ML0_ONE_PARAM(simple_token, data.sp_token=std::move(args->str); data.token_impl_type=Sec0_Data::Token_Impl_Type::SIMPLE;)

    template<Assoc assoc,bool continued>
    void right_or_left(Sec0_Data& data,Str_Loc* args,unsigned args_size,const size_t lineno,const size_t colno,const char* const filename){
        if(args_size==0){
            fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename);
            fputs("Warning: Empty %left/%right declaration!\n",stderr);
        }
        if constexpr(!continued)
            ++data.current_prec;
        for(;args_size!=0;++args,--args_size){
            try{
                const Token_Datas::Coordinate co = data.token_datas.at(args->str);
                if(!co.is_term){
                    fprintf(stderr,"%zu:%zu - %s\n",args->lineno,args->colno,filename);
                    fprintf(stderr,"Warning: %%left/%%right for non-terminals makes no sense: %s\n",args->str.c_str());
                }else{
                    Token_Data& td = data.token_datas[co];
                    td.prec = data.current_prec;
                    td.assoc = assoc;
                }
            }catch(const std::out_of_range&){
                fprintf(stderr,"%zu:%zu - %s\n",args->lineno,args->colno,filename);
                fprintf(stderr,"Error: Unknown token name encountered while parsing %%left/%%right declaration: %s\n",args->str.c_str());
                ++data.errors;
            }
        }
    }

    template<bool continued>
    void prec(Sec0_Data& data,Str_Loc* args,unsigned args_size,const size_t lineno,const size_t colno,const char* const filename){
        if(args_size==0){
            fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename);
            fputs("Warning: Empty %prec declaration!\n",stderr);
        }
        if constexpr(!continued)
            ++data.current_prec;
        for(;args_size!=0;++args,--args_size){
            try{
                const Token_Datas::Coordinate co = data.token_datas.at(args->str);
                if(!co.is_term){
                    fprintf(stderr,"%zu:%zu - %s\n",args->lineno,args->colno,filename);
                    fprintf(stderr,"Warning: %%prec for non-terminals makes no sense: %s\n",args->str.c_str());
                }else{
                    data.token_datas[co].prec = data.current_prec;
                }
            }catch(const std::out_of_range&){
                fprintf(stderr,"%zu:%zu - %s\n",args->lineno,args->colno,filename);
                fprintf(stderr,"Error: Unknown token name encountered while parsing %%prec declaration: %s\n",args->str.c_str());
                ++data.errors;
            }
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
    void term(Sec0_Data& data,Str_Loc* const args,unsigned args_size,const size_t lineno,const size_t colno,const char* const filename){
        auto& terms = is_term ? data.token_datas.terms : data.token_datas.nterms;

        auto redef_error = [&data,filename,&terms](const Str_Loc& str_loc){
            if(!data.token_datas.token_htable.try_emplace(str_loc.str,is_term,terms.size()).second){
                fprintf(stderr,"%zu:%zu - %s\n",str_loc.lineno,str_loc.colno,filename);
                fprintf(stderr,"Error: Redefinition of token \"%s\"!\n",str_loc.str.c_str());
                ++data.errors;
                return;
            }
        };

        // %(n)term accepts at most 4 arguments: (N)ame, (A)lias, (D)ef, a(L)loc.
        switch(args_size){
            case 0:
                fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename);
                fputs("Warning: %(n)term requires at least 1 argument!\n",stderr);
                return;
            case 1:
                redef_error(*args);
                terms.emplace_back(std::move(args->str));
                return;
            case 2:
                redef_error(*args);
                if(args[1].str.front()=='\"'){
                    // NA
                    redef_error(args[1]);
                    terms.emplace_back(std::move(args->str),std::move(args[1].str));
                }else{
                    // ND
                    terms.emplace_back(std::move(args->str));
                    parse_token_def(terms.back().types,terms.back().names,args[1].str);
                }
                return;
            case 3:
                redef_error(*args);
                if(args[1].str.front()=='\"'){
                    // NAD
                    redef_error(args[1]);
                    terms.emplace_back(std::move(args->str),std::move(args[1].str));
                    parse_token_def(terms.back().types,terms.back().names,args[2].str);
                }else{
                    // NDL
                    terms.emplace_back(std::move(args->str),std::string(),std::move(args[2].str));
                    parse_token_def(terms.back().types,terms.back().names,args[1].str);
                }
                return;
            default:
                redef_error(*args);
                redef_error(args[1]);
                terms.emplace_back(std::move(args->str),std::move(args[1].str),std::move(args[3].str));
                parse_token_def(terms.back().types,terms.back().names,args[2].str);
                for(unsigned i=4;i<args_size;++i)
                    terms.back().alloc.append(args[i].str);
                return;
        }
    } // term

    inline void code(Sec0_Data& data,Str_Loc* const args,const unsigned args_size,const size_t lineno,const size_t colno,const char* const filename){
        switch(args_size){
            case 0:
            case 1:
                fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename);
                fputs("Warning: Empty %code directive!\n",stderr);
                return;
            default:
                for(Sec0_Data::Code& code : data.codes){
                    if(code.qualifier==args->str){
                        for(unsigned i=1;i<args_size;++i){
                            args[i].str.resize(yuki::remove_trailing(args[i].str.begin(),args[i].str.end(),[](const char c){return isspace(static_cast<unsigned char>(c));}));
                            if(code.contents.empty())
                                code.contents = std::move(args[i].str);
                            else
                                code.contents.append(args[i].str);
                        }
                        return;
                    }
                }
                fprintf(stderr,"%zu:%zu - %s\n",args->lineno,args->colno,filename);
                fprintf(stderr,"Error: Unknown code qualifier \"%s\"!\n",args->str.c_str());
                return;
        }
    }

    struct Sec0_Entry{
        std::string_view key;
        void (*action)(Sec0_Data&,Str_Loc*,unsigned,size_t,size_t,const char*);

        struct SV{
            static constexpr std::string_view operator()(const Sec0_Entry se) {return se.key;}
        };

        struct Nil{
            static constexpr bool operator()(const Sec0_Entry se) {return !se.action;}
        };
    };

    [[noreturn]]
    inline void eof_error(const size_t lineno,const size_t colno,const char* const filename){
        fprintf(stderr,"%zu:%zu - %s\n",lineno,colno,filename);
        fputs("Fatal Error: EOF encountered while parsing section 0!\n",stderr);
        exit(EXIT_FAILURE);
    }
} // namespace sec0_impl




constexpr yuki::CHashTable_Str<sec0_impl::Sec0_Entry,sec0_impl::Sec0_Entry::SV,sec0_impl::Sec0_Entry::Nil,128> sec0_actions = {
    {"plr1",sec0_impl::plr1},
    {"clr1",sec0_impl::clr1},
    {"lalr1",sec0_impl::lalr1},
    {"variant_token",sec0_impl::variant_token},
    {"tuple_token",sec0_impl::tuple_token},
    {"action_switch",sec0_impl::action_switch},
    {"action_array",sec0_impl::action_array},
    {"no_final_function",sec0_impl::no_final_function},
    {"no_final_class",sec0_impl::no_final_class},
    {"no_default_ctor",sec0_impl::no_default_ctor},
    {"namespace",sec0_impl::nspace},
    {"parser",sec0_impl::parser},
    {"token_settings",sec0_impl::ts},
    {"lexer",sec0_impl::lexer},
    {"parser_tables",sec0_impl::pt},
    {"simple_token",sec0_impl::simple_token},
    {"default_left",sec0_impl::default_left},
    {"default_right",sec0_impl::default_right},
    {"left",sec0_impl::right_or_left<Assoc::LEFT,false>},
    {"right",sec0_impl::right_or_left<Assoc::RIGHT,false>},
    {"prec",sec0_impl::prec<false>},
    {"-left",sec0_impl::right_or_left<Assoc::LEFT,true>},
    {"-right",sec0_impl::right_or_left<Assoc::RIGHT,true>},
    {"-prec",sec0_impl::prec<true>},
    {"term",sec0_impl::term<true>},
    {"nterm",sec0_impl::term<false>},
    {"code",sec0_impl::code},
    {"__debug",sec0_impl::debug},
};


inline Sec0_Data parse_sec0(FILE* const in,const char* const filename){
    Sec0_Data data;

    yuki::U8Char u8c;

    using namespace yuki::literals;

  skip_spaces_first:
    u8c=data.input.get(in);
    while(yuki::unicode::is_WSpace(u8c))
        u8c=data.input.get(in);
    switch(u8c.raw()){
        case yuki::EOF_U8.raw():
            sec0_impl::eof_error(data.input.lineno,data.input.colno,filename);
        case '/'_u8.raw():
            switch(data.input.try_skip_comment(in)){
                case EOF:
                    sec0_impl::eof_error(data.input.lineno,0,filename);
                case 0:
                    fprintf(stderr,"%zu:%zu - %s\n",data.input.lineno_orig,data.input.colno_orig,filename);
                    fputs("Error: Section 0 does not begin with \'%\'!\n",stderr);
                    ++data.errors;
                    break;
                case static_cast<unsigned char>('\n'):
                    goto skip_spaces_first;
            }
            break;
        case '%'_u8.raw():
            break;
        default:
            fprintf(stderr,"%zu:%zu - %s\n",data.input.lineno_orig,data.input.colno_orig,filename);
            fputs("Error: Section 0 does not begin with \'%\'!\n",stderr);
            ++data.errors;
            break;
    }

    #ifndef YUKI_PG_SEC0_MAX_AGRS
    #define YUKI_PG_SEC0_MAX_AGRS 32
    #endif

    Str_Loc args[1+YUKI_PG_SEC0_MAX_AGRS];
    unsigned arg_current=0;

    constexpr size_t args_total = 1+YUKI_PG_SEC0_MAX_AGRS;

    auto shipout = [&args,&arg_current](Sec0_Data& data,const char* const filename){
        assert(arg_current>0 || !args[0].str.empty());
        assert(arg_current<=sizeof(args)/sizeof(args[0]));
        if(const sec0_impl::Sec0_Entry* const se=sec0_actions.find(args[0].str); se){
            se->action(
                data,
                args+1,
                arg_current>=args_total || args[arg_current].str.empty()?arg_current-1:arg_current,
                args[0].lineno,
                args[0].colno,
                filename
            );
            arg_current=0;
        }else{
            fprintf(stderr,"%zu:%zu - %s\n",args[0].lineno,args[0].colno,filename);
            fprintf(stderr,"Error: Unknown section 0 directive \"%s\"!\n",args[0].str.c_str());
            ++data.errors;
            arg_current=0;
        }
    };

    bool quote_mode=false;
    unsigned brace_level=0;
    bool skip_mode=false;
  skip_spaces:
    u8c=data.input.get(in);
    while(yuki::unicode::is_WSpace(u8c))
        u8c=data.input.get(in);
    switch(u8c.raw()){
        case yuki::EOF_U8.raw():
            sec0_impl::eof_error(data.input.lineno,data.input.colno,filename);
        case '/'_u8.raw():
            switch(data.input.try_skip_comment(in)){
                case EOF: sec0_impl::eof_error(data.input.lineno,0,filename);
                case static_cast<unsigned char>('\n'): goto skip_spaces;
            }
            break;
    }
    if(arg_current<args_total)
        args[arg_current].str.clear();
  parse:
    switch(u8c.raw()){
        case yuki::EOF_U8.raw():
            args_eof:
            shipout(data,filename);
            sec0_impl::eof_error(data.input.lineno,data.input.colno,filename);
        case '%'_u8.raw():
            if(!quote_mode && brace_level==0){
                if(args[0].str.empty())
                    return data;
                shipout(data,filename);
                skip_mode=false;
                goto skip_spaces;
            }else
                goto write_to_args_and_continue;
        case '\"'_u8.raw():
        case '\''_u8.raw():
            if(quote_mode){
                quote_mode=false;
                assert(arg_current<=args_total);
                if(!skip_mode){
                    if(arg_current<args_total){
                        u8c.write_to(args[arg_current].str);
                    }else
                        goto args_limit_error;
                }
                goto find_next_arg;
            }else if(brace_level==0){
                quote_mode=true;
                assert(arg_current<=args_total);
                if(arg_current<args_total && !args[arg_current].str.empty())
                    ++arg_current;
                if(arg_current<args_total){
                    args[arg_current].lineno=data.input.lineno_orig;
                    args[arg_current].colno=data.input.colno_orig;
                    args[arg_current].str.clear();
                }
            }
            goto write_to_args_and_continue;
        case '{'_u8.raw():
            if(!quote_mode && ++brace_level==1)
                goto find_next_arg;
            goto write_to_args_and_continue;
        case '}'_u8.raw():
            if(!quote_mode){
                switch(brace_level){
                    case 0:
                        fprintf(stderr,"%zu:%zu - %s\n",data.input.lineno_orig,data.input.colno_orig,filename);
                        fputs("Error: Unpaired closing brace!\n",stderr);
                        ++data.errors;
                        goto find_next_arg;
                    case 1:
                        --brace_level;
                        goto find_next_arg;
                    default:
                        --brace_level;
                        goto write_to_args_and_continue;
                }
            }
            goto write_to_args_and_continue;
        case '/'_u8.raw():
            switch(data.input.try_skip_comment(in)){
                case EOF: goto args_eof;
                case 0: goto write_to_args_and_continue;
                case static_cast<unsigned char>('\n'): u8c=U'\n'; goto args_space;
            }
            break;
        default:
            if(!yuki::unicode::is_WSpace(u8c)){
                goto write_to_args_and_continue;
            }else{
                args_space:
                if(quote_mode || brace_level!=0)
                    goto write_to_args_and_continue;
                else{
                    find_next_arg:
                    assert(arg_current<=args_total);
                    if(arg_current<args_total && !args[arg_current].str.empty())
                        ++arg_current;
                    goto skip_spaces;
                }
            }
    } // switch(u8c.raw())
  write_to_args_and_continue:
    assert(arg_current<=args_total);
    if(!skip_mode){
        if(arg_current<args_total){
            if(args[arg_current].str.empty()){
                args[arg_current].lineno=data.input.lineno_orig;
                args[arg_current].colno=data.input.colno_orig;
            }
            u8c.write_to(args[arg_current].str);
        }else{
            args_limit_error:
            fprintf(stderr,"%zu:%zu - %s\n",data.input.lineno_orig,data.input.colno_orig,filename);
            fprintf(stderr,"Error: Argument number exceeds implementation limit %d!\n",YUKI_PG_SEC0_MAX_AGRS);
            ++data.errors;
            skip_mode=true;
        }
    }
    u8c=data.input.get(in);
    goto parse;
}
} // namespace yuki::pg
