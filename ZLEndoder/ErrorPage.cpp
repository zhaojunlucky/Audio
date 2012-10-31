// ErrorPage.cpp : implementation file
//

#include "stdafx.h"
#include "ZLEndoder.h"
#include "ErrorPage.h"
#include "afxdialogex.h"


// CErrorPage dialog

IMPLEMENT_DYNAMIC(CErrorPage, CMFCPropertyPage)

CErrorPage::CErrorPage()
	: CMFCPropertyPage(CErrorPage::IDD)
{

}

CErrorPage::~CErrorPage()
{
}

void CErrorPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CErrorPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// CErrorPage message handlers
