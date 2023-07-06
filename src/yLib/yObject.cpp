#include "stdafx.h"
#include <map>
//#include <set>
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


long long instance_count = 0;//new������
//std::map<long long, yObject*>  obeject_map;
yObject::yObject()
{

	instance_count++;
	instance_num = instance_count;
	
	//obeject_map[instance_num] = this;
}
yObject::~yObject()
{
	//auto iter = obeject_map.find(instance_num);

	//if (iter == obeject_map.end())
	//{
	//	ERROR_LOG("");//��Ӧ�û����
	//}
	//else
	//{
	//	obeject_map.erase(iter);
	//}
}




_tstring yObject::printSelf()
{
	return  getInstanceName();
}

//_tstring yObject::getInstanceName()
//{
//	return InstanceName;
//}
void yObject::setInstanceName(const _tchar* name)
{
	if (!InstanceName.empty() && InstanceName != name )
	{
		//PAKLOG(IO)("change instanceName from %s to %s", InstanceName.c_str(), name);
	}

	InstanceName = name;
}
const _tstring yObject::getInstanceName()
{
	if (InstanceName.empty())
	{
#ifdef _UNICODE
		InstanceName = getObjectName() + std::to_wstring(m_object_uid);
#else
		InstanceName = getObjectName() + "." +std::to_string(instance_num);
#endif
	}
	return InstanceName;
}




/////////////////////////////////////////////////



// DataObject::DataObject(void)
// {
// 	//need_to_synchronization = false;
// }

// DataObject::~DataObject(void)
// {

// }

// void DataObject::set_name(const char* n)
// {
// 	m_name = std::string(n);
// }

// std::string DataObject::get_name()
// {
// 	return m_name;
// }


// // void DataObject::setPBOModified()
// // {
// // 	need_to_synchronization = true;
// // }
// // bool DataObject::is_need_to_synchronization()
// // {
// // 	return need_to_synchronization;
// // }
