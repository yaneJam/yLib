#pragma once
#include <queue>
class yMSG;
class yService;

#define MACRO_BLOCK_BEGIN \
HANDLE m_hEventExit = ::CreateEvent(NULL, TRUE, FALSE, NULL);\
if (!m_hEventExit)\
{\
	ERROR_LOG("cannot create event");\
	return yModule::FAILED;\
}\
ResetEvent(m_hEventExit);\
int cbresult;\
_tsptr<yValue> cb_response;\
_tsptr<yCMD> block_cmd(new yCMD);\
block_cmd->callback = [&cb_response,&cbresult, &m_hEventExit](int returnCode, _tsptr<yValue> response)\
{\
	if (returnCode == yCMD::SUCCESS)\
	{\
		cbresult = yCMD::SUCCESS;\
	}\
	else if (returnCode == yCMD::TIMEOUT)\
	{\
		cbresult = yCMD::TIMEOUT;\
	}\
	else\
	{\
		cbresult = yCMD::FAILED;\
	}\
	DEBUG_LOG("done");\
	::SetEvent(m_hEventExit);\
	cb_response = response;\
};


#define MACRO_BLOCK_END \
DEBUG_LOG("waiting");\
::WaitForSingleObject(m_hEventExit, 5000);\
if (m_hEventExit)\
{\
	::CloseHandle(m_hEventExit);\
}

#define MACRO_doSlot(fun) \
if (cmd->method == #fun)\
{\
	return fun(cmd);\
}


class IO_API IFlusher
{
public:
	virtual void Flush() = 0;
	//virtual void Flush2(){};
};


#define yLogD if(logOut&&loglevel<=DEBUG_LEVEL_DEBUG)yLogger::writer(DEBUG_LEVEL_DEBUG, getInstanceName().c_str())->writeUTF8
#define yLogW if(logOut&&loglevel<=DEBUG_LEVEL_WARN)yLogger::writer(DEBUG_LEVEL_WARN, getInstanceName().c_str())->writeUTF8
#define yLogE if(logOut&&loglevel<=DEBUG_LEVEL_ERROR)yLogger::writer(DEBUG_LEVEL_ERROR, getInstanceName().c_str())->writeUTF8



class yModuleImp;
class IO_API yModule :public yObject, public _tshare_from_this<yModule>
{
public:
	MACRO_Y_OBJECT(yModule)

	enum RESULT//˳�������ȼ�:FAILED>SUCCESS>PARAM_ERROR>STATE_ERROR>NO_RETURN
	{
		FAILED = 0,
		SUCCESS ,//����з���SUCESS
		PARAM_ERROR,
		STATE_ERROR,
		NO_RETURN,//û����Ч�ķ���ֵ
	};

	virtual bool init() { return true; };//��ȡ�����ļ�
	virtual 	yModule::RESULT  initDevice(_tsptr<yCMD> cmd) { return SUCCESS; };//connect ���ڴ�С֮��ĳ�ʼ�� ���첽
	virtual bool isDeviceReady() { return true; };
	void setLog(bool enable, DEBUG_LEVEL level= DEBUG_LEVEL_DEBUG);

	void addOwner(_tsptr<yModule> m);
	// void sequenceAdd(_tsptr<yModule> m);
	// void sequenceRemove(_tstring instancename);

	RESULT emitSignal(_tstring signalName);
	RESULT emitSignal(_tstring signalName, _tsptr<yCMD> cmd);

	virtual RESULT doSlot(_tsptr<yCMD> cmd);
	

	//void addService(yService* service);
	//void removeService(yService* service);


	//virtual bool fillDcmDataset(DcmDataset* resultDset){ return false; };
public:
	virtual bool getContext(_tsptr<yValue>& v);
	virtual bool setContext(_tsptr<yValue> v);

	virtual void remove_all_output();
	virtual void remove_all_connection();

	
	
	//��
	virtual bool setInput(_tsptr<DataObject> data, _tstring port = "default");
	virtual bool getOutput(_tsptr<DataObject>* data, _tstring = "default");//Ĭ��input����output
	virtual bool setConnection(_tsptr<yModule> input_port, _tstring input_index="default" , _tstring output_index = "default");
	virtual bool addOutputModule(_tsptr<yModule> output_port);



	//retiring
	//һ������˿�ֻ��һ��yModule��һ������˿ڿ�����������yModule
	//input_port = NULL��ɾ��
	//set_connection�ڲ������set_want_to_update
	virtual bool set_connection(_tsptr<yModule> input_port, int input_index = 0, int output_index = 0);
	virtual bool set_output_connection(_tsptr<yModule> output_port, int output_index = 0);

	virtual bool set_input(_tsptr<DataObject> data, int port = 0);
	virtual bool get_output(_tsptr<DataObject>* data, int port);//Ĭ��input����output



	void set_want_to_update();
	virtual bool need_to_update();
	virtual bool update();

protected:
	yModuleImp* imp;

	
	//��
	std::map<_tstring, std::pair<_twptr<yModule>, _tstring>> input_modules;//��ͬ����˿ڿ�����ͬһ��yModule�Ĳ�ͬ�˿�
	std::map<_tstring, _tsptr<DataObject>> datas;//��ֹ�ϼ�yModule��ɾ��

	//retiring
	std::map<int, std::pair<_twptr<yModule>, int>> input_ports;//��ͬ����˿ڿ�����ͬһ��yModule�Ĳ�ͬ�˿�
	std::map<int, _tsptr<DataObject>> inputs;//��ֹ�ϼ�yModule��ɾ��

	_tsptr<yValue> m_context;
protected:
	//virtual void notifyToService(yMSG msg);//������Ϣ��Service
	std::list<_twptr<yModule>>m_owners;
	#ifdef MSVC
	yRecursionLock RecursionLock;
	#else
	std::recursive_mutex RecursionLock;
	#endif
	int logOut;
	DEBUG_LEVEL loglevel;
private:
	//std::list<yService*> services;//��std:set�������һ��	
};


void IO_API cloneYCMD(_tsptr<yCMD>src, _tsptr<yCMD>dest);

struct IO_API yModuleHandle
{
	_tsptr<yModule> obj;
	long long objId = -1;
	_tstring  objType;
};

typedef yModuleHandle(*createModuleFunc)(_tstring mapperType);
