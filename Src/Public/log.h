#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

// 调试代码
enum EDEBUG
{
    EVERBOSE,
    EDEBUG,
    EINFO,
    EWARNNING,
    EERROR,
    EDEBUG_MAX
};

#define DEBUG_LEVEL EDEBUG
#define LOG(level, format, ...) \
    { \
        if (level >= DEBUG_LEVEL) { \
            printf("[%s %s line:%d]"format"\n",__FILE__,__FUNCTION__,__LINE__, ##__VA_ARGS__); \
        } \
    }

#endif
