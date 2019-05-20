#include "TestBase.h"
#include "TaskWithCallback.h"

//ÿ��actor������һ���߼������ָ�룬ÿ���߼�������һ������actor group���Ψһ��id
class Pepole : public ActorLogicBase
{
public:
	Pepole(int64_t id)
		: ActorLogicBase(TEST_GROUP_ID, id)
	{
	}
	void SayHi(void)
	{
		cout << "hi! " << m_peerName << endl;
	}
	void SayBye(void)
	{
		cout << "bye! " << m_peerName << endl;
	}
	void RememberPeerName(const std::string& name)
	{
		m_peerName = name;
	}
private:
	string m_peerName;
};

//ÿ�����񶼻�����һ�������߳�ִ��,����call. ͬһ��actor������֤������˳����ִ��,�����ܷ������ͬ�Ĺ����߳�
struct SayHiTask : public TaskBase
{
	SayHiTask(const string& name)
		: m_myName(name)
	{
	}
	void Call(ActorLogicBase* logic)
	{
		Pepole* pepole = static_cast<Pepole*>(logic);
		pepole->RememberPeerName(m_myName);
		pepole->SayHi();
	}
	string m_myName;
};

int main(void)
{
	_TEST_INIT_
	int64_t logicObjId = 1L; 
	g_context->AddActor(new Pepole(logicObjId)); //���һ��actor
	g_context->Req(TEST_GROUP_ID, logicObjId, new SayHiTask("dengqunli"), true); //Ͷ��һ������������߳�ִ��
	//����ֱ�Ӵ���һ���ص�����,����!
	g_context->Req(
		TEST_GROUP_ID, 
		logicObjId, 
		new TaskWithCallback([](ActorLogicBase* logic){
			Pepole* pepole = static_cast<Pepole*>(logic);
			pepole->SayBye();
		}), 
		true);

	g_context->SoftErase(TEST_GROUP_ID, logicObjId); //���actorû���µ��������ɾ����û�������actor��ռ��cpu,ֻռ���ڴ�
													//��ܱ�֤��actor��������������Ż�ɾ��actor
	std::this_thread::sleep_for(std::chrono::milliseconds(100)); //��֤����ִ�����ִ��������˳�
	g_context->GracefullExit(); //�����˳������߳�
	g_context->WaitExit(); //�ȴ������߳��˳�
	g_stopTimer = true; //�˳���ʱ��
	timerThread.join(); //�ȴ���ʱ���˳�
	cout << "input any string exit!" << endl;
	string exitstring; cin >> exitstring;
	return 0;
}