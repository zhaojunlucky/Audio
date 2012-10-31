#pragma once
#include <exception>
#include <stdio.h>
using namespace std;

namespace ZStringHelper{
class CZStringHelper
{
public:
	__declspec(dllexport) CZStringHelper(void){};
	__declspec(dllexport) ~CZStringHelper(void){};

	// calculate the size of the ptr
	__declspec(dllexport)  unsigned int static GetPtrSize(const wchar_t *ptr) 
	{
		if(ptr)
			return wcslen(ptr);
		return 0;
	}

	__declspec(dllexport) unsigned int static IndexOf(const wchar_t *ptr,const wchar_t c)
	{
		if(!ptr)
			return -1;
		unsigned int ptrSize = GetPtrSize(ptr);
		for(unsigned int i = 0; i <= ptrSize;i++)
			if(c == ptr[i])
				return i;
		return -1;
	}

	__declspec(dllexport) unsigned int static LastIndexOf(const wchar_t *ptr,const wchar_t c)
	{
		if(!ptr)
			return -1;
		unsigned int ptrSize = GetPtrSize(ptr);
		unsigned int i = ptrSize ;
		while(i >=0)
		{
			if(ptr[i] == c)
				return i;
			i--;
		}
		return -1;
	}

	__declspec(dllexport) unsigned int static SubString(const wchar_t *ptr,wchar_t *des,unsigned int start)
	{
		if(!des )
			return 0;
		unsigned int ptrSize = GetPtrSize(ptr)-1;
		if(start >= ptrSize)
			return 0;
		unsigned int copySize = ptrSize - start + 1;

		try
		{
			for(unsigned int i = 0; i < copySize ; i ++)
			{
				des[i] = ptr[i + start];
			}
		}
		//catch (CMemoryException* e)
		//{
		//	
		//}
		//catch (CFileException* e)
		//{
		//}
		catch (exception* e)
		{
			printf("%s\n",e->what());
			copySize = 0;
		}
		
		return copySize;
	}

	__declspec(dllexport) unsigned int static SubString(const wchar_t *ptr,wchar_t *des,unsigned int start,unsigned int end)
	{
		if(!des )
			return 0;
		unsigned int ptrSize = GetPtrSize(ptr);
		if(start >= ptrSize)
			return 0;
		unsigned int copySize = end - start + 1;
		if(copySize  > ptrSize - start + 1)
			return 0;

		try
		{
			for(unsigned int i = 0; i < copySize ; i ++)
			{
				des[i] = ptr[i + start];
			}
		}
		//catch (CMemoryException* e)
		//{
		//	
		//}
		//catch (CFileException* e)
		//{
		//}
		catch (exception* e)
		{
			printf("%s\n",e->what());
			copySize = 0;
		}

		return copySize;
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

	__declspec(dllexport) static void ToLowerCase(wchar_t *ptr,size_t len)
	{
		for(unsigned int i = 0; i < len; i++)
			ptr[i] = ToLowerCase(ptr[i]);
	}

	__declspec(dllexport) static void ToUpperCase(wchar_t *ptr,size_t len)
	{
		for(unsigned int i = 0; i < len; i++)
			ptr[i] = ToUpperCase(ptr[i]);
	}


	__declspec(dllexport) bool static EndWith(const wchar_t *ptr,const wchar_t *compared,bool ignoreCase = true)
	{
		unsigned int ptrSize = GetPtrSize(ptr);
		unsigned int comSize = GetPtrSize(compared);
		if(0 == ptrSize || 0 == comSize || ptrSize < comSize)
			return false;

		unsigned int iCom = comSize - 1;
		unsigned int iPtr = ptrSize - 1;

		if(ignoreCase)
		{
			wchar_t a ;//= ptr[iPtr];
			wchar_t b ;//= compared[iCom];
			while(comSize > 0)
			{
				a = ToLowerCase(ptr[iPtr]);
				b = ToLowerCase(compared[iCom]);

				if(a != b)
					return false;
				--comSize;
				--iPtr;
				--iCom;
			}
		}
		else
		{
			while(comSize > 0)
			{
				if(ptr[iPtr] != compared[iCom])
					return false;
				--comSize;
				--iPtr;
				--iCom;
			}
		}
		return true;
	}

	__declspec(dllexport) bool static StartWith(const wchar_t *ptr,const wchar_t *compared,bool ignoreCase = true)
	{
		unsigned int ptrSize = GetPtrSize(ptr);
		unsigned int comSize = GetPtrSize(compared);
		if(0 == ptrSize || 0 == comSize || ptrSize < comSize)
			return false;

		unsigned int i = 0;
		if(ignoreCase)
		{
			wchar_t a;
			wchar_t b;
			while(comSize > 0)
			{
				a = ToLowerCase(ptr[i]);
				b = ToLowerCase(compared[i]);
				if(a != b)
					return false;
				i++;
				comSize--;
			}
		}
		else
		{
			while(comSize > 0)
			{
				if(ptr[i] != compared[i])
					return false;
				i++;
				comSize--;
			}
		}
		
		return true;
	}
};

}