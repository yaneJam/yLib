#include "stdafx.h"
#include <set>
#include <queue>
#include <ctime>
#ifdef _WIN32
#else
#include <unistd.h>
#include <dlfcn.h>
#endif
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#define SERVICE_SET//�����Զ�ȥ��
#ifndef SERVICE_SET
std::list<yService*> services;//��std::set�������һ��
#else
//std::set<yService*> services;
#endif
//

ySingletone* pThis = NULL;

ySingletone::ySingletone()
{
	pThis = this;
	AppContex = _tsptr<yValue>(new yValue);

}
ySingletone::~ySingletone()
{
	
	#ifdef MSVC
	ySyncAccess a(ySingletone::get()->g_ThreadLock, __FUNCTION__);
	#else
	std::lock_guard<std::recursive_mutex> a(ySingletone::get()->g_ThreadLock);
	#endif

	//ySyncAccess a(ySingletone::g_ThreadLock, __FUNCTION__);
	//while (Msglist.size())
	//{
	//	Msglist.pop();
	//}
	
}


ySingletone* ySingletone::getApp()
{
	if (pThis == NULL)
	{
		ERROR_LOG("no app");
	}
	return pThis;
}

ySingletone* ySingletone::get()
{
	if (pThis == NULL)
	{
		ERROR_LOG("no app");
	}
	return pThis;
}
bool ySingletone::isSubScene(std::string str)
{
	//����SceneCode�Ƿ����str
	return ySingletone::get()->getSceneCode().find(str) != std::string::npos;
}

std::string ySingletone::getSubScene()
{
	//���ص�ǰ��Ŀ��(SceneCode��"_"֮ǰ���ַ�������ZH_TEST��ZH����û��"_"�򷵻������ַ���)
	_tstring str = ySingletone::get()->getSceneCode();
	int nPos_1 = str.find("_");
	return str.substr(0, nPos_1);
}


void ySingletone::setSceneCode(_tstring c)
{
	 SceneCode = c;
}
_tstring ySingletone::getSceneCode()
{
	//return "BD";
	return SceneCode;
}

_tstring ySingletone::getConfigDir()
{
	if (config_dir.empty())
	{
		return getProgramDir();
	}
	return config_dir;
}

_tstring  ySingletone::getTempDir()
{
	if (temp_dir.empty())
	{
		return getConfigDir();
	}
	return temp_dir;
}
void ySingletone::setTempDir(_tstring p)
{
	DEBUG_LOG("%s",p.c_str());
	createDirectories(p.c_str());
	DEBUG_LOG("created %s",p.c_str());
	bool checkWritelimit = false;//��������ļ��ж�дȨ��
	long ltime = time(NULL);
	_tchar id[256];
	_stprintf_s(id, 256 * sizeof(_tchar), _T("%d%d"), ltime, GetTickCount() % 1000);
	_tstring tictok = id;
	{
		_tsptr<JsonConfiger>configer(new JsonConfiger);
		configer->loadFile(p + "testWrite");
		(*configer)["tictok"] = tictok;
		DEBUG_LOG("saving %s",(p + "testWrite").c_str());
		configer->save();
		DEBUG_LOG("saved %s",(p + "testWrite").c_str());
	}
	{
		_tsptr<JsonConfiger>configer(new JsonConfiger);

		if (configer->loadFile(p + "testWrite"))
		{
			if (configer->hasNode("tictok"))
			{
				_tstring apple = (*configer)["tictok"].asString();
				if (apple == tictok)
				{
					checkWritelimit = true;
				}
			}
		}
	}
	if (checkWritelimit == false)
	{
#ifdef _WIN32
		_tstring error = _tstring("��ʱ�ļ������ô���:") + p;
		MessageBox(NULL, error.c_str(), "����", MB_OK);
#endif
		return;
	}
	temp_dir = p;
}

void ySingletone::setConfigDir(_tstring p)
{
	DEBUG_LOG("%s",p.c_str());
	createDirectories(p.c_str());
	DEBUG_LOG("created %s",p.c_str());
	config_dir = p;
	bool checkWritelimit = false;//��������ļ��ж�дȨ��
	long ltime = time(NULL);
	_tchar id[256];
	_stprintf_s(id, 256 * sizeof(_tchar), _T("%d%d"), ltime, GetTickCount() % 1000);
	_tstring tictok = id;
	{

		_tsptr<JsonConfiger>configer(new JsonConfiger);
		configer->loadFile(p + "testWrite");
		(*configer)["tictok"] = tictok;
		configer->save();

	}
	{
		_tsptr<JsonConfiger>configer(new JsonConfiger);

		if (configer->loadFile(p + "testWrite"))
		{
			if (configer->hasNode("tictok"))
			{
				_tstring apple = (*configer)["tictok"].asString();
				if (apple == tictok)
				{
					checkWritelimit = true;
				}
			}
		}
	}
	if (checkWritelimit == false)
	{
#ifdef _WIN32
		_tstring error = _tstring("�����ļ������ô���:") + p;
		MessageBox(NULL, error.c_str(), "����", MB_OK);
#endif
		return;
	}
#ifdef _WIN32
	SetCurrentDirectoryA(config_dir.c_str());
#else
	if(chdir(config_dir.c_str())!=0)
	{
	ERROR_LOG(" chdir error");
	}
#endif

}
#ifdef useUser

_tsptr<UserSqliteManager> ySingletone::getUserManager()
{
	if (usermanager == NULL)
	{
		usermanager = _tsptr<UserSqliteManager>(new UserSqliteManager);
	}
	return usermanager;
}
_tsptr<UserIE>  ySingletone::getUser()
{
	if (usermanager == NULL)
	{
		usermanager = _tsptr<UserSqliteManager>(new UserSqliteManager);//û��initĬ�Ϸÿ�Ȩ��
	}
	return usermanager->getCurrentUser();
}
#endif



typedef void(*pluginInfoFunc)();
bool ySingletone::scanPlugin(_tstring dllName)
{
#ifdef _WIN32
	if (pluginInfo == NULL)
	{
		pluginInfo = _tsptr<yValue>(new yValue);
	}
	_tsptr<yConfiger>  pluginConfiger = getPluginConfiger();
	HINSTANCE dll = LoadLibrary(dllName.c_str());
	if (dll)
	{
		pluginInfoFunc fun = (pluginInfoFunc)GetProcAddress(dll, "pluginInfo");//��ú���ָ��
		if (fun)
		{
			fun();//����configer�ļ�	
			_tstring pluginname = dllName;
			std::replace(pluginname.begin(), pluginname.end(), '\\', '/');
			size_t pos = pluginname.rfind("/");
			if (pos != _tstring::npos)
			{
				_tstring foot = pluginname.substr(pos + 1);
				pluginname = foot;
			}
			size_t pos2 = pluginname.rfind(".dll");
			if (pos2 != _tstring::npos)
			{
				_tstring head = pluginname.substr(0, pos2);
				pluginname = head;
			}

			auto configer = pluginConfiger->getChildConfiger(pluginname);//��ȡconfiger�ļ�
			if (configer)
			{
				(*pluginInfo)[pluginname] = *configer;
			}
			else
			{
				ERROR_LOG("no childconfiger %s", dllName.c_str());
			}

		}
		else
		{
			ERROR_LOG("load fun failed %s", dllName.c_str());
		}
		FreeLibrary(dll);
	}
	else
	{
		ERROR_LOG("load dll failed %s", dllName.c_str());
		return false;
	}
	return true;
#else
	ERROR_LOG("ySingletone::scanPlugin not supported");
	return false;
#endif
}

void ySingletone::scanPlugins(_tstring pluginDir)
{
#ifdef _WIN32
	//����dll
	std::list<_tstring> filename_list;
	WIN32_FIND_DATA data;
	HANDLE hFind;
	_tchar cFullPath[1024];
	_tchar cNewPath[1024];
	_stprintf_s(cFullPath, _T("%s\\*"), pluginDir.c_str());
	hFind = FindFirstFile(cFullPath, &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if ((!_tcscmp(_T("."), data.cFileName)) || (!_tcscmp(_T(".."), data.cFileName)))
			{
				continue;
			}
			if (data.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				continue;
			}
			//��׺
			_tstring filename = data.cFileName;
			if (filename.size() > 4)
			{
				DEBUG_LOG("%s", filename.c_str());
				filename = filename.substr(filename.size() - 4);
				if (filename == ".dll")
				{
					_stprintf_s(cFullPath, _T("%s\\%s"), pluginDir.c_str(), data.cFileName);
					filename_list.push_back(cFullPath);
				}
			}
			//_stprintf_s(cFullPath, _T("%s\\%s"), patientdir.c_str(), data.cFileName);
			//filename_list.push_back(cFullPath);


		} while (FindNextFile(hFind, &data));

		::FindClose(hFind);
	}
	
	//pluginInfo = _tsptr<yValue>(new yValue);
	//_tsptr<yConfiger>  pluginConfiger= getPluginConfiger();
	
	for (auto dllName : filename_list)
	{
		scanPlugin(dllName);
		//HINSTANCE dll = LoadLibrary(dllName.c_str());
		//if (dll)
		//{
		//	pluginInfoFunc fun = (pluginInfoFunc)GetProcAddress(dll, "pluginInfo");//��ú���ָ��
		//	if (fun)
		//	{
		//		fun();//����configer�ļ�	
		//		_tstring pluginname = dllName;
		//		std::replace(pluginname.begin(), pluginname.end(), '\\', '/');
		//		size_t pos = pluginname.rfind("/");
		//		if (pos != _tstring::npos)
		//		{
		//			_tstring foot = pluginname.substr(pos+1);
		//			pluginname = foot;
		//		}
		//		size_t pos2 = pluginname.rfind(".dll");
		//		if (pos2 != _tstring::npos)
		//		{
		//			_tstring head = pluginname.substr(0, pos2);
		//			pluginname = head;
		//		}

		//		auto configer = pluginConfiger->getChildConfiger(pluginname);//��ȡconfiger�ļ�
		//		if (configer)
		//		{
		//			(*pluginInfo)[pluginname] = *configer;
		//		}
		//		else
		//		{
		//			ERROR_LOG("no childconfiger %s", dllName.c_str());
		//		}

		//	}
		//	else
		//	{
		//		ERROR_LOG("load fun failed %s", dllName.c_str());
		//	}
		//	FreeLibrary(dll);
		//}
		//else
		//{
		//	ERROR_LOG("load dll failed %s", dllName.c_str());
		//	continue;
		//}
		
	}
	
	////��ȡconfiger�ļ�
	//
	//_tstring pluginName = "TCSObjects";
	//_tsptr<JConfiger> configer(new JConfiger);
	//_tstring configpath = getConfigDir()+ pluginName + ".cfg";

	//_tsptr<yConfiger>  plugininfo = getPluginInfo();

	//if (plugininfo->getChildConfiger(configpath))
	//{
	//	(*globalPlugin)["pluginName"] = *configer;
	//}
	//else
	//{
	//	ERROR_LOG("no configer %s", configpath.c_str());
	//}
#else
	ERROR_LOG("ySingletone::scanPlugins not supported");
#endif

}

bool ySingletone::pushPlugin(_tstring dllName)
{
	if (dlls.find(dllName) != dlls.end())//֮ǰ���ع���
	{
		return true;
	}
#ifdef _WIN32
	
	HINSTANCE dll = LoadLibrary(dllName.c_str());
	if (dll)
	{
		createModuleFunc fun = (createModuleFunc)GetProcAddress(dll, "createModule");//��ú���ָ��
		if (fun)
		{
			funs.push_back(fun);
			dlls[dllName] = dll;
		}
		else
		{
			ERROR_LOG("load fun failed %s", dllName.c_str());
			return false;
			//CloseHandle(dll);
		}
	}
	else
	{
		ERROR_LOG("load dll failed %s", dllName.c_str());
		return false;
	}
	return false;
#else
	//void* dll = dlopen(dllName.c_str(),RTLD_LAZY);
	void* dll = dlopen(dllName.c_str(),RTLD_NOW);
	if (dll)
	{
		createModuleFunc fun = (createModuleFunc)dlsym(dll, "createModule");//��ú���ָ��
		if (fun)
		{
			funs.push_back(fun);
			dlls[dllName] = dll;
		}
		else
		{
			ERROR_LOG("load fun failed %s", dllName.c_str());
			return false;
			//CloseHandle(dll);
		}
	}
	else
	{
		ERROR_LOG("load dll failed %s", dllName.c_str());
		return false;
	}
	return false;
	//ERROR_LOG("ySingletone::pushPlugin not supported");
	return false;
#endif
}

_tsptr<yModule> ySingletone::getModule(_tstring instanceName)
{
	////���濼��һ�¼��� ySyncAccess a(RecursionLock, __FUNCTION__);
	//std::map<_tstring, _tsptr<yModule>>::iterator iter = modules.find(instanceName);
	//if (iter != modules.end())
	//{
	//	return iter->second;
	//}
	//else
	//{
	//	return NULL;
	//}

	#ifdef MSVC
	ySyncAccess a(module_lock, __FUNCTION__);
	#else
	std::lock_guard<std::recursive_mutex> a(module_lock);
	#endif

	//ySyncAccess a(ySingletone::module_lock, __FUNCTION__);

	for (auto iter : modules)
	{
		if (iter->getInstanceName() == instanceName)
		{
			return iter;
		}
	}
	return NULL;
}

bool ySingletone::addModule(_tsptr<yModule> mod)
{
	#ifdef MSVC
	ySyncAccess a(module_lock, __FUNCTION__);
	#else
	std::lock_guard<std::recursive_mutex> a(module_lock);
	#endif

	//ySyncAccess a(ySingletone::module_lock, __FUNCTION__);
	modules.insert(mod);
	return true;
}
bool ySingletone::removeModule(_tsptr<yModule> mod)
{
		#ifdef MSVC
	ySyncAccess a(module_lock, __FUNCTION__);
	#else
	std::lock_guard<std::recursive_mutex> a(module_lock);
	#endif
	//ySyncAccess a(ySingletone::module_lock, __FUNCTION__);
	modules.erase(mod);
	return true;
}
bool ySingletone::removeModule(_tstring instancename)
{
	_tsptr<yModule> temp = getModule(instancename);
	if (temp)
	{
		return removeModule(temp);
	}
	return true;
}

_tsptr<yModule> ySingletone::createModule(_tstring objectName, _tstring instanceName)
{


	_tsptr<yModule> temp = getModule(instanceName);
	if (temp)
	{
		return temp;
	}

	//std::map<_tstring, _tsptr<yModule>>::iterator map_iter = modules.find(instanceName);
	//if (map_iter != modules.end())
	//{
	//	if (map_iter->second->getObjectName() == objectName)
	//	{
	//		return map_iter->second;//��������
	//	}
	//}
	_tsptr<yModule> seq;
	for (auto fun : funs)
	{
		yModuleHandle handle = fun(objectName);
		seq = handle.obj;
		if (seq)
		{
			#ifdef MSVC
			ySyncAccess a(ySingletone::module_lock, __FUNCTION__);
			#else
			std::lock_guard<std::recursive_mutex> a(module_lock);
			#endif
			seq->setInstanceName(instanceName.c_str());
			addModule(seq);

			//modules[instanceName]= seq;
			if(seq->init()== false )
			{
				ERROR_LOG("%s init failed", instanceName.c_str());
			}
			break;
		}
		
	}
	if (seq == NULL)
	{
		ERROR_LOG("failed to create %s", objectName.c_str());
	}
	//else
	//{
	//	DEBUG_LOG("created %s %s ", objectName.c_str(), instanceName.c_str());
	//}
	

	
	return seq;
}

_tsptr<yValue> ySingletone::getPluginInfo()
{
	return pluginInfo;
}

_tsptr<yConfiger> ySingletone::getPluginConfiger()
{
	if (!m_pluginConfiger)
	{
		m_pluginConfiger = _tsptr<JConfiger>(new JConfiger);

		if (!m_pluginConfiger->loadFile(getConfigDir() + "packageInfo.pak"))
		{
			createDirectories((getConfigDir()).c_str());
			m_pluginConfiger->save();
		}
	}
	return m_pluginConfiger;
};