#pragma once

#include"lexer.hpp"
#include<yuki/pg/lr1.hpp>
#include"gen/parser.token.hpp"
namespace xxx{
struct SParser_Tables{
    typedef yuki::pg::LR1_Action_Table<Token_Settings,12,5> Action_Table;
    typedef yuki::pg::LR1_Goto_Table<Token_Settings,12> Goto_Table;
    static constinit Action_Table action_table;
    static constinit Goto_Table goto_table;
};

struct SParser final : yuki::pg::AbsLR1Parser<Token_Settings>, private SParser_Tables {
    Lexer* lexer;

    using SParser_Tables::Action_Table;
    using SParser_Tables::Goto_Table;
    using SParser_Tables::action_table;
    using SParser_Tables::goto_table;

    constexpr SParser() noexcept = default;
    explicit constexpr SParser(Lexer* const l) noexcept : lexer(l) {}

    int parse(Lexer&);

    virtual int parse() override final {assert(lexer); return parse(*lexer);}


}; // struct SParser
} // namespace xxx
