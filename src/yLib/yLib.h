// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// //#include "targetver.h"

#ifdef _WIN32
// Windows Header Files:
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#else

#endif


// C RunTime Header Files
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

#if defined(IO_EXPORTS)
#if defined(_MSC_VER)
#define IO_API __declspec(dllexport)
#else
#define IO_API
#endif
#else
#if defined(_MSC_VER)
#define IO_API __declspec(dllimport)
#else
#define IO_API
#endif
#endif



#   ifdef _UNICODE
#define _tstring std::wstring
#define _tchar wchar_t
#define _tifstream std::wifstream
#define _tofstream std::wofstream
#define _tfstream std::wfstream
#define _tstring std::wstring
#else
#define _tchar char
#define _tstring std::string
#define _tifstream std::ifstream
#define _tofstream std::ofstream
#define _tfstream std::fstream
#define _tstring std::string
#endif


#define _tsptr std::shared_ptr 
#define _twptr std::weak_ptr 
#define _tshare_from_this std::enable_shared_from_this
#define  _tdynamic_pointer_cast std::dynamic_pointer_cast
#define  _tstatic_pointer_cast std::static_pointer_cast

//#include <string>
#include <vector>
#include <set>
#include <map>
#include <list>


_tstring IO_API get_directory_path(const _tchar* filepath);//输入文件绝对路径，输入该文件的文件夹
bool IO_API createDirectories(const _tchar* dirname);
_tstring IO_API normalizePath(_tstring path);
std::vector<_tstring> IO_API split(_tstring str, _tstring pattern);
_tstring IO_API transferDirPath(_tstring filepath);//��ʱ�ȷ�������
_tstring IO_API getProgramDir();//��ʱ�ȷ�������


#ifdef _WIN32
#else
//temporary define for linux
typedef unsigned long DWORD;
typedef struct _SYSTEMTIME{
DWORD wYear;
DWORD wMonth;
DWORD wDay;
DWORD wHour;
DWORD wMinute;
DWORD wSecond;
DWORD wMilliseconds;
}SYSTEMTIME;

void  GetLocalTime(SYSTEMTIME* systemtime);
unsigned long GetTickCount();
#define MAX_PATH 1024
#define sprintf_s snprintf
#define _stprintf_s snprintf
#define vsprintf_s vsnprintf
//#define vswprintf_s vsnwprintf //zxz:not declared ,try it next time
#define vswprintf_s vswprintf
#define OutputDebugStringA printf
#define _tcslen strlen
#define sscanf_s sscanf
#define _tcscpy_s strncpy
#define _T(x) x

#define  _access access
#define _mkdir mkdir
//#define _tcslen strlen
#define _tcscpy_s strncpy
#define MAX_PATH 1024

#endif

long inline MACRO_GETCOMPILEDATE()
{
	SYSTEMTIME lpCompileTime;

	char Mmm[4] = "Jan";

	sscanf_s(__DATE__, "%3s %hu %hu", Mmm,
		&lpCompileTime.wDay, &lpCompileTime.wYear);

	switch (*(DWORD*)Mmm) {
		//小端
	case (DWORD)('naJ') : lpCompileTime.wMonth = 1; break;
	case (DWORD)('beF') : lpCompileTime.wMonth = 2; break;
	case (DWORD)('raM') : lpCompileTime.wMonth = 3; break;
	case (DWORD)('rpA') : lpCompileTime.wMonth = 4; break;
	case (DWORD)('yaM') : lpCompileTime.wMonth = 5; break;
	case (DWORD)('nuJ') : lpCompileTime.wMonth = 6; break;
	case (DWORD)('luJ') : lpCompileTime.wMonth = 7; break;
	case (DWORD)('guA') : lpCompileTime.wMonth = 8; break;
	case (DWORD)('peS') : lpCompileTime.wMonth = 9; break;
	case (DWORD)('tcO') : lpCompileTime.wMonth = 10; break;
	case (DWORD)('voN') : lpCompileTime.wMonth = 11; break;
	case (DWORD)('ceD') : lpCompileTime.wMonth = 12; break;
		//Unix大端
	case (DWORD)('Jan') : lpCompileTime.wMonth = 1; break;
	case (DWORD)('Feb') : lpCompileTime.wMonth = 2; break;
	case (DWORD)('Mar') : lpCompileTime.wMonth = 3; break;
	case (DWORD)('Apr') : lpCompileTime.wMonth = 4; break;
	case (DWORD)('May') : lpCompileTime.wMonth = 5; break;
	case (DWORD)('Jun') : lpCompileTime.wMonth = 6; break;
	case (DWORD)('Jul') : lpCompileTime.wMonth = 7; break;
	case (DWORD)('Aug') : lpCompileTime.wMonth = 8; break;
	case (DWORD)('Sep') : lpCompileTime.wMonth = 9; break;
	case (DWORD)('Oct') : lpCompileTime.wMonth = 10; break;
	case (DWORD)('Nov') : lpCompileTime.wMonth = 11; break;
	case (DWORD)('Dec') : lpCompileTime.wMonth = 12; break;
	default:lpCompileTime.wMonth = 0;
	}
	return   lpCompileTime.wYear * 10000 + lpCompileTime.wMonth * 100 + lpCompileTime.wDay;
};


long inline MACRO_GETCOMPILETIME()
{
	SYSTEMTIME lpCompileTime;

	sscanf_s(__TIME__, "%hu:%hu:%hu", &lpCompileTime.wHour,
		&lpCompileTime.wMinute, &lpCompileTime.wSecond);

	return   lpCompileTime.wHour * 10000 + lpCompileTime.wMinute * 100 + lpCompileTime.wSecond;
};




#include "Json/json.h"

#include "yLogger.h"
#define IO_DEBUG yLogger::writer(DEBUG_LEVEL_DEBUG,__FUNCTION__)->write


#include "yValue.h"
#include "yConfiger.h"
#include "JConfiger.h"
#include "yObject.h"
#include "yMsg.h"
#include "yThread.h"//准备换成std::thread
#include "yModule.h"
#include "ySingletone.h"