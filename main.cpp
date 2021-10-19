#include"cconfig"
#include"debug.hpp"
#include"lr1_writer.h"
#include"cmd.hpp"
#include"meta_lexer0.h"
#include"meta_lexer1.h"
#include<fmt/core.h>
int main(int argc,char** argv){
    namespace ypg=yuki::pg;
    ypg::Cmd_Data cmd_data;
    yuki::Cmd_Lexer<ypg::Cmd_Data> cmd_lexer(std::move(ypg::opt_table));
    cmd_lexer.process(cmd_data,argc,argv);
    if(!cmd_data.post_process())
        std::exit(EXIT_FAILURE);

    ypg::Meta_Lexer0 meta_lexer0(cmd_data);
    meta_lexer0.lex();

    rewind(cmd_data.fp_in);

    switch(yuki::uint_auto_f(meta_lexer0.nterms.size()+meta_lexer0.terms.size())){
        case yuki::uint_enum::UCHAR : {
            ypg::Meta_Lexer1<unsigned char> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned char>::write(cmd_data,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs,meta_lexer1.assoc0);
            break;
        }
        case yuki::uint_enum::USHORT : {
            ypg::Meta_Lexer1<unsigned short> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned short>::write(cmd_data,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs,meta_lexer1.assoc0);
            break;
        }
        case yuki::uint_enum::UINT : {
            ypg::Meta_Lexer1<unsigned> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned>::write(cmd_data,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs,meta_lexer1.assoc0);
            break;
        }
        case yuki::uint_enum::ULONG : {
            ypg::Meta_Lexer1<unsigned long> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned long>::write(cmd_data,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs,meta_lexer1.assoc0);
            break;
        }
        case yuki::uint_enum::ULLONG : {
            ypg::Meta_Lexer1<unsigned long long> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            ypg::LR1_Writer<unsigned long long>::write(cmd_data,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs,meta_lexer1.assoc0);
            break;
        }
    }
}