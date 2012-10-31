#include "StdAfx.h"
#include "ZPlayList.h"
//
//

// index from 0
CZPlayList::CZPlayList(void)
{
}


CZPlayList::~CZPlayList(void)
{

}

void CZPlayList::AddItem(Media item)
{
	list.push_back(item);
}

void CZPlayList::InsertItem(int index,Media item)
{
	int listSize = list.size();
	if(index > listSize + 1)
	{
		index = listSize;
	}
	list.insert(list.begin()+index,item);
}

void CZPlayList::RemoveAt( int index)
{
	assert(index >=0 && index < list.size());
	list.erase(list.begin()+index);
}

void CZPlayList::RemoveAll()
{
	list.clear();
}

Media CZPlayList::GetItem(int index)
{
	assert(index >=0 && index < list.size());
	return list[index];
}

int CZPlayList::GetSize()
{
	return list.size();
}

void CZPlayList::RemoveAt(int start ,int end)
{
	assert(start > end && end >=0 && start < list.size());
	for(int i = end ; i >= start; i++)
	{
		RemoveAt(i);
	}
}

int CZPlayList::Contains(const Media &m)
{
	for(unsigned int i = 0; i < list.size(); i++)
	{
		Media tmpM = list[i];
	
		if(tmpM.isCue && m.isCue)
		{
			if(tmpM.isSingleFile && tmpM.isSingleFile)
			{
				if(tmpM.fileName == m.fileName)
					return i;
			}
			else if(!tmpM.isSingleFile && !tmpM.isSingleFile)
			{
				if(tmpM.fileName == m.fileName)
				{
					if(tmpM.start == m.start && tmpM.end == m.end)
						return i;
				}
			}
		}
		else if(!tmpM.isCue && !m.isCue)
		{
			if(tmpM.fileName == m.fileName)
				return i;
		}
		else if(m.fileName == tmpM.fileName)
			return i;
	}
	return -1;
}

 bool CZPlayList::Update(unsigned int index,const Media &m)
 {
	 if(index < 0 || index >= list.size())
		 return false;
	 list[index] = m;
	 return true;
 }