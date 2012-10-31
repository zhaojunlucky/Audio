#pragma once


// CProcessPage dialog

class CProcessPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CProcessPage)

public:
	CProcessPage();
	virtual ~CProcessPage();

// Dialog Data
	enum { IDD = IDD_PROCESSPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
};
