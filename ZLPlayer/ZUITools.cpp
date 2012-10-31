#include "StdAfx.h"
#include "ZUITools.h"


CZUITools::CZUITools(void)
{
}


CZUITools::~CZUITools(void)
{
}

void CZUITools::AddMediaItem(CZPlayListCtrl *pList,const Media *m)
{
	assert(0 != pList && 0 != m);
	pList->InsertItem(pList->GetItemCount(),m->title,(int)m->mediaFormat,m);
}