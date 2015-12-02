/*
 * LogCat.h
 *
 *  Created on: 2015-11-20
 *      Author: andy
 */

#ifndef LOGCAT_H_
#define LOGCAT_H_
#include <android/log.h>

#ifndef LOG_I
#define LOG_I(TAG,...) (__android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__))
#endif

#ifndef LOG_D
#define LOG_D(TAG,...) (__android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__))
#endif
#endif /* LOGCAT_H_ */
