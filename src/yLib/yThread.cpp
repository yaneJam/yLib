#include "stdafx.h"
#include "yThread.h"
#ifdef useSTDThread2
#else
#include <iostream>
#ifdef _WIN32
	static DWORD CALLBACK __ThreadProc(LPVOID lpParameter)
	{
		yThread* pThread = (yThread*)lpParameter;
		if (pThread->isUseLambdaFunc == false)
		{
			pThread->run();
		}
		else
		{
			pThread->pRunFunc();
		}
		pThread->working = false;
		pThread->onStop();
		pThread->isRunning = false;
		pThread->free();
		bool AutoDestroy = pThread->autoDestroy;
		pThread->_syncUnLock();
		if (AutoDestroy)
		{
			delete pThread;
		}
		return 0;
	}
#endif
	yThread::yThread(bool nAutoDestroy)
	{
		#ifdef MSVC
		#else
		__sync = std::unique_lock<std::recursive_mutex>(mtx);
		#endif
		isUseLambdaFunc = false;
		autoDestroy = nAutoDestroy;
		isRunning = false;
#ifdef WIN32
		hThread = NULL;
#endif
		pParameter = NULL;
		working = false;
	}

	yThread::yThread(std::function<void(void)> RunFunc)
	{	
		#ifdef MSVC
		#else
		__sync = std::unique_lock<std::recursive_mutex>(mtx);
		#endif
		pRunFunc = RunFunc;
		isUseLambdaFunc = true;
		autoDestroy = true;
		isRunning = false;
#ifdef _WIN32
		hThread = NULL;
#endif
		working = false;
		this->start();
	}

	yThread::~yThread()
	{

	}

	void yThread::free()
	{
#ifdef _WIN32
		if (hThread != NULL)
		{
			CloseHandle(hThread);
			hThread = NULL;
		}
#endif
	}

	void yThread::start()
	{
		_syncLock();
		if (isRunning == true)
		{

			_syncUnLock();
			return;
		}
		isRunning = true;
#ifdef _WIN32
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		working = true;

		if ((hThread = CreateThread(&sa, (ULONG)0, __ThreadProc, (LPVOID)this, 0, &dwThreadID)) == NULL)
		{
			isRunning = false;
			_syncUnLock();
		}
#endif
	}

	bool yThread::running()
	{
		return isRunning;
	}

	bool yThread::finished()
	{
		return !isRunning;
	}

	void yThread::stop()
	{
		working = false;
		_syncLock();
		_syncUnLock();
	}

	bool yThread::checkThreadStatus()
	{
		return working;
	}

	void yThread::_syncLock()
	{
		#ifdef MSVC
		__sync.lock()
		#else
		__sync.lock();
		#endif
	}

	void yThread::waitingForThreadEnd()
	{
		#ifdef _WIN32
		_syncLock();
		Sleep(2);
		_syncUnLock();
		#endif
		
	}

	void yThread::_syncUnLock()
	{
		#ifdef MSVC
		__sync.unLock();
	#else
		__sync.unlock();
			#endif
	}

#endif