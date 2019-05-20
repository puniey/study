#pragma once
#include "ActorContext.h"
#include "TimerExcuteQue.h"
#include <cstdint>
#include <iostream>
#include <string>
using namespace std;

TimerExcuteQue* g_pGlobalTimer = NULL; //��ʱ��,����ʱ�¼�,actor��ܱ���Ҳ���������ѿ����߳�,��������ʱ
bool g_stopTimer = false; //  true: ֹͣ����
ActorContext* g_context = NULL; //actor����
#define TEST_GROUP_ID 1 //��id,�����߳��ڶ����֮���ֽе���,�Ӷ���֤���������actor������ͬ�������cpuռ�õĹ�ƽ�ԣ��������
						//������ֻʹ����һ��actor�飬ʵ���п����������� 

void RunTimer(void) //��ʱ���̺߳���
{
	while (!g_stopTimer)
	{
		g_pGlobalTimer->Update();
		std::this_thread::sleep_for(std::chrono::milliseconds(3));
		if (g_context)
		{
			g_context->WakeWork();
		}
	}
}

//����������3�������߳�,ʵ���п�����������
#define _TEST_INIT_ \
	g_pGlobalTimer = new TimerExcuteQue; \
	g_pGlobalTimer->Init(); \
	std::thread timerThread(RunTimer); \
	g_context = new ActorContext(3); \
	g_context->AddGroup(TEST_GROUP_ID);  \
	g_context->Start();
