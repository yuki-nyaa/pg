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

    // Members for later process.
    FILE* fp_in = nullptr;
    FILE* fp_out_cpp = nullptr;
    FILE* fp_out_h = nullptr;
    FILE* fp_out_token = nullptr;
    FILE* fp_log = nullptr;

    FILE* fp_goto = tmpfile();

    bool post_process();
};

namespace cmd_impl{
    inline void i(Cmd_Data& cmd_data,const char*const*const argv,const size_t argc){
        switch(argc){
            case 0:
                fprintf(stderr,"Warning: Empty input filepath.\n");
                break;
            default:
                fprintf(stderr,"Warning: Multiple outputs specified. (Note: Enclose the path in quotes if it contains spaces.)\n");
                [[fallthrough]];
            case 1:
                cmd_data.fp_in = fopen(argv[0],"r");
                if(!cmd_data.fp_in)
                    fprintf(stderr,"Error: The input file \"%s\" somehow cannot be opened!\n",argv[0]);
                else
                    cmd_data.in = argv[0];
                break;
        }
    }

    inline void o(Cmd_Data& cmd_data,const char*const*const argv,const size_t argc){
        switch(argc){
            case 0:
                fprintf(stderr,"Warning: Empty output filepath.\n");
                break;
            default:
                fprintf(stderr,"Warning: Multiple outputs specified. (Note: Enclose the path in quotes if it contains spaces.)\n");
                [[fallthrough]];
            case 1:
                cmd_data.out_cpp = argv[0];
                break;
        }
    }

    inline void h(Cmd_Data& cmd_data,const char*const*const argv,const size_t argc){
        switch(argc){
            case 0:
                fprintf(stderr,"Warning: Empty output header filepath.\n");
                break;
            default:
                fprintf(stderr,"Warning: Multiple output headers specified. (Note: Enclose the path in quotes if it contains spaces.)\n");
                [[fallthrough]];
            case 1:
                cmd_data.out_h = argv[0];
                break;
        }
    }

    inline void t(Cmd_Data& cmd_data,const char*const*const argv,const size_t argc){
        switch(argc){
            case 0:
                fprintf(stderr,"Warning: Empty output token def filepath.\n");
                break;
            default:
                fprintf(stderr,"Warning: Multiple output token def files specified. (Note: Enclose the path in quotes if it contains spaces.)\n");
                [[fallthrough]];
            case 1:
                cmd_data.out_token = argv[0];
                break;
        }
    }

    inline void l(Cmd_Data& cmd_data,const char*const*const argv,const size_t argc){
        switch(argc){
            case 0:
                fprintf(stderr,"Warning: Empty log filepath.\n");
                break;
            default:
                fprintf(stderr,"Warning: Multiple log paths specified. (Note: Enclose the path in quotes if it contains spaces.)\n");
                [[fallthrough]];
            case 1:
                cmd_data.fp_log = fopen(argv[0],"w");
                if(!cmd_data.fp_log)
                    fprintf(stderr,"Error: The log file \"%s\" somehow cannot be opened!\n",argv[0]);
        }
    }

    inline void version(FILE* const out) {fprintf(out,"pg by Yuki, version 1.0\n");}
    inline void version(Cmd_Data&,const char*const*,size_t) {version(stderr); exit(EXIT_SUCCESS);}
}

inline constexpr yuki::Cmd_Option<Cmd_Data> coarr[] = {
    {"version",U'v',cmd_impl::version,"Print version information."},
    {"",U'i',cmd_impl::i,"The input file."},
    {"",U'o',cmd_impl::o,"The output cpp file."},
    {"",U'h',cmd_impl::h,"The output header file."},
    {"",U't',cmd_impl::t,"The output token def file."},
    {"",U'l',cmd_impl::l,"The output log file."}
};

inline bool Cmd_Data::post_process(){
    if(!fp_in){
        fprintf(stderr,"Error: No input is specified! (Note: use \"-i\" to specify input file.)\n");
        return false;
    }

    // Check for input file-ext collision.
    const auto [input_no_ext,input_ext] = yuki::vsplit_filename(in);

    if(input_ext=="cpp" || input_ext=="h" || input_ext=="hpp" || input_ext=="log"){
        fprintf(stderr,"Error: The input file has extension \".cpp\", \".h\", \".hpp\", or \".log\", which might collide with the output files!\n");
        return false;
    }

    // Set default values;

    std::string out_cpp_default;
    if(out_cpp.empty()){
        // The default cpp-out filename is the same as the input file.
        out_cpp_default.reserve(input_no_ext.size()+4);
        out_cpp_default.append(input_no_ext);
        out_cpp_default.append(".cpp");
        out_cpp = out_cpp_default;
    }
    fp_out_cpp=fopen(out_cpp.data(),"w");
    if(!fp_out_cpp){
        fprintf(stderr,"Error: The out file \"%s\" somehow cannot be created!\n",out_cpp.data());
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
        fprintf(stderr,"Error: The out header \"%s\" somehow cannot be created!\n",out_h.c_str());
        return false;
    }

    if(out_token.empty()){
        // The default token-out filename is the same as the CPP-OUT file.
        out_token.reserve(out_cpp_no_ext.size()+10);
        out_token.append(out_cpp_no_ext);
        out_token.append(".token.hpp");
    }
    out_cpp = std::string_view{};
    fp_out_token=fopen(out_token.c_str(),"w");
    if(!fp_out_token){
        fprintf(stderr,"Error: The out token def file \"%s\" somehow cannot be created!\n",out_token.c_str());
        return false;
    }

    return true;
} // bool Cmd_Data::post_process()
} // namespace yuki::pg
