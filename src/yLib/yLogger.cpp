#include "stdafx.h"
#include "yLogger.h"
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

//���Ż�
//1.set_logging_level�ĳ�static
//2.setOutputMode()��ߵ�return����������if (!yLogger::s_singleton)

#ifdef _WIN32
#include <tchar.h>
#include <direct.h>
#else 
#include <unistd.h>
#include <stdarg.h>
#include <wchar.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include<filesystem>
#endif

#include <vector>
#include <string>
#include <stdexcept>
#include <stdio.h>
//
//#include "shlwapi.h"
//#pragma comment(lib,"shlwapi.lib")
//#include <shlobj.h>
//
yLogger* yLogger::s_singleton = NULL;


//unicode תΪ ascii
std::string Unicode2ANSI(std::wstring& wstrcode)
{
#ifdef _WIN32
	int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
	if (asciisize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::logic_error("Invalid UTF-8 sequence.");
	}
	if (asciisize == 0)
	{
		throw std::logic_error("Error in conversion.");
	}
	std::vector<char> resultstring(asciisize);
	int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);
	if (convresult != asciisize)
	{
		throw std::logic_error("La falla!");
	}
	return std::string(&resultstring[0]);
#else
	//use wcstombs() or mbstowcs()
	return std::string("");
#endif
}
//UTF-8תUnicode
std::wstring UTF82Unicode(const std::string& utf8string)
{
#ifdef _WIN32
	int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
	if (widesize == ERROR_NO_UNICODE_TRANSLATION)
	{
		throw std::logic_error("Invalid UTF-8 sequence.");
	}
	if (widesize == 0)
	{
		throw std::logic_error("Error in conversion.");
	}
	std::vector<wchar_t> resultstring(widesize);
	int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
	if (convresult != widesize)
	{
		throw std::logic_error("La falla!");
	}
	return std::wstring(&resultstring[0]);
#else
	//use wcstombs() or mbstowcs()
	return std::wstring(L"");
#endif

}

//utf-8 ת ascii
std::string UTF82ANSI(std::string& strUTF8Code)
{
	std::string strRet("");

	//�Ȱ� utf8 תΪ unicode 
	std::wstring wstr = UTF82Unicode(strUTF8Code);
	//���� unicode תΪ ascii
	strRet = Unicode2ANSI(wstr);

	return strRet;
}

// bool CreateDirectories(const logger_char* dirname)
// {
// 	if (_tcslen(dirname) == 0)
// 	{
// 		return false;
// 	}
// 	logger_char	szDir[MAX_PATH] = { 0 };
// 	//_tcscpy(szDir, dirname);
// 	_tcscpy_s(szDir, MAX_PATH * sizeof(logger_char), dirname);
// 	size_t llength = _tcslen(szDir);
// 	logger_char* pcat = szDir + 2;
// 	while (1)
// 	{
// 		//-----��Ϊ��Щ�ļ�����С���㣬����ע����������
// 		//if (*pcat == '.')//dirname�����ļ���
// 		//{
// 		//	break;
// 		//}
// 		if (*pcat == '\\' || *pcat == '/')
// 		{
// 			*pcat = '\0';
// 			/*if (!::PathIsDirectory(szDir))
// 			{
// 				::CreateDirectory(szDir, NULL);
// 			}*/

// 			if (_access(szDir, 0) != 0)
// 			{
// 				_mkdir(szDir);
// 			}

// 			*pcat = '\\';
// 		}
// 		pcat++;
// 		if ((pcat - szDir) == llength)//dirname�������ļ���
// 		{
// 			/*if (!::PathIsDirectory(szDir))
// 			{
// 				::CreateDirectory(szDir, NULL);
// 			}*/
// 			if (_access(szDir, 0) != 0)
// 			{
// 				_mkdir(szDir);
// 			}
// 			break;
// 		}
// 	}
// 	//return ::PathIsDirectory(dirname) == true;
// 	return _access(szDir, 0) == 0;
// 	return true;
// }



logger_string MakeLogName()
{
	//CString name;
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	/* name.Format(_T("Log\\%i\\%02i\\log_%02i%02i%02i.txt"),
	systime.wYear,
	systime.wMonth,
	systime.wYear,
	systime.wMonth,
	systime.wDay);*/
	//logger_char MyDir[_MAX_PATH];//һ����C:\Users\DTX\AppData\Roaming
	//SHGetSpecialFolderPath(NULL, MyDir, CSIDL_APPDATA, 0);
	logger_char c_name[MAX_PATH];
	_stprintf_s(c_name, MAX_PATH * sizeof(logger_char), ("log\\%i\\%02i\\log_%02i%02i%02i.txt"),
		systime.wYear,
		systime.wMonth,
		systime.wYear,
		systime.wMonth,
		systime.wDay);


	logger_string name(c_name);
	return name;
}

logger_string MakeLogName(logger_string dir)
{
	
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	logger_char c_name[MAX_PATH];
	_stprintf_s(c_name, MAX_PATH * sizeof(logger_char), ("%s\\%i\\%02i\\log_%02i%02i%02i.txt"),
		dir.c_str(),
		systime.wYear,
		systime.wMonth,
		systime.wYear,
		systime.wMonth,
		systime.wDay);


	logger_string name(c_name);
	return name;
}

void yLogger::OpenFile(const logger_char* strFile, bool bAppend)
{
	#ifdef _WIN32
	printf("OpenFile [%s] \n",strFile);

	m_filename = strFile;
	logger_char	szFile[MAX_PATH + 1] = { 0 };
	if (_tcslen(strFile) > 3 && strFile[1] != ':')	//no absolute path designated//��ת���ɾ���·��
	{
		::GetModuleFileName(NULL, szFile, MAX_PATH);
		size_t llength = _tcslen(szFile);
		logger_char* pcat = szFile + (llength - 1);	//point to the last char
		int nCounts = 0;
		while (llength--)
		{
			pcat--;
			if (*pcat == '\\')
			{
				nCounts++;
			}
			//if(2<=nCounts)//����exe�ļ�����һ���ļ���
			//{
			//	break; 
			//}
			if (1 <= nCounts)//����exe�ļ������ļ���
			{
				break;
			}
		}
		if (*pcat == '\\')
		{
			pcat++;
			_tcscpy_s(pcat, (MAX_PATH - llength) * sizeof(logger_char), strFile);
		}
		else//something wrong, use the original filename, ignore path problem
		{
			_tcscpy_s(szFile, MAX_PATH * sizeof(logger_char), strFile);
		}
	}
	else
	{
		_tcscpy_s(szFile, MAX_PATH * sizeof(logger_char), strFile);
	}
	//	_tfopen_s(&m_pLogFile, szFile, bAppend ? _T("a") : _T("w"));
	//if (m_pLogFile == NULL)//·���ļ��в����ڣ������ļ���
	//	if (!(m_pLogFile = _tfopen(szFile, bAppend ? _T("a") : _T("w"))))


#ifdef MSVC
#else
#define _SH_DENYNO 0x40
#endif

	if (!(m_pLogFile = _tfsopen(szFile, bAppend ? ("a") : ("w"), _SH_DENYNO)))
	{
		logger_char	szDir[MAX_PATH] = { 0 };
		_tcscpy_s(szDir, MAX_PATH * sizeof(logger_char), szFile);
		size_t length = _tcslen(szDir);
		logger_char* pcat = szDir + (length - 1);
		while (length--)
		{
			pcat--;
			if (*pcat == '\\')
			{
				break;
			}

		}
		*pcat = '\0';
		createDirectories(szDir);

		//_tfopen_s(&m_pLogFile, szFile, bAppend ? _T("a") : _T("w"));
		m_pLogFile = _tfsopen(szFile, bAppend ? _T("a") : _T("w"), _SH_DENYNO);
	}
#else
	
	int ret = 0;
	DIR* mydir = NULL;
	

	char path[MAX_PATH];
	getcwd(path,sizeof(path));
	//m_filename = _tstring(path)+"\\"+strFile;
	m_filename=getProgramDir()+"\\"+strFile;
	_tstring dir = get_directory_path(m_filename.c_str());
	
	printf("dir:%s\n",dir.c_str());
	
	//return ;
	createDirectories(dir.c_str());

	std::replace(m_filename.begin(), m_filename.end(), '\\', '/');
	if (!(m_pLogFile = fopen(m_filename.c_str(), bAppend ? ("a") : ("w"))))
	{
		printf("yLogger::OpenFile(..) fopen failed ");
		// logger_char	szDir[MAX_PATH] = { 0 };
		// _tcscpy_s(szDir, m_filename.c_str(), MAX_PATH * sizeof(logger_char));
		// size_t length = _tcslen(szDir);
		// logger_char* pcat = szDir + (length - 1);
		// while (length--)
		// {
		// 	pcat--;
		// 	if (*pcat == '\\')
		// 	{
		// 		break;
		// 	}

		// }
		// *pcat = '\0';
		// createDirectories(szDir);

		// //_tfopen_s(&m_pLogFile, szFile, bAppend ? _T("a") : _T("w"));
		// m_pLogFile = fopen(szFile, bAppend ? _T("a") : _T("w"));
	}
	else
	{
		printf("yLogger::OpenFile(..) fopen success ");
	}


#endif
}
yLogger::yLogger(const logger_char* logname, yLogger_OUTPUTMODE output_mode)
{
	log_level = DEBUG_LEVEL_DEBUG;
	m_output_mode = output_mode;
	m_pConsoleFile = NULL;
	m_pLogFile = NULL;
	
	logger_string fileName = MakeLogName(logname);
	
	OpenFile(fileName.c_str(), true);

	if (m_pLogFile == NULL)//����ڹ���������У�m_pLogFile�ͻ����NULL
	{
		//OpenFile(MakeDefaultLogName().c_str(), true);
	}
	sz_max_size = 65536 + 256;
	m_temp_szLine = new char[sz_max_size];
	m_temp_arg = new char[65536];
	m_temp_warg = new wchar_t[65536];

}
yLogger::yLogger()
{
	log_level = DEBUG_LEVEL_DEBUG;
	m_output_mode = yLogger_OUTPUTMODE_file;
	m_pConsoleFile = NULL;
	m_pLogFile = NULL;
	
	logger_string logName = MakeLogName();
	printf("logName:[%s] \n",logName.c_str());
	OpenFile(logName.c_str(), true);

	if (m_pLogFile == NULL)//����ڹ���������У�m_pLogFile�ͻ����NULL
	{
		//OpenFile(MakeDefaultLogName().c_str(), true);
	}
	sz_max_size = 65536 + 256;
	m_temp_szLine = new char[sz_max_size];
	m_temp_arg = new char[65536];
	m_temp_warg = new wchar_t[65536];
}



void yLogger::setOutputMode(yLogger_OUTPUTMODE m)
{
	if (!yLogger::s_singleton)
	{
		singleton();
	}
	if (m == yLogger::s_singleton->m_output_mode)
	{
		return;
	}
	else
	{
		yLogger::s_singleton->m_output_mode = m;
#ifdef _WIN32
		if (yLogger::s_singleton->m_output_mode == yLogger_OUTPUTMODE_file_console)
		{
			AllocConsole();
			_tfreopen_s(&yLogger::s_singleton->m_pConsoleFile, ("CONOUT$"), ("w+t"), stdout);
			//_tfreopen_s(&yLogger::s_singleton->m_pConsoleFile, "CONIN$", "r", stdin);//��䲻��
			//_tfreopen(_T("CONIN$"), _T("r+t"), stdin);//���Ҳ����
		}
		else
		{
			if (yLogger::s_singleton->m_pConsoleFile)
			{
				fclose(yLogger::s_singleton->m_pConsoleFile);
				FreeConsole();
			}
		}
#else
	ERROR_LOG("AllocConsole not supported");
#endif
	}
}

yLogger::~yLogger()
{
	DEBUG_LOG(("yLogger destroyed"));
	CloseFile();
#ifdef _WIN32
	if (m_output_mode == yLogger_OUTPUTMODE_file_console)
	{
		FreeConsole();
	}
#endif
	
	delete[] m_temp_szLine;
	delete[] m_temp_arg;
	delete[] m_temp_warg;
}

void yLogger::CloseFile()
{
	if (m_pLogFile)
	{
		fclose(m_pLogFile);
		m_pLogFile = NULL;
	}
	if (m_pConsoleFile)
	{
		fclose(m_pConsoleFile);
		m_pConsoleFile = NULL;
	}
}

void yLogger::ChangeFile(const logger_char* strFile, bool bAppend)
{
	//if(strFile != m_filename.c_str)
	{
		CloseFile();
		OpenFile(strFile, bAppend);
	}
}
yLogger* yLogger::singleton(const logger_char* logname)
{

	if (NULL != s_singleton)
	{
		s_singleton->destroy();
	}
	{
		s_singleton = new yLogger(logname);
		DEBUG_LOG(("yLogger created"));
	}
	return s_singleton;
}
yLogger* yLogger::singleton()
{
	if (NULL == s_singleton)
	{
		s_singleton = new yLogger();
		DEBUG_LOG(("yLogger created"));
	}
	return s_singleton;
}
//yLogger* yLogger::singleton(const logger_char* str, yLogger_OUTPUTMODE output_mode)//�ļ���λ������һ��str
//{
//	if (NULL == s_singleton)
//	{
//		s_singleton = new yLogger(str, output_mode);
//		DEBUG_LOG(_T("yLogger created"));
//	}
//	setOutputMode(output_mode);
//
//	return s_singleton;
//}

void yLogger::destroy()
{
	if (s_singleton)
	{
		delete s_singleton;
		s_singleton = NULL;
	}
}

void yLogger::setLevel(DEBUG_LEVEL level)
{
	std::lock_guard<std::recursive_mutex> lock(mtx); 
	log_level = level;
}
std::shared_ptr<HDLogWriter> yLogger::writer(int level, const char* func)
{
	if (NULL == s_singleton)
	{
		s_singleton = new yLogger();
	}
	//if (s_singleton->log_level > level)
	//{
	//std::shared_ptr<HDLogWriter> result(new HDLogWriter(s_singleton, level, func));
	//}
	std::shared_ptr<HDLogWriter> result(new HDLogWriter(s_singleton, level, func));
	return result;
}


/////////////////////////

//////////////////////////////////////////////
HDLogWriter::HDLogWriter(yLogger* logger, int level, const char* func)
{
	m_owner = logger;
	m_logging_level = level;
	m_func = std::string(func);
}

void HDLogWriter::writeUTF8(const char* pszFormat, ...)
{
	if (m_owner->log_level > m_logging_level)
	{
		return;
	}
	std::lock_guard<std::recursive_mutex> lock(m_owner->mtx);
	
	{
		//char szLog[2048];
		va_list argList;
		va_start(argList, pszFormat);
		vsprintf_s(m_owner->m_temp_arg, 65535 * sizeof(char), pszFormat, argList);

		auto apple = std::string(m_owner->m_temp_arg);
		va_end(argList);
		SYSTEMTIME	time;
		::GetLocalTime(&time);
		sprintf_s(m_owner->m_temp_szLine, m_owner->sz_max_size * sizeof(char), "<%02d:%02d:%02d.%03d %s %s>\t%s\n",
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
			DEBUG_LEVEL_NAME[m_logging_level],
			m_func.c_str(),
			apple.c_str());//ת��̫�࣬������Ҫ���Ż�һ��

	}
	if (m_owner->m_pLogFile)//������������е�ʱ���޷��ڹ����ļ������д��־,��Ȩ�޲�����ʱ��Ҳ�޷�������־�ļ�
	{
		fputs(m_owner->m_temp_szLine, m_owner->m_pLogFile);
		fflush(m_owner->m_pLogFile);
	}
	if (m_owner->m_output_mode == yLogger_OUTPUTMODE_file_console)
	{
		fputs(m_owner->m_temp_szLine, stdout);
	}
	else if (m_owner->m_output_mode == yLogger_OUTPUTMODE_file_debugger)
	{
		::OutputDebugStringA(m_owner->m_temp_szLine);
	}
}

void  HDLogWriter::write(const wchar_t* pszFormat, ...)
{
	if (m_owner->log_level > m_logging_level)
	{
		return;
	}
	std::lock_guard<std::recursive_mutex> lock(m_owner->mtx);
	{
		//wchar_t szLog[2048];
		va_list argList;
		va_start(argList, pszFormat);
		vswprintf_s(m_owner->m_temp_warg, 65535 * sizeof(wchar_t), pszFormat, argList);
		va_end(argList);
		//Get current time
		SYSTEMTIME	time;
		::GetLocalTime(&time);
		auto apple = std::wstring(m_owner->m_temp_warg);
		sprintf_s(m_owner->m_temp_szLine, m_owner->sz_max_size * sizeof(char), "<%02d:%02d:%02d.%03d %s %s>\t%s\n",
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
			DEBUG_LEVEL_NAME[m_logging_level],
			m_func.c_str(),
			Unicode2ANSI(apple).c_str());
	}
	if (m_owner->m_pLogFile)//������������е�ʱ���޷��ڹ����ļ������д��־
	{
		fputs(m_owner->m_temp_szLine, m_owner->m_pLogFile);
		fflush(m_owner->m_pLogFile);
	}
	if (m_owner->m_output_mode == yLogger_OUTPUTMODE_file_console)
	{
		fputs(m_owner->m_temp_szLine, stdout);
	}
	else if (m_owner->m_output_mode == yLogger_OUTPUTMODE_file_debugger)
	{
		::OutputDebugStringA(m_owner->m_temp_szLine);
	}

}
void HDLogWriter::write(const char* pszFormat, ...)
{
	
	if (m_owner->log_level > m_logging_level)
	{
		return;
	}
	std::lock_guard<std::recursive_mutex> lock(m_owner->mtx);
	{
		//char szLog[2048];
		va_list argList;
		va_start(argList, pszFormat);
		vsprintf_s(m_owner->m_temp_arg, 65535 * sizeof(char), pszFormat, argList);
		//vsprintf(szLog, pszFormat, argList);
		va_end(argList);
		//Get current time
		SYSTEMTIME	time;
		::GetLocalTime(&time);
		sprintf_s(m_owner->m_temp_szLine, m_owner->sz_max_size * sizeof(char), "<%02d:%02d:%02d.%03d %s %s>\t%s\n",
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
			DEBUG_LEVEL_NAME[m_logging_level],
			m_func.c_str(),
			m_owner->m_temp_arg);//ת��̫�࣬������Ҫ���Ż�һ��
	}
	if (m_owner->m_pLogFile)//������������е�ʱ���޷��ڹ����ļ������д��־
	{
		fputs(m_owner->m_temp_szLine, m_owner->m_pLogFile);
		fflush(m_owner->m_pLogFile);
	}
	if (m_owner->m_output_mode == yLogger_OUTPUTMODE_file_console)
	{	
		fputs(m_owner->m_temp_szLine, stdout);
	}
	else if (m_owner->m_output_mode == yLogger_OUTPUTMODE_file_debugger)
	{
		::OutputDebugStringA(m_owner->m_temp_szLine);
	}

}
#ifdef _WIN32
bool m_run = false;
DWORD WINAPI TestThreadFun(LPVOID)
{
	m_run = true;
	while (m_run)
	{
		DWORD threadid = GetCurrentThreadId();
		DEBUG_LOG("%s thread id:%d ", __FUNCTION__, threadid);
		Sleep(500);
	}
	return 1;
}


void yLogger_Thread_Test(int thread_count)
{
	for (int i = 0; i < thread_count; i++)
	{
		::CreateThread(NULL, 0, TestThreadFun, NULL, 0, NULL);
	}


}
void yLogger_Thread_Test_End()
{
	m_run = false;
}
#endif

