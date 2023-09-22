#pragma once
#include"common.hpp"
#include<yuki/unicode/binary_properties.h>
#include<cctype>

namespace yuki::pg{

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
                case static_cast<unsigned char>('\n'): goto skip_spaces_before_left;
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
                case static_cast<unsigned char>('\n'): goto left_done;
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
                ++sec0_data.errors;
                rule_invalid=true;
            }else{
                rule.left = sec0_data.token_datas.to_num<Token_Kind_t>(co);
            }
        }catch(const std::out_of_range&){
            print_loc(stderr,left_lineno,left_colno,filename);
            fprintf(stderr,"Error: Unknown token name on the lhs of a production: %s\n",str_temp.c_str());
            ++sec0_data.errors;
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
            case static_cast<unsigned char>('\n'): goto skip_spaces0;
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
            case static_cast<unsigned char>('\n'): u8c=sec0_data.input.get(in); goto skip_spaces1;
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
                    ++sec0_data.errors;
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
                            case static_cast<unsigned char>('\n'): u8c=sec0_data.input.get(in); goto skip_spaces1;
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
            ++sec0_data.errors;
            }
            percent=yuki::EOF_U8;
            goto shipout;
        case '\"'_u8.raw():
            assert(str_temp.empty());
            str_temp.push_back('\"');
            while(1){
                u8c=sec0_data.input.get(in);
                switch(u8c.raw()){
                    case yuki::EOF_U8.raw():
                        print_loc(stderr,sec0_data.input.lineno,sec0_data.input.colno,filename);
                        fputs("Error: Missing closing double quote!\n",stderr);
                        ++sec0_data.errors;
                        str_temp.push_back('\"');
                        goto shipout;
                    case '\"'_u8.raw(): str_temp.push_back('\"'); u8c=sec0_data.input.get(in); goto write_to_rights;
                    default: u8c.write_to(str_temp); break;
                }
            }
        case '\''_u8.raw():
            assert(str_temp.empty());
            str_temp.push_back('\'');
            while(1){
                u8c=sec0_data.input.get(in);
                switch(u8c.raw()){
                    case yuki::EOF_U8.raw():
                        print_loc(stderr,sec0_data.input.lineno,sec0_data.input.colno,filename);
                        fputs("Error: Missing closing single quote!\n",stderr);
                        ++sec0_data.errors;
                        str_temp.push_back('\'');
                        goto shipout;
                    case '\''_u8.raw(): str_temp.push_back('\''); u8c=sec0_data.input.get(in); goto write_to_rights;
                    default: u8c.write_to(str_temp); break;
                }
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
                        ++sec0_data.errors;
                        goto shipout;
                    case '}'_u8.raw():
                        if(--brace_level==0)
                            goto init_or_code_done;
                        else{
                            target.push_back('}');
                            u8c=sec0_data.input.get(in);
                            break;
                        }
                    case '$'_u8.raw():{
                        const size_t dollar_lineno = sec0_data.input.lineno_orig;
                        const size_t dollar_colno = sec0_data.input.colno_orig;
                        switch(u8c=sec0_data.input.get(in); u8c.raw()){
                            case yuki::EOF_U8.raw():
                                target.push_back('$');
                                print_loc(stderr,sec0_data.input.lineno,sec0_data.input.colno,filename);
                                fputs("Error: Missing closing brace!\n",stderr);
                                ++sec0_data.errors;
                                goto shipout;
                            //case '{'_uc:
                            case '$'_u8.raw():
                                if(rule.left==(Token_Kind_t)-1){
                                    print_loc(stderr,dollar_lineno,dollar_colno,filename);
                                    fputs("Error: $$ in goal production!\n",stderr);
                                    ++sec0_data.errors;
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
                                    ++sec0_data.errors;
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
          init_or_code_done:
            //target.resize(yuki::remove_trailing_u8(target.begin(),target.end(),yuki::unicode::is_WSpace<yuki::U8Char>));
            if(is_init){
                if(rule.init.empty())
                    rule.init.push_back(' ');
                else if(rule.left==(Token_Kind_t)-1){
                    print_loc(stderr,current_right_lineno,current_right_colno,filename);
                    fputs("Error: The init clause for goal production should be empty!\n",stderr);
                    ++sec0_data.errors;
                    rule.init.clear();
                    rule.init.push_back(' ');
                }
            }
            u8c=sec0_data.input.get(in);
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
                            case static_cast<unsigned char>('\n'): u8c=sec0_data.input.get(in); goto write_to_rights;
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
            ++sec0_data.errors;
        }
        percent=yuki::EOF_U8;
    }else{
        try{
            rule.rights.push_back(sec0_data.token_datas.to_num<Token_Kind_t>(sec0_data.token_datas.token_htable.at(str_temp)));
        }catch(const std::out_of_range&){
            print_loc(stderr,current_right_lineno,current_right_colno,filename);
            fprintf(stderr,"Error: Unknown token name on the rhs of a production: %s\n",str_temp.c_str());
            ++sec0_data.errors;
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
                    ++sec0_data.errors;
                    goto rule_clear;
                case 1:
                    break;
                default:
                    print_loc(stderr,first_right_lineno,first_right_colno,filename);
                    fputs("Error: Goal production with more than 1 token on the rhs!\n",stderr);
                    ++sec0_data.errors;
                    rule.rights.resize(1);
                    break;
            }
            const yuki::IB_Pair<typename Rule_Set<Token_Kind_t>::const_iterator> ibp = rs.insert_goal(std::move(rule));
            if(!ibp.has_inserted){
                print_loc(stderr,first_right_lineno,first_right_colno,filename);
                fprintf(stderr,"Error: Redeclaration of goal symbol! (Note: Previously declared - %s -. Current declared - %s -.)\n",sec0_data.token_datas[ibp.iterator->rights.front()].name_or_alias().c_str(),sec0_data.token_datas[rule.rights.front()].name_or_alias().c_str());
                ++sec0_data.errors;
            }else
                sec0_data.token_datas.goal = ibp.iterator->rights.front();
        }else{
            if(!rs.insert(std::move(rule))){
                print_loc(stderr,first_right_lineno,first_right_colno,filename);
                fprintf(stderr,"Error: Duplicate production, with lhs - %s -!\n",rule.left==(Token_Kind_t)-1 ? "Goal_" : sec0_data.token_datas[rule.left].name_or_alias().c_str());
                ++sec0_data.errors;
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
} // namespace yuki::pg
