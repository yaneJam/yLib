#include "stdafx.h"


//#define useCryptoPP
#ifdef useCryptoPP

#include "cryptopp_8_6_0/cryptlib.h"
#include"cryptopp_8_6_0/osrng.h"
#include "cryptopp_8_6_0/base64.h"
#include"cryptopp_8_6_0/files.h"
#include"cryptopp_8_6_0/aes.h"
#include"cryptopp_8_6_0/modes.h"
#include"cryptopp_8_6_0/filters.h"
#include"cryptopp_8_6_0/hex.h"

using namespace CryptoPP;
#ifdef _DEBUG
#if _WIN64
#pragma comment(lib,"cryptopp_8_6_0/x64/Debug/cryptlib.lib" )
#else			
#pragma comment(lib,"cryptopp_8_6_0/Win32/Debug/cryptlib.lib" )
#endif
#else
#if _WIN64
#pragma comment(lib,"cryptopp_8_6_0/x64/Release/cryptlib.lib" )
#else
#pragma comment(lib,"cryptopp_8_6_0/Win32/Release/cryptlib.lib" )
#endif
#endif

using CryptoPP::Base64Encoder;
using CryptoPP::Base64Decoder;
using CryptoPP::Exception;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;
using CryptoPP::AES;
using CryptoPP::CBC_Mode;
using namespace std;


// ʹ��AES(CBCģʽ)���ܣ�����base64���������
string encrytByAES(const string& plain, const string& key, const string& iv) {
	string cipher;
	try
	{
		CBC_Mode< AES >::Encryption e;
		e.SetKeyWithIV((byte*)key.c_str(), key.size(), (byte*)iv.c_str());

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(plain, true,
			new StreamTransformationFilter(e,
				new StringSink(cipher)
			) // StreamTransformationFilter
		); // StringSource
	}
	catch (const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
	}

	// Pretty print
	string encoded;
	StringSource(cipher, true,
		new Base64Encoder(
			new StringSink(encoded)
		) // HexEncoder
	); // StringSource
	return encoded;
}

// ʹ��AES(CBCģʽ)���ܣ�encodeΪbase64���������
string decrytByAES(const string& encode, const string& key, const string& iv) {
	string encodeByte;
	StringSource(encode, true, new Base64Decoder(
		new StringSink(encodeByte)
	));

	string recovered;
	try
	{
		CBC_Mode< AES >::Decryption d;
		d.SetKeyWithIV((byte*)key.c_str(), key.size(), (byte*)iv.c_str());

		// The StreamTransformationFilter removes
		//  padding as required.
		StringSource s(encodeByte, true,
			new StreamTransformationFilter(d,
				new StringSink(recovered)
			) // StreamTransformationFilter
		); // StringSource
	}
	catch (const CryptoPP::Exception& e)
	{
		cerr << e.what() << endl;
	}

	return recovered;
}


int test_aes_main()
{
	cout << "----------------" << endl
		<< "Start AES test:" << endl;

	//byte key[AES::DEFAULT_KEYLENGTH] = {0,0,0,0,0,0,0,0 ,0,0,0,0 ,0,0,0,0 };
	string key = "key 16byte";
	//byte iv[AES::BLOCKSIZE] = { 0,0,0,0,0,0,0,0 ,0,0,0,0 ,0,0,0,0 };;
	string iv = "iv 16byte";
	string plain = "ABCEDDAAAVVV";

	/*********************************\
	\*********************************/
	cout << "��Կkey: " << key << endl;
	cout << "��ʼ����iv: " << iv << endl;
	/*********************************\
	\*********************************/

	cout << "����plain text: " << plain << endl;
	string encoded = encrytByAES(plain, key, iv);
	cout << "����cifer text: " << encoded << endl;
	string recovered = decrytByAES(encoded, key, iv);
	cout << "����recover text: " << recovered << endl;
	return 0;
}

#endif



#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
//---------------------------------------------------------------------------
#define ENC_FILE_MAX_SIZE 104857600
#define DEC_FILE_MAX_SIZE 104857604





//namespace HD
//{
	#ifdef MSVC
	HDRecursionLock yConfiger::g_ThreadLock = HDRecursionLock();//�᲻���ڴ�й©��
	#else
	std::recursive_mutex yConfiger::g_ThreadLock;// = std::recursive_mutex();
	//std::unique_lock<std::mutex>*  yConfiger::pRecursionLock =new  std::unique_lock<std::mutex> (mtx);
	#endif

	

	yConfiger::yConfiger()
	{
	}
	yConfiger::~yConfiger()
	{
	}

	bool checkUTF8State(const unsigned char *pBuf, int len)
	{
		if (((len > 3) && ((pBuf[0] == 0xEF) && (pBuf[1] == 0xBB) && (pBuf[2] == 0xBF))))
			return true;
		return false;
	}
	_tstring yConfiger::decodeString(_tstring content)
	{
		_tstring reuslt;
		int len = content.size();
		unsigned char *pMemory = new unsigned char[len+1];

		if (pMemory != NULL)
		{
			memcpy(pMemory, content.c_str(), len);
			pMemory[len] = '\0';

			if ((len > 4) && ((pMemory[0] == '!') && (pMemory[1] == 'H') && (pMemory[2] == 'D') && (pMemory[3] == 'F')))
			{
				for (int i = 4; i < len; i++)
				{
					*(pMemory + i) ^= 96;
				}
				if (checkUTF8State(pMemory + 4, len - 4)) //0xEF 0xBB 0xBF
					reuslt = (char *)(pMemory + 7);
				else
					reuslt = (char *)(pMemory + 4);
			}
			else
			{
				if (checkUTF8State(pMemory, len)) //0xEF 0xBB 0xBF
					reuslt = (char *)pMemory + 3;
				else
					reuslt = (char *)pMemory;
			}
			delete[] pMemory;
		}
		return reuslt;
	}

	_tstring yConfiger::encodeString(_tstring content)
	{
#ifdef useCryptoPP
		string key = "key 16byte";
		//byte iv[AES::BLOCKSIZE];
		string iv = "iv 16byte";

		iv.resize(16);
		key.resize(16);

		DEBUG_LOG("org: %s", content.c_str());
		string encoded = encrytByAES(content, key, iv);
		DEBUG_LOG("encoded: %s", encoded.c_str());
		string recovered = decrytByAES(encoded, key, iv);
		DEBUG_LOG("recovered: %s", recovered.c_str());

#endif
		_tstring result;
		int len = content.size();
		const unsigned char* pMemory = (const unsigned char*)content.data();
		/*if (len <= 4)
		{
			ERROR_LOG("length error");
			return content;
		}*/
		if (len > ENC_FILE_MAX_SIZE) //100MB����
		{
			ERROR_LOG("length too long ");
			return content;
		}
		if ((len > 4) && ((pMemory[0] == '!') && (pMemory[1] == 'H') && (pMemory[2] == 'D') && (pMemory[3] == 'F')))
		{
			return content;
		}
		result += "!HDF";
		for (int i = 0; i<len; i++) //���Ҽ���
		{
			unsigned char temp = (*(pMemory + i) ^ 96);
			//unsigned char temp = (*(pMemory + i) );
			result += temp;
		}
		return result;
	}
	//bool yConfiger::encodeFile(_tstring filePath)
	//{
	//	FILE *fp;
	//	fp = fopen(filePath.c_str(), _T("rb"));
	//	if (fp)
	//	{
	//		fseek(fp, 0, SEEK_END);
	//		int len = ftell(fp);
	//		if ((len == 0) || (len>ENC_FILE_MAX_SIZE)) //100MB����
	//		{
	//			fclose(fp);
	//			return false;
	//		}
	//		unsigned char *pMemory = new unsigned char[len + 1];
	//		if (pMemory != NULL)
	//		{
	//			rewind(fp);
	//			fread(pMemory, 1, len, fp);
	//			pMemory[len] = '\0';
	//			fclose(fp);
	//			if ((len > 4) && ((pMemory[0] == '!') && (pMemory[1] == 'H') && (pMemory[2] == 'D') && (pMemory[3] == 'F')))
	//			{
	//				return true;
	//			}
	//			for (int i = 0; i < len; i++) //���Ҽ���
	//			{
	//				*(pMemory + i) ^= 96;
	//			}
	//			fp = fopen(filePath.c_str(), _T("rb"));
	//			if (fp)
	//			{
	//				char head[10] = { "!HDF" };
	//				fwrite(head, 1, 4, fp); //д�����ͷ��
	//				fwrite(pMemory, 1, len, fp);
	//				fclose(fp);
	//			}
	//			delete[] pMemory;
	//		}
	//		else
	//		{
	//			fclose(fp);
	//			return false;
	//		}
	//		return true;
	//	}
	//	return false;
	//}

	//bool yConfiger::decodeFile(_tstring filePath)
	//{
	//	FILE *fp;
	//	fp = fopen(filePath.c_str(), _T("rb"));
	//	if (fp)
	//	{
	//		fseek(fp, 0, SEEK_END);
	//		int len = ftell(fp);
	//		if ((len == 0) || (len > DEC_FILE_MAX_SIZE)) //100MB����
	//		{
	//			fclose(fp);
	//			return false;
	//		}
	//		unsigned char *pMemory = new unsigned char[len + 1];
	//		if (pMemory != NULL)
	//		{
	//			rewind(fp);
	//			fread(pMemory, 1, len, fp);
	//			pMemory[len] = '\0';
	//			fclose(fp);
	//			if ((len > 4) && ((pMemory[0] == '!') && (pMemory[1] == 'H') && (pMemory[2] == 'D') && (pMemory[3] == 'F')))
	//			{
	//				for (int i = 4; i < len; i++) //����
	//				{
	//					*(pMemory + i) ^= 96;
	//				}
	//				fp = fopen(filePath.c_str(), _T("rb"));
	//				if (fp)
	//				{
	//					fwrite(pMemory + 4, 1, len - 4, fp);
	//					fclose(fp);
	//				}
	//			}
	//			delete[] pMemory;
	//		}
	//		else
	//		{
	//			fclose(fp);
	//			return false;
	//		}
	//		return true;
	//	}
	//	return false;
	//}
	
	_tstring yConfiger::getFilePath()
	{
		return configFilePath;
	}
	_tstring yConfiger::getContent(_tstring filePath)
	{
		_tstring content = "";
		FILE *fp;
		fp = fopen(filePath.c_str(), ("rb"));
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			int len = ftell(fp);
			if ((len == 0) || (len > DEC_FILE_MAX_SIZE)) //100MB����
			{
				WARN_LOG("size empty %s", filePath.c_str());
				fclose(fp);
				return content;
			}
			unsigned char *pMemory = new unsigned char[len + 1];
			if (pMemory != NULL)
			{
				rewind(fp);
				fread(pMemory, 1, len, fp);
				pMemory[len] = '\0';
				fclose(fp);
				_tstring filecontent = (char *)pMemory;
				content = decodeString(filecontent);
			}
			else
			{
				fclose(fp);
			}
			delete[] pMemory;
			//unsigned char *pMemory = new unsigned char[len + 1];
			//if (pMemory != NULL)
			//{
			//	rewind(fp);
			//	fread(pMemory, 1, len, fp);
			//	pMemory[len] = '\0';
			//	fclose(fp);

			//	
			//	if ((len > 4) && ((pMemory[0] == '!') && (pMemory[1] == 'H') && (pMemory[2] == 'D') && (pMemory[3] == 'F')))
			//	{
			//		for (int i = 4; i < len; i++)
			//		{
			//			*(pMemory + i) ^= 96;
			//		}
			//		if (checkUTF8State(pMemory + 4, len - 4)) //0xEF 0xBB 0xBF
			//			content = (char *)(pMemory + 7);
			//		else
			//			content = (char *)(pMemory + 4);
			//	}
			//	else
			//	{
			//		if (checkUTF8State(pMemory, len)) //0xEF 0xBB 0xBF
			//			content = (char *)pMemory + 3;
			//		else
			//			content = (char *)pMemory;
			//	}
			//	delete[] pMemory;
			//}
			//else
			//{
			//	fclose(fp);
			//}
		}

		if (content.empty())
		{
			ERROR_LOG("empty, path:%s", filePath.c_str());
		}
		return content;
	}

//}