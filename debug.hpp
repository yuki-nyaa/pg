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
#else
#define YUKI_PG_DBGO(...)
#endif
