#include"LR1_Writer.h"
#include"cmd.hpp"
#include"Meta_Lexer0.h"
#include"Meta_Lexer1.h"

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

    yuki::pg::Meta_Lexer0 meta_lexer0(std::move(cmd_data));
    meta_lexer0.lex();

    rewind(meta_lexer0.cmd_data.fp_in);

    try{
    switch(yuki::uint_auto_f(meta_lexer0.nterms.size()+meta_lexer0.terms.size())){
        case yuki::uint_enum::UCHAR : {
            yuki::pg::Meta_Lexer1<unsigned char> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            yuki::pg::LR1_Writer<unsigned char>::write(meta_lexer1.cmd_data,meta_lexer1.options,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs);
            return meta_lexer1.errors ? EXIT_FAILURE : EXIT_SUCCESS;
        }
        case yuki::uint_enum::USHORT : {
            yuki::pg::Meta_Lexer1<unsigned short> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            yuki::pg::LR1_Writer<unsigned short>::write(meta_lexer1.cmd_data,meta_lexer1.options,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs);
            return meta_lexer1.errors ? EXIT_FAILURE : EXIT_SUCCESS;
        }
        default : {
            yuki::pg::Meta_Lexer1<unsigned> meta_lexer1(std::move(meta_lexer0));
            meta_lexer1.lex();
            yuki::pg::LR1_Writer<unsigned>::write(meta_lexer1.cmd_data,meta_lexer1.options,meta_lexer1.code_htable,meta_lexer1.nterms,meta_lexer1.terms,meta_lexer1.rs);
            return meta_lexer1.errors ? EXIT_FAILURE : EXIT_SUCCESS;
        }
    }
    }catch(const std::runtime_error& e){
        fprintf(stderr,"ICE: %s\n",e.what());
        exit(EXIT_FAILURE);
    }
}
