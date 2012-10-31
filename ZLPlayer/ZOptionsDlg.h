#pragma once
#include "..\ZLControls\ZAeroHelper.h"
#include "..\ZLControls\ZLabel.h"
#include "..\ZLControls\ZListCtrl.h"
#include "..\ZLControls\ZMButton.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "..\zlcontrols\zmbutton.h"
#include "..\zlcontrols\zlabel.h"
#include "..\zlcontrols\zlabel.h"

// CZOptionsDlg dialog

class CZOptionsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZOptionsDlg)

public:
	CZOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CZOptionsDlg();

// Dialog Data
	enum { IDD = IDD_ZOPTIONSDLG };


	void AddItem(const CString ext,const CString des,const CString appKey);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CZLabel m_info;
	CZListCtrl m_file;
	virtual BOOL OnInitDialog();
	CZMButton m_apply;
	afx_msg void OnBnClickedApply();
	CZLabel m_mFilePlayL;
	CZLabel m_cImgChk;
	CZLabel m_lyricL;
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedButton1();
	CRichEditCtrl m_imgPath;
	CZMButton mCoverSelect;
	CZLabel m_fontL;
	CRichEditCtrl m_fontSize;
	CZLabel m_lockCI;
};
