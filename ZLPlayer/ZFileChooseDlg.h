#pragma once
#include "..\ZLControls\ZAeroHelper.h"
#include "..\ZLControls\ZLabel.h"
#include "..\ZLControls\ZListCtrl.h"
#include "..\ZLControls\ZMButton.h"
#include "..\ZLP\Media.h"
#include <vector>
#include "resource.h"
using namespace std;

// CZFileChooseDlg dialog

class CZFileChooseDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZFileChooseDlg)

public:
	CZFileChooseDlg(vector<Media> *m,CWnd* pParent = NULL);   // standard constructor
	virtual ~CZFileChooseDlg();

// Dialog Data
	enum { IDD = IDD_ZFILECHOOSEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	vector<Media> *m;
public:
	virtual BOOL OnInitDialog();
	CZListCtrl m_file;
	CZMButton m_apply;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCuechooseUnselectedall();
	afx_msg void OnCuechooseInvertselection();
	afx_msg void OnCuechooseSelectall();
	afx_msg void OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult);
};
