#pragma once

#ifdef YUKI_PG_META_DBG
    #include<yuki/print.hpp>
    #ifndef YUKI_PG_META_DBG_LOG
    #define YUKI_PG_META_DBG_LOG "YUKI_PG_META.log"
    #endif
    namespace yuki::pg{
    #ifndef YUKI_PG_META_DBG_FP
    inline FILE* const meta_dbg_fp_=fopen(YUKI_PG_META_DBG_LOG,"w");
    #define YUKI_PG_META_DBG_FP yuki::pg::meta_dbg_fp_
    #endif
    }
    #define YUKI_PG_META_DBGO(...) yuki::dbgout_base(YUKI_PG_META_DBG_FP,"YUKI_PG_META",__VA_ARGS__)
    #undef NDEBUG
    #define DEBUG_REFLEX YUKI_PG_META_REFLEX
#else
    #define YUKI_PG_META_DBGO(...)
    #define NDEBUG
    #undef DEBUG_REFLEX
#endif
