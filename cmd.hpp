#pragma once
#include<string>
#include<yuki/string_split.hpp>
#include<yuki/cmd.hpp>

namespace yuki::pg{
struct Cmd_Data{
    std::string_view in;
    std::string_view out_cpp;
    std::string out_h;
    std::string out_token;
    bool default_log=false;
    enum struct Alg_Type : unsigned char {PLR1,CLR1,LALR1} alg_type = Alg_Type::PLR1;
    bool ft=false;
    bool lr0_its=false;
    bool lr1_ias=false;
    unsigned max_errors=64;

    // Members for later process.
    FILE* fp_in = nullptr;
    FILE* fp_out_cpp = nullptr;
    FILE* fp_out_h = nullptr;
    FILE* fp_out_token = nullptr;
    FILE* fp_log = nullptr;

    bool post_process();
};

namespace cmd_impl{
    YUKI_CMD_ONE_PARAM_OPTION(i,Cmd_Data,
        fputs("Warning: Empty input filepath!\n",stderr),
        fputs("Warning: Multiple inputs specified! (Note: Enclose the path in quotes if it contains spaces.)\n",stderr),
        cmd_data.fp_in = fopen(*argv,"r");
        if(!cmd_data.fp_in)
            fprintf(stderr,"Error: The input file \"%s\" somehow cannot be opened!\n",*argv);
        else
            cmd_data.in = *argv;
    )

    YUKI_CMD_ONE_PARAM_OPTION(o,Cmd_Data,
        fputs("Warning: Empty output filepath!\n",stderr),
        fputs("Warning: Multiple outputs specified! (Note: Enclose the path in quotes if it contains spaces.)\n",stderr),
        cmd_data.out_cpp = *argv;
    )

    YUKI_CMD_ONE_PARAM_OPTION(h,Cmd_Data,
        fputs("Warning: Empty output header filepath!\n",stderr),
        fputs("Warning: Multiple output headers specified! (Note: Enclose the path in quotes if it contains spaces.)\n",stderr),
        cmd_data.out_h = *argv;
    )

    YUKI_CMD_ONE_PARAM_OPTION(t,Cmd_Data,
        fputs("Warning: Empty output token def filepath!\n",stderr),
        fputs("Warning: Multiple output token def files specified! (Note: Enclose the path in quotes if it contains spaces.)\n",stderr),
        cmd_data.out_token = *argv;
    )

    YUKI_CMD_ONE_PARAM_OPTION(l,Cmd_Data,
        cmd_data.default_log=true,
        fputs("Warning: Multiple log paths specified! (Note: Enclose the path in quotes if it contains spaces.)\n",stderr),
        cmd_data.fp_log = fopen(*argv,"w");
        if(!cmd_data.fp_log)
            fprintf(stderr,"Error: The log file \"%s\" somehow cannot be opened!\n",*argv);
    )

    YUKI_CMD_ONE_PARAM_OPTION(max_errors,Cmd_Data,
        fputs("Warning: No argument to \"--max-errors\"!\n",stderr),
        fputs("Warning: Multiple arguments to \"--max-errors\"!\n",stderr),
        const unsigned long long num = strtoull(*argv,nullptr,0);
        if(errno==ERANGE || num>(std::numeric_limits<unsigned>::max()-1)){
            fputs("Warning: The argument to \"--max-errors\" is too big!\n",stderr);
            cmd_data.max_errors = (std::numeric_limits<unsigned>::max()-1);
        }else
            cmd_data.max_errors = num;
    )

    inline void version(FILE* const out) {fputs("pg by Yuki, version 1.0\n",out);}
    inline void version(Cmd_Data&,const char*const*,const size_t argc){
        if(argc>0)
            fputs("Warning: \"--version\" should take no arguments!\n",stderr);
        version(stderr);
        exit(EXIT_SUCCESS);
    }

    inline void plr1(Cmd_Data& cmd_data,const char*const*,const size_t argc){
        if(argc>0)
            fputs("Warning: \"--plr1\" should take no arguments!\n",stderr);
        cmd_data.alg_type=Cmd_Data::Alg_Type::PLR1;
    }
    inline void clr1(Cmd_Data& cmd_data,const char*const*,const size_t argc){
        if(argc>0)
            fputs("Warning: \"--clr1\" should take no arguments!\n",stderr);
        cmd_data.alg_type=Cmd_Data::Alg_Type::CLR1;
    }
    inline void lalr1(Cmd_Data& cmd_data,const char*const*,const size_t argc){
        if(argc>0)
            fputs("Warning: \"--lalr1\" should take no arguments!\n",stderr);
        cmd_data.alg_type=Cmd_Data::Alg_Type::LALR1;
    }

    inline void ft(Cmd_Data& cmd_data,const char*const*,const size_t argc){
        if(argc>0)
            fputs("Warning: \"--ft\" should take no arguments!\n",stderr);
        cmd_data.ft=true;
    }
    inline void lr0_its(Cmd_Data& cmd_data,const char*const*,const size_t argc){
        if(argc>0)
            fputs("Warning: \"--lr0-its\" should take no arguments!\n",stderr);
        cmd_data.lr0_its=true;
    }
    inline void lr1_ias(Cmd_Data& cmd_data,const char*const*,const size_t argc){
        if(argc>0)
            fputs("Warning: \"--lr1-ias\" should take no arguments!\n",stderr);
        cmd_data.lr1_ias=true;
    }
}

inline constexpr yuki::Cmd_Option<Cmd_Data> coarr[] = {
    {"version",U'v',cmd_impl::version,"Print version information."},
    {"",U'i',cmd_impl::i,"The input file."},
    {"",U'o',cmd_impl::o,"The output cpp file."},
    {"",U'h',cmd_impl::h,"The output header file."},
    {"",U't',cmd_impl::t,"The output token def file."},
    {"",U'l',cmd_impl::l,"The output log file."},
    {"plr1",0,cmd_impl::plr1,"Use Pager's minimal LR1 algorithm."},
    {"clr1",0,cmd_impl::clr1,"Use Canonical-LR1 algorithm."},
    {"lalr1",0,cmd_impl::lalr1,"Use LALR1 algorithm."},
    {"ft",0,cmd_impl::ft,"Print the first table to log."},
    {"lr0-its",0,cmd_impl::lr0_its,"Print the LR0-Immediate-Transitions to log."},
    {"lr1-ias",0,cmd_impl::lr1_ias,"Print the LR1-Immediate-Actions to log."},
    {"max-errors",0,cmd_impl::max_errors,"Set the maximum error count."},
};

inline bool Cmd_Data::post_process(){
    if(!fp_in){
        fputs("Fatal Error: No input is specified! (Note: use \"-i\" to specify input file.)\n",stderr);
        return false;
    }

    // Check for input file-ext collision.
    const auto [input_no_ext,input_ext] = yuki::vsplit_filename(in);

    if(input_ext=="cpp" || input_ext=="h" || input_ext=="hpp" || input_ext=="log"){
        fputs("Fatal Error: The input file has extension \".cpp\", \".h\", \".hpp\", or \".log\", which might collide with the output files!\n",stderr);
        return false;
    }

    // Set default values;

    std::string str_temp;
    if(out_cpp.empty()){
        // The default cpp-out filename is the same as the input file.
        str_temp.reserve(input_no_ext.size()+4);
        str_temp.append(input_no_ext);
        str_temp.append(".cpp");
        out_cpp=str_temp;
    }
    fp_out_cpp=fopen(out_cpp.data(),"w");
    if(!fp_out_cpp){
        fprintf(stderr,"Fatal Error: The out file \"%s\" somehow cannot be created!\n",out_cpp.data());
        return false;
    }

    const std::string_view out_cpp_no_ext=yuki::vsplit_filename(out_cpp).zeroth;

    if(out_h.empty()){
        // The default h-out filename is the same as the CPP-OUT file.
        out_h.reserve(out_cpp_no_ext.size()+2);
        out_h.append(out_cpp_no_ext);
        out_h.append(".h");
    }
    fp_out_h=fopen(out_h.c_str(),"w");
    if(!fp_out_h){
        fprintf(stderr,"Fatal Error: The out header \"%s\" somehow cannot be created!\n",out_h.c_str());
        return false;
    }

    if(out_token.empty()){
        // The default token-out filename is the same as the CPP-OUT file.
        out_token.reserve(out_cpp_no_ext.size()+10);
        out_token.append(out_cpp_no_ext);
        out_token.append(".token.hpp");
    }
    fp_out_token=fopen(out_token.c_str(),"w");
    if(!fp_out_token){
        fprintf(stderr,"Fatal Error: The out token def file \"%s\" somehow cannot be created!\n",out_token.c_str());
        return false;
    }

    if(default_log && fp_log==nullptr){
        // The default log filename is the same as the CPP-OUT file.
        std::string str_temp1;
        str_temp1.reserve(out_cpp_no_ext.size()+4);
        str_temp1.append(out_cpp_no_ext);
        str_temp1.append(".log");
        fp_log=fopen(str_temp1.c_str(),"w");
        if(!fp_log)
            fprintf(stderr,"Warning: The default log file \"%s\" somehow cannot be created!\n",str_temp1.c_str());
    }

    out_cpp=std::string_view{}; // `out_cpp` (as a `std::string_view`) could possibly points to `str_temp`, which is going to be destroyed when the function returns.

    return true;
} // bool Cmd_Data::post_process()
} // namespace yuki::pg
