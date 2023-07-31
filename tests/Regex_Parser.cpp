#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"Regex_Parser.h"
namespace yuki::lex{
yuki::pg::lr1_action_return_type Regex_Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Regex_Parser_TS>& p_,Regex_Lexer& l_){
using enum Regex_Parser_TS::Token_Kind::enum_t;
switch(p_.state()){
case 0: S0: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),1); t_=l_.lex(); goto S1;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 0
case 1: S1: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case amount: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "*": p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    case "+": p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    case "?": p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case "|": p_.SHIFT_(std::move(t_),15); t_=l_.lex(); goto S15;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    case EOF_: p_.SET_STATE(0); return {EOF_,0};
    default: return {(size_t)-1,(size_t)-1};
} // case 1
case 2: S2: switch(t_.kind()){
    case amount: p_.SET_STATE(11); return {amount,11};
    case character: p_.SET_STATE(11); return {character,11};
    case meta_char: p_.SET_STATE(11); return {meta_char,11};
    case basic_char_class: p_.SET_STATE(11); return {basic_char_class,11};
    case "(": p_.SET_STATE(11); return {"(",11};
    case "[": p_.SET_STATE(11); return {"[",11};
    case "*": p_.SET_STATE(11); return {"*",11};
    case "+": p_.SET_STATE(11); return {"+",11};
    case "?": p_.SET_STATE(11); return {"?",11};
    case "|": p_.SET_STATE(11); return {"|",11};
    case "^": p_.SET_STATE(11); return {"^",11};
    case esc_N: p_.SET_STATE(11); return {esc_N,11};
    case EOF_: p_.SET_STATE(11); return {EOF_,11};
    default: return {(size_t)-1,(size_t)-1};
} // case 2
case 3: S3: switch(t_.kind()){
    case amount: p_.SET_STATE(12); return {amount,12};
    case character: p_.SET_STATE(12); return {character,12};
    case meta_char: p_.SET_STATE(12); return {meta_char,12};
    case basic_char_class: p_.SET_STATE(12); return {basic_char_class,12};
    case "(": p_.SET_STATE(12); return {"(",12};
    case "[": p_.SET_STATE(12); return {"[",12};
    case "*": p_.SET_STATE(12); return {"*",12};
    case "+": p_.SET_STATE(12); return {"+",12};
    case "?": p_.SET_STATE(12); return {"?",12};
    case "|": p_.SET_STATE(12); return {"|",12};
    case "^": p_.SET_STATE(12); return {"^",12};
    case esc_N: p_.SET_STATE(12); return {esc_N,12};
    case EOF_: p_.SET_STATE(12); return {EOF_,12};
    default: return {(size_t)-1,(size_t)-1};
} // case 3
case 4: S4: switch(t_.kind()){
    case amount: p_.SET_STATE(13); return {amount,13};
    case character: p_.SET_STATE(13); return {character,13};
    case meta_char: p_.SET_STATE(13); return {meta_char,13};
    case basic_char_class: p_.SET_STATE(13); return {basic_char_class,13};
    case "(": p_.SET_STATE(13); return {"(",13};
    case "[": p_.SET_STATE(13); return {"[",13};
    case "*": p_.SET_STATE(13); return {"*",13};
    case "+": p_.SET_STATE(13); return {"+",13};
    case "?": p_.SET_STATE(13); return {"?",13};
    case "|": p_.SET_STATE(13); return {"|",13};
    case "^": p_.SET_STATE(13); return {"^",13};
    case esc_N: p_.SET_STATE(13); return {esc_N,13};
    case EOF_: p_.SET_STATE(13); return {EOF_,13};
    default: return {(size_t)-1,(size_t)-1};
} // case 4
case 5: S5: switch(t_.kind()){
    case amount: p_.SET_STATE(23); return {amount,23};
    case character: p_.SET_STATE(23); return {character,23};
    case meta_char: p_.SET_STATE(23); return {meta_char,23};
    case basic_char_class: p_.SET_STATE(23); return {basic_char_class,23};
    case "(": p_.SET_STATE(23); return {"(",23};
    case "[": p_.SET_STATE(23); return {"[",23};
    case "*": p_.SET_STATE(23); return {"*",23};
    case "+": p_.SET_STATE(23); return {"+",23};
    case "?": p_.SET_STATE(23); return {"?",23};
    case "|": p_.SET_STATE(23); return {"|",23};
    case "^": p_.SET_STATE(23); return {"^",23};
    case esc_N: p_.SET_STATE(23); return {esc_N,23};
    case EOF_: p_.SET_STATE(23); return {EOF_,23};
    default: return {(size_t)-1,(size_t)-1};
} // case 5
case 6: S6: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),16); t_=l_.lex(); goto S16;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "?=": p_.SHIFT_(std::move(t_),17); t_=l_.lex(); goto S17;
    case "?l:": p_.SHIFT_(std::move(t_),18); t_=l_.lex(); goto S18;
    case "?d:": p_.SHIFT_(std::move(t_),19); t_=l_.lex(); goto S19;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 6
case 7: S7: switch(t_.kind()){
    case Char_Class: p_.SHIFT_(std::move(t_),20); t_=l_.lex(); goto S20;
    case Char_Interval: p_.SHIFT_(std::move(t_),21); t_=l_.lex(); goto S21;
    case Char_Intervals: p_.SHIFT_(std::move(t_),22); t_=l_.lex(); goto S22;
    case character: p_.SHIFT_(std::move(t_),23); t_=l_.lex(); goto S23;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "^": p_.SHIFT_(std::move(t_),24); t_=l_.lex(); goto S24;
    default: return {(size_t)-1,(size_t)-1};
} // case 7
case 8: S8: switch(t_.kind()){
    case amount: p_.SET_STATE(14); return {amount,14};
    case character: p_.SET_STATE(14); return {character,14};
    case meta_char: p_.SET_STATE(14); return {meta_char,14};
    case basic_char_class: p_.SET_STATE(14); return {basic_char_class,14};
    case "(": p_.SET_STATE(14); return {"(",14};
    case ")": p_.SET_STATE(14); return {")",14};
    case "[": p_.SET_STATE(14); return {"[",14};
    case "*": p_.SET_STATE(14); return {"*",14};
    case "+": p_.SET_STATE(14); return {"+",14};
    case "?": p_.SET_STATE(14); return {"?",14};
    case "|": p_.SET_STATE(14); return {"|",14};
    case "^": p_.SET_STATE(14); return {"^",14};
    case esc_N: p_.SET_STATE(14); return {esc_N,14};
    case EOF_: p_.SET_STATE(14); return {EOF_,14};
    default: return {(size_t)-1,(size_t)-1};
} // case 8
case 9: S9: switch(t_.kind()){
    case amount: p_.SET_STATE(15); return {amount,15};
    case character: p_.SET_STATE(15); return {character,15};
    case meta_char: p_.SET_STATE(15); return {meta_char,15};
    case basic_char_class: p_.SET_STATE(15); return {basic_char_class,15};
    case "(": p_.SET_STATE(15); return {"(",15};
    case ")": p_.SET_STATE(15); return {")",15};
    case "[": p_.SET_STATE(15); return {"[",15};
    case "*": p_.SET_STATE(15); return {"*",15};
    case "+": p_.SET_STATE(15); return {"+",15};
    case "?": p_.SET_STATE(15); return {"?",15};
    case "|": p_.SET_STATE(15); return {"|",15};
    case "^": p_.SET_STATE(15); return {"^",15};
    case esc_N: p_.SET_STATE(15); return {esc_N,15};
    case EOF_: p_.SET_STATE(15); return {EOF_,15};
    default: return {(size_t)-1,(size_t)-1};
} // case 9
case 10: S10: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case amount: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case "*": p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    case "+": p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    case "?": p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case character: p_.SET_STATE(1); return {character,1};
    case meta_char: p_.SET_STATE(1); return {meta_char,1};
    case basic_char_class: p_.SET_STATE(1); return {basic_char_class,1};
    case "(": p_.SET_STATE(1); return {"(",1};
    case ")": p_.SET_STATE(1); return {")",1};
    case "[": p_.SET_STATE(1); return {"[",1};
    case "|": p_.SET_STATE(1); return {"|",1};
    case "^": p_.SET_STATE(1); return {"^",1};
    case esc_N: p_.SET_STATE(1); return {esc_N,1};
    case EOF_: p_.SET_STATE(1); return {EOF_,1};
    default: return {(size_t)-1,(size_t)-1};
} // case 10
case 11: S11: switch(t_.kind()){
    case amount: p_.SET_STATE(7); return {amount,7};
    case character: p_.SET_STATE(7); return {character,7};
    case meta_char: p_.SET_STATE(7); return {meta_char,7};
    case basic_char_class: p_.SET_STATE(7); return {basic_char_class,7};
    case "(": p_.SET_STATE(7); return {"(",7};
    case "[": p_.SET_STATE(7); return {"[",7};
    case "*": p_.SET_STATE(7); return {"*",7};
    case "+": p_.SET_STATE(7); return {"+",7};
    case "?": p_.SET_STATE(7); return {"?",7};
    case "|": p_.SET_STATE(7); return {"|",7};
    case "^": p_.SET_STATE(7); return {"^",7};
    case esc_N: p_.SET_STATE(7); return {esc_N,7};
    case EOF_: p_.SET_STATE(7); return {EOF_,7};
    default: return {(size_t)-1,(size_t)-1};
} // case 11
case 12: S12: switch(t_.kind()){
    case amount: p_.SET_STATE(4); return {amount,4};
    case character: p_.SET_STATE(4); return {character,4};
    case meta_char: p_.SET_STATE(4); return {meta_char,4};
    case basic_char_class: p_.SET_STATE(4); return {basic_char_class,4};
    case "(": p_.SET_STATE(4); return {"(",4};
    case "[": p_.SET_STATE(4); return {"[",4};
    case "*": p_.SET_STATE(4); return {"*",4};
    case "+": p_.SET_STATE(4); return {"+",4};
    case "?": p_.SET_STATE(4); return {"?",4};
    case "|": p_.SET_STATE(4); return {"|",4};
    case "^": p_.SET_STATE(4); return {"^",4};
    case esc_N: p_.SET_STATE(4); return {esc_N,4};
    case EOF_: p_.SET_STATE(4); return {EOF_,4};
    default: return {(size_t)-1,(size_t)-1};
} // case 12
case 13: S13: switch(t_.kind()){
    case amount: p_.SET_STATE(5); return {amount,5};
    case character: p_.SET_STATE(5); return {character,5};
    case meta_char: p_.SET_STATE(5); return {meta_char,5};
    case basic_char_class: p_.SET_STATE(5); return {basic_char_class,5};
    case "(": p_.SET_STATE(5); return {"(",5};
    case "[": p_.SET_STATE(5); return {"[",5};
    case "*": p_.SET_STATE(5); return {"*",5};
    case "+": p_.SET_STATE(5); return {"+",5};
    case "?": p_.SET_STATE(5); return {"?",5};
    case "|": p_.SET_STATE(5); return {"|",5};
    case "^": p_.SET_STATE(5); return {"^",5};
    case esc_N: p_.SET_STATE(5); return {esc_N,5};
    case EOF_: p_.SET_STATE(5); return {EOF_,5};
    default: return {(size_t)-1,(size_t)-1};
} // case 13
case 14: S14: switch(t_.kind()){
    case amount: p_.SET_STATE(6); return {amount,6};
    case character: p_.SET_STATE(6); return {character,6};
    case meta_char: p_.SET_STATE(6); return {meta_char,6};
    case basic_char_class: p_.SET_STATE(6); return {basic_char_class,6};
    case "(": p_.SET_STATE(6); return {"(",6};
    case "[": p_.SET_STATE(6); return {"[",6};
    case "*": p_.SET_STATE(6); return {"*",6};
    case "+": p_.SET_STATE(6); return {"+",6};
    case "?": p_.SET_STATE(6); return {"?",6};
    case "|": p_.SET_STATE(6); return {"|",6};
    case "^": p_.SET_STATE(6); return {"^",6};
    case esc_N: p_.SET_STATE(6); return {esc_N,6};
    case EOF_: p_.SET_STATE(6); return {EOF_,6};
    default: return {(size_t)-1,(size_t)-1};
} // case 14
case 15: S15: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),25); t_=l_.lex(); goto S25;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 15
case 16: S16: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case amount: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case ")": p_.SHIFT_(std::move(t_),26); t_=l_.lex(); goto S26;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "*": p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    case "+": p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    case "?": p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case "|": p_.SHIFT_(std::move(t_),15); t_=l_.lex(); goto S15;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 16
case 17: S17: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),27); t_=l_.lex(); goto S27;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 17
case 18: S18: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),28); t_=l_.lex(); goto S28;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 18
case 19: S19: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),29); t_=l_.lex(); goto S29;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 19
case 20: S20: switch(t_.kind()){
    case "*": p_.SHIFT_(std::move(t_),30); t_=l_.lex(); goto S30;
    case "+": p_.SHIFT_(std::move(t_),31); t_=l_.lex(); goto S31;
    case "-": p_.SHIFT_(std::move(t_),32); t_=l_.lex(); goto S32;
    default: return {(size_t)-1,(size_t)-1};
} // case 20
case 21: S21: switch(t_.kind()){
    case character: p_.SET_STATE(25); return {character,25};
    case "]": p_.SET_STATE(25); return {"]",25};
    default: return {(size_t)-1,(size_t)-1};
} // case 21
case 22: S22: switch(t_.kind()){
    case Char_Interval: p_.SHIFT_(std::move(t_),34); t_=l_.lex(); goto S34;
    case character: p_.SHIFT_(std::move(t_),23); t_=l_.lex(); goto S23;
    case "]": p_.SHIFT_(std::move(t_),35); t_=l_.lex(); goto S35;
    default: return {(size_t)-1,(size_t)-1};
} // case 22
case 23: S23: switch(t_.kind()){
    case "-": p_.SHIFT_(std::move(t_),36); t_=l_.lex(); goto S36;
    case character: p_.SET_STATE(27); return {character,27};
    case "]": p_.SET_STATE(27); return {"]",27};
    default: return {(size_t)-1,(size_t)-1};
} // case 23
case 24: S24: switch(t_.kind()){
    case Char_Class: p_.SHIFT_(std::move(t_),37); t_=l_.lex(); goto S37;
    case Char_Interval: p_.SHIFT_(std::move(t_),38); t_=l_.lex(); goto S38;
    case Char_Intervals: p_.SHIFT_(std::move(t_),39); t_=l_.lex(); goto S39;
    case character: p_.SHIFT_(std::move(t_),23); t_=l_.lex(); goto S23;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    default: return {(size_t)-1,(size_t)-1};
} // case 24
case 25: S25: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case amount: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case "*": p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    case "+": p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    case "?": p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case character: p_.SET_STATE(2); return {character,2};
    case meta_char: p_.SET_STATE(2); return {meta_char,2};
    case basic_char_class: p_.SET_STATE(2); return {basic_char_class,2};
    case "(": p_.SET_STATE(2); return {"(",2};
    case ")": p_.SET_STATE(2); return {")",2};
    case "[": p_.SET_STATE(2); return {"[",2};
    case "|": p_.SET_STATE(2); return {"|",2};
    case "^": p_.SET_STATE(2); return {"^",2};
    case esc_N: p_.SET_STATE(2); return {esc_N,2};
    case EOF_: p_.SET_STATE(2); return {EOF_,2};
    default: return {(size_t)-1,(size_t)-1};
} // case 25
case 26: S26: switch(t_.kind()){
    case amount: p_.SET_STATE(3); return {amount,3};
    case character: p_.SET_STATE(3); return {character,3};
    case meta_char: p_.SET_STATE(3); return {meta_char,3};
    case basic_char_class: p_.SET_STATE(3); return {basic_char_class,3};
    case "(": p_.SET_STATE(3); return {"(",3};
    case ")": p_.SET_STATE(3); return {")",3};
    case "[": p_.SET_STATE(3); return {"[",3};
    case "*": p_.SET_STATE(3); return {"*",3};
    case "+": p_.SET_STATE(3); return {"+",3};
    case "?": p_.SET_STATE(3); return {"?",3};
    case "|": p_.SET_STATE(3); return {"|",3};
    case "^": p_.SET_STATE(3); return {"^",3};
    case esc_N: p_.SET_STATE(3); return {esc_N,3};
    case EOF_: p_.SET_STATE(3); return {EOF_,3};
    default: return {(size_t)-1,(size_t)-1};
} // case 26
case 27: S27: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case amount: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case ")": p_.SHIFT_(std::move(t_),40); t_=l_.lex(); goto S40;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "*": p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    case "+": p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    case "?": p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case "|": p_.SHIFT_(std::move(t_),15); t_=l_.lex(); goto S15;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 27
case 28: S28: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case amount: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case ")": p_.SHIFT_(std::move(t_),41); t_=l_.lex(); goto S41;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "*": p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    case "+": p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    case "?": p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case "|": p_.SHIFT_(std::move(t_),15); t_=l_.lex(); goto S15;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 28
case 29: S29: switch(t_.kind()){
    case Regex: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case Char_Class: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case amount: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case character: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case meta_char: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case ")": p_.SHIFT_(std::move(t_),42); t_=l_.lex(); goto S42;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case "*": p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    case "+": p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    case "?": p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case "|": p_.SHIFT_(std::move(t_),15); t_=l_.lex(); goto S15;
    case "^": p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    case esc_N: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    default: return {(size_t)-1,(size_t)-1};
} // case 29
case 30: S30: switch(t_.kind()){
    case Char_Class: p_.SHIFT_(std::move(t_),43); t_=l_.lex(); goto S43;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    default: return {(size_t)-1,(size_t)-1};
} // case 30
case 31: S31: switch(t_.kind()){
    case Char_Class: p_.SHIFT_(std::move(t_),44); t_=l_.lex(); goto S44;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    default: return {(size_t)-1,(size_t)-1};
} // case 31
case 32: S32: switch(t_.kind()){
    case Char_Class: p_.SHIFT_(std::move(t_),45); t_=l_.lex(); goto S45;
    case basic_char_class: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "[": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    default: return {(size_t)-1,(size_t)-1};
} // case 32
case 33: S33: switch(t_.kind()){
    case amount: p_.SET_STATE(16); return {amount,16};
    case character: p_.SET_STATE(16); return {character,16};
    case meta_char: p_.SET_STATE(16); return {meta_char,16};
    case basic_char_class: p_.SET_STATE(16); return {basic_char_class,16};
    case "(": p_.SET_STATE(16); return {"(",16};
    case ")": p_.SET_STATE(16); return {")",16};
    case "[": p_.SET_STATE(16); return {"[",16};
    case "]": p_.SET_STATE(16); return {"]",16};
    case "*": p_.SET_STATE(16); return {"*",16};
    case "+": p_.SET_STATE(16); return {"+",16};
    case "?": p_.SET_STATE(16); return {"?",16};
    case "|": p_.SET_STATE(16); return {"|",16};
    case "^": p_.SET_STATE(16); return {"^",16};
    case "-": p_.SET_STATE(16); return {"-",16};
    case esc_N: p_.SET_STATE(16); return {esc_N,16};
    case EOF_: p_.SET_STATE(16); return {EOF_,16};
    default: return {(size_t)-1,(size_t)-1};
} // case 33
case 34: S34: switch(t_.kind()){
    case character: p_.SET_STATE(24); return {character,24};
    case "]": p_.SET_STATE(24); return {"]",24};
    default: return {(size_t)-1,(size_t)-1};
} // case 34
case 35: S35: switch(t_.kind()){
    case amount: p_.SET_STATE(17); return {amount,17};
    case character: p_.SET_STATE(17); return {character,17};
    case meta_char: p_.SET_STATE(17); return {meta_char,17};
    case basic_char_class: p_.SET_STATE(17); return {basic_char_class,17};
    case "(": p_.SET_STATE(17); return {"(",17};
    case ")": p_.SET_STATE(17); return {")",17};
    case "[": p_.SET_STATE(17); return {"[",17};
    case "]": p_.SET_STATE(17); return {"]",17};
    case "*": p_.SET_STATE(17); return {"*",17};
    case "+": p_.SET_STATE(17); return {"+",17};
    case "?": p_.SET_STATE(17); return {"?",17};
    case "|": p_.SET_STATE(17); return {"|",17};
    case "^": p_.SET_STATE(17); return {"^",17};
    case "-": p_.SET_STATE(17); return {"-",17};
    case esc_N: p_.SET_STATE(17); return {esc_N,17};
    case EOF_: p_.SET_STATE(17); return {EOF_,17};
    default: return {(size_t)-1,(size_t)-1};
} // case 35
case 36: S36: switch(t_.kind()){
    case character: p_.SHIFT_(std::move(t_),46); t_=l_.lex(); goto S46;
    default: return {(size_t)-1,(size_t)-1};
} // case 36
case 37: S37: switch(t_.kind()){
    case "]": p_.SHIFT_(std::move(t_),47); t_=l_.lex(); goto S47;
    default: return {(size_t)-1,(size_t)-1};
} // case 37
case 38: S38: switch(t_.kind()){
    case character: p_.SET_STATE(25); return {character,25};
    case "]": p_.SET_STATE(25); return {"]",25};
    default: return {(size_t)-1,(size_t)-1};
} // case 38
case 39: S39: switch(t_.kind()){
    case Char_Interval: p_.SHIFT_(std::move(t_),34); t_=l_.lex(); goto S34;
    case character: p_.SHIFT_(std::move(t_),23); t_=l_.lex(); goto S23;
    case "]": p_.SHIFT_(std::move(t_),48); t_=l_.lex(); goto S48;
    default: return {(size_t)-1,(size_t)-1};
} // case 39
case 40: S40: switch(t_.kind()){
    case amount: p_.SET_STATE(8); return {amount,8};
    case character: p_.SET_STATE(8); return {character,8};
    case meta_char: p_.SET_STATE(8); return {meta_char,8};
    case basic_char_class: p_.SET_STATE(8); return {basic_char_class,8};
    case "(": p_.SET_STATE(8); return {"(",8};
    case ")": p_.SET_STATE(8); return {")",8};
    case "[": p_.SET_STATE(8); return {"[",8};
    case "*": p_.SET_STATE(8); return {"*",8};
    case "+": p_.SET_STATE(8); return {"+",8};
    case "?": p_.SET_STATE(8); return {"?",8};
    case "|": p_.SET_STATE(8); return {"|",8};
    case "^": p_.SET_STATE(8); return {"^",8};
    case esc_N: p_.SET_STATE(8); return {esc_N,8};
    case EOF_: p_.SET_STATE(8); return {EOF_,8};
    default: return {(size_t)-1,(size_t)-1};
} // case 40
case 41: S41: switch(t_.kind()){
    case amount: p_.SET_STATE(9); return {amount,9};
    case character: p_.SET_STATE(9); return {character,9};
    case meta_char: p_.SET_STATE(9); return {meta_char,9};
    case basic_char_class: p_.SET_STATE(9); return {basic_char_class,9};
    case "(": p_.SET_STATE(9); return {"(",9};
    case ")": p_.SET_STATE(9); return {")",9};
    case "[": p_.SET_STATE(9); return {"[",9};
    case "*": p_.SET_STATE(9); return {"*",9};
    case "+": p_.SET_STATE(9); return {"+",9};
    case "?": p_.SET_STATE(9); return {"?",9};
    case "|": p_.SET_STATE(9); return {"|",9};
    case "^": p_.SET_STATE(9); return {"^",9};
    case esc_N: p_.SET_STATE(9); return {esc_N,9};
    case EOF_: p_.SET_STATE(9); return {EOF_,9};
    default: return {(size_t)-1,(size_t)-1};
} // case 41
case 42: S42: switch(t_.kind()){
    case amount: p_.SET_STATE(10); return {amount,10};
    case character: p_.SET_STATE(10); return {character,10};
    case meta_char: p_.SET_STATE(10); return {meta_char,10};
    case basic_char_class: p_.SET_STATE(10); return {basic_char_class,10};
    case "(": p_.SET_STATE(10); return {"(",10};
    case ")": p_.SET_STATE(10); return {")",10};
    case "[": p_.SET_STATE(10); return {"[",10};
    case "*": p_.SET_STATE(10); return {"*",10};
    case "+": p_.SET_STATE(10); return {"+",10};
    case "?": p_.SET_STATE(10); return {"?",10};
    case "|": p_.SET_STATE(10); return {"|",10};
    case "^": p_.SET_STATE(10); return {"^",10};
    case esc_N: p_.SET_STATE(10); return {esc_N,10};
    case EOF_: p_.SET_STATE(10); return {EOF_,10};
    default: return {(size_t)-1,(size_t)-1};
} // case 42
case 43: S43: switch(t_.kind()){
    case "]": p_.SHIFT_(std::move(t_),49); t_=l_.lex(); goto S49;
    default: return {(size_t)-1,(size_t)-1};
} // case 43
case 44: S44: switch(t_.kind()){
    case "]": p_.SHIFT_(std::move(t_),50); t_=l_.lex(); goto S50;
    default: return {(size_t)-1,(size_t)-1};
} // case 44
case 45: S45: switch(t_.kind()){
    case "]": p_.SHIFT_(std::move(t_),51); t_=l_.lex(); goto S51;
    default: return {(size_t)-1,(size_t)-1};
} // case 45
case 46: S46: switch(t_.kind()){
    case character: p_.SET_STATE(26); return {character,26};
    case "]": p_.SET_STATE(26); return {"]",26};
    default: return {(size_t)-1,(size_t)-1};
} // case 46
case 47: S47: switch(t_.kind()){
    case amount: p_.SET_STATE(20); return {amount,20};
    case character: p_.SET_STATE(20); return {character,20};
    case meta_char: p_.SET_STATE(20); return {meta_char,20};
    case basic_char_class: p_.SET_STATE(20); return {basic_char_class,20};
    case "(": p_.SET_STATE(20); return {"(",20};
    case ")": p_.SET_STATE(20); return {")",20};
    case "[": p_.SET_STATE(20); return {"[",20};
    case "]": p_.SET_STATE(20); return {"]",20};
    case "*": p_.SET_STATE(20); return {"*",20};
    case "+": p_.SET_STATE(20); return {"+",20};
    case "?": p_.SET_STATE(20); return {"?",20};
    case "|": p_.SET_STATE(20); return {"|",20};
    case "^": p_.SET_STATE(20); return {"^",20};
    case "-": p_.SET_STATE(20); return {"-",20};
    case esc_N: p_.SET_STATE(20); return {esc_N,20};
    case EOF_: p_.SET_STATE(20); return {EOF_,20};
    default: return {(size_t)-1,(size_t)-1};
} // case 47
case 48: S48: switch(t_.kind()){
    case amount: p_.SET_STATE(21); return {amount,21};
    case character: p_.SET_STATE(21); return {character,21};
    case meta_char: p_.SET_STATE(21); return {meta_char,21};
    case basic_char_class: p_.SET_STATE(21); return {basic_char_class,21};
    case "(": p_.SET_STATE(21); return {"(",21};
    case ")": p_.SET_STATE(21); return {")",21};
    case "[": p_.SET_STATE(21); return {"[",21};
    case "]": p_.SET_STATE(21); return {"]",21};
    case "*": p_.SET_STATE(21); return {"*",21};
    case "+": p_.SET_STATE(21); return {"+",21};
    case "?": p_.SET_STATE(21); return {"?",21};
    case "|": p_.SET_STATE(21); return {"|",21};
    case "^": p_.SET_STATE(21); return {"^",21};
    case "-": p_.SET_STATE(21); return {"-",21};
    case esc_N: p_.SET_STATE(21); return {esc_N,21};
    case EOF_: p_.SET_STATE(21); return {EOF_,21};
    default: return {(size_t)-1,(size_t)-1};
} // case 48
case 49: S49: switch(t_.kind()){
    case amount: p_.SET_STATE(19); return {amount,19};
    case character: p_.SET_STATE(19); return {character,19};
    case meta_char: p_.SET_STATE(19); return {meta_char,19};
    case basic_char_class: p_.SET_STATE(19); return {basic_char_class,19};
    case "(": p_.SET_STATE(19); return {"(",19};
    case ")": p_.SET_STATE(19); return {")",19};
    case "[": p_.SET_STATE(19); return {"[",19};
    case "]": p_.SET_STATE(19); return {"]",19};
    case "*": p_.SET_STATE(19); return {"*",19};
    case "+": p_.SET_STATE(19); return {"+",19};
    case "?": p_.SET_STATE(19); return {"?",19};
    case "|": p_.SET_STATE(19); return {"|",19};
    case "^": p_.SET_STATE(19); return {"^",19};
    case "-": p_.SET_STATE(19); return {"-",19};
    case esc_N: p_.SET_STATE(19); return {esc_N,19};
    case EOF_: p_.SET_STATE(19); return {EOF_,19};
    default: return {(size_t)-1,(size_t)-1};
} // case 49
case 50: S50: switch(t_.kind()){
    case amount: p_.SET_STATE(18); return {amount,18};
    case character: p_.SET_STATE(18); return {character,18};
    case meta_char: p_.SET_STATE(18); return {meta_char,18};
    case basic_char_class: p_.SET_STATE(18); return {basic_char_class,18};
    case "(": p_.SET_STATE(18); return {"(",18};
    case ")": p_.SET_STATE(18); return {")",18};
    case "[": p_.SET_STATE(18); return {"[",18};
    case "]": p_.SET_STATE(18); return {"]",18};
    case "*": p_.SET_STATE(18); return {"*",18};
    case "+": p_.SET_STATE(18); return {"+",18};
    case "?": p_.SET_STATE(18); return {"?",18};
    case "|": p_.SET_STATE(18); return {"|",18};
    case "^": p_.SET_STATE(18); return {"^",18};
    case "-": p_.SET_STATE(18); return {"-",18};
    case esc_N: p_.SET_STATE(18); return {esc_N,18};
    case EOF_: p_.SET_STATE(18); return {EOF_,18};
    default: return {(size_t)-1,(size_t)-1};
} // case 50
case 51: S51: switch(t_.kind()){
    case amount: p_.SET_STATE(22); return {amount,22};
    case character: p_.SET_STATE(22); return {character,22};
    case meta_char: p_.SET_STATE(22); return {meta_char,22};
    case basic_char_class: p_.SET_STATE(22); return {basic_char_class,22};
    case "(": p_.SET_STATE(22); return {"(",22};
    case ")": p_.SET_STATE(22); return {")",22};
    case "[": p_.SET_STATE(22); return {"[",22};
    case "]": p_.SET_STATE(22); return {"]",22};
    case "*": p_.SET_STATE(22); return {"*",22};
    case "+": p_.SET_STATE(22); return {"+",22};
    case "?": p_.SET_STATE(22); return {"?",22};
    case "|": p_.SET_STATE(22); return {"|",22};
    case "^": p_.SET_STATE(22); return {"^",22};
    case "-": p_.SET_STATE(22); return {"-",22};
    case esc_N: p_.SET_STATE(22); return {esc_N,22};
    case EOF_: p_.SET_STATE(22); return {EOF_,22};
    default: return {(size_t)-1,(size_t)-1};
} // case 51
default: return {(size_t)-1,(size_t)-1};
} // switch(p_.state())
} // yuki::pg::lr1_action_return_type Regex_Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Regex_Parser_TS>& p_,Regex_Lexer& l_)

constinit Regex_Parser_Tables::Goto_Table Regex_Parser_Tables::goto_table = {
}; // constinit Regex_Parser_Tables::Goto_Table Regex_Parser_Tables::goto_table

int Regex_Parser::parse(Regex_Lexer& l_){
    reset();
    stack_.reserve(128);
    Action_Table action_table(l_);

    while(true){
        yuki::pg::lr1_action_return_type art_ = action_table(*this,l_);
        switch(art_.rule){
            case 1:{ // Regex -> Regex Regex
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                FSM& token0_ = stack_[start_+0].token.get<1>();
                FSM& token1_ = stack_[start_+1].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},ff.make_seq(token0_,token1_));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> Regex Regex\n");
                break;
            } // case 1 //Regex -> Regex Regex
            case 4:{ // Regex -> Regex "*"
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                FSM& token0_ = stack_[start_+0].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},ff.make_ast(branch,token0_));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> Regex \"*\"\n");
                break;
            } // case 4 //Regex -> Regex "*"
            case 5:{ // Regex -> Regex "+"
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                FSM& token0_ = stack_[start_+0].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},ff.make_plus(branch,token0_));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> Regex \"+\"\n");
                break;
            } // case 5 //Regex -> Regex "+"
            case 6:{ // Regex -> Regex "?"
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                FSM& token0_ = stack_[start_+0].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},ff.make_qmark(branch,token0_));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> Regex \"?\"\n");
                break;
            } // case 6 //Regex -> Regex "?"
            case 7:{ // Regex -> Regex amount
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                FSM& token0_ = stack_[start_+0].token.get<1>();
                Amount& token1_ = stack_[start_+1].token.get<4>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},ff.make_amount(branch,token0_,token1_));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> Regex amount\n");
                break;
            } // case 7 //Regex -> Regex amount
            case 2:{ // Regex -> Regex "|" Regex
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                FSM& token0_ = stack_[start_+0].token.get<1>();
                FSM& token2_ = stack_[start_+2].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end},ff.make_alt(branch,token0_,token2_));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> Regex \"|\" Regex\n");
                break;
            } // case 2 //Regex -> Regex "|" Regex
            case 11:{ // Regex -> Char_Class
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                yuki::IntegralCIs_OV<char32_t>& token0_ = stack_[start_+0].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},ff.make_fsm(branch,std::move(token0_)));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> Char_Class\n");
                break;
            } // case 11 //Regex -> Char_Class
            case 12:{ // Regex -> character
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                char32_t& token0_ = stack_[start_+0].token.get<5>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},ff.make_fsm(branch,yuki::CInterval<char32_t>{token0_,token0_}));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> character\n");
                break;
            } // case 12 //Regex -> character
            case 13:{ // Regex -> meta_char
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                int& token0_ = stack_[start_+0].token.get<6>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},ff.make_fsm(branch,token0_));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> meta_char\n");
                break;
            } // case 13 //Regex -> meta_char
            case 3:{ // Regex -> "(" Regex ")"
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                FSM& token1_ = stack_[start_+1].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end},token1_);
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> \"(\" Regex \")\"\n");
                break;
            } // case 3 //Regex -> "(" Regex ")"
            case 8:{ // Regex -> "(" "?=" Regex ")"
                assert(stack_.size()>=4);
                const size_t start_ = stack_.size()-4;
                FSM& token2_ = stack_[start_+2].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+3].token.location_range().end},token2_.make_head());
                FSM& token_target_ = token_target_complete_.get<1>();

                {
            if(!heads_.empty() && heads_.back()==branch){
                fprintf(stderr,"Error: Multiple lookaheads in branch %zu!\n",branch);
                ++errors;
            }else
                heads_.emplace_back(branch);
                }

                stack_.pop_back(4);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> \"(\" \"?=\" Regex \")\"\n");
                break;
            } // case 8 //Regex -> "(" "?=" Regex ")"
            case 9:{ // Regex -> "(" "?l:" Regex ")"
                assert(stack_.size()>=4);
                const size_t start_ = stack_.size()-4;
                FSM& token2_ = stack_[start_+2].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+3].token.location_range().end},token2_.make_lazy());
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(4);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> \"(\" \"?l:\" Regex \")\"\n");
                break;
            } // case 9 //Regex -> "(" "?l:" Regex ")"
            case 10:{ // Regex -> "(" "?d:" Regex ")"
                assert(stack_.size()>=4);
                const size_t start_ = stack_.size()-4;
                FSM& token2_ = stack_[start_+2].token.get<1>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+3].token.location_range().end},token2_.make_dlazy());
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(4);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> \"(\" \"?d:\" Regex \")\"\n");
                break;
            } // case 10 //Regex -> "(" "?d:" Regex ")"
            case 14:{ // Regex -> "^"
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},ff.make_fsm(branch,MetaChar::BOL));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> \"^\"\n");
                break;
            } // case 14 //Regex -> "^"
            case 15:{ // Regex -> esc_N
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Regex>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},ff.make_esc_N(branch));
                FSM& token_target_ = token_target_complete_.get<1>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Regex);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Regex -> esc_N\n");
                break;
            } // case 15 //Regex -> esc_N
            case 23:{ // Char_Class -> basic_char_class
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                yuki::IntegralCIs_OV<char32_t>& token0_ = stack_[start_+0].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},std::move(token0_));
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> basic_char_class\n");
                break;
            } // case 23 //Char_Class -> basic_char_class
            case 16:{ // Char_Class -> "[" Char_Interval "]"
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                yuki::CInterval<char32_t>& token1_ = stack_[start_+1].token.get<3>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end},token1_);
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> \"[\" Char_Interval \"]\"\n");
                break;
            } // case 16 //Char_Class -> "[" Char_Interval "]"
            case 17:{ // Char_Class -> "[" Char_Intervals "]"
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                yuki::IntegralCIs_OV<char32_t>& token1_ = stack_[start_+1].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end},std::move(token1_));
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> \"[\" Char_Intervals \"]\"\n");
                break;
            } // case 17 //Char_Class -> "[" Char_Intervals "]"
            case 20:{ // Char_Class -> "[" "^" Char_Class "]"
                assert(stack_.size()>=4);
                const size_t start_ = stack_.size()-4;
                yuki::IntegralCIs_OV<char32_t>& token2_ = stack_[start_+2].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+3].token.location_range().end},yuki::negate<{U'\0',yuki::UNICODE_MAX_32}>(token2_));
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(4);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> \"[\" \"^\" Char_Class \"]\"\n");
                break;
            } // case 20 //Char_Class -> "[" "^" Char_Class "]"
            case 21:{ // Char_Class -> "[" "^" Char_Intervals "]"
                assert(stack_.size()>=4);
                const size_t start_ = stack_.size()-4;
                yuki::IntegralCIs_OV<char32_t>& token2_ = stack_[start_+2].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+3].token.location_range().end},yuki::negate<{U'\0',yuki::UNICODE_MAX_32}>(token2_));
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(4);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> \"[\" \"^\" Char_Intervals \"]\"\n");
                break;
            } // case 21 //Char_Class -> "[" "^" Char_Intervals "]"
            case 18:{ // Char_Class -> "[" Char_Class "+" Char_Class "]"
                assert(stack_.size()>=5);
                const size_t start_ = stack_.size()-5;
                yuki::IntegralCIs_OV<char32_t>& token1_ = stack_[start_+1].token.get<2>();
                yuki::IntegralCIs_OV<char32_t>& token3_ = stack_[start_+3].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+4].token.location_range().end},token1_+token3_);
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(5);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> \"[\" Char_Class \"+\" Char_Class \"]\"\n");
                break;
            } // case 18 //Char_Class -> "[" Char_Class "+" Char_Class "]"
            case 19:{ // Char_Class -> "[" Char_Class "*" Char_Class "]"
                assert(stack_.size()>=5);
                const size_t start_ = stack_.size()-5;
                yuki::IntegralCIs_OV<char32_t>& token1_ = stack_[start_+1].token.get<2>();
                yuki::IntegralCIs_OV<char32_t>& token3_ = stack_[start_+3].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+4].token.location_range().end},token1_*token3_);
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(5);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> \"[\" Char_Class \"*\" Char_Class \"]\"\n");
                break;
            } // case 19 //Char_Class -> "[" Char_Class "*" Char_Class "]"
            case 22:{ // Char_Class -> "[" Char_Class "-" Char_Class "]"
                assert(stack_.size()>=5);
                const size_t start_ = stack_.size()-5;
                yuki::IntegralCIs_OV<char32_t>& token1_ = stack_[start_+1].token.get<2>();
                yuki::IntegralCIs_OV<char32_t>& token3_ = stack_[start_+3].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Class>,{stack_[start_].token.location_range().begin,stack_[start_+4].token.location_range().end},token1_-token3_);
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(5);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Class);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Class -> \"[\" Char_Class \"-\" Char_Class \"]\"\n");
                break;
            } // case 22 //Char_Class -> "[" Char_Class "-" Char_Class "]"
            case 27:{ // Char_Interval -> character
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                char32_t& token0_ = stack_[start_+0].token.get<5>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Interval>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},token0_,token0_);
                yuki::CInterval<char32_t>& token_target_ = token_target_complete_.get<3>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Interval);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Interval -> character\n");
                break;
            } // case 27 //Char_Interval -> character
            case 26:{ // Char_Interval -> character "-" character
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                char32_t& token0_ = stack_[start_+0].token.get<5>();
                char32_t& token2_ = stack_[start_+2].token.get<5>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Interval>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end},token0_,token2_);
                yuki::CInterval<char32_t>& token_target_ = token_target_complete_.get<3>();

                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Interval);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Interval -> character \"-\" character\n");
                break;
            } // case 26 //Char_Interval -> character "-" character
            case 25:{ // Char_Intervals -> Char_Interval
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                yuki::CInterval<char32_t>& token0_ = stack_[start_+0].token.get<3>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Intervals>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end},token0_);
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Intervals);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Intervals -> Char_Interval\n");
                break;
            } // case 25 //Char_Intervals -> Char_Interval
            case 24:{ // Char_Intervals -> Char_Intervals Char_Interval
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                yuki::IntegralCIs_OV<char32_t>& token0_ = stack_[start_+0].token.get<2>();
                yuki::CInterval<char32_t>& token1_ = stack_[start_+1].token.get<3>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Char_Intervals>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},std::move((token0_.insert(token1_),token0_)));
                yuki::IntegralCIs_OV<char32_t>& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Char_Intervals);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Char_Intervals -> Char_Intervals Char_Interval\n");
                break;
            } // case 24 //Char_Intervals -> Char_Intervals Char_Interval
            case 0:{ // Goal_ -> Regex
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                FSM& token0_ = stack_[start_+0].token.get<1>();
                {
                }

                stack_.pop_back(1);
                return 0;
            }
            default:{
                yuki::print_error(stderr,"Syntax Error!\n");
                reset();
                yuki::print_error(stderr,"Stack Clear!\n");
                return 1;
            }
        } // switch(art_.rule)
    } // while(true)
} // int Regex_Parser::parse(Regex_Lexer&)
} // namespace yuki::lex


