//@md
//---------------------------------------------------------------------------
#pragma once
#define useSTDThread2

#ifdef useSTDThread2
#include <thread>
#else
//#include "ySyncAccess.h"
#include <functional>



//namespace HD
//{
	class IO_API yThread
	{
	public:
		yThread(bool nAutoDestroy = false);
		yThread(std::function<void(void)> pRunFunc);
		virtual ~yThread();
		void start();
		virtual void run() {};
		virtual void onStop() {};
		bool running();
		bool finished();
		void stop();
		void free();
		void _syncLock();
		void _syncUnLock();
		bool checkThreadStatus();
	public:
#ifdef _WIN32
		HANDLE hThread;
		SECURITY_ATTRIBUTES sa;
DWORD dwThreadID;
#endif
		
		bool isUseLambdaFunc;
		bool isRunning;
		bool autoDestroy;
		bool manualClose;
		void* pParameter;
		bool working;
		void waitingForThreadEnd();
		std::function<void(void)> pRunFunc;
	private:
	#ifdef MSVC
		ySyncAccess __sync;
		#else
			std::recursive_mutex mtx;
			std::unique_lock<std::recursive_mutex> __sync;
		#endif
	};
//}

#endif