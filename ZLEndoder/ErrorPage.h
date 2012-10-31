#pragma once


// CErrorPage dialog

class CErrorPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CErrorPage)

public:
	CErrorPage();
	virtual ~CErrorPage();

// Dialog Data
	enum { IDD = IDD_ERRORPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
