// This is an internal header. Users shouldn't include this directly.
#pragma once

#ifdef YUKI_PG_DBG
#include<yuki/print.hpp>
#ifndef YUKI_PG_DBG_LOG
#define YUKI_PG_DBG_LOG "YUKI_PG.log"
#endif
namespace yuki::pg{
#ifndef YUKI_PG_DBG_FP
inline FILE* const dbg_fp_=fopen(YUKI_PG_DBG_LOG,"w");
#define YUKI_PG_DBG_FP yuki::pg::dbg_fp_
#endif
}
#define YUKI_PG_DBGO(...) yuki::dbgout_base(YUKI_PG_DBG_FP,"YUKI_PG",__VA_ARGS__)
#define  CONSTEXPR_YUKI_PG_DBG // Debug output would render constexpr-functions non-constexpr.
#else
#define YUKI_PG_DBGO(...)
#define  CONSTEXPR_YUKI_PG_DBG constexpr
#endif

#ifdef YUKI_PG_DBG_ANY_TOKEN_DTOR
#define YUKI_PG_DBGO_ANY_TOKEN_DTOR(...) YUKI_PG_DBGO(__VA_ARGS__)
#else
#define YUKI_PG_DBGO_ANY_TOKEN_DTOR(...)
#endif

#ifdef YUKI_PG_DBG_ANY_TOKEN_MOVE_ASSIGN
#define YUKI_PG_DBGO_ANY_TOKEN_MOVE_ASSIGN(...) YUKI_PG_DBGO(__VA_ARGS__)
#else
#define YUKI_PG_DBGO_ANY_TOKEN_MOVE_ASSIGN(...)
#endif
