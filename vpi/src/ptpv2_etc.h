#ifndef PTPV2_ETC_H
#define PTPV2_ETC_H

#if defined(_MSC_VER)
#define PTPV2_DEBUG(...) \
    do { \
        if(ptpv2_cfg.debug) { \
            fprintf(stdout, "%s:%i %s(): ", __FILE__, __LINE__, __FUNCTION__);\
            fprintf(stdoutsterr, __VA_ARGS__);\
        } \
    } while(0)

#define PTPV2_ERROR(...) \
    do { \
        fprintf(stderr, "%s:%i %s(): ", __FILE__, __LINE__, __FUNCTION__);\
        fprintf(stderr, __VA_ARGS__);\
    } while(0)

#define DEBUG_PLAIN(fmt,...) DEBUG("[PLAIN] ", fmt, __VA_ARGS__)

#else

#define PTPV2_DEBUG(x...) \
    do { \
        if(ptpv2_cfg.debug) { \
            OUTPUT_SYSLOG(stdout, ##x); \
        } \
    } while(0)

#define PTPV2_ERROR(x...) \
    do { \
        OUTPUT_SYSLOG(stderr, ##x); \
    } while(0)

/// Output a message to syslog. Not meant to be used directly.
#define OUTPUT_SYSLOG(stream,fmt,x...) \
    fprintf(stream, "%s:%i %s(): " fmt, \
            __FILE__, __LINE__, __FUNCTION__, ##x); \

#define DEBUG_PLAIN(fmt,x...) DEBUG("[PLAIN] ", fmt, ##x)
#endif

#endif
