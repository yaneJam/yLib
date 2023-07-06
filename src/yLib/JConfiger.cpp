#include "stdafx.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif



//namespace HD
//{
	JConfiger::JConfiger()
	{
		styled = true;
	}
	JConfiger::~JConfiger()
	{

	}

	yValue & JConfiger::operator=(yValue& other)
	{

		json_value = yValue(other).asJsonValue();

		//yConfigerֻ��owner(yConfiger)������m_parent(yValue)
		//if (m_parent)
		//{
		//	m_parent->appendNode(NodeName, *this);
		//}
		return *this;
	}


	bool JConfiger::save(yValue value, _tstring filepath, bool enc)
	{
		_tsptr<JConfiger> apple(new JConfiger);
		(*apple) = value;
		return apple->save(filepath, false);
	}

	bool JConfiger::save(_tstring filepath, bool enc)
	{
			#ifdef MSVC
		HDSyncAccess a(yConfiger::g_ThreadLock, __FUNCTION__);
	#else
		//DEBUG_LOG("waiting");
		std::lock_guard<std::recursive_mutex> a(yConfiger::g_ThreadLock);
		//DEBUG_LOG("done");
	#endif
		if (filepath.empty())
		{
			ERROR_LOG("no path ");
			return false;
		}


		FILE* fHandle = fopen(filepath.c_str(), "wb");
		if (NULL == fHandle)
		{
			ERROR_LOG("open config file error, path:%s", filepath.c_str());
			return false;
		}
		/*std::ofstream file(filepath.c_str());
		if (!file.is_open())
		{
			ERROR_LOG("open config file error, path:%s", filepath.c_str());
			return false;
		}*/
		if (styled == true)
		{
			Json::StyledWriter writer;
			std::string data = writer.write(json_value);

			if (enc)
			{
				std::string apple = encodeString(data);
				data = apple;
			}
			if (data.empty())
			{
				WARN_LOG("data.empty");
			}

			int iWrited = fwrite(data.c_str(), sizeof(_tchar), data.size(), fHandle);
			if (iWrited != data.size())
			{
				WARN_LOG("fwrite Error!");
			}
			fclose(fHandle);
			//file << data;
			//file.flush();//���߳̿��ܻ�û���浽�ֱ����ˣ����Լ������
			//file.close();
		}
		else
		{
			Json::FastWriter writer;
			std::string data = writer.write(json_value);
			if (enc)
			{
				std::string apple = encodeString(data);
				data = apple;
			}

			int iWrited = fwrite(data.c_str(), sizeof(_tchar), data.size(), fHandle);
			if (iWrited != data.size())
			{
				WARN_LOG("fwrite Error!");
			}
			fclose(fHandle);

			//file << data;
			//file.flush();//���߳̿��ܻ�û���浽�ֱ����ˣ����Լ������
			//file.close();

		}

		return true;
	}

	bool JConfiger::save(bool enc)
	{
	#ifdef MSVC
		HDSyncAccess a(yConfiger::g_ThreadLock, __FUNCTION__);
	#else
		//DEBUG_LOG("waiting");
		std::lock_guard<std::recursive_mutex> a(yConfiger::g_ThreadLock);
		//DEBUG_LOG("done");
	#endif
		if (configFilePath.empty())
		{
			ERROR_LOG("no path ");
			return false;
		}
		save(configFilePath, enc);

		//std::ofstream file(configFilePath.c_str());
		//if (!file.is_open())
		//{
		//	ERROR_LOG("open config file error, path:%s", configFilePath.c_str());
		//	return false;
		//}
		//if (styled == true)
		//{
		//	Json::StyledWriter writer;
		//	std::string data = writer.write(json_value);
		//
		//	if (enc)
		//	{
		//		std::string apple =  encodeString(data);
		//		data = apple;
		//	}
		//	if (data.empty())
		//	{
		//		WARN_LOG("data.empty");
		//	}

		//	file << data;
		//	file.flush();//���߳̿��ܻ�û���浽�ֱ����ˣ����Լ������
		//	file.close();
		//
		//	//
		//}
		//else
		//{
		//	Json::FastWriter writer;
		//	std::string data = writer.write(json_value);
		//	if (enc)
		//	{
		//		std::string apple = encodeString(data);
		//		data = apple;
		//	}

		//	file << data;
		//	file.flush();//���߳̿��ܻ�û���浽�ֱ����ˣ����Լ������
		//	file.close();

		//}

		return true;
	}

	bool JConfiger::loadContent(_tstring content)
	{
		

	#ifdef MSVC
		HDSyncAccess a(yConfiger::g_ThreadLock, __FUNCTION__);
	#else
		std::lock_guard<std::recursive_mutex> a(yConfiger::g_ThreadLock);
	#endif
	
		Json::Reader reader;
		if (!reader.parse(content, json_value, true))
		{
			ERROR_LOG("parsing config file error, content :%s", content.c_str());
			return false;
		}


		return true;
	}


	bool JConfiger::loadFile(_tstring filePath)
	{
	#ifdef MSVC
		HDSyncAccess a(yConfiger::g_ThreadLock, __FUNCTION__);
	#else
		std::lock_guard<std::recursive_mutex> a(yConfiger::g_ThreadLock);
	#endif
		configFilePath = filePath;
		NodeName = configFilePath;//�����жϵ���־Ҫ��nodeName
		Json::Reader reader;

		std::string str = getContent(filePath);
		if (str.empty())
		{
			WARN_LOG("empty, path:%s", filePath.c_str());
		}
		if (!reader.parse(str, json_value, true))
		{
			ERROR_LOG("parsing config file error, path:%s", filePath.c_str());
			return false;
		}


		return true;
	}





	_tsptr<yConfiger> JConfiger::getChildConfiger(_tstring configerName)
	{
		_tsptr<yConfiger> child(new JConfiger);
		if (!json_value.isMember(configerName) || !json_value[configerName].isString())
		{
			json_value[configerName] = configerName + ".cfg";
			save();
		}

		/*
		todo:
		if (!pathexist)
		{

		}*/
		std::string filepath = json_value[configerName].asString();//���·��
		int pos1 = configFilePath.find_last_of(('\\'), configFilePath.length());
		int pos2 = configFilePath.find_last_of(('/'), configFilePath.length());
		int pos = pos1 > pos2 ? pos1 : pos2;

		std::string confieRoot_path = configFilePath.substr(0, pos) + ('/');
		filepath = confieRoot_path + filepath;

		/*
		todo:
		if (!pathexist)
		{
		createdirectory()
		}*/

		if (child->loadFile(filepath))
		{
			return child;
		}
		else
		{
			child->save();//���filepath�ļ��в����ڣ�Ȩ�޲����������,�Ȳ���
		}



		return child;


	};

//}