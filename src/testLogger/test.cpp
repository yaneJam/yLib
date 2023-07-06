#include "yLib/yLib.h"
#include <thread>
#ifdef _WIN32
void usleep(int t)
{
	Sleep(t / 1000);
}
#else
#include <unistd.h>
#endif


int main(int argc,char* argv[])
{

	printf("testing beigned \n");
	DEBUG_LOG("test logger ");
	WARN_LOG("test logger ");
	INFO_LOG("test logger ");
	ERROR_LOG("test logger ");


	yLogger::singleton()->setOutputMode(yLogger_OUTPUTMODE_file_debugger);
	DEBUG_LOG("test yLogger_OUTPUTMODE_file_debugger ");

	yLogger::singleton()->setOutputMode(yLogger_OUTPUTMODE_file_console);
	DEBUG_LOG("test yLogger_OUTPUTMODE_file_console , not supported this feature in linux");
	
	printf("testing printf  \n");
	for(int i =0;i<100;i++)
	{
		auto t = std::thread([i]{
			usleep(600000-i*5600);
			DEBUG_LOG("thread logger %d",i);
		}
		);
		//std::thread  t(apple,i);
		t.detach();
		DEBUG_LOG("created thread %d",i);
		usleep(5000);
	}
	usleep(5000000);
	yLogger::singleton()->destroy();
	printf("testing finished  \n");
	return 0;
}
