// EncoderSheet.cpp : implementation file
//

#include "stdafx.h"
#include "ZLEndoder.h"
#include "EncoderSheet.h"
#include "InfomationPage.h"
#include "FileChoosePage.h"
#include "ParametersPage.h"
#include "ProcessPage.h"

// CEncoderSheet

IMPLEMENT_DYNAMIC(CEncoderSheet, CMFCPropertySheet)

CEncoderSheet::CEncoderSheet()
{
	m_psh.dwFlags |= PSH_AEROWIZARD | PSH_USEICONID;
    //m_psh.pszIcon  = MAKEINTRESOURCE(IDI_MAINICON);
	m_psh.pStartPage = 0;
	m_infoPage = (CInfomationPage*)new CInfomationPage;
	m_fileChoose = (CFileChoosePage*)new CFileChoosePage;
	m_parameters = (CParametersPage*)new CParametersPage;
	m_process = (CProcessPage*)new CProcessPage;
	AddPage(m_infoPage);
	AddPage(m_fileChoose);
	AddPage(m_parameters);
	AddPage(m_process);
}

CEncoderSheet::~CEncoderSheet()
{
	if(m_infoPage)
		delete m_infoPage;
	if(m_fileChoose)
		delete m_fileChoose;
	if(m_parameters)
		delete m_parameters;
	if(m_process)
		delete m_process;
}


BEGIN_MESSAGE_MAP(CEncoderSheet, CMFCPropertySheet)
END_MESSAGE_MAP()



// CEncoderSheet message handlers

LRESULT CEncoderSheet::OnPropPageNext(int page) 
{
   
	//return CFileChoosePage::IDD;
    return 0;
}
