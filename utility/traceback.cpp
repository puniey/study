#include <execinfo.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cxxabi.h>
#include <execinfo.h>
#include <stdlib.h>

#define BT_DEPTH 20
//panying.puniey [2018/8/7] ��ӡ��ջ
std::string _demangle(char const * symbol)
{
	size_t size;
	int status;
	char temp[128];
	char* demangled;
	if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp))
	{
		if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, &size, &status)))
		{
			std::string result(demangled);
			free(demangled);
			return result;
		}
	}

	if (1 == sscanf(symbol, "%127s", temp))
	{
		return temp;
	}
	return symbol;
}
//ͨ���ܵ����� ��ȡ���ú������ļ��к�
string pipeexc(const char* cmd)
{
	FILE *pp = popen(cmd, "r"); //�����ܵ�
	if (!pp)
	{
		return "";
	}
	char tmp[1024];
	string ret;
	int i = 0;
	while (fgets(tmp, sizeof(tmp), pp) != NULL)
	{
		if (i++ == 0) //��һ��Ϊ������Ϣ ���� ����Ϊ�ļ��к���Ϣ
			continue;
		if (tmp[strlen(tmp) - 1] == '\n')
		{
			tmp[strlen(tmp) - 1] = '\0'; //ȥ�����з�
		}
		ret += tmp;
	}
	pclose(pp); //�رչܵ�
	return ret;
}
//ת��ջ֡Ϊ������ú���
void parseName(char * str, char *exeName, char *addr)
{
	char *strTemp = str;
	char * addrTemp;
	while (*strTemp)
	{
		if (*strTemp == '(')
			memcpy(exeName, str, strTemp - str);

		if (*strTemp == '[')
			addrTemp = strTemp;

		if (*strTemp == ']')
			memcpy(addr, str + (addrTemp - str) + 1, strTemp - addrTemp - 1);
		strTemp++;
	}
}
void DumpTraceBack()
{
	int nptrs = 0;
	void* buffer[BT_DEPTH];
	char** strings;

	nptrs = ::backtrace(buffer, BT_DEPTH);
	strings = ::backtrace_symbols(buffer, nptrs);
	if (strings == NULL)
		return;
	string strinfo = "\n\t\t";
	char cmd[500] = { 0 };
	char exeName[100] = { 0 };
	char addr[100] = { 0 };
	for (int i = 2; i < nptrs; ++i)
	{
		memset(cmd, 0, sizeof(cmd));
		memset(exeName, 0, sizeof(exeName));
		memset(addr, 0, sizeof(addr));

		parseName(strings[i], exeName, addr);
		sprintf(cmd, "addr2line -f -e %s %s", exeName, addr);
		strinfo += _demangle(strings[i]) + '\t';
		strinfo += pipeexc(cmd);
		if ((i + 1) != nptrs)
			strinfo += "\n\t\t";
	}
	LOG(ERROR) << strinfo;
	free(strings);
	return;
}
