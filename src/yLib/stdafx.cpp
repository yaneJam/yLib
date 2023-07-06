// stdafx.cpp : source file that includes just the standard includes
// Sample_SLN.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file
#ifdef _WIN32
#include <direct.h>
#include<io.h>
#else
//temp defination for linux
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#endif

#include <stdio.h>

#ifdef _WIN32
bool createDirectories(const _tchar* dirname)
{
	if (_tcslen(dirname) == 0)
	{
		return false;
	}
	_tchar	szDir[MAX_PATH] = { 0 };
	//_tcscpy(szDir, dirname);
	_tcscpy_s(szDir, MAX_PATH * sizeof(_tchar), dirname);
	size_t llength = _tcslen(szDir);
	_tchar* pcat = szDir + 2;
	while (1)
	{
		//-----��Ϊ��Щ�ļ�����С���㣬����ע����������
		//if (*pcat == '.')//dirname�����ļ���
		//{
		//	break;
		//}
		if (*pcat == '\\' || *pcat == '/')
		{
			*pcat = '\0';
			/*if (!::PathIsDirectory(szDir))
			{
				::CreateDirectory(szDir, NULL);
			}*/

			if (_access(szDir, 0) != 0)
			{
				_mkdir(szDir);
			}

			*pcat = '\\';
		}
		pcat++;
		if ((pcat - szDir) == llength)//dirname�������ļ���
		{
			/*if (!::PathIsDirectory(szDir))
			{
				::CreateDirectory(szDir, NULL);
			}*/
			if (_access(szDir, 0) != 0)
			{
				_mkdir(szDir);
			}
			break;
		}
	}
	//return ::PathIsDirectory(dirname) == TRUE;
	return _access(szDir, 0) == 0;
	return true;
}
#else
bool createDirectories(const _tchar* dirname)
{
	//std::filesystem::create_directories(dir);

	// mydir = opendir(dir.c_str());
	// if(mydir==NULL)
	// {
	// 	ret = mkdir(dir.c_str(),0755);
	// 	if(ret!=0)
	// 	{
	// 		printf("yLogger::OpenFile(..) failed\n");
	// 	}
	// }


	if (_tcslen(dirname) == 0)
	{
		return false;
	}
	_tchar	szDir[MAX_PATH] = { 0 };
	//_tcscpy(szDir, dirname);
	_tcscpy_s(szDir, dirname, MAX_PATH * sizeof(_tchar));
	size_t llength = _tcslen(szDir);
	_tchar* pcat = szDir + 1;
	while (1)
	{
		//-----��Ϊ��Щ�ļ�����С���㣬����ע����������
		//if (*pcat == '.')//dirname�����ļ���
		//{
		//	break;
		//}
		if (*pcat == '\\' || *pcat == '/')
		{
			*pcat = '\0';
			/*if (!::PathIsDirectory(szDir))
			{
				::CreateDirectory(szDir, NULL);
			}*/

			if (_access(szDir, 0) != 0)
			{
				//printf("mkdir %s\n",szDir);
				_mkdir(szDir,0755);
			}

			*pcat = '/';
		}
		pcat++;
		if ((pcat - szDir) == llength)//dirname�������ļ���
		{
			/*if (!::PathIsDirectory(szDir))
			{
				::CreateDirectory(szDir, NULL);
			}*/
			if (_access(szDir, 0) != 0)
			{
				//printf("mkdir %s\n",szDir);
				_mkdir(szDir,0755);
			}
			break;
		}
	}
	return _access(szDir, 0) == 0;
	return true;


};

void  GetLocalTime(SYSTEMTIME* systemtime)
{
	time_t curtime;
	time(&curtime);
	char* apple = ctime(&curtime);
	char weekday[20];
	char month[20];


	sscanf( apple,"%s %s %d %d:%d:%d %d",weekday,month,&systemtime->wDay, &systemtime->wHour,&systemtime->wMinute,&systemtime->wSecond,&systemtime->wYear);

	switch (*(DWORD*)month) {
		//小端
	case (DWORD)('naJ') : systemtime->wMonth = 1; break;
	case (DWORD)('beF') : systemtime->wMonth = 2; break;
	case (DWORD)('raM') : systemtime->wMonth = 3; break;
	case (DWORD)('rpA') : systemtime->wMonth = 4; break;
	case (DWORD)('yaM') : systemtime->wMonth = 5; break;
	case (DWORD)('nuJ') : systemtime->wMonth = 6; break;
	case (DWORD)('luJ') : systemtime->wMonth = 7; break;
	case (DWORD)('guA') : systemtime->wMonth = 8; break;
	case (DWORD)('peS') : systemtime->wMonth = 9; break;
	case (DWORD)('tcO') : systemtime->wMonth = 10; break;
	case (DWORD)('voN') : systemtime->wMonth = 11; break;
	case (DWORD)('ceD') : systemtime->wMonth = 12; break;
		//Unix大端
	case (DWORD)('Jan') : systemtime->wMonth = 1; break;
	case (DWORD)('Feb') : systemtime->wMonth = 2; break;
	case (DWORD)('Mar') : systemtime->wMonth = 3; break;
	case (DWORD)('Apr') : systemtime->wMonth = 4; break;
	case (DWORD)('May') : systemtime->wMonth = 5; break;
	case (DWORD)('Jun') : systemtime->wMonth = 6; break;
	case (DWORD)('Jul') : systemtime->wMonth = 7; break;
	case (DWORD)('Aug') : systemtime->wMonth = 8; break;
	case (DWORD)('Sep') : systemtime->wMonth = 9; break;
	case (DWORD)('Oct') : systemtime->wMonth = 10; break;
	case (DWORD)('Nov') : systemtime->wMonth = 11; break;
	case (DWORD)('Dec') : systemtime->wMonth = 12; break;
	default:systemtime->wMonth = 0;
	}

	struct timeval tv;
	gettimeofday(&tv,NULL);
	systemtime->wMilliseconds = tv.tv_usec/1000;

};
unsigned long GetTickCount()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	unsigned long ms = ts.tv_sec*1000 + ts.tv_nsec/1000000;
	return ms;
};
#endif
_tstring get_directory_path(const _tchar* filepath)
{
	//std::string dirpath = filepath;
	//size_t pos1 = dirpath.find_last_of(_T('\\'), dirpath.length());
	//size_t pos2 = dirpath.find_last_of(_T('/'), dirpath.length());
	//size_t pos = pos1 > pos2 ? pos1 : pos2;
	//dirpath = dirpath.substr(0, pos) + _T('\\');  // Return the directory without the file name

	std::string dirpath = filepath;
	std::replace(dirpath.begin(), dirpath.end(), '\\', '/');
	size_t pos = dirpath.find_last_of(('/'), dirpath.length());
	dirpath = dirpath.substr(0, pos) + ('/');

	return dirpath;
}



_tstring transferDirPath(_tstring filepath)
{
	size_t pos1 = filepath.find_last_of(_T('\\'), filepath.length());
	size_t pos2 = filepath.find_last_of(_T('/'), filepath.length());
	size_t pos = pos1> pos2 ? pos1 : pos2;

	std::string dir = filepath.substr(0, pos) + _T('\\');
	return dir;

}
_tstring getProgramDir()
{
#ifdef _WIN32
	_tchar exeFullPath[MAX_PATH]; // Full path
	_tstring strPath = _T("");

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (_tstring)exeFullPath;    // Get full path of the file
	size_t pos = strPath.find_last_of(_T('\\'), strPath.length());


	return (strPath.substr(0, pos) + _T('\\'));//exe ��·��

#else
	char path[256];
	int len  = readlink("/proc/self/exe",path,sizeof(path)-1);
	if(len>0)
	{
		path[len] = '\0';
		_tstring strPath = path;
		size_t pos = strPath.find_last_of(_T('/'), strPath.length());
		return (strPath.substr(0, pos) + _T('/'));//exe ��·��
	}
	else
	{
		printf("Failed to get the executable path\n");
		return _tstring("");
	}
#endif
}
_tstring normalizePath(_tstring path)
{
	/*if (BaseApp::get() == NULL)
	{
		ERROR_LOG("no BaseApp");
		return path;
	}
	std::string dirpath = BaseApp::get()->getProgramDir() + path + "\\";*/

	if (path.at(0) == '.')
	{
		path = ySingletone::get()->getConfigDir() + path;
	}

	std::string dirpath = path;
	std::replace(dirpath.begin(), dirpath.end(), '\\', '/');
	//DEBUG_LOG("%s", dirpath.c_str())
	while (1)//对"./"这种路径进行规整
	{
		size_t pos = dirpath.rfind("/./");
		if (pos != _tstring::npos)
		{
			_tstring head = dirpath.substr(0, pos);
			_tstring foot = dirpath.substr(pos);
			foot = foot.substr(2);//裁掉"/."
			dirpath = head + foot;
		}
		else
		{
			break;
		}

	}
	while (1)//对"../"这种路径进行规整
	{
		size_t pos = dirpath.find("/../");
		if (pos != _tstring::npos)
		{
			_tstring head = dirpath.substr(0, pos);
			head = head.substr(0, head.rfind("/"));
			_tstring foot = dirpath.substr(pos);
			foot = foot.substr(3);//裁掉"/.."

			dirpath = head + foot;
		}
		else
		{
			break;
		}

	}
	createDirectories(dirpath.c_str());
	return dirpath;

}


//字符串分割函数
std::vector<_tstring> split(_tstring str, _tstring pattern)
{
	_tstring::size_type pos;
	std::vector<_tstring> result;
	str += pattern;//扩展字符串以方便操作
	int size = (int)str.size();

	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			_tstring s = str.substr(i, pos - i);
			result.push_back(s);
			i = (int)pos + (int)pattern.size() - 1;
		}
	}
	return result;
}
