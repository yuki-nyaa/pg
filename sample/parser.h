// LR1-parser class definition and parsing table declarations.
#include<yuki/ParserGen/lr1.hpp>
#include"parser.common.hpp"
#include"lexer.h"
namespace xxx{
    struct Parser : private yuki::pg::lr1::AbsParser<Token_Settings,Lexer,11U,4U>{
        public:
            typedef yuki::pg::lr1::AbsParser<Token_Settings,Lexer,11U,4U> Base_;
            using Base_::has_alg;
            using Base_::Token_Settings;
            using Base_::Lexer_t;
            using Base_::lexer_ptr;
            using Base_::state_t;
            using Base_::rule_num_t;
            using Base_::STATE_MAX;
            using Base_::STATE_INITIAL;
            using Base_::RULE_NUM_MAX;
            using Base_::Action_Table_t;
            using Base_::Goto_Table_t;
            using Base_::Token_State_Pair;

            Parser(Lexer_t* p=nullptr,state_t state=STATE_INITIAL) : Base_(p,state) {}

            using Base_::get_state;

            void initialize(){
                state_ = STATE_INITIAL;
                vec_.clear();
            }

            static constinit Action_Table_t action_table;
            static constinit Goto_Table_t goto_table;

            int parse();
    };
} // namespace xxx
