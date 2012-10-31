#pragma once
#include <string>
#include "CharWCharHelper.h"
#include <exception>

namespace DDString
{
	#ifdef UNICODE 
		#define CZString CZStringW
	#else
		#define CZString CZStringA
	#endif

	class CZStringA
	{
	public:
		__declspec(dllexport) CZStringA(const char* d,size_t length)
		{
			dataAlloc = 0;
			dataLength = 0;
			data = 0;
			SetData(d,length);
		}

		__declspec(dllexport) CZStringA(CZStringA &str)
		{
			data = 0;
			dataAlloc = 0;				
			dataLength = 0;
			if(0 == str.GetDataLength())
			{
				
			}
			else
			{
				SetData(str.GetData(),str.GetDataLength());
			}
		}
		__declspec(dllexport) CZStringA()
		{
			data = new char[10];
			data[0] = '\0';
			dataAlloc = 10;
			dataLength = 0;
		}
		__declspec(dllexport) ~CZStringA()
		{
			if(data)
				delete data;
		}
	public:
		__declspec(dllexport) const char* GetData() const
		{
			if(0 == dataLength)
				return 0;
			return data;
		}
		__declspec(dllexport) operator const char*()
		{
			return GetData();
		}
		__declspec(dllexport) size_t SetData(const CZStringA &str)
		{
			this->SetData(str.GetData(),str.GetDataLength());
			return this->GetDataLength();
		}
		__declspec(dllexport) size_t SetData(const char*d,size_t length)
		{
			char *p = new char[length+8];
			for(size_t i =0; i < length;i++)
				p[i] = d[i];
			p[length] = '\0';

			dataAlloc = length+8;
			dataLength = length;
			if(data)
				delete data;
			data = p;
			return length;
		}

		__declspec(dllexport) size_t SetData(const wchar_t *d,size_t length)
		{
			const wchar_t *ptr;
			wchar_t *tmp=0;
			if(wcslen(d) != length)
			{
				tmp = new wchar_t[length+1];
				for(size_t i = 0;i<length;i++)
					tmp[i] = d[i];
				tmp[length]=L'\0';
				ptr = &tmp[0];
			}
			else
				ptr = &d[0];
			CCharWCharHelper cwh(ptr);
			dataLength = cwh.GetMultiByteLength();
			if(dataLength >= dataAlloc)
			{
				if(data)
					delete data;
				dataAlloc = dataLength+8;
				data = new char[dataAlloc];
			}
			strcpy_s(data,dataAlloc,cwh.GetMultiChar());
			data[dataLength] = '\0';
			return dataLength;
		}

		__declspec(dllexport) size_t GetDataLength() const
		{
			return dataLength;
		}

		__declspec(dllexport) void Empty()
		{
			dataLength = 0;
			data[0] = '\0';
		}

		__declspec(dllexport) size_t IndexOf(char c)
		{
			if(0 == dataLength)
				return -1;
			for(unsigned int i =0 ;i <dataLength;i++)
				if(c == data[i])
					return i;
			return -1;
		}

		__declspec(dllexport)  CZStringA SubString(size_t index)
		{
			if(index >=0 && index < dataLength)
			{
				CZStringA tmp;
				tmp.SetData(&data[index],dataLength-index-1);
				return tmp;
			}
			else
				return CZStringA();

		}
		__declspec(dllexport) CZStringA &operator = (const CZStringA &str)
		{
			if(&str == this)
				return *this;
			if(0 == str.GetDataLength())
				this->Empty();
			else
				this->SetData(str.GetData(),str.GetDataLength());
			return *this;
		}

		__declspec(dllexport)  CZStringA SubString(size_t index,size_t length)
		{
			if(index >=0 && index < dataLength && 0<length && index+length <= dataLength)
			{
				CZStringA tmp;
				tmp.SetData(&data[index],length );
				return tmp;
			}
			else
				return CZStringA();
		}

		__declspec(dllexport) size_t IndexOf(wchar_t c,size_t start)
		{
			if(0 == dataLength || start >= dataLength)
				return -1;
			for(unsigned int i =start ;i <dataLength;i++)
				if(c == data[i])
					return i;
			return -1;
		}

		__declspec(dllexport) size_t LastIndexOf(wchar_t c,size_t start)
		{
			if(0 == dataLength)
				return -1;
			for(unsigned int i =dataLength-1 ;i>=0;i--)
				if(c == data[i])
					return i;
			return -1;
		}

		__declspec(dllexport) size_t LastIndexOf(wchar_t c,size_t start,size_t lastStart)
		{
			if(0 == dataLength || lastStart >=dataLength )
				return -1;
			for(unsigned int i =lastStart ;i>=0;i--)
				if(c == data[i])
					return i;
			return -1;
		}

		__declspec(dllexport) static char ToLowerCase(const char &c)
		{
			if('A' <= c && c <= 'Z')
				return c + 'a'-'A';
			return c;
		}

		__declspec(dllexport) static char ToUpperCase(const char &c)
		{
			if('a' <= c && c <= 'z')
				return c - 'a'+'A';
			return c;
		}

		__declspec(dllexport) CZStringA ToLowerCase()
		{
			CZStringA str;
			if(dataLength > 0)
			{
				char *tData = new char[dataLength+1];

				for(size_t i = 0; i < dataLength; i++)
				{
					tData[i] = ToLowerCase(data[i]);
				}
				tData[dataLength] = L'\0';
				str.SetData(tData,dataLength);
				delete tData;
			}
			return str;
		}

		__declspec(dllexport) CZStringA ToUpperCase()
		{
			CZStringA str;
			if(dataLength > 0)
			{
				wchar_t *tData = new wchar_t[dataLength+1];

				for(size_t i = 0; i < dataLength; i++)
				{
					tData[i] = ToUpperCase(data[i]);
				}
				tData[dataLength] = L'\0';
				str.SetData(tData,dataLength);
				delete tData;
			}
			return str;
		}

		__declspec(dllexport) const char &operator[] (size_t index) const
		{
			if(index >= 0 && index < dataLength)
				return data[index];
			else
				throw std::exception("index out of range!");
		}

		__declspec(dllexport) bool StartWith(const CZStringA &str,bool ignoreCase = true)
		{
			if(dataLength >= str.GetDataLength())
			{
				if(ignoreCase)
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(ToLowerCase(str[i]) != ToLowerCase(data[i]))
							return false;
					}
					return true;
				}
				else
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(str[i] != data[i])
							return false;
					}
					return true;
				}
			}
			return false;
		}

		__declspec(dllexport) bool EndWith(const CZStringA &str,bool ignoreCase = true)
		{
			if(dataLength >= str.GetDataLength())
			{
				size_t len = dataLength - str.GetDataLength();
				if(ignoreCase)
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(ToLowerCase(str[i]) != ToLowerCase(data[i+len]))
							return false;
					}
					return true;
				}
				else
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(str[i] != data[i+len])
							return false;
					}
					return true;
				}
			}
			return false;
		}

		__declspec(dllexport) const CZStringA operator + (const CZStringA &str)
		{
			CZStringA str1;
			char *tData = new char[dataLength + str.GetDataLength() + 1];
			strcpy_s(tData,dataLength,data);
			char *ptr = &tData[dataLength];
			strcpy_s(ptr,str.GetDataLength(),str.GetData());
			tData[dataLength + str.GetDataLength() + 1] = L'\0';
			str1.SetData(tData,dataLength + str.GetDataLength());
			delete tData;
			return str1;
		}


	   __declspec(dllexport) const CZStringA operator + (const char *str)
		{
			CZStringA str1(str,strlen(str));
			char *tData = new char[dataLength + str1.GetDataLength() + 1];
			strcpy_s(tData,dataLength,data);
			char *ptr = &tData[dataLength];
			strcpy_s(ptr,str1.GetDataLength(),str1.GetData());
			tData[dataLength + str1.GetDataLength() + 1] = L'\0';
			str1.SetData(tData,dataLength + str1.GetDataLength());
			delete tData;
			return str1;
		}

	 __declspec(dllexport) bool operator == (const CZStringA &j)
		{
			if(dataLength == j.GetDataLength())
			{
				if(dataLength > 0)
					return ( 0 == strcmp( data , j.GetData() ) );
			} 
			return false;
		}
	private:
		char *data;


		size_t dataAlloc;
		size_t dataLength;
	};

	class CZStringW
	{
	public:
		__declspec(dllexport) CZStringW(const wchar_t* d,size_t length)
		{
			dataAlloc = 0;
			dataLength = 0;
			data = 0;
			SetData(d,length);
		}
		__declspec(dllexport) CZStringW()
		{
			data = new wchar_t[10];
			data[0] = L'\0';
			dataAlloc = 10;
			dataLength = 0;
		}
		__declspec(dllexport) CZStringW(const CZStringW &str)
		{
			data = 0;
			dataAlloc = 0;
				dataLength = 0;
			if(0 == str.GetDataLength())
			{
				
				
			}
			else
			{
				SetData(str.GetData(),str.GetDataLength());
			}
		}
		__declspec(dllexport) ~CZStringW()
		{
			if(data)
				delete data;
		}
	public:
		__declspec(dllexport) const wchar_t* GetData() const
		{
			if(0 == dataLength)
				return 0;
			return data;
		}
		__declspec(dllexport) operator const wchar_t*()
		{
			return GetData();
		}
		__declspec(dllexport) size_t SetData(const wchar_t*d,size_t length)
		{
			wchar_t *p = new wchar_t[length+8];
			for(size_t i =0; i < length;i++)
				p[i] = d[i];
			p[length] = '\0';

			dataAlloc = length+8;
			dataLength = length;
			if(data)
				delete data;
			data = p;
			return length;
		}
		__declspec(dllexport) size_t SetData(const CZStringW &str)
		{
			this->SetData(str.GetData(),str.GetDataLength());
			return this->GetDataLength();
		}
		
		__declspec(dllexport) size_t SetData(const char*d,size_t length)
		{
			const char *ptr;
			char *tmp = 0;
			if(strlen(d) != length)
			{
				tmp = new char[length+1];
				for(size_t i = 0;i<length;i++)
					tmp[i] = d[i];
				tmp[length]='\0';
				ptr = &tmp[0];
			}
			else
				ptr = &d[0];
			CCharWCharHelper cwh(ptr);
			dataLength = cwh.GetWideCharLength();
			if(dataLength >= dataAlloc)
			{
				if(data)
					delete data;
				dataAlloc = dataLength +8;
				data = new wchar_t[dataAlloc];
			}
			wcscpy_s(data,dataAlloc,cwh.GetWideChar());
			data[dataLength] = '\0';


			if(tmp)
				delete tmp;
			return dataLength;
		}


		__declspec(dllexport) size_t GetDataLength() const
		{
			return dataLength;
		}

		__declspec(dllexport) void Empty()
		{
			dataLength = 0;
			data[0] = L'\0';
		}

		__declspec(dllexport) size_t IndexOf(wchar_t c)
		{
			if(0 == dataLength)
				return -1;
			for(unsigned int i =0 ;i <dataLength;i++)
				if(c == data[i])
					return i;
			return -1;
		}
		__declspec(dllexport) CZStringW &operator = (const CZStringW &str)
		{
			if(&str == this)
				return *this;
			if(0 == str.GetDataLength())
				this->Empty();
			else
				this->SetData(str.GetData(),str.GetDataLength());
			return *this;
		}

		__declspec(dllexport)  CZStringW SubString(size_t index)
		{
			if(index >=0 && index < dataLength)
			{
				CZStringW tmp;
				tmp.SetData(&data[index],dataLength - index-1);
				return tmp;
			}
			else
				return CZStringW();

		}

		__declspec(dllexport)  CZStringW SubString(size_t index,size_t length)
		{
			if(index >=0 && index < dataLength && 0<length && index+length <= dataLength)
			{
				CZStringW tmp;
				tmp.SetData(&data[index],length);
				return tmp;
			}
			else
				return CZStringW();
		}

		__declspec(dllexport) size_t IndexOf(wchar_t c,size_t start)
		{
			if(0 == dataLength || start >= dataLength)
				return -1;
			for(unsigned int i =start ;i <dataLength;i++)
				if(c == data[i])
					return i;
			return -1;
		}

		__declspec(dllexport) size_t LastIndexOf(wchar_t c,size_t start)
		{
			if(0 == dataLength)
				return -1;
			for(unsigned int i =dataLength-1 ;i>=0;i--)
				if(c == data[i])
					return i;
			return -1;
		}

		__declspec(dllexport) size_t LastIndexOf(wchar_t c,size_t start,size_t lastStart)
		{
			if(0 == dataLength || lastStart >= dataLength)
				return -1;
			for(unsigned int i = lastStart ;i>=0;i--)
				if(c == data[i])
					return i;
			return -1;
		}

		__declspec(dllexport) static wchar_t ToLowerCase(const wchar_t &c)
		{
			if(L'A' <= c && c<= L'Z')
				return c + 'a'-'A';
			return c;
		}

		__declspec(dllexport) static wchar_t ToUpperCase(const wchar_t &c)
		{
			if(L'a' <= c && c<= L'z')
				return c - 'a'+'A';
			return c;
		}

		__declspec(dllexport) CZStringW ToLowerCase()
		{
			CZStringW str;
			if(dataLength > 0)
			{
				wchar_t *tData = new wchar_t[dataLength+1];

				for(size_t i = 0; i < dataLength; i++)
				{
					tData[i] = ToLowerCase(data[i]);
				}
				tData[dataLength] = L'\0';
				str.SetData(tData,dataLength);
				delete tData;
			}
			return str;
		}

		__declspec(dllexport) CZStringW ToUpperCase()
		{
			CZStringW str;
			if(dataLength > 0)
			{
				wchar_t *tData = new wchar_t[dataLength+1];

				for(size_t i = 0; i < dataLength; i++)
				{
					tData[i] = ToUpperCase(data[i]);
				}
				tData[dataLength] = L'\0';
				str.SetData(tData,dataLength);
				delete tData;
			}
			return str;
		}

		__declspec(dllexport) const wchar_t &operator[] (size_t index) const
		{
			if(index >= 0 && index < dataLength)
				return data[index];
			else
				throw std::exception("index out of range!");
		}

		__declspec(dllexport) bool StartWith(const CZStringW &str,bool ignoreCase = true)
		{
			if(dataLength >= str.GetDataLength())
			{
				if(ignoreCase)
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(ToLowerCase(str[i]) != ToLowerCase(data[i]))
							return false;
					}
					return true;
				}
				else
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(str[i] != data[i])
							return false;
					}
					return true;
				}
			}
			return false;
		}

		__declspec(dllexport) bool EndWith(const CZStringW &str,bool ignoreCase = true)
		{
			if(dataLength >= str.GetDataLength())
			{
				size_t len = dataLength - str.GetDataLength();
				if(ignoreCase)
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(ToLowerCase(str[i]) != ToLowerCase(data[i+len]))
							return false;
					}
					return true;
				}
				else
				{
					for(size_t i = 0;i < str.GetDataLength(); i++)
					{
						if(str[i] != data[i+len])
							return false;
					}
					return true;
				}
			}
			return false;
		}

		__declspec(dllexport)  CZStringW operator + ( CZStringW &str)
		{
			CZStringW str1;
			wchar_t *tData = new wchar_t[dataLength + str.GetDataLength() + 1];
			wcscpy_s(tData,dataLength,data);
			wchar_t *ptr = &tData[dataLength];
			wcscpy_s(ptr,str.GetDataLength(),str.GetData());
			tData[dataLength + str.GetDataLength() + 1] = L'\0';
			str1.SetData(tData,dataLength + str.GetDataLength());
			delete tData;
			return str1;
		}

		__declspec(dllexport)  CZStringW operator + ( wchar_t *str)
		{
			CZStringW str1(str,wcslen(str));
			wchar_t *tData = new wchar_t[dataLength + str1.GetDataLength() + 1];
			wcscpy_s(tData,dataLength,data);
			wchar_t *ptr = &tData[dataLength];
			wcscpy_s(ptr,str1.GetDataLength(),str1.GetData());
			tData[dataLength + str1.GetDataLength() + 1] = L'\0';
			str1.SetData(tData,dataLength + str1.GetDataLength());
			delete tData;
			return str1;
		}

		__declspec(dllexport) bool operator == (const CZStringW &j)
		{
			if(dataLength == j.GetDataLength())
			{
				if(dataLength > 0)
					return ( 0 == wcscmp( data , j.GetData() ) );
			} 
			return false;
		}
	private:
		wchar_t *data;


		size_t dataAlloc;
		size_t dataLength;
	};
}