#include "TestBase.h"
#include "TaskAsync.h"

//ÿ��actor������һ���߼������ָ�룬ÿ���߼�������һ������actor group���Ψһ��id
class Friend : public ActorLogicBase
{
public:
	Friend(int64_t id, const string& myname)
		: ActorLogicBase(TEST_GROUP_ID, id)
		, m_myName(myname)
	{
	}
	void SayHiTo(const string& name)
	{
		cout << "hi! " << name << "!" << " my name is " << m_myName << "." << endl;
	}
	const string& GetName(void)
	{
		return m_myName;
	}
private:
	string m_myName;
};

//ÿ�����񶼻�����һ�������߳�ִ��,����call. ͬһ��actor������֤������˳����ִ��,�����ܷ������ͬ�Ĺ����߳�
struct SayHiTask : public TaskBase
{
	SayHiTask(const string& name)
	: m_name(name)
	{
	}
	void Call(ActorLogicBase* logic)
	{
		Friend* pepole = static_cast<Friend*>(logic);
		pepole->SayHiTo(m_name);
	}
	string m_name;
};

struct GetNameTask : public TaskBase
{
	static GetNameTask* CreateWithPull(void)
	{
		return new GetNameTask();
	}
	GetNameTask(void) { m_pull = new TaskPullObj<string>(); }
	void Call(ActorLogicBase* logic)
	{
		Friend* pepole = static_cast<Friend*>(logic);
		TaskPullObj<string>* p = static_cast<TaskPullObj<string>*>(m_pull);
		p->SetReturnObjPtr(new string(pepole->GetName()));
		AckPull(logic);
	}
};

struct IntroduceFriend : public TaskAsync
{
	IntroduceFriend(int64_t beIntroducedPepole, int64_t introduceToPepole)
	{
		m_beIntroducedPepole = beIntroducedPepole;
		m_introduceToPepole = introduceToPepole;
	}
	int AsyncCall(ActorLogicBase* logic, TaskPullBase* ack, int num, bool timeout)
	{
		Friend* introducer = static_cast<Friend*>(logic);
		PT_BEGIN(&m_pt);
		{
			Pull<GetNameTask>(logic, TEST_GROUP_ID, m_beIntroducedPepole, -1);
		}
		PT_YIELD(&m_pt);
		{
			if (timeout) {
				cout << "fetch name timeout!" << endl;
				PT_EXIT(&m_pt);
			}
			TaskPullObj<string>* pullName = static_cast<TaskPullObj<string>*>(ack);
			auto nameptr = pullName->FetchObjPtr();
			g_context->Req(TEST_GROUP_ID, m_introduceToPepole, new SayHiTask(*nameptr), true);
			delete nameptr;
		}
		PT_END(&m_pt);
	}
	int64_t m_beIntroducedPepole;
	int64_t m_introduceToPepole;
};

//dengqunli��ȡhanmeimei������,Ȼ�����ָ���lilei, Ȼ��֪ͨlilei��hanmeimei˵hi, ����������IntroduceFriend����
//ע��: 1) TaskAsyncʹ�õ���stacklessЭ�̣�pull֮��Ĵ��벻�ܷ���pull֮ǰ��ջ�ռ�(��ջ����),���豣��״̬�����Դ��������
//		2) ͬһgroup��actorϰ���϶���ͬһ����Ķ���,�߼��ϵĵ��ù�ϵͨ�����໥�ģ�
//		���Բ���������֮���໥pull��Ϣ���������actorͬʱ�໥pull���׳�������),
//      ��������ͬһgroup��actor֮���໥pull,���Զ���� __DISABLE_PULL_IN_THE_SAME_GROUP__
//		�κ�ʱ��Ҫ��֤actor֮���pull�����ǵ����,����Щactor�����߼��ϵķֲ���һ���Ƽ�������
//		������Ϊ�˷���û���������
int main(void)
{
	_TEST_INIT_
	g_context->AddActor(new Friend(1L, "dengqunli"));
	g_context->AddActor(new Friend(2L, "lilei")); 
	g_context->AddActor(new Friend(3L, "hanmeimei")); 
	g_context->Req(TEST_GROUP_ID, 1L, new IntroduceFriend(3L, 2L), true); //Ͷ��һ������������߳�ִ��

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