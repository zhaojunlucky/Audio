#pragma once


// CParametersPage dialog

class CParametersPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CParametersPage)

public:
	CParametersPage();
	virtual ~CParametersPage();

// Dialog Data
	enum { IDD = IDD_PARAMETERSPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
