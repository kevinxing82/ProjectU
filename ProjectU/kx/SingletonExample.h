#include "kxGlobal.h"
#include <windows.h>
#include <process.h>
class Lock
{
private:
	CRITICAL_SECTION* m_cs;
public:
	Lock(CRITICAL_SECTION*  cs) : m_cs(cs)
	{
		EnterCriticalSection(m_cs);
	}
	~Lock()
	{
		LeaveCriticalSection(m_cs);
	}
};

class Singleton
{
private:
	Singleton();
	Singleton(const Singleton &);
	Singleton& operator = (const Singleton &);

public:
	static Singleton *Instantialize();
	static Singleton *pInstance;
	static CRITICAL_SECTION cs;
};

Singleton* Singleton::pInstance = 0;

Singleton* Singleton::Instantialize()
{
	if (pInstance == NULL)
	{   //double check
		Lock lock = Lock(&cs);           //用lock实现线程安全，用资源管理类，实现异常安全
								 //使用资源管理类，在抛出异常的时候，资源管理类对象会被析构，析构总是发生的无论是因为异常抛出还是语句块结束。
		if (pInstance == NULL)
		{
			pInstance = new Singleton();
		}

		lock.~Lock();
	}
	return pInstance;
}

