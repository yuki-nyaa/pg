#pragma once

#include"list.token.hpp"

#ifdef YUKI_PG_Parser_DBG
#include<yuki/print.hpp>
#ifndef YUKI_PG_Parser_DBG_LOG
#define YUKI_PG_Parser_DBG_LOG "YUKI_PG_Parser_DBG.log"
#endif
namespace xxx{
#ifndef YUKI_PG_Parser_DBG_FP
inline FILE* const YUKI_PG_Parser_DBG_fp_=fopen(YUKI_PG_Parser_DBG_LOG,"w");
#define YUKI_PG_Parser_DBG_FP xxx::YUKI_PG_Parser_DBG_fp_
#endif
} // namespace xxx
#define YUKI_PG_Parser_DBGO(...) yuki::dbgout_base(fp_dbg_,"YUKI_PG_Parser_DBG",__VA_ARGS__)
#define CONSTEXPR_YUKI_PG_Parser_DBG // Debug output would render constexpr-functions non-constexpr.
#else
#define YUKI_PG_Parser_DBGO(...)
#define CONSTEXPR_YUKI_PG_Parser_DBG constexpr
#endif

namespace xxx{
struct Parser{
    template<typename L_>
    int parse(L_&);
  private:
    yuki::pg::stack_t<yuki::pg::Token_State_Pair<Token_Settings::Token_t>> stack_;
    #ifdef YUKI_PG_Parser_DBG
    FILE* fp_fbg_=YUKI_PG_Parser_DBG_FP;
    #endif
  public:

}; // struct Parser
} // namespace xxx
