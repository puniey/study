#include "Python.h"
#include "traceback.h"
#include "frameobject.h"
#include <string>
#include <iostream>
#include <unordered_map>
#include <thread>
#include <chrono>

using std::cout;
using std::endl;

//C++ Python-3.7.2联合编程 多线程安全 
class CPyMethod
{
public:
	static PyObject* System(PyObject* self, PyObject* args);

public:
	static PyMethodDef m_pyMethods[];
};

PyObject* CPyMethod::System(PyObject* self, PyObject* args)
{
	const char* cmd = NULL;
	int status = 0;

	if (!PyArg_ParseTuple(args, "s", &cmd))
	{
		PyErr_SetString(PyExc_TypeError, "parse args failed");
		return NULL;
	}

	status = system(cmd);
	return PyLong_FromLong(10);
}

PyMethodDef CPyMethod::m_pyMethods[] = {
	{ "system", CPyMethod::System, METH_VARARGS, "" },
	{ NULL, NULL, 0, NULL }
};

PyModuleDef usermodule = {
	PyModuleDef_HEAD_INIT,
	"user",
	NULL,
	-1,
	CPyMethod::m_pyMethods
};

PyMODINIT_FUNC PyInit_user()
{
	PyObject* m;
	m = PyModule_Create(&usermodule);
	if (m == NULL)
		return NULL;

	return m;
}
//python全局解释器锁 保证python解释器在一个时刻只有一个线程在执行
class CPyGILThreadLock
{
public:
	CPyGILThreadLock()
		: m_pThreadState(NULL)
		, m_nCurGILState(0)
	{
		m_nCurGILState = PyGILState_Check();
		if (!m_nCurGILState)
		{
			m_GILState = PyGILState_Ensure();
		}
		m_pThreadState = PyEval_SaveThread();
		PyEval_RestoreThread(m_pThreadState);
	}

	~CPyGILThreadLock()
	{
		m_pThreadState = PyEval_SaveThread();
		PyEval_RestoreThread(m_pThreadState);
		if (!m_nCurGILState)
		{
			PyGILState_Release(m_GILState);
		}
	}
private:
	PyGILState_STATE	m_GILState;
	PyThreadState*		m_pThreadState;
	int					m_nCurGILState;
};
class CPyExtend
{
public:
	//初始化python解释器 在主线程中执行
	static bool Init()
	{
		PyImport_AppendInittab("user", PyInit_user);
		Py_Initialize();
		if (!Py_IsInitialized())
		{
			fprintf(stderr, "python interpreter initial failed.\n");
			exit(1);
		}
		PyRun_SimpleString("import sys");
		PyRun_SimpleString("sys.path.append('./')");
		PyObject* pUserModule = PyImport_ImportModule("user");
		if (!pUserModule)
		{
			fprintf(stderr, "import user module failed.\n");
			exit(1);
		}
		if (PyEval_ThreadsInitialized())
		{
			//释放主线程的全局锁
			PyEval_SaveThread();
		}
		return true;
	}
	//重载python模块
	static std::string ReloadModule(const std::string& strModuleName)
	{
		std::string strResult;
		if (m_mapPyModules.find(strModuleName) == m_mapPyModules.end())
		{
			return strResult + "reload module [" + strModuleName + "] failed: not import module before";
		}

		PyObject* pModule = m_mapPyModules[strModuleName];
		PyObject* pNewModule = PyImport_ReloadModule(pModule);
		if (pNewModule)
		{
			m_mapPyModules[strModuleName] = pNewModule;
			Py_DECREF(pModule);
			return strResult + "reload module [" + strModuleName + "] success";
		}
		else
		{
			return strResult + "reload module [" + strModuleName + "] failed: " + Traceback();
		}
	}
	//执行python模块中函数
	static PyObject* Call(const std::string& strModuleName, const std::string& strFuncName, const char* formate, ...)
	{
		if (m_mapPyModules.find(strModuleName) == m_mapPyModules.end())
		{
			PyObject* pNewModule = PyImport_ImportModule(strModuleName.c_str());
			if (!pNewModule)
			{
				return NULL;
			}
			m_mapPyModules[strModuleName] = pNewModule;
		}

		PyObject* pModule = m_mapPyModules[strModuleName];
		PyObject* pCallObj = PyObject_GetAttrString(pModule, strFuncName.c_str());

		if (!pCallObj)
		{
			return NULL;
		}
		if (!PyCallable_Check(pCallObj))
		{
			Py_DECREF(pCallObj);
			return NULL;
		}

		PyObject* pArgs = NULL;

		if (formate == NULL || strlen(formate) == 0)
		{
			//参数为空 构造一个空元组参数
			pArgs = PyTuple_New(0);
		}
		else if (strlen(formate) == 1 || (strlen(formate) == 2 && formate[1] == '#'))
		{
			//如果只有一个参数 必须为元组类型
			std::string msg;
			msg += strModuleName + "." + strFuncName + " args must be tuple type";
			RaiseTypeException(msg);
			Py_DECREF(pCallObj);
			return NULL;
		}
		else
		{
			va_list ap;
			va_start(ap, formate);
			pArgs = Py_VaBuildValue(formate, ap);
			va_end(ap);
		}

		if (!pArgs)
		{
			Py_DECREF(pCallObj);
			return NULL;
		}

		PyObject* pRet = PyObject_Call(pCallObj, pArgs, NULL);
		Py_DECREF(pCallObj);
		Py_DECREF(pArgs);
		return pRet;
	}
	//打印错误堆栈
	static std::string Traceback()
	{
		std::string strTraceBack = "[PythonException]: ";
		PyObject* pExcType = NULL;
		PyObject* pExcValue = NULL;
		PyObject* pExcTraceback = NULL;
		PyErr_Fetch(&pExcType, &pExcValue, &pExcTraceback);
		if (pExcType)
		{
			Py_DECREF(pExcType);
		}
		if (pExcValue)
		{
			PyObject* pObjVal = PyObject_Str(pExcValue);
			Py_DECREF(pExcValue);
			if (pObjVal)
			{
				const char* errmsg = PyUnicode_AsUTF8(pObjVal);
				Py_DECREF(pObjVal);
				//此处抛出异常直接吞掉
				if (errmsg == NULL)
				{
					if (pExcTraceback)
					{
						Py_DECREF(pExcTraceback);
					}
					PyErr_Clear();
					return "[PythonException]: has exception in CPyExtend::Traceback PyUnicode_AsUTF8";
				}
				strTraceBack += errmsg;
			}
		}
		if (pExcTraceback)
		{
			for (PyTracebackObject* tb = (PyTracebackObject*)pExcTraceback; tb != NULL; tb = tb->tb_next)
			{
				PyObject* pLine = PyUnicode_FromFormat(", File: \"%U\", line: %d, in \"%U\"",
					tb->tb_frame->f_code->co_filename,
					tb->tb_lineno,
					tb->tb_frame->f_code->co_name
					);
				if (pLine)
				{
					const char* errmsg = PyUnicode_AsUTF8(pLine);
					Py_DECREF(pLine);
					//此处抛出异常直接吞掉
					if (errmsg == NULL)
					{
						Py_DECREF(pExcTraceback);
						PyErr_Clear();
						return "[PythonException]: has exception in CPyExtend::Traceback PyUnicode_AsUTF8";
					}
					strTraceBack += errmsg;
				}
			}
			Py_DECREF(pExcTraceback);
		}
		cout << strTraceBack << endl;
		return strTraceBack;
	}
	//抛出一个类型转换异常
	static void RaiseTypeException(const std::string& strError = "parse args failed")
	{
		PyErr_SetString(PyExc_TypeError, strError.c_str());
	}
private:
	static std::unordered_map<std::string, PyObject*>		m_mapPyModules;
};

std::unordered_map<std::string, PyObject*> CPyExtend::m_mapPyModules;

//调用python API时 需要先获得全局解释器锁GIL
#define BEGIN_ALLOW_USE_PYTHON_API				{CPyGILThreadLock lock;
#define END_ALLOW_USE_PYTHON_API				}
#define BEGIN_ALLOW_USE_PYTHON_API_WITH_ARG(o)	{CPyGILThreadLock lock; PyObject* o = NULL;
#define END_ALLOW_USE_PYTHON_APII_WITH_ARG(o)	if(o){Py_DECREF(o);} }

void func1()
{
	while (true)
	{
		BEGIN_ALLOW_USE_PYTHON_API_WITH_ARG(pRet);
		pRet = CPyExtend::Call("mytest1", "foo", "i", 100);
		if (pRet)
		{
			/*int ret = PyLong_AsLong(pRet);
			cout << std::this_thread::get_id() << ": func1 ret: " << ret << endl;*/
			std::cout << "func1 python call success" << endl;
		}
		else
		{
			std::cout << "func1 python call failed" << endl;
			CPyExtend::Traceback();
		}
		/*std::cout << CPyExtend::ReloadModule("mytest1") << std::endl;
		std::cout << CPyExtend::ReloadModule("mytest2") << std::endl;
		cout << CPyExtend::ReloadModule("mytest3") << endl;
		cout << CPyExtend::ReloadModule("mytest4") << endl;*/
		CPyExtend::ReloadModule("mytest1");
		CPyExtend::ReloadModule("mytest2");
		CPyExtend::ReloadModule("mytest3");
		CPyExtend::ReloadModule("mytest4");
		END_ALLOW_USE_PYTHON_APII_WITH_ARG(pRet);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

void func2()
{
	while (true)
	{
		BEGIN_ALLOW_USE_PYTHON_API_WITH_ARG(pRet);
		pRet = CPyExtend::Call("mytest2", "foo", "(i)", 100);
		if (pRet)
		{
			//int ret = PyLong_AsLong(pRet);
			//cout << std::this_thread::get_id() << ": func2 ret: " << ret << endl;
			//std::cout << "func2 python call success" << endl;
		}
		else
		{
			//std::cout << "func2 python call failed" << endl;
			CPyExtend::Traceback();
		}

		/*if (CPyExtend::ReloadModule("mytest2"))
		{
		cout << std::this_thread::get_id() << ": func2 reload success" << endl;
		}
		else
		{
		cout << std::this_thread::get_id() << "func2 reload failed" << endl;
		}
		if (CPyExtend::ReloadModule("mytest3"))
		{
		cout << std::this_thread::get_id() << ": func3 reload success" << endl;
		}
		else
		{
		cout << std::this_thread::get_id() << "func3 reload failed" << endl;
		}
		if (CPyExtend::ReloadModule("mytest4"))
		{
		cout << std::this_thread::get_id() << ": func4 reload success" << endl;
		}
		else
		{
		cout << std::this_thread::get_id() << "func4 reload failed" << endl;
		}*/
		END_ALLOW_USE_PYTHON_APII_WITH_ARG(pRet);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

void func3()
{
	while (true)
	{
		BEGIN_ALLOW_USE_PYTHON_API_WITH_ARG(pRet);
		pRet = CPyExtend::Call("mytest3", "foo", "(i)", 100);
		if (pRet)
		{
			//int ret = PyLong_AsLong(pRet);
			//cout << std::this_thread::get_id() << ": func3 ret: " << ret << endl;
			//std::cout << "func3 python call success" << endl;
		}
		else
		{
			//std::cout << "func3 python call failed" << endl;
			CPyExtend::Traceback();
		}

		/*if (CPyExtend::ReloadModule("mytest3"))
		{
		cout << std::this_thread::get_id() << ": func3 reload success" << endl;
		}
		else
		{
		cout << std::this_thread::get_id() << "func3 reload failed" << endl;
		}
		if (CPyExtend::ReloadModule("mytest4"))
		{
		cout << std::this_thread::get_id() << ": func4 reload success" << endl;
		}
		else
		{
		cout << std::this_thread::get_id() << "func4 reload failed" << endl;
		}*/
		END_ALLOW_USE_PYTHON_APII_WITH_ARG(pRet);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

void func4()
{
	while (true)
	{
		BEGIN_ALLOW_USE_PYTHON_API_WITH_ARG(pRet);
		pRet = CPyExtend::Call("mytest4", "foo", "(i)", 100);
		if (pRet)
		{
			/*int ret = PyLong_AsLong(pRet);
			cout << std::this_thread::get_id() << ": func4 ret: " << ret << endl;*/
			//std::cout << "func4 python call success" << endl;
		}
		else
		{
			//std::cout << " func4 python call failed" << endl;
			CPyExtend::Traceback();
		}

		/*if (CPyExtend::ReloadModule("mytest4"))
		{
		cout << std::this_thread::get_id() << ": func4 reload success" << endl;
		}
		else
		{
		cout << std::this_thread::get_id() << "func4 reload failed" << endl;
		}*/
		END_ALLOW_USE_PYTHON_APII_WITH_ARG(pRet);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

int main(int argc, char** argv)
{
	CPyExtend::Init();
	std::thread t1(std::bind(func1));
	std::thread t2(std::bind(func2));
	std::thread t3(std::bind(func3));
	std::thread t4(std::bind(func4));
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	return 0;
}