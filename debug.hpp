#pragma once
#include<yuki/print.hpp>

#ifndef YUKI_PG_DBG_LOG
#define YUKI_PG_DBG_LOG "YUKI_PG.log"
#endif
#ifndef YUKI_PG_DBG_FP
#define YUKI_PG_DBG_FP yuki::pg::dbg_fp_
#endif

#ifdef YUKI_PG_DBG
namespace yuki::pg{
inline FILE* dbg_fp_=fopen(YUKI_PG_DBG_LOG,"w");
void dbgout(std::string_view format,const auto&... messages) {yuki::dbgout_base(YUKI_PG_DBG_FP,"YUKI_PG",format,messages...);}
}
#define YUKI_PG_DBGO(...) yuki::pg::dbgout(__VA_ARGS__)
#else
#define YUKI_PG_DBGO(...)
#endif