#include "stdafx.h"
#include "yValue.h"
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
//������parent��child�໥֪ͨ

//namespace HD
//{
	//classyValuePrivate{
	//public:
	//	CValuePrivate(yValue* o)
	//	{
	//		ow = o;
	//	};
	//	~CValuePrivate()
	//	{
	//		if (ow->m_parent)
	//		{
	//			ow->m_parent->imp->childDied(ow->m_parent);
	//		}
	//
	//		for (auto iter : m_childs)
	//		{
	//			iter->imp->parentDied(ow->m_parent);
	//		}
	//	};
	//	
	//	yValue* ow;
	//
	//};
	yValue::yValue()
	{
		pRecursionLock = NULL;
		m_parent = NULL;
		arrayIndex = -1;
	}


	yValue::yValue(const yValue& v)
	{
		pRecursionLock = NULL;
		arrayIndex = v.arrayIndex;
		m_parent = v.m_parent;
		if (m_parent)
		{
			m_parent->addChild(this);
		}
		NodeName = v.NodeName;
		json_value = v.asJsonValue();
	}

	yValue::yValue(double num)
	{
		pRecursionLock = NULL;
		m_parent = NULL;
		json_value = num;
		arrayIndex = -1;
	}
	yValue::yValue(bool num)
	{
		pRecursionLock = NULL;
		m_parent = NULL;
		json_value = num;
		arrayIndex = -1;
	}

	yValue::yValue(int num)
	{
		pRecursionLock = NULL;
		m_parent = NULL;
		json_value = num;
		arrayIndex = -1;
	}
	yValue::yValue(std::string str)
	{
		pRecursionLock = NULL;
		m_parent = NULL;
		json_value = str;
		arrayIndex = -1;
	}
	yValue::yValue(const _tchar *cstr)
	{
		pRecursionLock = NULL;
		m_parent = NULL;
		json_value = cstr;
		arrayIndex = -1;
	}
	yValue::yValue(Json::Value v, _tstring nodeName, yValue* parent)
	{
		pRecursionLock = NULL;
		NodeName = nodeName;
		m_parent = parent;
		if (m_parent)
		{
			m_parent->addChild(this);
		}
		arrayIndex = -1;
		json_value = v;
	}

	yValue::yValue(Json::Value v, int index, yValue* parent)
	{
		pRecursionLock = NULL;
		arrayIndex = index;
		m_parent = parent;
		if (m_parent)
		{
			m_parent->addChild(this);
		}
		json_value = v;
	}

	yValue::~yValue()
	{

		if (m_parent)
		{
			m_parent->childDied(this);
		}

		for (auto iter : m_childs)
		{
			iter->parentDied(this);
		}

		if (pRecursionLock)
		{
			delete pRecursionLock;
		}
	}

	void yValue::clear()
	{
		json_value = Json::Value();
	}


	bool yValue::isString()
	{
		return json_value.isString();
	}


	std::string yValue::asString()
	{

		if (!json_value.isString())
		{

			//IO_DEBUG("%s is not a string value", NodeName.c_str());
			return "";
		}

		return json_value.asString();
	}

	bool yValue::isInt()
	{

		return json_value.isInt();
	}
	int yValue::asInt()
	{

		if (!json_value.isInt())
		{
			if (json_value.isDouble())
			{
				//IO_DEBUG("%s is not a int value but a double", NodeName.c_str());
				return (int)(json_value.asDouble());
			}
			IO_DEBUG("%s is not a int value", NodeName.c_str());
			return -1;
		}
		return json_value.asInt();
	}
	bool yValue::isBool()
	{
		return json_value.isBool();
	}
	bool yValue::asBool()
	{
		return json_value.asBool();
	}

	bool yValue::isDouble()
	{
		return json_value.isDouble();
	}
	double yValue::asDouble()
	{

		if (!json_value.isDouble())
		{
			if (json_value.isInt())
			{
				//IO_DEBUG("%s is not a double value but a int", NodeName.c_str());
				return (float)(json_value.asInt());
			}
			IO_DEBUG("%s is not a double value", NodeName.c_str());
			return -1.0;
		}
		return json_value.asDouble();
	}

	bool yValue::hasNode(const std::string &key)
	{
		if (json_value.type() != Json::nullValue && json_value.type() != Json::objectValue)
		{
			IO_DEBUG("%s is not a objectValue", NodeName.c_str());
			return false;
		}
		return json_value.isMember(key);
	}

	std::vector<_tstring> yValue::getAllNodeName()
	{
		if (json_value.type() != Json::nullValue && json_value.type() != Json::objectValue)
		{

			IO_DEBUG("%s is not a objectValue", NodeName.c_str());
			return Json::Value::Members();
		}
		return json_value.getMemberNames();
	}
	yValue yValue::getNode(const std::string &key)
	{
#define Updata20201130 //����ڶ�������at����ȥ��bug
#ifdef Updata20201130
		if (!json_value.isMember(key) )
#else
		if (!json_value.isMember(key) && !NodeName.empty())
#endif
		
		
		{
			//IO_DEBUG("%s is not a member of %s", key.c_str(), NodeName.c_str());
			return yValue(Json::Value(Json::nullValue), key, this);
		}

		return yValue(json_value.get(key, json_value), key, this);
	}

	yValue yValue::appendNode(_tstring key, yValue value)
	{

		if (key.empty())
		{
			IO_DEBUG("empty");
			return yValue(json_value);
		}



		if (json_value.type() != value.asJsonValue().type() && json_value.type() != Json::nullValue && json_value.type() != Json::objectValue)
		{
			ERROR_LOG("%s type already exist,append failed", NodeName.c_str());

			return yValue(json_value, NodeName, m_parent);
		}
		json_value[key] = value.asJsonValue();

		yValue result = yValue(json_value);
		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (m_parent&&arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, *this);
		}
		modifytime++;
		return result;
	}

	yValue yValue::removeNode(const std::string &key)
	{

		if (json_value.isMember(key))
		{
			json_value.removeMember(key);
		}
		else
		{
			IO_DEBUG("%s is not a member,remove failed ", key.c_str());
			return yValue(json_value);
		}


		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (m_parent&&arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, *this);
		}

		modifytime++;
		return yValue(json_value);
	}


	const _tstring yValue::getNodeName()
	{
		return NodeName;
	}
	const yValue* yValue::getParent()
	{
		return m_parent;
	}

	_tstring yValue::asContent(bool style)
	{
		if (style)
		{
			Json::StyledWriter writer;
			std::string data = writer.write(json_value);
			return data;
		}
		else
		{
			Json::FastWriter writer;
			std::string data = writer.write(json_value);
			return data;
		}
		
		
	}

	const Json::Value& yValue::asJsonValue() const
	{
		return json_value;
	}

	yValue& yValue::operator=(bool other)
	{

		json_value = yValue(other).asJsonValue();
		if (m_parent && !NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (m_parent && arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, other);
		}
		return *this;
	}
	
	yValue & yValue::operator=(double other)
	{

		json_value = yValue(other).asJsonValue();
		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (m_parent&&arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, other);
		}
		return *this;
	}
	yValue & yValue::operator=(int other)
	{

		json_value = yValue(other).asJsonValue();
		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (m_parent&&arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, other);
		}
		return *this;
	}

	yValue & yValue::operator = (const _tchar *cstr)
	{

		json_value = yValue(cstr).asJsonValue();
		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (m_parent&&arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, cstr);
		}
		return *this;
	}
	yValue & yValue::operator=(_tstring str)
	{

		json_value = yValue(str).asJsonValue();
		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (m_parent&&arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, str);
		}
		return *this;
	}


	yValue & yValue::operator=(const yValue& other)
	{

		json_value = yValue(other).asJsonValue();
		if (m_parent == NULL)//
		{
			m_parent = other.m_parent;
			if (m_parent)
			{
				m_parent->addChild(this);
			}

			NodeName = other.NodeName;
		}
		else if (!NodeName.empty())//����Լ��Ǵ�һ��configer���get�����ģ�parent�����Լ��ģ����Լ���configer�Լ��Ľڵ��������
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, other);
		}
		return *this;
	}

	void yValue::pushBack(yValue v)
	{
		if (json_value.type() != Json::nullValue && json_value.type() != Json::arrayValue)
		{

			ERROR_LOG("%s is not a arrayValue,append failed", NodeName.c_str());
			return;
		}
		json_value.append(v.asJsonValue());
		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, *this);
		}

	}

	void yValue::setAt(unsigned int index, yValue v)
	{

		if (json_value.type() != Json::nullValue && json_value.type() != Json::arrayValue)
		{
			ERROR_LOG("%s is not a arrayValue,append failed", NodeName.c_str());
			return;
		}
		//int size = json_value.size();
		//Json::Value::iterator iter = json_value.begin();
		//for (int i = 0; i < size; i++)
		//{
		//	Json::Value value = *iter;
		//}
		//Json::Value test1 = json_value[index];
		//Json::Value test2 = v.asJsonValue();

		json_value[index] = v.asJsonValue();
		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		modifytime++;
	}

	bool yValue::isObject()
	{
		if ( json_value.type() != Json::objectValue)
		{
			return false;
		}
		return true;
	}
	bool yValue::isArray()
	{
		if (json_value.type() != Json::nullValue && json_value.type() != Json::arrayValue)
		{
			return false;
		}
		return true;
	}


// class MyClass
// {
// public:
// 	MyClass(){}
// 	MyClass(const MyClass& a) {}
// 	int b;
//     void myFunc(MyClass& obj) { std::cout << "myFunc" << std::endl; }
// };


	bool yValue::removeAt(unsigned int index)
	{
		if (json_value.type() != Json::nullValue && json_value.type() != Json::arrayValue)
		{
			ERROR_LOG("%s is not a arrayValue,get failed", NodeName.c_str());

			return false;
		}

		unsigned int size = json_value.size();
		Json::Value newjson_value;
		if (index < size)
		{
			Json::Value::iterator iter = json_value.begin();

			int count = 0;
			for (unsigned int i = 0; i < size; i++)
			{
				Json::Value value = *iter;
				if (i == index)//index���Ž�ȥ
				{

				}
				else
				{
					newjson_value[count] = *iter;
					count++;
				}
				iter++;
			}
			json_value = newjson_value;
		}
		else
		{
			IO_DEBUG("index:%d bigger than size:%d ", index, size);
			return false;
		}
		//IO_DEBUG("%d ",  json_value.size());

		if (m_parent&&!NodeName.empty())
		{
			m_parent->appendNode(NodeName, *this);
		}
		else if (arrayIndex != -1)
		{
			m_parent->setAt(arrayIndex, *this);
		}
		return true;
	}

	yValue yValue::getAt(unsigned int index)
	{

		if (json_value.type() != Json::nullValue && json_value.type() != Json::arrayValue)
		{
			ERROR_LOG("%s is not a arrayValue,get failed", NodeName.c_str());

			return yValue(json_value, NodeName, m_parent);
		}

		unsigned int size = json_value.size();
		if (index < size)
		{
			Json::Value::iterator iter = json_value.begin();
			for (unsigned int i = 0; i < index; i++)
			{
				Json::Value value = *iter;

				iter++;

			}
			return yValue(*iter, index, this);
		}
		else
		{
			return yValue(Json::Value(), index, this);
		}

	}
	void yValue::asArray(ConfigArray & arrayvalue)
	{
		unsigned int size = json_value.size();
		Json::Value::iterator iter = json_value.begin();
		for (unsigned int i = 0; i < size; i++)
		{
			Json::Value value = *iter;
			arrayvalue.push_back(yValue(value, i, this));

			// yValue temp = yValue(value, i, this);
			// std::vector<yValue> apple;
			// apple.push_back(temp);
			


			// // MyClass obj;
			// // std::vector<MyClass> myVector;
			// // myVector.push_back(obj);
		
			iter++;
		}
	}


	void yValue::lock()
	{

		if (pRecursionLock == NULL)
		{
			#ifdef MSVC
			pRecursionLock = new HDRecursionLock;
			#else
			pRecursionLock = new std::unique_lock<std::recursive_mutex>(mtx);

			#endif


		}
		if (pRecursionLock)
		{
			pRecursionLock->lock();
		}
		if (m_parent)
		{
			m_parent->lock();
		}
	}

	void yValue::unLock()
	{
		if (pRecursionLock)
		{
			#ifdef MSVC
			pRecursionLock->unLock();
			#else
			pRecursionLock->lock();
			#endif
			


		
		}
		if (m_parent)
		{
			//���parent����lock֮�󴫽����ģ���ʱparent��û��lock�����ܻ���������
			m_parent->unLock();
		}

	}

	void yValue::childDied(yValue* child)
	{

		auto iter = m_childs.find(child);
		if (iter == m_childs.end())
		{
			ERROR_LOG("no child");//��Ӧ�ý�����
			return;
		}
		m_childs.erase(iter);
	}
	void yValue::parentDied(yValue* parent)
	{

		if (m_parent != parent)
		{
			ERROR_LOG("parent no right");//��Ӧ�ý�����
			return;
		}
		m_parent = NULL;
	}
	void yValue::addChild(yValue* child)
	{
		m_childs.insert(child);
	}

	long yValue::getModifyTime()
	{
		return modifytime;
	}
	void yValue::setModifyTime(long modify)
	{
		modifytime = modify;
	}


//}
