// FileChoosePage.cpp : implementation file
//

#include "stdafx.h"
#include "ZLEndoder.h"
#include "FileChoosePage.h"
#include "afxdialogex.h"


// CFileChoosePage dialog

IMPLEMENT_DYNAMIC(CFileChoosePage, CMFCPropertyPage)

CFileChoosePage::CFileChoosePage()
	: CMFCPropertyPage(CFileChoosePage::IDD)
{

}

CFileChoosePage::~CFileChoosePage()
{
}

void CFileChoosePage::DoDataExchange(CDataExchange* pDX)
{
	CMFCPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_fileList);
}


BEGIN_MESSAGE_MAP(CFileChoosePage, CMFCPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CFileChoosePage::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_ADDFILES, &CFileChoosePage::OnBnClickedAddfiles)
END_MESSAGE_MAP()


// CFileChoosePage message handlers


BOOL CFileChoosePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  Add extra initialization here
	((CButton*)GetDlgItem(IDC_FLACF))->SetCheck(TRUE);

	m_fileList.InsertColumn(0,L"Title",0,200);
	m_fileList.InsertColumn(1,L"Format",0,50);
	m_fileList.InsertColumn(2,L"Size",0,100);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CFileChoosePage::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* pSheet = (CPropertySheet*) GetParent();
    //pSheet->SetWizardButtons(PSWIZB_BACK);
	return CMFCPropertyPage::OnSetActive();
}


LRESULT CFileChoosePage::OnWizardNext()
{
	// TODO: Add your specialized code here and/or call the base class

	return CMFCPropertyPage::OnWizardNext();
}


LRESULT CFileChoosePage::OnWizardBack()
{
	// TODO: Add your specialized code here and/or call the base class

	return CMFCPropertyPage::OnWizardBack();
}


void CFileChoosePage::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(m_fileList.GetItemCount() > 0)
		((CPropertySheet*) GetParent())->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else
		((CPropertySheet*) GetParent())->SetWizardButtons(PSWIZB_BACK);
	*pResult = 0;
}



void CFileChoosePage::OnBnClickedAddfiles()
{
	// TODO: Add your control notification handler code here
	CString fileName;
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT,L"All Supported Media Files|*.flac;*.ape;*.tta;*.ogg;*.m4a;*.tak;*.ofr;*.cue;*.wv;*.mp3;*.wav;*.aac|");
	if(IDOK == dlg.DoModal())
	{
	}
}
