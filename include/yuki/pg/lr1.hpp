// Facilities for (LA)LR1 parser.
#pragma once
#include<type_traits>
#include<concepts>
#include<cassert>
#include<initializer_list>
#include<yuki/vector.hpp>
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
    LR1_Action_Entry() noexcept = default;

    template<typename S_R>
    constexpr LR1_Action_Entry(unsigned kind_target,S_R s_r) noexcept :
        kind_{static_cast<LR1_Action_Kind>(kind_target)},state_(0)
    {
        switch(kind_){
            case LR1_Action_Kind::S : state_=s_r;return;
            case LR1_Action_Kind::R : rule_=s_r;return;
            default:;
        }
    }

    constexpr LR1_Action_Kind kind() const noexcept {return kind_;}

    constexpr state_t state() const noexcept {
        assert(kind_==LR1_Action_Kind::S);
        return state_;
    }
    constexpr rule_num_t rule() const noexcept {
        assert(kind_==LR1_Action_Kind::R);
        return rule_;
    }
};

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
    constexpr LR1_Action_Table(std::initializer_list<Action_Table_Tuple> l) noexcept : table_{} {
        for(const Action_Table_Tuple att : l){
            table_[att.state][TS::terminal_kind_to_index(att.kind)]=att.ae;
        }
    }
    constexpr LR1_Action_Entry<state_t,rule_num_t> operator()(state_t state,typename TS::Token_Kind_t kind) const noexcept {
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
    state_t table_[state_size][TS::nterminal_total];
    struct Goto_Table_Tuple{
        state_t state;
        typename TS::Token_Kind_t kind;
        state_t ge;
    };
  public:
    constexpr LR1_Goto_Table(std::initializer_list<Goto_Table_Tuple> l) noexcept : table_{} {
        for(const Goto_Table_Tuple gtt : l){
            table_[gtt.state][TS::nterminal_kind_to_index(gtt.kind)]=gtt.ge;
        }
    }

    constexpr state_t operator()(state_t state,typename TS::Token_Kind_t kind) const noexcept {
        assert(state<state_size_);
        return table_[state][TS::nterminal_kind_to_index(kind)];
    }
};




template<typename TS>
struct AbsLR1Parser : AbsParser<TS>{
    struct Token_State_Pair{
        typename AbsParser<TS>::Any_Token token;
        size_t state;
    };

    constexpr size_t state() const noexcept {return state_;}
    constexpr const yuki::Vector<Token_State_Pair>& stack() const noexcept {return stack_;}

    void reset(){
        state_ = 0;
        stack_.clear();
    }

  protected:
    size_t state_;
    yuki::Vector<Token_State_Pair> stack_;
};
} // namespace yuki::pg
