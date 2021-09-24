// This is an internal header. Users shouldn't include this directly.
#pragma once
#include<yuki/print.hpp>

#ifndef YUKI_PG_TARGET_DBG_LOG
#define YUKI_PG_TARGET_DBG_LOG "YUKI_PG_TARGET.log"
#endif
#ifndef YUKI_PG_TARGET_DBG_FP
#define YUKI_PG_TARGET_DBG_FP yuki::pg::dbg_fp_
#endif

namespace yuki::pg{
    inline FILE* dbg_fp_=fopen(YUKI_PG_TARGET_DBG_LOG,"w");
    void dbgout(std::string_view format,const auto&... messages) {yuki::dbgout_base(YUKI_PG_TARGET_DBG_FP,"YUKI_PG_TARGET",format,messages...);}
}

#ifdef YUKI_PG_TARGET_DBG
#define YUKI_PG_TARGET_DBGO(...) yuki::pg::dbgout(__VA_ARGS__)
#define  CONSTEXPR_YUKI_PG_TARGET_DBG // Debug output would render constexpr-functions non-constexpr.
#else
#define YUKI_PG_TARGET_DBGO(...)
#define  CONSTEXPR_YUKI_PG_TARGET_DBG constexpr
#endif

#ifdef YUKI_PG_TARGET_DBG_ANY_TOKEN_DTOR
#define YUKI_PG_TARGET_DBGO_ANY_TOKEN_DTOR(...) yuki::pg::dbgout(__VA_ARGS__)
#else
#define YUKI_PG_TARGET_DBGO_ANY_TOKEN_DTOR(...)
#endif

#ifdef YUKI_PG_TARGET_DBG_ANY_TOKEN_MOVE_ASSIGN
#define YUKI_PG_TARGET_DBGO_ANY_TOKEN_MOVE_ASSIGN(...) yuki::pg::dbgout(__VA_ARGS__)
#else
#define YUKI_PG_TARGET_DBGO_ANY_TOKEN_MOVE_ASSIGN(...)
#endif