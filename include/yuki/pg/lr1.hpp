// Facilities for (LA)LR1 parser.
#pragma once
#include<type_traits>
#include<concepts>
#include<cassert>
#include<initializer_list>
#include<vector>
#include<yuki/pg/core.hpp>

namespace yuki::pg{
enum struct LR1_Action_Kind : unsigned char {ERR=0,S=1,R=2}; // The values are set explicitly, so that instead of writing the tedious "LR1_Action_Kind::XXX" we can simply write a number in the generated parsing table, thus reducing the file size, (but at the cost of some readability).

template<std::unsigned_integral state_t,std::unsigned_integral rule_num_t>
struct LR1_Action_Entry{
  private:
    LR1_Action_Kind kind_;
    union{
        state_t state_;
        rule_num_t rule_;
    };
  public:
    constexpr LR1_Action_Entry() noexcept : kind_(LR1_Action_Kind::ERR), state_(0) {}

    template<typename S_R>
    constexpr void set(LR1_Action_Kind kind_target, S_R s_r) noexcept {
        kind_=kind_target;
        switch(kind_){
            case LR1_Action_Kind::S :{ state_=s_r; break; }
            case LR1_Action_Kind::R :{ rule_=s_r; break; }
        }
    }
    template<typename S_R>
    constexpr void set(unsigned kind_target, S_R s_r) noexcept {set<S_R>(static_cast<LR1_Action_Kind>(kind_target),s_r);}

    template<typename S_R>
    constexpr LR1_Action_Entry(LR1_Action_Kind kind_target, S_R s_r) noexcept {set(kind_target,s_r);}
    template<typename S_R>
    constexpr LR1_Action_Entry(unsigned kind_target, S_R s_r) noexcept {set(static_cast<LR1_Action_Kind>(kind_target),s_r);}

    constexpr LR1_Action_Kind kind() const noexcept {return kind_;}

    constexpr state_t state() const noexcept {
        assert(kind_==LR1_Action_Kind::S);
        return state_;
    }
    constexpr rule_num_t rule() const noexcept {
        assert(kind_==LR1_Action_Kind::R);
        return rule_;
    }
}; // struct LR1_Action_Entry<state_t,rule_num_t>

template<std::unsigned_integral state_t>
using LR1_Goto_Entry = state_t;

template<typename TS, size_t state_size_, size_t rule_size_>
struct LR1_Action_Table{
  public:
    typedef yuki::uint_auto_t<state_size_> state_t;
    typedef yuki::uint_auto_t<rule_size_> rule_num_t;
    static constexpr state_t state_size = state_size_;
    static constexpr rule_num_t rule_size = rule_size_;
  private:
    LR1_Action_Entry<state_t,rule_num_t> table_[state_size][TS::terminal_total];
    struct Action_Table_Tuple{
        state_t state;
        typename TS::Token_Kind_t kind;
        LR1_Action_Entry<state_t,rule_num_t> ae;
    };
  public:
    constexpr LR1_Action_Table() noexcept : table_{} {}
    constexpr LR1_Action_Table(std::initializer_list<Action_Table_Tuple> l) noexcept : table_{} {
        for(const Action_Table_Tuple& att : l){
            table_[att.state][TS::terminal_kind_to_index(att.kind)]=att.ae;
        }
    }

    constexpr LR1_Action_Entry<state_t,rule_num_t>& operator()(state_t state,typename TS::Token_Kind_t kind) noexcept {
        assert(state<state_size_);
        return table_[state][TS::terminal_kind_to_index(kind)];
    }
    constexpr const LR1_Action_Entry<state_t,rule_num_t>& operator()(state_t state,typename TS::Token_Kind_t kind) const noexcept {
        assert(state<state_size_);
        return table_[state][TS::terminal_kind_to_index(kind)];
    }
};

template<typename TS, size_t state_size_>
struct LR1_Goto_Table{
  public:
    typedef yuki::uint_auto_t<state_size_> state_t;
    static constexpr state_t state_size = state_size_;
  private:
    LR1_Goto_Entry<state_t> table_[state_size][TS::nterminal_total];
    struct Goto_Table_Tuple{
        state_t state;
        typename TS::Token_Kind_t kind;
        LR1_Goto_Entry<state_t> ge;
    };
  public:
    constexpr LR1_Goto_Table() noexcept : table_{} {}
    constexpr LR1_Goto_Table(std::initializer_list<Goto_Table_Tuple> l) noexcept : table_{} {
        for(const Goto_Table_Tuple& gtt : l){
            table_[gtt.state][TS::nterminal_kind_to_index(gtt.kind)]=gtt.ge;
        }
    }

    constexpr LR1_Goto_Entry<state_t>& operator()(state_t state,typename TS::Token_Kind_t kind) noexcept {
        assert(state<state_size_);
        return table_[state][TS::nterminal_kind_to_index(kind)];
    }
    constexpr const LR1_Goto_Entry<state_t>& operator()(state_t state,typename TS::Token_Kind_t kind) const noexcept {
        assert(state<state_size_);
        return table_[state][TS::nterminal_kind_to_index(kind)];
    }
};



template<typename TS,typename L,size_t state_size_p,size_t rule_size_p>
struct AbsLR1Parser : AbsParser<TS>{
    static_assert(std::is_same_v<TS,typename L::Token_Settings>);
    static_assert(state_size_p!=0);
    static_assert(rule_size_p!=0);
  public:
    typedef yuki::pg::Any_Token<TS> Any_Token;

    typedef L Lexer_t;

    typedef yuki::uint_auto_t<state_size_p-1> state_t;
    typedef yuki::uint_auto_t<rule_size_p-1> rule_num_t;
    static constexpr size_t state_size = state_size_p;
    static constexpr size_t rule_size = rule_size_p;

    typedef LR1_Action_Table<TS,state_size_p,rule_size_p> Action_Table;
    typedef LR1_Goto_Table<TS,state_size_p> Goto_Table;

    struct Token_State_Pair{
        Any_Token token;
        state_t state;
    };

    template<typename T>
    using stack_t = std::vector<T>;

    // MNumber 512. The number 512 is rather arbitrary, but should suffice for normal parsing.
    explicit AbsLR1Parser(Lexer_t* p=nullptr,state_t state = 0,size_t stack_depth_expected=512) : lexer_(p), state_(state) {stack_.reserve(stack_depth_expected);}

    constexpr state_t state() const noexcept {return state_;}
    constexpr const stack_t<Token_State_Pair>& stack() const noexcept {return stack_;}
    constexpr const Lexer_t& lexer() const noexcept {return *lexer_;}

    void reset(){
        state_ = 0;
        stack_.clear();
    }
  protected:
    Lexer_t* lexer_;
    stack_t<Token_State_Pair> stack_;
    state_t state_;
};
} // namespace yuki::pg

#ifdef YUKI_PG_TARGET_PRINT
#include<fmt/core.h>
template<typename CharT>
fmt::formatter<yuki::pg::LR1_Action_Kind,CharT> : yuki::simple_formatter<yuki::pg::LR1_Action_Kind,CharT> {
    template<typename FormatContext>
    auto format(yuki::pg::LR1_Action_Kind ak,FormatContext& ctx){
        return fmt::format_to(ctx.out(),"{}",static_cast<unsigned>(ak));
    }
};
template<typename CharT,std::unsigned_integral state_t,std::unsigned_integral rule_num_t>
fmt::formatter<yuki::pg::LR1_Action_Entry<state_t,rule_num_t>,CharT> : yuki::simple_formatter<yuki::pg::LR1_Action_Entry<state_t,rule_num_t>,CharT> {
    template<typename FormatContext>
    auto format(const LR1_Action_Entry<state_t,rule_num_t>& ae,FormatContext& ctx){
        yuki::pg::LR1_Action_Kind k=ae.get_kind();
        fmt::format_to(ctx.out(),"({},",l);
        switch(k){
            case LR1_Action_Kind::S : {fmt::format_to(ctx.out(),"{}",ae.get_state());break;}
            case LR1_Action_Kind::R : {fmt::format_to(ctx.out(),"{}",ae.get_rule());break;}
        }
        return fmt::format_to(ctx.out(),")");
    }
};
#endif