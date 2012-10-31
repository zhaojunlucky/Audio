#pragma once


// CEncoderSheet

class CEncoderSheet : public CMFCPropertySheet
{
	DECLARE_DYNAMIC(CEncoderSheet)

public:
	CEncoderSheet();
	virtual ~CEncoderSheet();
	LRESULT OnPropPageNext(int page);
protected:
	DECLARE_MESSAGE_MAP()

	
private:
	CMFCPropertyPage* m_infoPage;
	CMFCPropertyPage* m_fileChoose;
	CMFCPropertyPage* m_parameters;
	CMFCPropertyPage* m_process;
};


