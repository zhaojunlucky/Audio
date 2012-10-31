// ZFileChooseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZLPlayer.h"
#include "ZFileChooseDlg.h"
#include "afxdialogex.h"


// CZFileChooseDlg dialog

IMPLEMENT_DYNAMIC(CZFileChooseDlg, CDialogEx)

CZFileChooseDlg::CZFileChooseDlg(vector<Media> *m,CWnd* pParent /*=NULL*/)
	: CDialogEx(CZFileChooseDlg::IDD, pParent)
{
	this->m = m;
}

CZFileChooseDlg::~CZFileChooseDlg()
{
}

void CZFileChooseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_file);
	DDX_Control(pDX, IDC_BUTTON1, m_apply);
}


BEGIN_MESSAGE_MAP(CZFileChooseDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CZFileChooseDlg::OnBnClickedButton1)
	ON_COMMAND(ID_CUECHOOSE_UNSELECTEDALL, &CZFileChooseDlg::OnCuechooseUnselectedall)
	ON_COMMAND(ID_CUECHOOSE_INVERTSELECTION, &CZFileChooseDlg::OnCuechooseInvertselection)
	ON_COMMAND(ID_CUECHOOSE_SELECTALL, &CZFileChooseDlg::OnCuechooseSelectall)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CZFileChooseDlg::OnRclickList1)
END_MESSAGE_MAP()


// CZFileChooseDlg message handlers


BOOL CZFileChooseDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CZAeroHelper cz;
	if(cz.IsAeroEnabled())
	{
		cz.EnableAero(this);
	}
	m_apply.LoadStdPngImage(IDB_APPLY);
	CRect re;
	m_file.GetWindowRect(re);
	m_file.InsertColumn(0,L"Cue Files",LVCFMT_LEFT,re.Width());
	assert(NULL != m);
	for(int i = 0; i < m->size() ; i++)
	{
		int item = m_file.InsertItem(m_file.GetItemCount(),((*m)[i]).title,ZChecked);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CZFileChooseDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	for(int i = m_file.GetItemCount() - 1; i >= 0;i--)
		if(ZUnChecked == m_file.GetItemState(i))
			m->erase(m->begin()+i);
	this->EndDialog(0);
}


void CZFileChooseDlg::OnCuechooseUnselectedall()
{
	// TODO: Add your command handler code here
	for(int i = 0; i < m_file.GetItemCount() ; i++)
	{
		m_file.SetItemState(i,ZUnChecked);
	}
}


void CZFileChooseDlg::OnCuechooseInvertselection()
{
	// TODO: Add your command handler code here
	for(int i = 0; i < m_file.GetItemCount() ; i++)
	{
		m_file.SetItemState(i,ZCheckState((m_file.GetItemState(i)+1)%2));
	}
	m_file.Invalidate();
}


void CZFileChooseDlg::OnCuechooseSelectall()
{
	// TODO: Add your command handler code here
	for(int i = 0; i < m_file.GetItemCount() ; i++)
	{
		m_file.SetItemState(i,ZChecked);
	}
	m_file.Invalidate();
}


void CZFileChooseDlg::OnRclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	POSITION pos = m_file.GetFirstSelectedItemPosition();
	if(pos)
	{
		CPoint point;   
		::GetCursorPos(&point); 
		CMenu   pMenu; 
		pMenu.LoadMenu(IDR_PL_MENU); 
		CMenu*  pPopup = pMenu.GetSubMenu(2);		
		pPopup-> TrackPopupMenu(TPM_RIGHTBUTTON,point.x,point.y,this); 
	}	
	m_file.Invalidate();
}
