#pragma once

#include"lexer.hpp"
#include<yuki/pg/lr1.hpp>
#include"gen/parser.token.hpp"
namespace xxx{
struct Parser_Tables{
    typedef yuki::pg::LR1_Action_Table<Token_Settings,12,5> Action_Table;
    typedef yuki::pg::LR1_Goto_Table<Token_Settings,12> Goto_Table;
    static constinit Action_Table action_table;
    static constinit Goto_Table goto_table;
};

struct Parser final : yuki::pg::AbsLR1Parser<Token_Settings>, private Parser_Tables {
    Lexer* lexer;

    using Parser_Tables::Action_Table;
    using Parser_Tables::Goto_Table;
    using Parser_Tables::action_table;
    using Parser_Tables::goto_table;

    constexpr Parser() noexcept = default;
    explicit constexpr Parser(Lexer* const l) noexcept : lexer(l) {}

    int parse(Lexer&);

    virtual int parse() override final {assert(lexer); return parse(*lexer);}


}; // struct Parser
} // namespace xxx
