#include"cconfig"
#include"debug.hpp"
#include"lr1_writer.h"
#include"cmd.hpp"
#include"meta_lexer0.h"
#include"meta_lexer1.h"
#include<fmt/core.h>
int main(int argc,char** argv){
    namespace ypg=yuki::pg;
    static ypg::Cmd_Data cmd_data;
    yuki::Cmd_Lexer<ypg::Cmd_Data> cmd_lexer(std::move(ypg::opt_table));
    cmd_lexer.process(cmd_data,argc,argv);
    if(!cmd_data.post_process())
        std::exit(EXIT_FAILURE);

    ypg::Meta_Lexer0 meta_lexer0(std::move(cmd_data));
    meta_lexer0.lex();

    rewind(meta_lexer0.cmd_data.fp_in);

    try{
    switch(yuki::uint_auto_f(meta_lexer0.nterms.size()+meta_lexer0.terms.size())){
        case yuki::uint_enum::UCHAR : {
            ypg::Meta_Lexer1<unsigned char> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned char>::write(meta_lexer1.cmd_data,meta_lexer1.options,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs);
            break;
        }
        case yuki::uint_enum::USHORT : {
            ypg::Meta_Lexer1<unsigned short> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned short>::write(meta_lexer1.cmd_data,meta_lexer1.options,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs);
            break;
        }
        default : {
            ypg::Meta_Lexer1<unsigned> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned>::write(meta_lexer1.cmd_data,meta_lexer1.options,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs);
            break;
        }
    }
    }catch(const std::runtime_error& e){
        fprintf(stderr,"ICE: %s\n",e.what());
        std::exit(EXIT_FAILURE);
    }
}