#pragma once


// CInfomationPage dialog

class CInfomationPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CInfomationPage)

public:
	CInfomationPage();
	virtual ~CInfomationPage();

// Dialog Data
	enum { IDD = IDD_INFOMATIONPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
};
