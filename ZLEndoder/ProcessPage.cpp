// ProcessPage.cpp : implementation file
//

#include "stdafx.h"
#include "ZLEndoder.h"
#include "ProcessPage.h"
#include "afxdialogex.h"


// CProcessPage dialog

IMPLEMENT_DYNAMIC(CProcessPage, CMFCPropertyPage)

CProcessPage::CProcessPage()
	: CMFCPropertyPage(CProcessPage::IDD)
{

}

CProcessPage::~CProcessPage()
{
}

void CProcessPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProcessPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// CProcessPage message handlers


BOOL CProcessPage::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	((CPropertySheet*) GetParent())->SetWizardButtons(PSWIZB_BACK);
	return CMFCPropertyPage::OnSetActive();
}
