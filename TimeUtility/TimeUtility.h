#pragma once
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#define ONE_HOUR	3600	//one hour
#define ONE_DAY		86400	//one day
#define ONE_WEEK	604800	//one week

//常用时间相关接口

inline int64_t GetCurrentMS()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//是否同一天
inline bool IsTheSameDay(time_t t1, time_t t2)
{
	struct tm tm1;
	localtime_r(&t1, &tm1);

	struct tm tm2;
	localtime_r(&t2, &tm2);

	return tm1.tm_year == tm2.tm_year && tm1.tm_yday == tm2.tm_yday;
}
//获得未来某个时间的时间戳
inline time_t GetFeatureTime(time_t tNow, int day, int hour)
{
	struct tm tm;
	localtime_r(&tNow, &tm);
	
	struct tm featureTime = { 0 };
	featureTime.tm_year = tm.tm_year;
	featureTime.tm_mon = tm.tm_mon;
	featureTime.tm_mday = tm.tm_mday;

	time_t tFeatureTime = mktime(&featureTime);
	tFeatureTime += (day * 3600 * 24 + hour * 3600);
	return tFeatureTime;
}
//获得某个时间当天某个小时的时间戳
inline time_t GetDayBeginTime(time_t tCurTime, int hourDiff)
{
	struct tm tm;
	localtime_r(&tCurTime, &tm);
	struct tm dayBeginTime = { 0 };
	dayBeginTime.tm_year = tm.tm_year;
	dayBeginTime.tm_mon = tm.tm_mon;
	dayBeginTime.tm_mday = tm.tm_mday;
	dayBeginTime.tm_hour = hourDiff;
	return mktime(&dayBeginTime);
}
//获得当前时间这周的某天某时的时间戳
inline time_t GetWeekBeginTime(time_t tCurTime, int weekday, int hourDiff)
{
	struct tm tm;
	localtime_r(&tCurTime, &tm);
	
	struct tm weekBeginTime = { 0 };
	weekBeginTime.tm_year = tm.tm_year;
	weekBeginTime.tm_mon = tm.tm_mon;
	weekBeginTime.tm_mday = tm.tm_mday;
	weekBeginTime.tm_hour = hourDiff;
	int dayDiff = weekday - tm.tm_wday;
	return  mktime(&weekBeginTime) + (dayDiff * 60 * 60 * 24);
}
//获得某个时间在当年的第几周
inline int GetWeekday(time_t tCurTime)
{
	struct tm tm;
	localtime_r(&tCurTime, &tm);

	return tm.tm_wday;
}
//获得某个时间在当天的第几分钟
inline int GetDayMinutes(time_t tCurTime)
{
	struct tm tm;
	localtime_r(&tCurTime, &tm);

	return tm.tm_hour * 60 + tm.tm_min;
}
//格式化字符串时间至时间戳
inline time_t StringToDatetime(const std::string& str)
{
	char *cha = (char*)str.data();             // 将string转换成char*。
	tm tm_ = { 0 };                                    // 定义tm结构体。
	int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
	sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// 将string存储的日期时间，转换为int临时变量。
	tm_.tm_year = year - 1900;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
	tm_.tm_mon = month - 1;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
	tm_.tm_mday = day;                         // 日。
	tm_.tm_hour = hour;                        // 时。
	tm_.tm_min = minute;                       // 分。
	tm_.tm_sec = second;                       // 秒。
	tm_.tm_isdst = 0;                          // 非夏令时。
	time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
	return t_;                                 // 返回值。 
}