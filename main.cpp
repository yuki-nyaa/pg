#include"cconfig"
#include<unordered_map>
#include<yuki/CHashTable_Str.hpp>
#include<yuki/uchar.hpp>
#include<yuki/unicode/binary_properties.h>
#include<cctype>
#include"common.hpp"

namespace yuki::pg{

namespace sec0_impl{
    inline void debug(Sec0_Data&,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){
        if(arg_num==0)
            fprintf(stderr,"%s - %%__debug\n",filename);
        fprintf(stderr,"(%zu)--%zu:%zu %s\n",arg_num,lineno,colno,arg.data());
    }

    #define YUKI_PG_META_S0_ZERO_PARAM(name,...) \
    inline void name(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){ \
        if(arg_num>0){ \
            print_loc(stderr,lineno,colno,filename); \
            fprintf(stderr,"Warning: %" #name "with extraneous argument: %s\n",arg.data()); \
        } \
        __VA_ARGS__ \
    }

    YUKI_PG_META_S0_ZERO_PARAM(variant_token,data.token_impl_type=Sec0_Data::Token_Impl_Type::VARIANT;)
    //YUKI_PG_META_S0_ZERO_PARAM(tuple_token,data.token_impl_type=Sec0_Data::Token_Impl_Type::TUPLE;)

    YUKI_PG_META_S0_ZERO_PARAM(plr1,data.alg_type=Sec0_Data::Alg_Type::PLR1;)
    YUKI_PG_META_S0_ZERO_PARAM(clr1,data.alg_type=Sec0_Data::Alg_Type::CLR1;)
    YUKI_PG_META_S0_ZERO_PARAM(lalr1,data.alg_type=Sec0_Data::Alg_Type::LALR1;)

    YUKI_PG_META_S0_ZERO_PARAM(no_default_ctor,data.no_default_ctor=true;)

    YUKI_PG_META_S0_ZERO_PARAM(default_left,data.assoc0=Assoc::LEFT;)
    YUKI_PG_META_S0_ZERO_PARAM(default_right,data.assoc0=Assoc::RIGHT;)

    #define YUKI_PG_META_S0_ONE_PARAM(name,...) \
    inline void name(Sec0_Data& data,const size_t arg_num,const std::string_view arg,const size_t lineno,const size_t colno,const char* const filename){ \
        if(arg_num>1){ \
            print_loc(stderr,lineno,colno,filename); \
            fprintf(stderr,"Warning: %" #name "with extraneous argument: %s\n",arg.data()); \
            return; \
        } \
        __VA_ARGS__ \
    }

    YUKI_PG_META_S0_ONE_PARAM(nspace,data.nspace=arg;)
    YUKI_PG_META_S0_ONE_PARAM(parser,data.parser=arg;)
    YUKI_PG_META_S0_ONE_PARAM(token_settings,data.ts=arg;)
    YUKI_PG_META_S0_ONE_PARAM(lexer,data.lexer=arg;)
    YUKI_PG_META_S0_ONE_PARAM(debug_prefix,data.debug_prefix=arg;)

    YUKI_PG_META_S0_ONE_PARAM(simple_token, data.sp_token=arg; data.token_impl_type=Sec0_Data::Token_Impl_Type::SIMPLE;)

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


[[noreturn]] void eof_error(const unsigned errors){
    fputs("Fatal Error: No production was specified!\n",stderr);
    fprintf(stderr,"%u errors encountered.\n",errors);
    exit(EXIT_FAILURE);
}


Sec0_Data parse_sec0(FILE* const in,const char* const filename,const unsigned max_errors){
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
                default:
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
                default: goto head_done;
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
            default: goto skip_spaces;
        }
    }else if(u8c==yuki::EOF_U8)
        eof_error(data.errors()+1);
  parse_arg:
    const size_t lineno = data.input.lineno_orig;
    const size_t colno = data.input.colno_orig;
    switch(u8c.raw()){
        case '\"'_u8.raw():
            assert(arg.empty());
            switch(data.input.parse_quoted<'\"'>(in,arg,filename)){
                case EOF: data.advance_errors(); eof_error(data.errors()+1);
                case '\"'_uc: u8c=data.input.get(in); goto shipout_1_arg;
                default: assert(false); std::unreachable();
            }
        case '\''_u8.raw():
            assert(arg.empty());
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
                    case '/'_u8.raw():
                        switch(data.input.try_skip_comment(in)){
                            case EOF:
                                print_loc(stderr,data.input.lineno,data.input.colno,filename);
                                fputs("Error: Missing closing brace!\n",stderr);
                                eof_error(data.errors()+1);
                            case 0: arg.push_back('/'); break;
                            case static_cast<unsigned char>('\n'): arg.push_back('\n'); break;
                            case static_cast<unsigned char>('/'): arg.push_back(' '); break;
                        }
                        u8c=data.input.get(in);
                        break;
                    case '\"'_u8.raw():
                        switch(data.input.parse_quoted<'\"'>(in,arg,filename)){
                            case EOF: eof_error(data.errors()+1);
                            case '\"'_uc: u8c=data.input.get(in); break;
                            default: assert(false); std::unreachable();
                        }
                        break;
                    case '\''_u8.raw():
                        switch(data.input.parse_quoted<'\''>(in,arg,filename)){
                            case EOF: eof_error(data.errors()+1);
                            case '\''_uc: u8c=data.input.get(in); break;
                            default: assert(false); std::unreachable();
                        }
                        break;
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
                            default: goto shipout_1_arg;
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
} // parse_sec0




constexpr bool sv_eq(const std::string_view lhs,const std::string_view rhs) {return lhs==rhs;}

template<std::unsigned_integral Token_Kind_t>
Rule_Set<Token_Kind_t> parse_sec12(Sec0_Data& sec0_data,FILE* const in,const char* const filename){
    using namespace yuki::literals;

    Rule_Set<Token_Kind_t> rs;
    Rule<Token_Kind_t> rule;

    yuki::U8Char u8c;
    std::string str_temp;

  skip_spaces_before_left:
    do{
        u8c=sec0_data.input.get(in);
    }while(yuki::unicode::is_WSpace(u8c));
    switch(u8c.raw()){
        case yuki::EOF_U8.raw():
            return rs;
        case '/'_u8.raw():
            switch(sec0_data.input.try_skip_comment(in)){
                case EOF: return rs;
                case 0: break;
                default: goto skip_spaces_before_left;
            }
            break;
        case '%'_u8.raw():
            for(const Sec0_Data::QC_Pair pair : sec0_data.code_qualifiers){
                if(pair.qualifier=="SEC2_"){
                    std::string& code = sec0_data.codes[pair.cat];
                    sec0_data.input.get(in);
                    for(int i=fgetc(in); i!=EOF; i=fgetc(in))
                        code.push_back(static_cast<unsigned char>(i));
                    return rs;
                }
            }
    }
  left:
    const size_t left_lineno = sec0_data.input.lineno_orig;
    const size_t left_colno = sec0_data.input.colno_orig;
    u8c.write_to(str_temp);
    u8c=sec0_data.input.get(in);

    for(; !yuki::unicode::is_WSpace(u8c); u8c.write_to(str_temp),u8c=sec0_data.input.get(in)){
        if(u8c=='/'_u8){
            switch(sec0_data.input.try_skip_comment(in)){
                case EOF: goto left_done;
                case 0: break;
                default: goto left_done;
            }
        }else if(u8c==yuki::EOF_U8)
            goto left_done;
    }
  left_done:
    assert(!str_temp.empty());
    bool rule_invalid=false;
    if(sv_eq(str_temp,"Goal_")){
        rule.left=-1;
    }else{
        try{
            const Token_Datas::Coordinate co = sec0_data.token_datas.token_htable.at(str_temp);
            if(co.is_term){
                print_loc(stderr,left_lineno,left_colno,filename);
                fprintf(stderr,"Error: Terminal name on the lhs of a production: %s\n",str_temp.c_str());
                sec0_data.advance_errors();
                rule_invalid=true;
            }else{
                rule.left = sec0_data.token_datas.to_num<Token_Kind_t>(co);
            }
        }catch(const std::out_of_range&){
            print_loc(stderr,left_lineno,left_colno,filename);
            fprintf(stderr,"Error: Unknown token name on the lhs of a production: %s\n",str_temp.c_str());
            sec0_data.advance_errors();
            rule_invalid=true;
        }
    }
    str_temp.clear();
  skip_spaces0:
    size_t first_right_lineno, first_right_colno;
    yuki::U8Char percent=yuki::EOF_U8;
    size_t percent_lineno, percent_colno;
    do{
        u8c=sec0_data.input.get(in);
    }while(yuki::unicode::is_WSpace(u8c));
    if(u8c=='/'_u8){
        switch(sec0_data.input.try_skip_comment(in)){
            case EOF: u8c=yuki::EOF_U8;goto shipout;
            case 0: break;
            default: goto skip_spaces0;
        }
    }else if(u8c==yuki::EOF_U8)
        goto shipout;
    first_right_lineno = sec0_data.input.lineno_orig;
    first_right_colno = sec0_data.input.colno_orig;
  skip_spaces1:
    while(yuki::unicode::is_WSpace(u8c))
        u8c=sec0_data.input.get(in);
    if(u8c=='/'_u8){
        switch(sec0_data.input.try_skip_comment(in)){
            case EOF: rs.empty(),u8c=yuki::EOF_U8;goto shipout;
            case 0: break;
            default: u8c=sec0_data.input.get(in); goto skip_spaces1;
        }
    }else if(u8c==yuki::EOF_U8)
        goto shipout;
  right:
    {
    const size_t current_right_lineno = sec0_data.input.lineno_orig;
    const size_t current_right_colno = sec0_data.input.colno_orig;
    switch(u8c.raw()){
        case '%'_u8.raw():
            percent_lineno = sec0_data.input.lineno_orig;
            percent_colno = sec0_data.input.colno_orig;
            percent = (u8c=sec0_data.input.get(in));
            switch(percent.raw()){
                case 'r'_u8.raw():case 's'_u8.raw(): break;
                default:{
                    char buf[5] = {};
                    percent.write_to(buf);
                    print_loc(stderr,percent_lineno,percent_colno,filename);
                    fprintf(stderr,"Error: %%%s... directive is unknown!\n",buf);
                    sec0_data.advance_errors();
                    [[fallthrough]];
                }
                case 'e'_u8.raw(): percent=yuki::EOF_U8; break;
            }
            while(1){
                switch(u8c.raw()){
                    case yuki::EOF_U8.raw(): goto percent_arg_error;
                    case '/'_u8.raw():
                        switch(sec0_data.input.try_skip_comment(in)){
                            case EOF: u8c=yuki::EOF_U8;goto percent_arg_error;
                            case 0: break;
                            default: u8c=sec0_data.input.get(in); goto skip_spaces1;
                        }
                        break;
                    default:
                        if(yuki::unicode::is_WSpace(u8c)){
                            u8c=sec0_data.input.get(in);
                            goto skip_spaces1;
                        }else
                            break;
                    case '{'_u8.raw():case '\"'_u8.raw():case '\''_u8.raw():case '%'_u8.raw(): goto right;
                    case '|'_u8.raw(): case ';'_u8.raw(): goto percent_arg_error;
                }
                u8c=sec0_data.input.get(in);
            }
            assert(false);
          percent_arg_error:
            {
            char buf[5] = {};
            percent.write_to(buf);
            print_loc(stderr,sec0_data.input.lineno_orig,sec0_data.input.colno_orig,filename);
            fprintf(stderr,"Error: %%%s expects an argument but none was given!\n",buf);
            sec0_data.advance_errors();
            }
            percent=yuki::EOF_U8;
            goto shipout;
        case '\"'_u8.raw():
            assert(str_temp.empty());
            switch(sec0_data.input.parse_quoted<'\"'>(in,str_temp,filename)){
                case EOF: sec0_data.advance_errors(); str_temp.push_back('\"'); goto shipout;
                case '\"'_uc: u8c=sec0_data.input.get(in); goto write_to_rights;
                default: assert(false); std::unreachable();
            }
        case '\''_u8.raw():
            assert(str_temp.empty());
            switch(sec0_data.input.parse_quoted<'\''>(in,str_temp,filename)){
                case EOF: sec0_data.advance_errors(); str_temp.push_back('\''); goto shipout;
                case '\''_uc: u8c=sec0_data.input.get(in); goto write_to_rights;
                default: assert(false); std::unreachable();
            }
        case '{'_u8.raw():{
            unsigned brace_level=1;
            const bool is_init = rule.init.empty();
            std::string& target = is_init ? rule.init : rule.code;
            do{
                u8c=sec0_data.input.get(in);
            }while(yuki::unicode::is_WSpace(u8c));
            while(1){
                switch(u8c.raw()){
                    case yuki::EOF_U8.raw():
                        print_loc(stderr,sec0_data.input.lineno,sec0_data.input.colno,filename);
                        fputs("Error: Missing closing brace!\n",stderr);
                        sec0_data.advance_errors();
                        goto shipout;
                    case '}'_u8.raw():
                        u8c=sec0_data.input.get(in);
                        if(--brace_level==0)
                            goto init_or_code_done;
                        else{
                            target.push_back('}');
                            break;
                        }
                    case '/'_u8.raw():
                        switch(sec0_data.input.try_skip_comment(in)){
                            case EOF:
                                print_loc(stderr,sec0_data.input.lineno,sec0_data.input.colno,filename);
                                fputs("Error: Missing closing brace!\n",stderr);
                                sec0_data.advance_errors();
                                goto shipout;
                            case 0: target.push_back('/'); break;
                            case static_cast<unsigned char>('\n'): target.push_back('\n'); break;
                            case static_cast<unsigned char>('/'): target.push_back(' '); break;
                        }
                        u8c=sec0_data.input.get(in);
                        break;
                    case '\"'_u8.raw():
                        switch(sec0_data.input.parse_quoted<'\"'>(in,target,filename)){
                            case EOF: sec0_data.advance_errors(); target.push_back('\"'); goto shipout;
                            case '\"'_uc: u8c=sec0_data.input.get(in); break;
                            default: assert(false); std::unreachable();
                        }
                        break;
                    case '\''_u8.raw():
                        switch(sec0_data.input.parse_quoted<'\''>(in,target,filename)){
                            case EOF: sec0_data.advance_errors(); target.push_back('\''); goto shipout;
                            case '\''_uc: u8c=sec0_data.input.get(in); break;
                            default: assert(false); std::unreachable();
                        }
                        break;
                    case '$'_u8.raw():{
                        const size_t dollar_lineno = sec0_data.input.lineno_orig;
                        const size_t dollar_colno = sec0_data.input.colno_orig;
                        switch(u8c=sec0_data.input.get(in); u8c.raw()){
                            case yuki::EOF_U8.raw():
                                target.push_back('$');
                                print_loc(stderr,sec0_data.input.lineno,sec0_data.input.colno,filename);
                                fputs("Error: Missing closing brace!\n",stderr);
                                sec0_data.advance_errors();
                                goto shipout;
                            //case '{'_uc:
                            case '$'_u8.raw():
                                if(rule.left==(Token_Kind_t)-1){
                                    print_loc(stderr,dollar_lineno,dollar_colno,filename);
                                    fputs("Error: $$ in goal production!\n",stderr);
                                    sec0_data.advance_errors();
                                    target.push_back('0');
                                }else{
                                    target.append("/*target*/");
                                }
                                u8c=sec0_data.input.get(in);
                                break;
                            case '0'_u8.raw():case '1'_u8.raw():case '2'_u8.raw():case '3'_u8.raw():case '4'_u8.raw():case '5'_u8.raw():case '6'_u8.raw():case '7'_u8.raw():case '8'_u8.raw():case '9'_u8.raw():{
                                size_t num=0;
                                switch(u8c.raw()){
                                    case '0'_u8.raw(): break;
                                    case '1'_u8.raw(): num=1; break;
                                    case '2'_u8.raw(): num=2; break;
                                    case '3'_u8.raw(): num=3; break;
                                    case '4'_u8.raw(): num=4; break;
                                    case '5'_u8.raw(): num=5; break;
                                    case '6'_u8.raw(): num=6; break;
                                    case '7'_u8.raw(): num=7; break;
                                    case '8'_u8.raw(): num=8; break;
                                    case '9'_u8.raw(): num=9; break;
                                    default: assert(false); std::unreachable();
                                }
                                u8c=sec0_data.input.get(in);
                                while(1){
                                    switch(u8c.raw()){
                                        case '0'_u8.raw(): num*=10; break;
                                        case '1'_u8.raw(): num*=10; num+=1; break;
                                        case '2'_u8.raw(): num*=10; num+=2; break;
                                        case '3'_u8.raw(): num*=10; num+=3; break;
                                        case '4'_u8.raw(): num*=10; num+=4; break;
                                        case '5'_u8.raw(): num*=10; num+=5; break;
                                        case '6'_u8.raw(): num*=10; num+=6; break;
                                        case '7'_u8.raw(): num*=10; num+=7; break;
                                        case '8'_u8.raw(): num*=10; num+=8; break;
                                        case '9'_u8.raw(): num*=10; num+=9; break;
                                        default: goto num_parse_done;
                                    }
                                    u8c=sec0_data.input.get(in);
                                }
                              num_parse_done:
                                if(num>=rule.rights.size()){
                                    print_loc(stderr,dollar_lineno,dollar_colno,filename);
                                    fprintf(stderr,"Error: Token index out of range! (Note: rights.size()=%zu, given=%zu.)\n",rule.rights.size(),num);
                                    sec0_data.advance_errors();
                                    num=0;
                                }
                                target.append("/*rights[").append(std::to_string(num)).append("]*/");
                                break;
                            } // case '0'-'9'_uc
                            default:
                                target.push_back('$');
                                u8c.write_to(target);
                                u8c=sec0_data.input.get(in);
                                break;
                        } // switch(u8c=sec0_data.input.get(in); u8c.raw())
                        break;
                    } // case '$'_u8
                    default:
                        u8c.write_to(target);
                        u8c=sec0_data.input.get(in);
                        break;
                } // switch(u8c.raw())
            } // while(1)
            assert(false);
          init_or_code_done:
            //target.resize(yuki::remove_trailing_u8(target.begin(),target.end(),yuki::unicode::is_WSpace<yuki::U8Char>));
            if(is_init){
                if(rule.init.empty())
                    rule.init.push_back(' ');
                else if(rule.left==(Token_Kind_t)-1){
                    print_loc(stderr,current_right_lineno,current_right_colno,filename);
                    fputs("Error: The init clause for goal production should be empty!\n",stderr);
                    sec0_data.advance_errors();
                    rule.init.clear();
                    rule.init.push_back(' ');
                }
            }
            goto skip_spaces1;
        } // case '{'_u8
        case '|'_u8.raw():case ';'_u8.raw(): goto shipout;
        default:
            assert(str_temp.empty());
            while(1){
                u8c.write_to(str_temp);
                u8c=sec0_data.input.get(in);
                switch(u8c.raw()){
                    case yuki::EOF_U8.raw(): goto shipout;
                    case '/'_u8.raw():
                        switch(sec0_data.input.try_skip_comment(in)){
                            case EOF: u8c=yuki::EOF_U8;goto shipout;
                            case 0: break;
                            default: u8c=sec0_data.input.get(in); goto write_to_rights;
                        }
                        break;
                    default:
                        if(yuki::unicode::is_WSpace(u8c)){
                            u8c=sec0_data.input.get(in);
                            goto write_to_rights;
                        }else
                            break;
                    case '{'_u8.raw():case '\"'_u8.raw():case '\''_u8.raw():case '%'_u8.raw(): goto write_to_rights;
                    case '|'_u8.raw(): case ';'_u8.raw(): goto shipout;
                }
            }
    } // switch(u8c.raw())
    assert(false);
  write_to_rights:
    assert(!str_temp.empty());
    if(percent!=yuki::EOF_U8) [[unlikely]] {
        try{
            const size_t prec= isdigit(static_cast<unsigned char>(str_temp.front())) ? std::stoull(str_temp) : sec0_data.token_datas.at(str_temp).prec;
            switch(percent.raw()){
                case 'r'_u8.raw():
                    YUKI_PG_META_DBGO("%r {}\n",str_temp);
                    rule.prec_rr=prec;
                    break;
                case 's'_u8.raw():
                    YUKI_PG_META_DBGO("%s {}\n",str_temp);
                    rule.prec_sr=prec;
                    break;
                default: assert(false); std::unreachable();
            }
        }catch(const std::out_of_range&){
            char buf[5]={};
            percent.write_to(buf);
            print_loc(stderr,current_right_lineno,current_right_colno,filename);
            fprintf(stderr,"Error: Unknown token name - %s - for the argument of %%%s!\n",str_temp.c_str(),buf);
            sec0_data.advance_errors();
        }
        percent=yuki::EOF_U8;
    }else{
        try{
            rule.rights.push_back(sec0_data.token_datas.to_num<Token_Kind_t>(sec0_data.token_datas.token_htable.at(str_temp)));
        }catch(const std::out_of_range&){
            print_loc(stderr,current_right_lineno,current_right_colno,filename);
            fprintf(stderr,"Error: Unknown token name on the rhs of a production: %s\n",str_temp.c_str());
            sec0_data.advance_errors();
        }
    }
    str_temp.clear();
    goto skip_spaces1;
    }
  shipout:
    if(!rule_invalid){
        if(rule.init.size()==1 && rule.init.front()==' ')
            rule.init.clear();
        if(rule.left==(Token_Kind_t)-1){
            switch(rule.rights.size()){
                case 0:
                    print_loc(stderr,first_right_lineno,first_right_colno,filename);
                    fputs("Error: Empty goal production!\n",stderr);
                    sec0_data.advance_errors();
                    goto rule_clear;
                case 1:
                    break;
                default:
                    print_loc(stderr,first_right_lineno,first_right_colno,filename);
                    fputs("Error: Goal production with more than 1 token on the rhs!\n",stderr);
                    sec0_data.advance_errors();
                    rule.rights.resize(1);
                    break;
            }
            const yuki::IB_Pair<typename Rule_Set<Token_Kind_t>::const_iterator> ibp = rs.insert_goal(std::move(rule));
            if(!ibp.has_inserted){
                print_loc(stderr,first_right_lineno,first_right_colno,filename);
                fprintf(stderr,"Error: Redeclaration of goal symbol! (Note: Previously declared - %s -. Current declared - %s -.)\n",sec0_data.token_datas[ibp.iterator->rights.front()].name_or_alias().c_str(),sec0_data.token_datas[rule.rights.front()].name_or_alias().c_str());
                sec0_data.advance_errors();
            }else
                sec0_data.token_datas.goal = ibp.iterator->rights.front();
        }else{
            if(!rs.insert(std::move(rule))){
                print_loc(stderr,first_right_lineno,first_right_colno,filename);
                fprintf(stderr,"Error: Duplicate production, with lhs - %s -!\n",rule.left==(Token_Kind_t)-1 ? "Goal_" : sec0_data.token_datas[rule.left].name_or_alias().c_str());
                sec0_data.advance_errors();
            }
        }
    }
  rule_clear:
    rule.clear();
    switch(u8c.raw()){
        case '|'_u8.raw(): u8c=sec0_data.input.get(in); goto skip_spaces1;
        case ';'_u8.raw(): goto skip_spaces_before_left;
        default: return rs;
    }
} // parse_sec12




void write_traits_alloc(FILE* const out_token,const char* const ts_cstr,const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms);

void write_token(FILE* const out_token,Sec0_Data& sec0_data){
    #define IND YUKI_PG_IND
    #define IND2 IND IND
    #define IND3 IND IND IND
    #define IND4 IND IND IND IND

    const Sec0_Data::Token_Impl_Type token_impl_type = sec0_data.token_impl_type;
    yuki::Vector<Token_Data>& nterms = sec0_data.token_datas.nterms;
    yuki::Vector<Token_Data>& terms = sec0_data.token_datas.terms;

    fputs("#pragma once\n\n",out_token);
    fputs(sec0_data.codes[Sec0_Data::Code_Cat::TOKEN_TOP].c_str(),out_token);
    fputc(static_cast<unsigned char>('\n'),out_token);
    fputc(static_cast<unsigned char>('\n'),out_token);
    fputs(
        "#include<cassert>\n"
        "#include<yuki/tmp.hpp>\n"
        "#include<yuki/pg/core.hpp>\n",
        out_token
    );
    switch(token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: fputs("#include<yuki/pg/VToken.hpp>\n\n",out_token);break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: fputs("#include<yuki/pg/SToken.hpp>\n\n",out_token);break;
        //case Sec0_Data::Token_Impl_Type::TUPLE: fputs("#include<yuki/pg/TToken.hpp>\n#include<yuki/Allocator.hpp>\n\n",out_token);break;
    }
    fprintf(out_token,
        "%s\n"
        "struct %s{\n"
        IND "static constexpr yuki::pg::Token_Impl_Type token_impl_type = yuki::pg::Token_Impl_Type::",
        sec0_data.nspace_head.c_str(), sec0_data.ts.c_str()
    );
    switch(token_impl_type){
        case Sec0_Data::Token_Impl_Type::VARIANT: fputs("VARIANT;\n\n",out_token);break;
        case Sec0_Data::Token_Impl_Type::SIMPLE: fputs("SIMPLE;\n\n",out_token);break;
        //case Sec0_Data::Token_Impl_Type::TUPLE: fputs("TUPLE;\n\n",out_token);break;
    }
    fprintf(out_token,
        IND "static constexpr size_t token_total = %zu;\n"
        IND "typedef yuki::uint_auto_t<token_total> Token_Kind_t;\n"
        IND "struct Token_Kind{\n"
        IND2 "enum enum_t : Token_Kind_t {",
        nterms.size()+terms.size()
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
                if(!td.type.empty()){
                    if(const auto pair=vtoken_types.try_emplace(td.type,vtoken_types.size()+1); pair.second){
                        td.vtoken_index = vtoken_types.size();
                        vtoken_types_linear[vtoken_types.size()-1] = pair.first->first.c_str();
                    }else
                        td.vtoken_index = pair.first->second;
                }
                break;
            //case Sec0_Data::Token_Impl_Type::TUPLE:
            //    if(td.alloc.empty()){
            //        td.alloc="yuki::Array_Allocator<Token::";
            //        td.alloc.append(td.name).push_back('>');
            //    }else if(isdigit(static_cast<unsigned char>(td.alloc.front()))){
            //        char* end=nullptr;
            //        strtoull(td.alloc.c_str(),&end,0);
            //        alloc_temp.assign(td.alloc.c_str(),static_cast<const char*>(end));
            //        td.alloc="yuki::Array_Allocator<Token::";
            //        td.alloc.append(td.name).append(",").append(alloc_temp).push_back('>');
            //    }
            //    break;
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
    fputs(
        "};\n"
        IND "};\n",
        out_token
    );
    fputs(IND "static constexpr const char* token_name[token_total] = {",out_token);
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
    fputs("};\n\n",out_token);
    fprintf(out_token,
        IND "static constexpr Token_Kind_t nterminal_first = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t nterminal_last = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t terminal_first = Token_Kind::%s;\n"
        IND "static constexpr Token_Kind_t terminal_last = Token_Kind::EOF_;\n"
        IND "static constexpr Token_Kind_t eof_ = Token_Kind::EOF_;\n"
        "\n"
        IND "static constexpr size_t nterminal_total = %zu;\n"
        IND "static constexpr size_t terminal_total = %zu;\n"
        "\n",
        nterms[0].name.c_str(), nterms.back().name.c_str(), terms[0].name.c_str(),
        nterms.size(), terms.size()
    );
    fputs(
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
        "\n",
        out_token
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
                    fputs(IND2,out_token);
                fprintf(out_token,"%zu, ",td.vtoken_index);
                if(token_count==YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM-1){
                    fputc(static_cast<unsigned char>('\n'),out_token);
                    token_count=0;
                }else
                    ++token_count;
            }
            for(const Token_Data& td : terms){
                if(token_count==0)
                    fputs(IND2,out_token);
                fprintf(out_token,"%zu, ",td.vtoken_index);
                if(token_count==YUKI_PG_TOKEN_INDEX_TABLE_MAX_LINE_ITEM-1){
                    fputc(static_cast<unsigned char>('\n'),out_token);
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
                    fputs(IND2,out_token);
                fprintf(out_token,", %s",vtoken_types_linear[token_count]);
                ++token_count;
                if(token_count%YUKI_PG_VTOKEN_MAX_LINE_ITEM==0)
                    fputc(static_cast<unsigned char>('\n'),out_token);
            }
            fprintf(out_token,
                "%s"
                IND "> Token_t;\n"
                IND "typedef void Token;\n"
                "}; // struct %s\n"
                "%s\n",
                token_count%YUKI_PG_VTOKEN_MAX_LINE_ITEM==0 ? "" : "\n",
                sec0_data.ts.c_str(),
                sec0_data.nspace_tail.c_str()
            );
            break;
        }
        case Sec0_Data::Token_Impl_Type::SIMPLE:{
            fprintf(out_token,
                IND "typedef yuki::pg::SToken<Token_Kind_t,%s> Token_t;\n"
                IND "typedef void Token;\n"
                "}; // struct %s\n"
                "%s\n",
                sec0_data.sp_token.c_str(),
                sec0_data.ts.c_str(),
                sec0_data.nspace_tail.c_str()
            );
            break;
        }
        //case Sec0_Data::Token_Impl_Type::TUPLE:{
        //    fprintf(out_token,IND "typedef yuki::pg::Any_TToken<%s> Token_t;\n\n",sec0_data.ts.c_str());
        //    fputs(
        //        IND "template<Token_Kind_t kind_p,typename... Ts>\n"
        //        IND "using TToken_Tp = yuki::pg::TToken<Token_Kind_t,kind_p,Ts...>;\n"
        //        "\n"
        //        IND "struct Token{\n",
        //        out_token
        //    );
        //    for(const Token_Data& td : nterms){
        //        fprintf(out_token,IND2 "using %s = TToken_Tp<Token_Kind::%s",td.name.c_str(),td.name.c_str());
        //        for(const std::string& type : td.types)
        //            fprintf(out_token,",%s",type.c_str());
        //        fputs(">;\n",out_token);
        //    }
        //    for(const Token_Data& td : terms){
        //        fprintf(out_token,IND2 "using %s = TToken_Tp<Token_Kind::%s",td.name.c_str(),td.name.c_str());
        //        for(const std::string& type : td.types)
        //            fprintf(out_token,",%s",type.c_str());
        //        fputs(">;\n",out_token);
        //    }
        //    fputs(IND "}; // struct Token\n\n",out_token);
        //    write_traits_alloc(out_token,sec0_data.ts.c_str(),nterms,terms);
        //    fputs(sec0_data.nspace_tail.c_str(),out_token);
        //    fputc(static_cast<unsigned char>('\n'),out_token);
        //    break;
        //}
    }
    fputs(sec0_data.codes[Sec0_Data::Code_Cat::TOKEN_BOTTOM].c_str(),out_token);
    fputc(static_cast<unsigned char>('\n'),out_token);
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
    fputc(static_cast<unsigned char>('\n'),out_token);
    for(const Token_Data& td : terms)
        fprintf(out_token,"template<> struct %s::is_terminal<%s::Token::%s> : std::true_type {};\n",ts_cstr,ts_cstr,td.name.c_str());
    fputc(static_cast<unsigned char>('\n'),out_token);
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

    fputc(static_cast<unsigned char>('\n'),out_token);

    fprintf(out_token,"struct %s::Allocator :\n",ts_cstr);
    for(const Token_Data& td : nterms)
        fprintf(out_token,IND "private type_to_alloc_t<Token::%s>,\n",td.name.c_str());
    for(size_t i = 0;i!=terms.size();++i){
        fprintf(out_token,IND "private type_to_alloc_t<Token::%s>",terms[i].name.c_str());
        if(i+1!=terms.size())
            fputc(static_cast<unsigned char>(','),out_token);
        fputc(static_cast<unsigned char>('\n'),out_token);
    }
    fputs(
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
        IND2 "switch(k){\n",
        out_token
    );
    for(const Token_Data& td : nterms)
        fprintf(out_token,IND3 "case %s : {destroy_deallocate<Token::%s>(p);break;}\n",td.name.c_str(),td.name.c_str());
    for(const Token_Data& td : terms)
        fprintf(out_token,IND3 "case %s : {destroy_deallocate<Token::%s>(p);break;}\n",td.name.c_str(),td.name.c_str());
    fprintf(out_token,IND2 "}\n" IND "}\n}; // struct %s::Allocator\n",ts_cstr);
} // write_traits_alloc
} // namespace yuki::pg




#include"cmd.hpp"
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

    yuki::pg::Sec0_Data sec0_data = yuki::pg::parse_sec0(cmd_data.fp_in,cmd_data.in.data(),cmd_data.max_errors);
    if(sec0_data.token_datas.nterms.empty()){
        fputs("Fatal Error: No non-terminals are declared!\n",stderr);
        fprintf(stderr,"%u errors encountered.\n",sec0_data.errors()+1);
        return EXIT_FAILURE;
    }
    sec0_data.token_datas.terms.emplace_back("EOF_");
    sec0_data.token_datas.terms.back().assoc=yuki::pg::Assoc::LEFT;
    if(sec0_data.debug_prefix.empty())
        sec0_data.debug_prefix.append("YUKI_PG_").append(sec0_data.parser).append("_DBG");
    if(!sec0_data.nspace.empty()){
        sec0_data.nspace_head.append("namespace ").append(sec0_data.nspace).push_back('{');
        sec0_data.nspace_tail.append("} // namespace ").append(sec0_data.nspace);
    }

    yuki::pg::write_token(cmd_data.fp_out_token,sec0_data);
    fclose(cmd_data.fp_out_token);
    cmd_data.fp_out_token=nullptr;

    switch(yuki::uint_auto_f(sec0_data.token_datas.nterms.size()+sec0_data.token_datas.terms.size()+1)){
        case yuki::uint_enum::UCHAR:{
            const yuki::pg::Rule_Set<unsigned char> rs = parse_sec12<unsigned char>(sec0_data,cmd_data.fp_in,cmd_data.in.data());
            if(rs.empty())
                yuki::pg::eof_error(sec0_data.errors()+1);
            yuki::pg::lr::write(cmd_data,sec0_data,rs);
            break;
        }
        case yuki::uint_enum::USHORT:{
            const yuki::pg::Rule_Set<unsigned short> rs = parse_sec12<unsigned short>(sec0_data,cmd_data.fp_in,cmd_data.in.data());
            if(rs.empty())
                yuki::pg::eof_error(sec0_data.errors()+1);
            yuki::pg::lr::write(cmd_data,sec0_data,rs);
            break;
        }
        case yuki::uint_enum::UINT:{
            const yuki::pg::Rule_Set<unsigned> rs = parse_sec12<unsigned>(sec0_data,cmd_data.fp_in,cmd_data.in.data());
            if(rs.empty())
                yuki::pg::eof_error(sec0_data.errors()+1);
            yuki::pg::lr::write(cmd_data,sec0_data,rs);
            break;
        }
        default:
            fprintf(stderr,"Fatal Error: Token count exceeds implementation limit %u!\n",std::numeric_limits<unsigned>::max());
            sec0_data.advance_errors();
            break;
    }
    if(sec0_data.errors()!=0){
        fprintf(stderr,"%u errors encountered.\n",sec0_data.errors());
        return EXIT_FAILURE;
    }else
        return EXIT_SUCCESS;
}
