// Facilities for (LA)LR1 parser.
#pragma once
#include<type_traits>
#include<concepts>
#include<cassert>
#include<initializer_list>
#include<iostream>
#include<vector>
#include<yuki/ParserGen/core.hpp>
namespace yuki::pg{
    namespace lr1{}
    namespace lalr1 = lr1;
}

namespace yuki::pg::lr1{
    typedef unsigned char Action_Kind_t;
    struct Action_Kind{
        enum Enum : Action_Kind_t {ERR=0,S=1,R=2,ACC=3}; // The values are set explicitly, so that instead of writing the tedious "Action_Kind::XXX" we can simply write a number in the generated parsing table, thus reducing the file size, (but at the cost of some readability).
    };

    inline std::ostream& operator<<(std::ostream& o, const Action_Kind_t ak){
        o<<static_cast<int>(ak);
        return o;
    }

    template<std::unsigned_integral state_t,std::unsigned_integral rule_num_t>
    struct Action_Entry{
        private:
            Action_Kind_t kind;
            union{
                state_t state;
                rule_num_t rule;
            };
        public:
            constexpr Action_Entry() noexcept : kind(Action_Kind::ERR){}

            template<typename S_R>
            constexpr void set(Action_Kind_t kind_target, S_R s_r) noexcept {
                kind=kind_target;
                switch(kind){
                    case Action_Kind::S :{ state=s_r; break; }
                    case Action_Kind::R :{ rule=s_r; break; }
                }
            }

            template<typename S_R>
            constexpr Action_Entry(Action_Kind_t kind_target, S_R s_r) noexcept {set(kind_target,s_r);}

            constexpr Action_Kind_t get_kind() const noexcept {return kind;}

            constexpr state_t get_state() const noexcept {
                assert(kind==Action_Kind::S);
                return state;
            }
            constexpr rule_num_t get_rule() const noexcept {
                assert(kind==Action_Kind::R);
                return rule;
            }
    };

    template<std::unsigned_integral state_t,std::unsigned_integral rule_num_t>
    std::ostream& operator<<(std::ostream& o,const Action_Entry<state_t,rule_num_t>& ae){
        Action_Kind_t k=ae.get_kind();
        o<<"("<<k<<",";
        switch(k){
            case Action_Kind::S : { o<<ae.get_state(); break; }
            case Action_Kind::R : { o<<ae.get_rule(); break; }
        }
        o<<")";
        return o;
    }

    template<std::unsigned_integral state_t>
    using Goto_Entry = state_t;

    template<
        typename TS,
        std::unsigned_integral state_t,
        state_t state_size_,
        std::unsigned_integral rule_num_t
    >
    struct Action_Table{
        private:
            Action_Entry<state_t,rule_num_t> table_[TS::terminal_size][state_size_];
            struct Action_Table_Tuple{
                typename TS::Token_Kind_t kind;
                state_t state;
                Action_Entry<state_t,rule_num_t> ae;
            };
        public:
            static constexpr typename TS::Token_Kind_t terminal_size = TS::terminal_size;
            static constexpr state_t state_size = state_size_;
            constexpr Action_Table() noexcept : table_() {}
            constexpr Action_Table(std::initializer_list<Action_Table_Tuple> l) noexcept : Action_Table() {
                for(const Action_Table_Tuple& att : l){
                    table_[TS::terminal_kind_normalize(att.kind)][att.state]=att.ae;
                }
            }

            constexpr Action_Entry<state_t,rule_num_t>& operator()(typename TS::Token_Kind_t kind,state_t state) noexcept {
                assert(state<state_size_);
                return table_[TS::terminal_kind_normalize(kind)][state];
            }
            constexpr const Action_Entry<state_t,rule_num_t>& operator()(typename TS::Token_Kind_t kind,state_t state) const noexcept {
                assert(state<state_size_);
                return table_[TS::terminal_kind_normalize(kind)][state];
            }
    };

    template<
        typename TS,
        std::unsigned_integral state_t,
        state_t state_size_
    >
    struct Goto_Table{
        private:
            Goto_Entry<state_t> table_[TS::nterminal_size][state_size_];
            struct Goto_Table_Tuple{
                typename TS::Token_Kind_t kind;
                state_t state;
                Goto_Entry<state_t> ge;
            };
        public:
            static constexpr typename TS::Token_Kind_t nterminal_size = TS::nterminal_size;
            static constexpr state_t state_size = state_size_;
            constexpr Goto_Table() noexcept : table_() {}
            constexpr Goto_Table(std::initializer_list<Goto_Table_Tuple> l) noexcept : Goto_Table() {
                for(const Goto_Table_Tuple& gtt : l){
                    table_[TS::nterminal_kind_normalize(gtt.kind)][gtt.state]=gtt.ge;
                }
            }

            constexpr Goto_Entry<state_t>& operator()(typename TS::Token_Kind_t kind,state_t state) noexcept {
                assert(state<state_size_);
                return table_[TS::nterminal_kind_normalize(kind)][state];
            }
            constexpr const Goto_Entry<state_t>& operator()(typename TS::Token_Kind_t kind,state_t state) const noexcept {
                assert(state<state_size_);
                return table_[TS::nterminal_kind_normalize(kind)][state];
            }
    };

    // Abstract (LA)LR1 parser.
    template<typename TS,typename L,auto STATE_MAX_A,auto RULE_NUM_MAX_A,size_t stack_depth_expected=20> requires std::is_same_v<typename L::Token_Settings,TS>
    struct AbsParser{
        public:
            static constexpr bool has_alg(unsigned char a) noexcept {
                if(a==Parsing_Algorithm::LALR1 || Parsing_Algorithm::LR1)
                    return true;
                else
                    return false;
            };
            typedef TS Token_Settings;

            typedef L Lexer_t;
            Lexer_t* lexer_ptr=nullptr;

            typedef decltype(STATE_MAX_A) state_t;
            typedef decltype(RULE_NUM_MAX_A) rule_num_t;
            static constexpr state_t STATE_MAX = STATE_MAX_A;
            static constexpr state_t STATE_INITIAL = 0;
            static constexpr rule_num_t RULE_NUM_MAX = RULE_NUM_MAX_A;

            typedef Action_Table<TS,state_t,STATE_MAX+1,rule_num_t> Action_Table_t;
            typedef Goto_Table<TS,state_t,STATE_MAX+1> Goto_Table_t;

            struct Token_State_Pair{
                typename TS::any_token token;
                state_t state;
            };

            AbsParser(Lexer_t* p=nullptr,state_t state=STATE_INITIAL) : vec_(),state_(state),lexer_ptr(p) {
                vec_.reserve(stack_depth_expected);
            }

            constexpr state_t get_state() const noexcept {return state_;}

        protected:
            std::vector<Token_State_Pair> vec_;
            state_t state_=STATE_INITIAL;
    };
} // namespace yuki::pg
