#pragma once
#include <cassert>
#include <stdint.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <string.h>

using std::string;
//字符串相关常用工具
// 字符串拷贝
inline int SafeStrCpy(char* pcDst, int iDstSize, const char* pcSrc)
{
	if (!pcDst || iDstSize <= 0)
	{
		assert(0);
		return 0;
	}

	int iSize = 0;
	if (pcSrc)
	{
		while (*pcSrc)
		{
			if (iSize >= (iDstSize - 1))
			{
				break;
			}
			*(pcDst++) = *(pcSrc++);
			iSize++;
		}
	}

	*pcDst = '\0';
	return iSize;
}

// for client proto, check the length
inline int64_t StrIdToInt(const char* acId)
{
	char acAccoutID[32] = { 0 };
	SafeStrCpy(acAccoutID, sizeof(acAccoutID), acId);
    return atoll(acAccoutID);
}

// for client proto, check the length
inline int64_t StrIdToInt(const std::string& strId)
{
	if (strId.size() > 30)
	{
		return 0;
	}
	return StrIdToInt(strId.c_str());
}

inline void IntIdToString(int64_t int64Id, std::string&  strId)
{
	char acBuffer[32] = { 0 };
	snprintf(acBuffer, sizeof(acBuffer)-5, "%lld", int64Id);
	strId = acBuffer;
}

inline int IntIdToString(int64_t int64Id, char* acBuffer, int bufferlen)
{
	return snprintf(acBuffer, bufferlen, "%lld", int64Id);
}

inline int IntIdToString(int intId, char* acBuffer, int bufferlen)
{
	return snprintf(acBuffer, bufferlen, "%d", intId);
}

inline bool  StrictAtol(const char* p, int64_t& v)
{
	if (*p == '\0')
		return false;

	int64_t ret = 0;
	while (char c = *p++)
	{
		if (c < '0' || c > '9')
		{
			return false;
		}
		ret *= 10;
		ret += (c - '0');
	}
	v = ret;
	return true;
}

inline void TrimString(std::string & rkStr)
{
	int sz = rkStr.size();
	for (int i = 0; i < sz; ++i)
	{
		if (!isspace(rkStr[i]))
		{
			if(i > 0) rkStr.erase(0, i);
			break;
		}
	}
	sz = rkStr.size();
	for (int i = sz - 1; i >= 0; --i)
	{
		if (!isspace(rkStr[i]))
		{
			if(i + 1 < sz) rkStr.erase(i + 1);
			break;
		}
	}
}

inline const char* StrnChr(const char* str, char c, int n) {
	const char * p = str;
	while (n-- > 0 && *p) {
		if (*p == c)
			return p;
		++p;
	}
	return NULL;
}

inline void SplitStr(const std::string& rkStr, const std::string& rkSep, std::vector<std::string>& rkResult)
{
	if (rkStr.empty())
	{
		return;
	}
	size_t uiStart = 0;
	size_t uiPos = 0;
	do
	{
		uiPos = rkStr.find_first_of(rkSep, uiStart);
		if (uiPos == uiStart)
		{
			uiStart = uiPos + 1;
		}
		else if (uiPos == std::string::npos)
		{
			rkResult.push_back(rkStr.substr(uiStart));
			break;
		}
		else
		{
			rkResult.push_back(rkStr.substr(uiStart, uiPos - uiStart));
			uiStart = uiPos + 1;
		}
		uiStart = rkStr.find_first_not_of(rkSep, uiStart);
	} while (uiPos != std::string::npos);
}