#ifndef _Debug_h_
#define _Debug_h_

/* log4cplus */
#include <log4cplus/logger.h>           //class Logger
#include <log4cplus/loggingmacros.h>    //LOG4_TRACE()

/* Foundation */
#include "Include/Foundation/SystemInclude.h"  //__func__

#define  __LOCATION__ __func__ << "," << __LINE__ << "> "

#define LOG4_TRACE(x) LOG4CPLUS_TRACE(log4cplus::Logger::getRoot(), __LOCATION__ << x)
#define LOG4_DEBUG(x) LOG4CPLUS_DEBUG(log4cplus::Logger::getRoot(), __LOCATION__ << x)
#define LOG4_INFO(x) LOG4CPLUS_INFO(log4cplus::Logger::getRoot(), __LOCATION__ << x)
#define LOG4_WARN(x) LOG4CPLUS_WARN(log4cplus::Logger::getRoot(), __LOCATION__ << x)
#define LOG4_ERROR(x) LOG4CPLUS_ERROR(log4cplus::Logger::getRoot(), __LOCATION__ << x)
#define LOG4_FATAL(x) LOG4CPLUS_FATAL(log4cplus::Logger::getRoot(), __LOCATION__ << x)

#define LOG4_TRACE_C(x) LOG4CPLUS_TRACE(log4cplus::Logger::getRoot(), x)
#define LOG4_DEBUG_C(x) LOG4CPLUS_DEBUG(log4cplus::Logger::getRoot(), x)
#define LOG4_INFO_C(x) LOG4CPLUS_INFO(log4cplus::Logger::getRoot(), x)
#define LOG4_WARN_C(x) LOG4CPLUS_WARN(log4cplus::Logger::getRoot(), x)
#define LOG4_ERROR_C(x) LOG4CPLUS_ERROR(log4cplus::Logger::getRoot(), x)
#define LOG4_FATAL_C(x) LOG4CPLUS_FATAL(log4cplus::Logger::getRoot(), x)

std::string ConvertUtf8ToString(const char *src);

#endif /* _Debug_h_ */
