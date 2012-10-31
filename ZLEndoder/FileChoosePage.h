#pragma once
#include "afxcmn.h"


// CFileChoosePage dialog

class CFileChoosePage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CFileChoosePage)

public:
	CFileChoosePage();
	virtual ~CFileChoosePage();

// Dialog Data
	enum { IDD = IDD_FILECHOOSEPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual LRESULT OnWizardBack();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_fileList;
	afx_msg void OnBnClickedAddfiles();
};
