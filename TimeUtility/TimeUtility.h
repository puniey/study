#pragma once
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string>

#define ONE_HOUR	3600	//one hour
#define ONE_DAY		86400	//one day
#define ONE_WEEK	604800	//one week

//����ʱ����ؽӿ�

inline int64_t GetCurrentMS()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//�Ƿ�ͬһ��
inline bool IsTheSameDay(time_t t1, time_t t2)
{
	struct tm tm1;
	localtime_r(&t1, &tm1);

	struct tm tm2;
	localtime_r(&t2, &tm2);

	return tm1.tm_year == tm2.tm_year && tm1.tm_yday == tm2.tm_yday;
}
//���δ��ĳ��ʱ���ʱ���
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
//���ĳ��ʱ�䵱��ĳ��Сʱ��ʱ���
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
//��õ�ǰʱ�����ܵ�ĳ��ĳʱ��ʱ���
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
//���ĳ��ʱ���ڵ���ĵڼ���
inline int GetWeekday(time_t tCurTime)
{
	struct tm tm;
	localtime_r(&tCurTime, &tm);

	return tm.tm_wday;
}
//���ĳ��ʱ���ڵ���ĵڼ�����
inline int GetDayMinutes(time_t tCurTime)
{
	struct tm tm;
	localtime_r(&tCurTime, &tm);

	return tm.tm_hour * 60 + tm.tm_min;
}
//��ʽ���ַ���ʱ����ʱ���
inline time_t StringToDatetime(const std::string& str)
{
	char *cha = (char*)str.data();             // ��stringת����char*��
	tm tm_ = { 0 };                                    // ����tm�ṹ�塣
	int year, month, day, hour, minute, second;// ����ʱ��ĸ���int��ʱ������
	sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// ��string�洢������ʱ�䣬ת��Ϊint��ʱ������
	tm_.tm_year = year - 1900;                 // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ�䣬����tm_yearΪint��ʱ������ȥ1900��
	tm_.tm_mon = month - 1;                    // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ʱ������ȥ1��
	tm_.tm_mday = day;                         // �ա�
	tm_.tm_hour = hour;                        // ʱ��
	tm_.tm_min = minute;                       // �֡�
	tm_.tm_sec = second;                       // �롣
	tm_.tm_isdst = 0;                          // ������ʱ��
	time_t t_ = mktime(&tm_);                  // ��tm�ṹ��ת����time_t��ʽ��
	return t_;                                 // ����ֵ�� 
}