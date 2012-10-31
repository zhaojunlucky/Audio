// InfomationPage.cpp : implementation file
//

#include "stdafx.h"
#include "ZLEndoder.h"
#include "InfomationPage.h"
#include "afxdialogex.h"
#include "EncoderSheet.h"

// CInfomationPage dialog

IMPLEMENT_DYNAMIC(CInfomationPage, CMFCPropertyPage)

CInfomationPage::CInfomationPage()
	: CMFCPropertyPage(CInfomationPage::IDD)
{
	 this->m_pPSP->dwFlags |= PSP_USEHEADERTITLE;
}

CInfomationPage::~CInfomationPage()
{
}

void CInfomationPage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInfomationPage, CMFCPropertyPage)
END_MESSAGE_MAP()


// CInfomationPage message handlers


BOOL CInfomationPage::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();
    pSheet->SetWizardButtons(PSWIZB_NEXT);
	return CPropertyPage::OnSetActive();
}


LRESULT CInfomationPage::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class
	//CPropertySheet* pSheet = (CPropertySheet*) GetParent();
	//pSheet->SetWizardButtons(PSWIZB_NEXT);
	CEncoderSheet* pSheet = (CEncoderSheet*) GetParent();
    return pSheet->OnPropPageNext(1);
	//return CMFCPropertyPage::OnWizardNext();
}
