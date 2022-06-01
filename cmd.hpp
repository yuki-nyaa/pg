#pragma once
#include<string>
#include<fmt/core.h>
#include<yuki/string_split.hpp>
#include<yuki/cmd.hpp>
namespace yuki::pg{
struct Cmd_Data{
    std::string in;
    std::string out_cpp;
    std::string out_h;
    std::string out_token;
    std::string out_log;
    // Members for later process.
    FILE* fp_in = nullptr;
    FILE* fp_out_cpp = nullptr;
    FILE* fp_out_h = nullptr;
    FILE* fp_out_token = nullptr;
    FILE* fp_out_log = nullptr;

    const char* tmp_goto = "yuki_pg_tmp_goto";
    FILE* fp_goto = nullptr;

    std::string sp_token;
    std::string nspace;
    std::string parser = "Parser";
    std::string ts = "Token_Settings";
    std::string lexer = "Lexer";
    std::string parser_tables;

    bool no_final_function = false;
    bool no_final_class = false;
    bool no_default_ctor = false;

    bool is_switch = true;

    size_t lr1_stack_reserve = 128;

    ~Cmd_Data() noexcept {close_all();}

    void close_all(){
        if(fp_in!=nullptr && fp_in!=stdin) {fclose(fp_in);fp_in=nullptr;}
        if(fp_out_cpp!=nullptr && fp_out_cpp!=stdout && fp_out_cpp!=stderr) {fclose(fp_out_cpp);fp_out_cpp=nullptr;}
        if(fp_out_h!=nullptr && fp_out_h!=stdout && fp_out_h!=stderr) {fclose(fp_out_h);fp_out_h=nullptr;}
        if(fp_out_token!=nullptr && fp_out_token!=stdout && fp_out_token!=stderr) {fclose(fp_out_token);fp_out_token=nullptr;}
        if(fp_out_log!=nullptr && fp_out_log!=stdout && fp_out_log!=stderr) {fclose(fp_out_log);fp_out_log=nullptr;}
        if(fp_goto!=nullptr) {fclose(fp_goto);remove(tmp_goto);fp_goto=nullptr;}
    }

    bool post_process();
};

namespace cmd_impl{
    inline void i(Cmd_Data& cmd_data,yuki::Vector<std::string>& args){
        cmd_data.in=std::move(args[0]);
    }
    inline void o(Cmd_Data& cmd_data,yuki::Vector<std::string>& args){
        cmd_data.out_cpp=std::move(args[0]);
    }
    inline void h(Cmd_Data& cmd_data,yuki::Vector<std::string>& args){
        cmd_data.out_h=std::move(args[0]);
    }
    inline void t(Cmd_Data& cmd_data,yuki::Vector<std::string>& args){
        cmd_data.out_token=std::move(args[0]);
    }
    inline void l(Cmd_Data& cmd_data,yuki::Vector<std::string>& args){
        cmd_data.out_log=std::move(args[0]);
    }
    inline void version(Cmd_Data&,yuki::Vector<std::string>&){
        fmt::print(stdout,"ParserGen by Yuki, version {}.{}\n",1,0);
        std::exit(EXIT_SUCCESS);
    }
}

inline yuki::cmd_opt_table_t<Cmd_Data> opt_table={ // Option property table
    {"version",{yuki::Cmd_Token::L, 0, 0, &cmd_impl::version}},
    {"v",{yuki::Cmd_Token::Sa, 0, 0, &cmd_impl::version}},

    {"i",{yuki::Cmd_Token::Sa,1,1, &cmd_impl::i}},
    {"o",{yuki::Cmd_Token::Sa,1,1, &cmd_impl::o}},
    {"h",{yuki::Cmd_Token::Sa,1,1, &cmd_impl::h}},
    {"t",{yuki::Cmd_Token::Sa,1,1, &cmd_impl::t}},
    {"l",{yuki::Cmd_Token::Sa,1,1, &cmd_impl::l}},
};

inline bool Cmd_Data::post_process(){
    // Check for empty input.
    if(in.empty()){
        yuki::print_error(stderr,"no input is specified.\n");
        yuki::print_note(stderr,"use \'-i\' to specify input file.\n");
        return false;
    }
    fp_in=fopen(in.c_str(),"r");
    if(!fp_in){
        yuki::print_error(stderr,"the input file \'{}\' cannot be opened!\n",in);
        return false;
    }

    // Check for input file-ext collision.
    auto [input_no_ext,input_ext] = yuki::vsplit_filename(in);

    if(input_ext=="cpp" || input_ext=="h" || input_ext=="hpp" || input_ext=="log"){
        yuki::print_error(stderr,"input file has extension \'cpp\', \'h\', \'hpp\' or \'log\', which might collide with the output files.\n");
        return false;
    }

    // Set default values;
    // The default cpp-out filename is the same as the input file.
    if(out_cpp.empty()){
        out_cpp.reserve(input_no_ext.size()+4);
        out_cpp.append(input_no_ext);
        out_cpp.append(".cpp");
    }
    fp_out_cpp=fopen(out_cpp.c_str(),"w");
    // The default h-out filename is the same as the CPP-OUT file.
    std::string_view out_cpp_no_ext=yuki::vsplit_filename(out_cpp).zeroth;
    if(out_h.empty()){
        out_h.reserve(out_cpp_no_ext.size()+2);
        out_h.append(out_cpp_no_ext);
        out_h.append(".h");
    }
    fp_out_h=fopen(out_h.c_str(),"w");
    // The default token-out filename is the same as the CPP-OUT file.
    if(out_token.empty()){
        out_token.reserve(out_cpp_no_ext.size()+10);
        out_token.append(out_cpp_no_ext);
        out_token.append(".token.hpp");
    }
    fp_out_token=fopen(out_token.c_str(),"w");
    // Log-out is optional.
    if(!out_log.empty()){
        fp_out_log=fopen(out_log.c_str(),"w");
    }

    if((fp_goto=fopen(tmp_goto,"r"))){
        yuki::print_error(stderr,"Temp file \"{}\" already exists!\n",tmp_goto);
        fclose(fp_goto); fp_goto = nullptr;
        return false;
    }
    fp_goto = fopen(tmp_goto,"w+");

    return true;
} // bool Cmd_Data::post_process()

} // namespace yuki::pg