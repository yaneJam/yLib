#include "stdafx.h"
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

void cloneYCMD(_tsptr<yCMD>src, _tsptr<yCMD>dest)
{
	if (src && dest)
	{
		dest->caller = src->caller;
		dest->method = src->method;
		dest->request = src->request;//
		dest->response = src->response;//��ʱ������response��ߵ����ݣ��Ժ��ٿ�
		dest->resultCode = src->resultCode;
		dest->cmdID = src->cmdID;
		dest->timerID = src->timerID;
		dest->uuid = src->uuid;
		dest->callback = src->callback;
		dest->data = src->data;//��ʱ������response��ߵ����ݣ��Ժ��ٿ�
		dest->ptr = src->ptr;
	}
};

class  yModuleImp
{
public:
	std::map<_twptr<yModule>, int, std::owner_less<_twptr<yModule>>> map_output_ports;//update�ã������ظ�
	std::list<_twptr<yModule>> map_output_modules;

	bool want_to_update;
};

yModule::yModule(void)
{
	imp = new yModuleImp;
	logOut = true;
	loglevel = DEBUG_LEVEL_DEBUG;
	imp->want_to_update = false;
}

yModule::~yModule(void)
{
	

	delete imp;
}

void yModule::setLog(bool enable, DEBUG_LEVEL level)
{
	logOut = enable;
	loglevel = level;
}

void yModule::remove_all_output()
{
	
	imp->map_output_modules.clear();
	imp->map_output_ports.clear();
}
void yModule::remove_all_connection()
{
	input_ports.clear();
	imp->map_output_modules.clear();
	imp->map_output_ports.clear();

	inputs.clear();

}
void yModule::set_want_to_update()
{
	imp->want_to_update = true;
}

bool yModule::getContext(_tsptr<yValue>& v)
{
	if (m_context == NULL)
	{
		m_context = _tsptr<yValue>(new yValue);
	}

	m_context->lock();
	v = m_context;
	m_context->unLock();


	return true;
}
bool yModule::setContext(_tsptr<yValue> v)
{
	if (v == NULL)
	{
		m_context = v;
	}
	else
	{
		v->lock();
		m_context = v;
		v->unLock();
	}

	return true;
}

bool yModule::getOutput(_tsptr<DataObject>* data, _tstring port)
{

	if (!datas[port])//����datas�����û��
	{
		if (!input_modules[port].first.expired())//����inport�����û��
		{
			_tsptr<yModule> input_port = input_modules[port].first.lock();
			input_port->getOutput(&datas[port], input_modules[port].second);
		}
	}
	if (!datas[port])
	{
		DEBUG_LOG("%s get no data %s ", getInstanceName().c_str(), port.c_str());
	}

	*data = datas[port];//Ĭ������output=inputport[].output
	return true;
}


bool yModule::setConnection(_tsptr<yModule> input_port, _tstring input_index, _tstring output_index)
{
	_tsptr<yModule> sp_this = shared_from_this();
	if (input_port == NULL)
	{
		input_modules.erase(input_index);
		set_want_to_update();
		return true;
	}
	if (input_port->addOutputModule(sp_this))
	{
		input_modules[input_index] = std::pair < _twptr<yModule>, _tstring >(input_port, output_index);
		set_want_to_update();
		return true;
	}

	return false;
}
bool yModule::addOutputModule(_tsptr<yModule> output_port)
{
	
	imp->map_output_modules.push_back(output_port);
	return true;
}
bool yModule::setInput(_tsptr<DataObject> data, _tstring port)
{

	(datas[port]) = data;

	return true;
}

bool yModule::get_output(_tsptr<DataObject>* data, int port)
{
	if (!inputs[port])
	{
		if (!input_ports[port].first.expired())
		{
			_tsptr<yModule> input_port = input_ports[port].first.lock();
			input_port->get_output(&inputs[port], input_ports[0].second);
		}
	}
	*data = inputs[port];//Ĭ������output=inputport[].output
	return true;
}

bool yModule::set_input(_tsptr<DataObject> data, int port)
{
	(inputs[port]) = data;
	return true;
}


bool yModule::set_connection(_tsptr<yModule> input_port, int input_index, int output_index)
{
	_tsptr<yModule> sp_this = shared_from_this();
	if (input_port == NULL)
	{
		input_ports.erase(input_index);
		set_want_to_update();
		return true;
	}
	if (input_port->set_output_connection(sp_this, output_index))
	{
		input_ports[input_index] = std::pair < _twptr<yModule>, int >(input_port, output_index);
		set_want_to_update();
		return true;
	}

	return false;
}

bool yModule::set_output_connection(_tsptr<yModule> output_port, int output_index)
{
	imp->map_output_ports[output_port] = output_index;
	return true;
}

bool yModule::need_to_update()
{
	return imp->want_to_update;
}

bool yModule::update()
{
	imp->want_to_update = false;

	//��
	int size = imp->map_output_ports.size();
	std::map<_twptr<yModule>, int>::iterator iter = imp->map_output_ports.begin();
	for (int i = 0; i < size; i++)
	{
		_twptr<yModule> wp_output_port = iter->first;
		if (!wp_output_port.expired())
		{
			_tsptr<yModule> output_port = wp_output_port.lock();
			output_port->update();
		}
		iter++;
	}
	//��

	//std::list< _twptr<yModule>>::iterator iter_owner = m_owners.begin();
	for (auto iter_owner = m_owners.begin(); iter_owner != m_owners.end(); )
	{
		_tsptr<yModule> output_module;
		if (!iter_owner->expired())
		{
			output_module = (iter_owner->lock());
			output_module->update();
			iter_owner++;
		}
		else
		{
			//yLogD("erasing");
			iter_owner = m_owners.erase(iter_owner);//û�õ���ָ�������������֪��list������erase�᲻�������Ҫ����ʵ�鿴һ��
			//yLogD("erased");
		}
	}
	return true;
}






// void yModule::addService(yService* service)
// {
// 	#ifdef MSVC
// 	ySyncAccess a(RecursionLock, __FUNCTION__);
// 	#else
// 	std::lock_guard<std::recursive_mutex> a(RecursionLock);
// 	#endif
// 	//ySyncAccess a(RecursionLock,__FUNCTION__);
// 	size_t size = services.size();
// 	std::list<yService*>::iterator iter = services.begin();
// 	bool duplicate = false;
// 	for (int i = 0; i < size; i++)
// 	{
// 		if (*iter == service)
// 		{
// 			duplicate = true;
// 			break;
// 		}
// 		iter++;
// 	}
// 	if (!duplicate)
// 	{
// 		services.push_back(service);
// 	}
	
// }
// void yModule::removeService(yService* service)
// {
// 		#ifdef MSVC
// 	ySyncAccess a(RecursionLock, __FUNCTION__);
// 	#else
// 	std::lock_guard<std::recursive_mutex> a(RecursionLock);
// 	#endif
// 	//ySyncAccess a(RecursionLock, __FUNCTION__);
// 	size_t servicecount = services.size();
// 	std::list<yService*>::iterator service_iter = services.begin();
// 	for (int i = 0; i < servicecount; i++)
// 	{
// 		if (*service_iter == service)
// 		{
			
// 			services.erase(service_iter);
			
// 			break;
// 		}
// 		service_iter++;
// 	}
	
// }

// void yModule::notifyToService(yMSG msg)
// {
	
// 	//////////////////////////////������////////////////////////
// 	msg.Sender = this;
// 	std::list<yService*> TEMP;
// 	{
// 			#ifdef MSVC
// 	ySyncAccess a(RecursionLock, __FUNCTION__);
// 	#else
// 	std::lock_guard<std::recursive_mutex> a(RecursionLock);
// 	#endif
// 		//ySyncAccess a(RecursionLock, __FUNCTION__);
// 		size_t servicecount = services.size();
// 		std::list<yService*>::iterator service_iter = services.begin();
// 		for (int i = 0; i < servicecount; i++)
// 		{
// 			TEMP.push_back(*service_iter);
// 			service_iter++;
// 		}
// 	}

// 	size_t servicecount = TEMP.size();
// 	std::list<yService*>::iterator TEMP_iter = TEMP.begin();
// 	//DEBUG_LOG("count %d", servicecount);
// 	for (int i = 0; i < servicecount; i++)
// 	{
// 		//DEBUG_LOG("%d %s", i, (*TEMP_iter)->getObjectName().c_str());
// 		(*TEMP_iter)->ModuleCallback(msg);
// 		TEMP_iter++;
// 	}
// 	//DEBUG_LOG("done");

// 	//////////////////////////////����ǰ////////////////////////
// 	//��������������//ySyncAccess a(RecursionLock, __FUNCTION__);
// 	/*msg.Sender = this;
// 	size_t servicecount = services.size();
// 	std::list<yService*>::iterator service_iter = services.begin();
// 	for (int i = 0; i < servicecount; i++)
// 	{
		
// 		(*service_iter)->ModuleCallback(msg);
		
// 		service_iter++;
// 	}*/

// }

yModule::RESULT yModule::doSlot(_tsptr<yCMD> cmd)
{
	if (cmd && !cmd->method.empty())
	{
		ERROR_LOG("doSlot not supported %s %s", getInstanceName().c_str(), cmd->method.c_str());
	}
	else
	{
		ERROR_LOG("doSlot not supported %s ", getInstanceName().c_str());
	}
	
	return FAILED;
};
void  yModule::addOwner(_tsptr<yModule> m)
{
	//zxz������Ӧ�ü��߳���
	//std::list< _twptr<yModule>>::iterator iter_owner = m_owners.begin();
	for (auto iter_owner = m_owners.begin(); iter_owner != m_owners.end(); )
	{
		_tsptr<yModule> owner;
		if (!iter_owner->expired())
		{
			owner = (iter_owner->lock());
			if (owner->getInstanceName() == m->getInstanceName())//ȥ��
			{
				//yLogD("%s erasing %s",getInstanceName().c_str(), owner->getInstanceName().c_str());
				iter_owner = m_owners.erase(iter_owner);
				//yLogD("erased");
			}
			else
			{
				iter_owner++;
			}
		}
		else
		{
			//yLogD("erasing 2");
			iter_owner = m_owners.erase(iter_owner);//û�õ���ָ�������������֪��list������erase�᲻�������Ҫ����ʵ�鿴һ��
			//yLogD("erased 2");
		}

	}
	_twptr<yModule> wptr = m;
	//std::set<_twptr<yModule>>test;
	//test.insert(wptr);
	m_owners.push_back(wptr);//list����ȥ�أ�set����insert weak_ptr
}

yModule::RESULT  yModule::emitSignal(_tstring signalName)
{
	_tcmd(cmd);//�Ժ���Կ��Ǵ�һ��������ظ����ã����Ч��
	return emitSignal(signalName, cmd);
}

//先注释掉，后面看addModule要不要放到yModule里边来
// void yModule::sequenceAdd(_tsptr<yModule> m)
// {
// 	bool count = 0;
// 	RESULT  result = NO_RETURN;
// 	std::list< _twptr<yModule>>::iterator iter_owner = m_owners.begin();
// 	for (; iter_owner != m_owners.end(); )
// 	{
// 		_tsptr<yModule> owner;
// 		if (!iter_owner->expired())
// 		{
// 			owner = (iter_owner->lock());
// 			if (owner->getInterface("ySequence"))
// 			{
// 				_tdynamic_pointer_cast<ySequence>(owner)->addModule(m);
// 			}
// 			iter_owner++;
// 		}
// 		else
// 		{
// 			yLogD("erasing");
// 			iter_owner = m_owners.erase(iter_owner);//û�õ���ָ�������������֪��list������erase�᲻�������Ҫ����ʵ�鿴һ��
// 			yLogD("erased");
// 		}
// 	}
// }

// void yModule::sequenceRemove(_tstring instancename)
// {
// 	bool count = 0;
// 	RESULT  result = NO_RETURN;
// 	std::list< _twptr<yModule>>::iterator iter_owner = m_owners.begin();
// 	for (; iter_owner != m_owners.end(); )
// 	{
// 		_tsptr<yModule> owner;
// 		if (!iter_owner->expired() )
// 		{
// 			owner = (iter_owner->lock());
// 			if (owner->getInterface("ySequence"))
// 			{
// 				_tdynamic_pointer_cast<ySequence>(owner)->removeModule(instancename);
// 			}
// 			iter_owner++;
// 		}
// 		else
// 		{
// 			yLogD("erasing");
// 			iter_owner = m_owners.erase(iter_owner);//û�õ���ָ�������������֪��list������erase�᲻�������Ҫ����ʵ�鿴һ��
// 			yLogD("erased");
// 		}
// 	}
// }


yModule::RESULT  yModule::emitSignal(_tstring signalName, _tsptr<yCMD> cmd)
{
	int count = 0;
	RESULT  result = NO_RETURN;
	std::list< _twptr<yModule>>::iterator iter_owner = m_owners.begin();
	for (; iter_owner != m_owners.end(); )
	{
		_tsptr<yModule> owner;
		if (!iter_owner->expired())
		{
			owner = (iter_owner->lock());
			cmd->method = signalName;
			cmd->caller = getInstanceName();
			RESULT current_result = owner->doSlot(cmd);	
			if (current_result < result)//����ж������ֵ �����ȼ�ɸѡ
			{
				result = current_result;
			}	
			count++;
			iter_owner++;
		}
		else
		{
			yLogD("erasing");
			iter_owner = m_owners.erase(iter_owner);//û�õ���ָ�������������֪��list������erase�᲻�������Ҫ����ʵ�鿴һ��
			yLogD("erased");
		}
	}
	
	if (count>0)
	{
		return	result;
	}
	else
	{
		yLogE("no owner for %s ", getInstanceName().c_str());
		if (cmd&&cmd->callback)
		{
			_tsptr<yValue> response(new yValue);
			(*response)["errorDiscripton"] = "no owner for " + getInstanceName();
			cmd->callback(yCMD::FAILED, response);
		}

		return yModule::FAILED;
	}
	


}

// ySequence::ySequence()
// {
// 	mState = "__defaultState__";
	
// }
// ySequence::~ySequence()
// {
// 	if (working)
// 	{
// 		stop();
// 	}
// }



// bool ySequence::isDeviceReady()
// {
// 	for (auto iter:  modules)
// 	{
// 		if (iter->isDeviceReady() == false)
// 		{
// 			yLogD("%s not ready",iter->getInstanceName().c_str());
// 			return false;
// 		}
// 	}
// 	return true;
// }


// _tstring ySequence::getState()
// {
// 	return mState;
// }

// void  ySequence::setState(_tstring st)
// {
// 	mState = st;
// }

// void ySequence::addModule(_tsptr<yModule> m)
// {
// 	#ifdef MSVC
// 	ySyncAccess a(RecursionLock, __FUNCTION__);
// 	#else
// 	std::lock_guard<std::recursive_mutex> a(RecursionLock);
// 	#endif
// 	modules.insert(m);
// 	_tsptr<yModule> sp_this = shared_from_this();
// 	m->addOwner(sp_this);

// 	//ySyncAccess a(RecursionLock, __FUNCTION__);

// 	//if (m)
// 	//{
// 	//	if (modules.find(m->getInstanceName()) != modules.end())
// 	//	{
// 	//	
// 	//	}
// 	//	modules[m->getInstanceName()] = m;
// 	//	_tsptr<yModule> sp_this = shared_from_this();
// 	//	m->addOwner(sp_this);
// 	//}
// }





// bool ySequence::removeModule(_tstring instancename)
// {
// 	_tsptr<yModule> temp = getModule(instancename);
// 	if (temp)
// 	{
// 		#ifdef MSVC
// 		ySyncAccess a(RecursionLock, __FUNCTION__);
// 		#else
// 		std::lock_guard<std::recursive_mutex> a(RecursionLock);
// 		#endif
	
// 		modules.erase(temp);
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}

// 	/*ySyncAccess a(RecursionLock, __FUNCTION__);
// 	std::map<_tstring, _tsptr<yModule>>::iterator iter = modules.find(name);
// 	if (iter != modules.end())
// 	{
// 		modules.erase(iter);
// 		return true;
// 	}
// 	else
// 	{
// 		WARN_LOG("module not found :%s", name.c_str());
// 		return false;
// 	}*/
// }


// _tsptr<yModule> ySequence::getModule(_tstring instanceName)
// {
// 	#ifdef MSVC
// 	ySyncAccess a(RecursionLock, __FUNCTION__);
// 	#else
// 	std::lock_guard<std::recursive_mutex> a(RecursionLock);
// 	#endif

// 	for (auto iter : modules)
// 	{
// 		if (iter->getInstanceName() == instanceName)
// 		{
// 			return iter;
// 		}
// 	}
// 	return NULL;

// 	/*ySyncAccess a(RecursionLock, __FUNCTION__);
// 	std::map<_tstring, _tsptr<yModule>>::iterator iter = modules.find(name);
// 	if (iter != modules.end())
// 	{
// 		return iter->second;
// 	}
// 	else
// 	{
// 		return NULL;
// 	}*/
// }

// void ySequence::addInitObject(_tstring objectName, _tstring instanceName)
// {
// 	yValue object;
// 	object["objectName"] = objectName;
// 	object["instanceName"] = instanceName;

// 	objectTable->pushBack(object);
// }


// void ySequence::addInitCMD(_tstring state, _tstring stateNext, _tstring eventCaller, _tstring eventTarget, eventType etype, _tstring signalName, _tstring slotName, int logLevel)
// {
// 	yValue event;
// 	event["state"] = state;
// 	event["stateNext"] = stateNext;
// 	event["eventCaller"] = eventCaller;
// 	event["eventTarget"] = eventTarget;
// 	event["eventType"] = etype;
// 	event["signalName"] = signalName;
// 	event["slotName"] = slotName;
// 	event["logLevel"] = logLevel;
// 	eventTable->pushBack(event);
// }


// void ySequence::run()
// {
// 	while (working)
// 	{
// 		Sleep(100);
		
// 		bool redo = true;
// 		while (redo)
// 		{
// 			_tsptr<MSG_LATER> later;
// 			{
// 				#ifdef MSVC
// 				ySyncAccess a(m_signalLock, __FUNCTION__);
// 				#else
// 				std::lock_guard<std::recursive_mutex> a(m_signalLock);
// 				#endif

			
// 				if (msg_queue.empty())
// 				{
// 					redo = false;
// 					break;
// 				}
// 				else if (busy == false)
// 				{
// 					later = msg_queue.front();
// 					msg_queue.pop();
// 				//	DEBUG_LOG("pop %s ", later->cmd->method.c_str());
// 					redo = (msg_queue.size() != 0);
// 				}
// 				else
// 				{
// 					break;
// 				}
// 			}
// 			//doSlotǰ����һ˲�䣬�п���busy�ᱻ�����̵߳�doSlotռ��

// 			executeMsg(later);


// 			//_tsptr<yCMD> cmd;
// 			//{
// 			//	ySyncAccess a(m_signalLock, __FUNCTION__);
// 			//	if (cmd_queue.empty())
// 			//	{
// 			//		redo = false;
// 			//		break;
// 			//	}
// 			//	else if (busy == false)
// 			//	{
// 			//		cmd = cmd_queue.front();
// 			//		cmd_queue.pop();
// 			//		redo = (cmd_queue.size() != 0);
// 			//	}
// 			//	else
// 			//	{
// 			//		break;
// 			//	}
// 			//}
// 			////doSlotǰ����һ˲�䣬�п���busy�ᱻ�����̵߳�doSlotռ��
		
// 			//executeCmd(cmd);
			
			
// 		}

// 		//timer
// 		{

// 		}
		
		
// 	}
// }

// yModule::RESULT ySequence::doSlot(_tsptr<yCMD> cmd)
// {
// 	if (m_Enable == false)
// 	{
// 		//yLogD("not enabled");
// 		return NO_RETURN;
// 	}

// 	if (cmd == NULL)
// 	{
// 		yLogE("no cmd");
// 		return yModule::PARAM_ERROR;
// 	}
	
// 	if (cmd->method.empty())
// 	{
// 		yLogE("no method");
// 		return yModule::PARAM_ERROR;
// 	}

// 	return executeCmd(cmd);
// }
// yModule::RESULT  ySequence::executeMsg(_tsptr<MSG_LATER> later)
// {
// 	#ifdef MSVC
// 	ySyncAccess a(m_signalLock, __FUNCTION__);
// 	#else
// 	std::lock_guard<std::recursive_mutex> a(m_signalLock);
// 	#endif
// 	busy = true;


// 	if (later->logLevel == -1)
// 	{

// 	}
// 	else
// 	{
// 		yLogD("CALLING MESSAGE : slot[%s]  state:%s target:%s ", later->cmd->method.c_str(), mState.c_str(), later->object->getInstanceName().c_str(), getInstanceName().c_str());
// 	}
// 	later->object->doSlot(later->cmd);
	
// 	busy = false;

// 	return  yModule::NO_RETURN;
// }



// yModule::RESULT ySequence::executeCmd(_tsptr<yCMD> cmd)
// {
// 	#ifdef MSVC
// 	ySyncAccess a(m_signalLock, __FUNCTION__);
// 	#else
// 	std::lock_guard<std::recursive_mutex> a(m_signalLock);
// 	#endif

// 	//ͬ���¼���ͬ��target���ܻ��������ص����¶��results���nextStates
// 	std::list<yModule::RESULT> results;
// 	std::list<_tstring> nextStates;

// 	_tstring signalName = cmd->method;
// 	std::vector<event> temp_eventArray = m_eventArray;//��ֹ��ߵݹ��doSlot������m_eventArray��Ҫ�Ȱ�m_eventArray���Ƴ���
// 	if (temp_eventArray.size() == 0)
// 	{
// 		yLogD("m_eventArray empty state %s", mState.c_str());
// 	}
// 	std::vector<event>::iterator iter = temp_eventArray.begin();
// 	while (iter != temp_eventArray.end())
// 	{
// 		if (/*iter->state == mState && */iter->signalName == signalName)
// 		{
// 			_tsptr<yModule> object = getModule(iter->eventTarget);
// 			if (object == NULL)
// 			{
// 				//��ʱ�Ȳ�������ˣ���Ϊû�к����������������±�������
// 				//_tsptr<yModule> temp = ySingletone::get()->getModule(iter->eventTarget);
// 				//if (temp)
// 				//{
// 				//	yLogD("found %s in global", iter->eventTarget.c_str());
// 				//	object = temp;
// 				//	//addModule(object);//add��������remove��,���综�߹������棬��Ҫ��̬��add��remove
// 				//}
// 				//else
// 				if (iter->eventTarget == "__noObject__")
// 				{

// 				}
// 				else
// 				{
// 					yLogE("object not loaded :%s", iter->eventTarget.c_str());
// 					iter++;
// 					continue;
// 				}
// 			}
// 		/*	std::map<_tstring, _tsptr<yModule>>::iterator target = modules.find(iter->eventTarget);;
// 			_tsptr<yModule> object;
// 			if (target == modules.end())
// 			{
// 				_tsptr<yModule> temp = ySingletone::get()->getModule(iter->eventTarget);
// 				if (temp)
// 				{
// 					yLogD("found %s in global", iter->eventTarget.c_str());
// 					object = temp;
// 					addModule(object);
// 				}
// 				else
// 				{
// 					yLogE("object not loaded :%s", iter->eventTarget.c_str());
// 					iter++;
// 					continue;
// 				}
			
// 			}
// 			else
// 			{
// 				object = target->second;
// 			}*/
			
// 			_tstring slotName = iter->slotName;
// 			int logLevel = iter->logLevel;
// 			yModule::RESULT result;
// 			_tstring stateNext;
// 			if (cmd->caller.empty())
// 			{
// 				yLogW("no caller, guess to be %s", iter->eventCaller.c_str());
// 				cmd->caller = iter->eventCaller;
// 			}
// 			else if (iter->eventCaller != cmd->caller)
// 			{
// 				//yLogD("caller not match %s - %s ", iter->eventCaller.c_str(), cmd->caller.c_str());
// 				iter++;
// 				continue;
// 			}

// 			if (object && !slotName.empty())
// 			{
// 				if (iter->eventType == eReturn)//����busy��д�������ܷ��ص���NO_RETURN������Ľ�һ��
// 				{
// 					cmd->method = slotName;//ת��method��
// 					result = object->doSlot(cmd);
// 				}
// 				else if (iter->eventType == eMessage)//����ֻ��һ��eventType
// 				{
// 					//if (busy == true)//ySyncAccessͬһ���߳̿��Եݹ��ظ�����������Ҫ��busy
// 					int apple;
// 					{
// 						//ySyncAccess a(m_signalLock, __FUNCTION__);
// 						//cmd_queue.push(cmd);
// 						_tsptr<MSG_LATER> later(new MSG_LATER);
// 						//cmd->method = slotName;//ת��method��
// 						later->object = object;

// 						_tsptr<yCMD> latercmd(new yCMD);//һ��cmd����۵�ʱ����ֱ�Ӹ�cmd��method������������,���sequence����executeCmd��ʱ��Ҳ���ܸ�method
// 						cloneYCMD(cmd, latercmd);
// 						latercmd->method = slotName;
// 						later->logLevel = logLevel;
// 						later->cmd = latercmd;
// 						//later->slotName = slotName;//һ��cmd����۵�ʱ����ֱ�Ӹ�cmd��method������������
// 						msg_queue.push(later);
// 						if (working == false)
// 						{
// 							start();//������һ���߳�
// 						}
// 						result =   yModule::SUCCESS;//��Ϊ���ȼ���STATE_ERROR�ߣ�����msg��Ϣʹ��succes
// 						apple = msg_queue.size();
// 						//DEBUG_LOG("push %s  %d", slotName.c_str(),apple);
// 					}
// 					int banana = msg_queue.size();

					
// 				}
// 				else if (iter->eventType == eBlockCallback)//MACRO_BLOCK_BEGIN���ܻ���������۲�
// 				{
// 					MACRO_BLOCK_BEGIN;	//callbackThreadID��=-1�Ļ����ڻص���ص��������������cmd���ͷ��ˣ������ٽ���������
// 					block_cmd->caller = iter->eventCaller;//Ӧ����caller�Լ����õģ���ʱ�ȴ������ļ���
// 					block_cmd->request = cmd->request;
// 					block_cmd->method = slotName;
// 				//	block_cmd->callbackThreadID = -1;//��object�Լ����߳���߻ص�
// 					object->doSlot(block_cmd);
// 					MACRO_BLOCK_END;

// 					if (cbresult == yCMD::SUCCESS)
// 					{

// 						result = yModule::SUCCESS;
// 					}
// 					else if (cbresult == yCMD::TIMEOUT)
// 					{
// 						yLogE("TIME OUT : event [%s] ",block_cmd->method.c_str());//�ȴ�5��
// 						result = yModule::FAILED;
// 					}
// 					else
// 					{
// 						result = yModule::FAILED;
// 					}
// 					//DEBUG_LOG("MACRO_BLOCK_END");
// 					if (cmd->callback)
// 					{
// 						cmd->callback(cbresult, cb_response);
// 					}
					

// 				}
// 				else if (iter->eventType == eNonBlockCallback)
// 				{
// 					yLogE("wrong eventType %d", iter->eventType);
// 					result = yModule::PARAM_ERROR;
// 				}
// 				else
// 				{
// 					yLogE("wrong eventType %d", iter->eventType);
// 					result = yModule::PARAM_ERROR;
// 				}

// 				stateNext = iter->stateNext;
// 			}
// 			else if (iter->eventTarget == "__noObject__")
// 			{
// 				result = yModule::SUCCESS;
// 				stateNext = iter->stateNext;
// 			}
// 			else
// 			{
// 				yLogE("target not found %s", iter->eventTarget);
// 				result = yModule::PARAM_ERROR;
// 			}



// 			results.push_back(result);

// 			if (result == yModule::STATE_ERROR)
// 			{
// 				yLogW("EVENT_STATE_ERROR : signal[%s] slot[%s]  state[%s]", iter->signalName.c_str(),slotName.c_str(), mState.c_str());
// 			}
// 			else if (result == yModule::FAILED)
// 			{
// 				yLogW("EVENT_FAILED :signal[%s] slot[%s]  state[%s]", iter->signalName.c_str(), slotName.c_str(), mState.c_str());
// 			}
// 			else  if (result == yModule::SUCCESS)
// 			{
// 				if (logLevel == -1)
// 				{

// 				}
// 				else
// 				{
// 					yLogD("EVENT_SUCCESS :signal[%s] slot[%s]  state[%s] target[%s] ", iter->signalName.c_str(), slotName.c_str(), mState.c_str(), iter->eventTarget.c_str(), getInstanceName().c_str());
// 				}
// 				if (stateNext != mState && stateNext != "__anyState__")
// 				{
// 					nextStates.push_back(stateNext);
// 				}
// 				else
// 				{
// 					//
// 				}
// 			}

// 			else if (result == yModule::PARAM_ERROR)
// 			{
// 				yLogW("EVENT_PARAM_ERROR : signal[%s] slot[%s]  state[%s] ", iter->signalName.c_str(), slotName.c_str(), mState.c_str());
// 			}
// 			else
// 			{
// 				yLogE("EVENT_NO_RESULT :signal[%s] slot[%s]  state[%s]", iter->signalName.c_str(), slotName.c_str(), mState.c_str());
// 			}
// 		}
// 		iter++;
// 	}
	

// 	yModule::RESULT finalResult = finalizeCmd(cmd, signalName,results, nextStates);



// 	return finalResult;
// }


// yModule::RESULT ySequence::finalizeCmd(_tsptr<yCMD> cmd, _tstring signalName, std::list<yModule::RESULT>& results, std::list<_tstring>& nextStates)
// {
// 	//_tstring eventName = signalName;/ cmd->method是slotName所以不用cmd->method
// 	//多个返回值，取yModule::FAILED
// 	yModule::RESULT finalResult = yModule::NO_RETURN;
// 	if (results.size() == 0)
// 	{
// 		finalResult = yModule::STATE_ERROR;
// 		yLogD("signal or target not found,no result for [%s] in state[%s],caller[%s]", signalName.c_str(), mState.c_str(), cmd->caller.c_str());
// 	/*	if (cmd->callback)
// 		{
// 			_tsptr <yValue> response(new yValue);
// 			_tstring  errorDiscription = "StateTableError: event: " + signalName + " not supported for state:" + mState;
// 			(*response)["errorDiscription"] = errorDiscription;
// 			cmd->callback(yCMD::FAILED, response);
// 		}*/
// 	}
// 	else
// 	{


// 		finalResult = *results.begin();
// 		for (auto iter : results)//如果有多个返回值 按优先级筛选
// 		{
// 			if (finalResult != iter)
// 			{
// 				for (auto logiter : results)
// 				{
// 					yLogE("result confusing : signal:%s  result:%d ", signalName.c_str(), logiter);
// 					if (iter < finalResult)
// 					{
// 						finalResult = iter;
// 					}
// 				}
// 				break;
// 			}
// 		}
// 	}

// 	if (finalResult == yModule::SUCCESS)
// 	{
// 		_tstring finalState;
// 		if (nextStates.size() == 0)
// 		{
// 			finalState = mState;//没有状态切换
// 		}
// 		else if (nextStates.size() == 1)
// 		{
// 			finalState = *nextStates.begin();
// 		}
// 		else
// 		{
// 			//多个下一个状态，配置的不对才会，取第一个状态，并报错
// 			finalState = *nextStates.begin();
	
// 			for (auto nextstate : nextStates)
// 			{
// 				if (finalState != nextstate)
// 				{
// 					for (auto logiter : nextStates)
// 					{
// 						yLogE("result confusing : signal[%s]  nextState[%s] ", signalName.c_str(), logiter.c_str());
// 					}
// 					//多个下一个状态，配置的不对才会，不改变状态，并报错
// 					finalState = mState;
// 					break;
// 				}
// 			}
			

// 		}
// 		if (finalState != mState)
// 		{
// 			onLeaveState(mState);
// 			yLogD("STATE_CHANGED : [%s] switch state from [%s] to [%s]", signalName.c_str(), mState.c_str(), finalState.c_str());
// 			changeState(finalState);
// 			//mState = finalState;
// 			onEnterState(mState);
// 		}
// 	}

	

	
// 	return finalResult;
// }

// void ySequence::changeState(_tstring state)
// {
// 	mState = state;
// 	std::map<_tstring, std::vector<event>>::iterator map_iter = m_state_event_map.find(mState);
// 	if (map_iter == m_state_event_map.end())
// 	{
// 		yLogE(" no event in state [%s]", mState.c_str());
// 		//return NO_RETURN;
// 		m_eventArray.clear();
// 	}
// 	else
// 	{
// 		m_eventArray = map_iter->second;
// 	}
	
// 	std::map<_tstring, std::vector<event>>::iterator any_iter = m_state_event_map.find("__anyState__");
// 	if (any_iter == m_state_event_map.end())
// 	{
// 		yLogD("no event for __anyState__");
// 	}
// 	else
// 	{
// 		std::vector<event>  m_any_eventArray = any_iter->second;
// 		m_eventArray.insert(m_eventArray.end(), m_any_eventArray.begin(), m_any_eventArray.end());
// 		/*for (auto iter : m_eventArray)
// 		{
// 			DEBUG_LOG("%s %s %s", iter.state.c_str(), iter.signalName.c_str(), iter.eventCaller.c_str());
// 		}*/

// 	}
	
// }

// bool ySequence::clearConfig()
// {
	
// 	if (checkThreadStatus())
// 	{
// 		DEBUG_LOG("waiting");
// 		stop();//ֹͣ�߳�
// 		DEBUG_LOG("done");//��ȷ���᲻�Ῠ�����Ӹ���־
// 	}
// 	{
// 		#ifdef MSVC
// 		ySyncAccess a(m_signalLock, __FUNCTION__);
// 		#else
// 		std::lock_guard<std::recursive_mutex> a(m_signalLock);
// 		#endif

// 		/*while (cmd_queue.size())
// 		{
// 			DEBUG_LOG("poping signal");
// 			cmd_queue.pop();
// 		}*/
// 		while (msg_queue.size())
// 		{
// 			WARN_LOG("poping signal");
// 			msg_queue.pop();
// 		}

// 	}
	
// 	mState = "__defaultState__";
// 	m_eventArray.clear();
// 	m_state_event_map.clear();
// 	eventNames.clear();
// 	//modules.clear();//可能在clear之前就add了，比如MainGUI对象
// 	return true;
// }

// void ySequence::clearModule()
// {
// 	modules.clear();//可能在clear之前就add了，比如MainGUI对象
// }

// void ySequence::setEnable(bool enable)
// {
// 	m_Enable = enable;
// 	yLogD("setEnable %d", m_Enable);
// }


// bool ySequence::startConfig(_tstring configName)
// {
// 	_tsptr<BaseConfiger> configer = ySingletone::get()->getConfiger();
// 	_tstring plugindir = "";
// 	if (configName.substr(1, 2) == ":/")//����·�����ǲ��
// 	{
// 		plugindir = get_directory_path(configName.c_str()) + "/bin/x64/";
// 		configer = _tsptr<JConfiger>(new JConfiger);
// 		configer->loadFile(configName);
// 	}
// 	else
// 	{
// 		configer = configer->getChildConfiger(configName);
// 	}
	
	

// 	clearConfig();

// 	int loglevel = (*configer)["description"]["loglevel"].asInt();
// 	if (loglevel > -1)
// 	{
// 		setLog(true, (DEBUG_LEVEL)loglevel);
// 	}
	

// 	pluginTable = _tsptr< yValue>(new  yValue);
// 	*pluginTable = (*configer)["pluginTable"];
// 	eventTable = _tsptr< yValue>(new  yValue);
// 	*eventTable = (*configer)["eventTable"];
// 	objectTable = _tsptr< yValue>(new  yValue);
// 	*objectTable = (*configer)["objectTable"];

// 	//����dll���
// 	yValue::ConfigArray plugins;
// 	pluginTable->asArray(plugins);
// 	if (ySingletone::get())
// 	{
// 		for (auto dllname : plugins)
// 		{
// 			_tstring plugin = plugindir + dllname.asString();
// 			ySingletone::get()->pushPlugin(plugin);		
// 		}
// 	}
// 	else
// 	{
// 		yLogE("no ySingletone");
// 	}

	
	
// 	if (eventTable == NULL || objectTable == NULL)
// 	{
// 		yLogE("no eventTable or objectTable");
// 		return false;
// 	}
// 	//�����¼���
// 	yValue::ConfigArray eventArray;
// 	eventTable->asArray(eventArray);
// 	for (auto iter : eventArray)
// 	{
// 		event instance;
// 		instance.state = iter["state"].asString();
// 		instance.stateNext = iter["stateNext"].asString();
// 		instance.eventCaller = iter["eventCaller"].asString();
// 		instance.eventTarget = iter["eventTarget"].asString();
// 		instance.eventType = (eventType)iter["eventType"].asInt();
// 		instance.signalName = iter["signalName"].asString();
// 		instance.slotName = iter["slotName"].asString();
// 		instance.logLevel = iter["logLevel"].asInt();
// 		m_eventArray.push_back(instance);
// 		m_state_event_map[instance.state].push_back(instance);
// 		eventNames.insert(instance.signalName);
// 	}

// 	//���ض���
// 	yValue::ConfigArray objectArray;
// 	objectTable->asArray(objectArray);
// 	for (auto iter : objectArray)
// 	{
		
// 		_tstring  objectName = iter["objectName"].asString();
// 		_tstring  instanceName = iter["instanceName"].asString();

// 		if(ySingletone::get())
// 		{
// 			_tsptr<yModule> object = ySingletone::get()->getModule(instanceName);
// 			if (object == NULL&& objectName !="__userItem__")
// 			{
// 				//UTF8_DEBUG_LOG("creating %s %s ", objectName.c_str(), instanceName.c_str());
// 				object = ySingletone::get()->createModule(objectName, instanceName);
// 				//UTF8_DEBUG_LOG("created %s %s ", objectName.c_str(), instanceName.c_str());

// 			}
// 			if (object)
// 			{
// 				//object->setInstanceName(instanceName.c_str());
// 				addModule(object);
// 			}
// 			else
// 			{
// 				if (objectName != "__userItem__")
// 				{
// 					yLogE("create object failed %s", objectName.c_str());
// 				}
				
// 			}
// 		}
// 		else
// 		{
// 			yLogE("no ySingletone");
// 		}
// 	}
// 	changeState("__defaultState__");
// 	return true;
// }

// bool ySequence::init()
// {		
// 	return true;
// }
















// //////////////////////////////////

