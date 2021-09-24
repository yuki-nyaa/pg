// Facilities for (LA)LR1 parser.
#pragma once
#include<type_traits>
#include<concepts>
#include<cassert>
#include<initializer_list>
#include<vector>
#include<yuki/ParserGen/core.hpp>
#ifdef YUKI_PG_TARGET_PRINT
#include<fmt/core.h>
#endif
namespace yuki::pg{
namespace lr1{}
namespace lalr1 = lr1;
}

namespace yuki::pg::lr1{
enum struct Action_Kind : unsigned char {ERR=0,S=1,R=2,ACC=3}; // The values are set explicitly, so that instead of writing the tedious "Action_Kind::XXX" we can simply write a number in the generated parsing table, thus reducing the file size, (but at the cost of some readability).
}
#ifdef YUKI_PG_TARGET_PRINT
template<typename CharT>
fmt::formatter<yuki::pg::lr1::Action_Kind,CharT> : yuki::simple_formatter<yuki::pg::lr1::Action_Kind,CharT> {
    template<typename FormatContext>
    auto format(Action_Kind ak,FormatContext& ctx){
        return fmt::format_to(ctx.out(),"{}",static_cast<unsigned>(ak));
    }
};
#endif
namespace yuki::pg::lr1{
template<std::unsigned_integral state_t,std::unsigned_integral rule_num_t>
struct Action_Entry{
  private:
    Action_Kind kind_;
    union{
        state_t state_;
        rule_num_t rule_;
    };
  public:
    constexpr Action_Entry() noexcept : kind_(Action_Kind::ERR), state_(){}

    template<typename S_R>
    constexpr void set(Action_Kind kind_target, S_R s_r) noexcept {
        kind_=kind_target;
        switch(kind_){
            case Action_Kind::S :{ state_=s_r; break; }
            case Action_Kind::R :{ rule_=s_r; break; }
        }
    }
    template<typename S_R>
    constexpr void set(unsigned kind_target, S_R s_r) noexcept {set<S_R>(static_cast<Action_Kind>(kind_target),s_r);}

    template<typename S_R>
    constexpr Action_Entry(Action_Kind kind_target, S_R s_r) noexcept {set(kind_target,s_r);}
    template<typename S_R>
    constexpr Action_Entry(unsigned kind_target, S_R s_r) noexcept {set(static_cast<Action_Kind>(kind_target),s_r);}

    constexpr Action_Kind kind() const noexcept {return kind_;}

    constexpr state_t state() const noexcept {
        assert(kind_==Action_Kind::S);
        return state_;
    }
    constexpr rule_num_t rule() const noexcept {
        assert(kind_==Action_Kind::R);
        return rule_;
    }
}; // struct Action_Entry<state_t,rule_num_t>
} // namespace yuki::pg::lr1
#ifdef YUKI_PG_TARGET_PRINT
template<typename CharT,std::unsigned_integral state_t,std::unsigned_integral rule_num_t>
fmt::formatter<yuki::pg::lr1::Action_Entry<state_t,rule_num_t>,CharT> : yuki::simple_formatter<yuki::pg::lr1::Action_Entry<state_t,rule_num_t>,CharT> {
    template<typename FormatContext>
    auto format(const Action_Entry<state_t,rule_num_t>& ae,FormatContext& ctx){
        Action_Kind k=ae.get_kind();
        fmt::format_to(ctx.out(),"({},",l);
        switch(k){
            case Action_Kind::S : {fmt::format_to(ctx.out(),"{}",ae.get_state());break;}
            case Action_Kind::R : {fmt::format_to(ctx.out(),"{}",ae.get_rule());break;}
        }
        return fmt::format_to(ctx.out(),")");
    }
};
#endif
namespace yuki::pg::lr1{
template<std::unsigned_integral state_t>
using Goto_Entry = state_t;

template<typename TS, size_t state_size_, size_t rule_size_>
struct Action_Table{
  public:
    typedef yuki::uint_auto_t<state_size_> state_t;
    typedef yuki::uint_auto_t<rule_size_> rule_num_t;
    static constexpr state_t state_size = state_size_;
    static constexpr rule_num_t rule_size = rule_size_;
    static constexpr size_t terminal_total = TS::terminal_total;
  private:
    Action_Entry<state_t,rule_num_t> table_[state_size][terminal_total];
    struct Action_Table_Tuple{
        state_t state;
        typename TS::Token_Kind_t kind;
        Action_Entry<state_t,rule_num_t> ae;
    };
  public:
    constexpr Action_Table() noexcept : table_() {}
    constexpr Action_Table(std::initializer_list<Action_Table_Tuple> l) noexcept : Action_Table() {
        for(const Action_Table_Tuple& att : l){
            table_[att.state][TS::terminal_kind_to_index(att.kind)]=att.ae;
        }
    }

    constexpr Action_Entry<state_t,rule_num_t>& operator()(state_t state,typename TS::Token_Kind_t kind) noexcept {
        assert(state<state_size_);
        return table_[state][TS::terminal_kind_to_index(kind)];
    }
    constexpr const Action_Entry<state_t,rule_num_t>& operator()(state_t state,typename TS::Token_Kind_t kind) const noexcept {
        assert(state<state_size_);
        return table_[state][TS::terminal_kind_to_index(kind)];
    }
};

template<typename TS, size_t state_size_>
struct Goto_Table{
  public:
    typedef yuki::uint_auto_t<state_size_> state_t;
    static constexpr state_t state_size = state_size_;
    static constexpr size_t nterminal_total = TS::nterminal_total;
  private:
    Goto_Entry<state_t> table_[state_size][nterminal_total];
    struct Goto_Table_Tuple{
        state_t state;
        typename TS::Token_Kind_t kind;
        Goto_Entry<state_t> ge;
    };
  public:
    constexpr Goto_Table() noexcept : table_() {}
    constexpr Goto_Table(std::initializer_list<Goto_Table_Tuple> l) noexcept : Goto_Table() {
        for(const Goto_Table_Tuple& gtt : l){
            table_[gtt.state][TS::nterminal_kind_to_index(gtt.kind)]=gtt.ge;
        }
    }

    constexpr Goto_Entry<state_t>& operator()(state_t state,typename TS::Token_Kind_t kind) noexcept {
        assert(state<state_size_);
        return table_[state][TS::nterminal_kind_to_index(kind)];
    }
    constexpr const Goto_Entry<state_t>& operator()(state_t state,typename TS::Token_Kind_t kind) const noexcept {
        assert(state<state_size_);
        return table_[state][TS::nterminal_kind_to_index(kind)];
    }
};

// Abstract (LA)LR1 parser.
template<typename TS,typename L,unsigned long long STATE_MAX_P,unsigned long long RULE_NUM_MAX_P> requires std::is_same_v<typename L::Token_Settings,TS>
struct AbsParser{
  public:
    typedef TS Token_Settings;

    typedef L Lexer_t;

    typedef yuki::uint_auto_t<STATE_MAX_P> state_t;
    typedef yuki::uint_auto_t<RULE_NUM_MAX_P> rule_num_t;
    static constexpr state_t STATE_MAX = STATE_MAX_P;
    static constexpr state_t STATE_INITIAL = 0;
    static constexpr rule_num_t RULE_NUM_MAX = RULE_NUM_MAX_P;

    typedef Action_Table<TS,STATE_MAX+1,RULE_NUM_MAX+1> Action_Table_t;
    typedef Goto_Table<TS,STATE_MAX+1> Goto_Table_t;

    typedef yuki::pg::Any_Token<TS> Any_Token;
    typedef TS::Token Token;
    typedef TS::Token_Kind Token_Kind;

    struct Token_State_Pair{
        Any_Token token;
        state_t state;
    };

    template<typename T>
    using stack_t = std::vector<T>;

    // MNumber 500. The number 500 is rather arbitrary, but should suffice for normal parsing.
    explicit AbsParser(Lexer_t* p=nullptr,state_t state = STATE_INITIAL,size_t stack_depth_expected=500) : lexer_(p), state_(state) {stack_.reserve(stack_depth_expected);}

    constexpr state_t state() const noexcept {return state_;}
    constexpr const stack_t<Token_State_Pair>& stack() const noexcept {return stack_;}
    constexpr const Lexer_t& lexer() const noexcept {return *lexer_;}

    void reset(){
        state_ = STATE_INITIAL;
        stack_.clear();
    }
  protected:
    Lexer_t* lexer_;
    stack_t<Token_State_Pair> stack_;
    state_t state_;
};

} // namespace yuki::pg::lr1
