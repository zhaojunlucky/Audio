// ParametersPage.cpp : implementation file
//

#include "stdafx.h"
#include "ZLEndoder.h"
#include "ParametersPage.h"
#include "afxdialogex.h"


// CParametersPage dialog

IMPLEMENT_DYNAMIC(CParametersPage, CMFCPropertyPage)

CParametersPage::CParametersPage()
	: CMFCPropertyPage(CParametersPage::IDD)
{

}

CParametersPage::~CParametersPage()
{
}

void CParametersPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParametersPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// CParametersPage message handlers
