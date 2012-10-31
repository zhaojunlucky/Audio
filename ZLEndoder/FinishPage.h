#pragma once


// CFinishPage dialog

class CFinishPage : public CMFCPropertyPage
{
	DECLARE_DYNAMIC(CFinishPage)

public:
	CFinishPage();
	virtual ~CFinishPage();

// Dialog Data
	enum { IDD = IDD_FINISH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
