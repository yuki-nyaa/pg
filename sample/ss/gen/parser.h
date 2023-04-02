#pragma once

#include"lexer.hpp"
#include<yuki/pg/lr1.hpp>
#include"gen/parser.token.hpp"
namespace xxx{
struct SParser_Tables{
    struct Action_Table{
      private:
        Token_Settings::Token_t t_;
      public:
        Action_Table(Lexer& l_) noexcept : t_(l_.lex()) {}
        yuki::pg::lr1_action_return_type operator()(yuki::pg::AbsLR1Parser<Token_Settings>&,Lexer&);
    };
    typedef yuki::pg::LR1_Goto_Table<Token_Settings,12> Goto_Table;
    static constinit Goto_Table goto_table;
};

struct SParser final : yuki::pg::AbsLR1Parser<Token_Settings>, private SParser_Tables {
    Lexer* lexer;

    using SParser_Tables::Action_Table;
    using SParser_Tables::Goto_Table;
    using SParser_Tables::goto_table;

    constexpr SParser() noexcept = default;
    explicit constexpr SParser(Lexer* const l) noexcept : lexer(l) {}

    int parse(Lexer&);

    virtual int parse() override final {assert(lexer); return parse(*lexer);}


}; // struct SParser
} // namespace xxx
