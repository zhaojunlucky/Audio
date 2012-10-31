// ZOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZLPlayer.h"
#include "ZOptionsDlg.h"
#include "afxdialogex.h"
#include "..\ZLControls\ZRegHelper.h"
#include "..\ZLControls\ZElevation.h"
#include "ZSetting.h"
#include "..\Utility\FileHelper.h"
#include "common.h"

// CZOptionsDlg dialog

IMPLEMENT_DYNAMIC(CZOptionsDlg, CDialogEx)

CZOptionsDlg::CZOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZOptionsDlg::IDD, pParent)
{
	
}

CZOptionsDlg::~CZOptionsDlg()
{
	
}

void CZOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO, m_info);
	DDX_Control(pDX, IDC_LIST1, m_file);
	DDX_Control(pDX, IDC_APPLY, m_apply);
	DDX_Control(pDX, IDC_STATIC1, m_mFilePlayL);
	DDX_Control(pDX, IDC_STATIC3, m_cImgChk);
	DDX_Control(pDX, IDC_STATIC5, m_lyricL);
	DDX_Control(pDX, IDC_RICHEDIT21, m_imgPath);
	DDX_Control(pDX, IDC_BUTTON1, mCoverSelect);
	DDX_Control(pDX, IDC_STATICFont, m_fontL);
	DDX_Control(pDX, IDC_RICHEDIT22, m_fontSize);
	DDX_Control(pDX, IDC_STATICtyhty, m_lockCI);
}


BEGIN_MESSAGE_MAP(CZOptionsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_APPLY, &CZOptionsDlg::OnBnClickedApply)
	ON_BN_CLICKED(IDC_CHECK2, &CZOptionsDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_BUTTON1, &CZOptionsDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CZOptionsDlg message handlers


BOOL CZOptionsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CZAeroHelper cz;
	if(cz.IsAeroEnabled())
	{
		cz.EnableAero(this);
	}
	CRect re;
	m_file.GetWindowRect(re);
	mCoverSelect.LoadStdPngImage(IDB_COVERSELECT);
	m_apply.LoadStdPngImage(IDB_APPLY);
	m_file.InsertColumn(0,L"File Type",LVCFMT_LEFT,150);//150
	m_file.InsertColumn(1,L"Description",LVCFMT_LEFT,90);
	
	//////////////////////////////////////////////////////////////////////////
	AddItem(L".mp3",L"MP3 Audio",L"ZLP.MP3");
	AddItem(L".wv",L"Wv Audio",L"ZLP.WV");
	AddItem(L".wav",L"Wav Audio",L"ZLP.WAV");
	AddItem(L".m4a",L"M4A Audio",L"ZLP.M4A");
	AddItem(L".aac",L"Aac Audio",L"ZLP.AAC");
	AddItem(L".tta",L"Tta Audio",L"ZLP.TTA");
	AddItem(L".ogg",L"Ogg Audio",L"ZLP.Ogg");
	AddItem(L".tak",L"Tak Audio",L"ZLP.TAK");
	AddItem(L".ofr",L"Ofr Audio",L"ZLP.OFR");
	AddItem(L".cue",L"Cue Audio",L"ZLP.CUE");
	AddItem(L".ape",L"Ape Audio",L"ZLP.APE");
	AddItem(L".flac",L"Flac Audio",L"ZLP.FLAC");
	m_file.SetColumnWidth(0,150);
	
	
	
	DWORD color = 0;
	BOOL opaque = FALSE;
	cz.GetColorizationColor(&color,&opaque);
	m_imgPath.SetBackgroundColor(FALSE,  color!=0?color: RGB(12,34,23)); 
	m_fontSize.SetBackgroundColor(FALSE,  color!=0?color: RGB(12,34,23)); 
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(0,101,255);
	m_imgPath.SetDefaultCharFormat(cf);
	m_fontSize.SetDefaultCharFormat(cf);


	//////////////////////////////////////////////////////////////////////////
	//m_imgPath.SetWindowTextW(L"TEXT");
	//m_imgPath.
	CZSetting set;
	((CButton*)(GetDlgItem( IDC_CHECK1 )))->SetCheck(set.ReadAddFilePlayMode()? 1:0 ) ; 
	((CButton*)(GetDlgItem( IDC_CHECK3 )))->SetCheck(set.ReadLyricStatus()? 1:0 ) ; 
	((CButton*)(GetDlgItem( IDC_CHECK4 )))->SetCheck(set.ReadLockCoverImage()? 1:0 ) ; 
	CString path ;
	set.ReadCoverImage(path);
	path = path.Trim();
	CFileHelper fileHelper;
	if(!path.IsEmpty() && fileHelper.FileExist(path))
	{
		((CButton*)(GetDlgItem( IDC_CHECK2 )))->SetCheck(1) ; 
		m_imgPath.SetWindowTextW(path);
	}
	else
		OnBnClickedCheck2();
	int fontSize = set.ReadFontSize();
	path.Format(L"%d",fontSize);
	m_fontSize.SetWindowTextW(path);
	//OnBnClickedCheck2();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CZOptionsDlg::AddItem(const CString ext,const CString des,const CString appKey)
{

	int item = m_file.InsertItem(m_file.GetItemCount(),ext,ZUnChecked);
	m_file.SetItemText(item,1,des);
	CZRegHelper reg;
	if(reg.CheckFileRelation(ext,appKey))
	{
		m_file.SetItemState(item,ZChecked);
	}

}


void CZOptionsDlg::OnBnClickedApply()
{
	// TODO: Add your control notification handler code here
	CString fontStr;
	m_fontSize.GetWindowTextW(fontStr);
	int fontSize = 0;
	if( 1 != swscanf(fontStr,L"%d",&fontSize) || fontSize < 20 || fontSize > 42)
	{
		AfxMessageBox(L"The font size must between 20 and 42!");
		m_fontSize.SetFocus();
		return;
	}

	CZSetting set;
	bool addFilePlay = (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK1));
	bool showLyric = (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK3));
	bool cover = (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK2));
	bool lockCover = (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK4));
	CString path;
	m_imgPath.GetWindowTextW(path);
	CFileHelper fh;
	if(cover && fh.FileExist(path))
		set.SaveCoverImage(path);
	else
		set.SaveCoverImage(L"");
	set.SaveAddFilePlayMode(addFilePlay);
	set.SaveLyricStatus(showLyric);
	set.SaveFontSize(fontSize);
	set.SaveLockCoverImage(lockCover);
	::SendMessage(::AfxGetMainWnd()->m_hWnd,WM_UPDATE_SETTING,0,0);
#ifndef _DEBUG
	WCHAR szApplication[MAX_PATH];
    DWORD cchLength = _countof(szApplication);
    QueryFullProcessImageName(GetCurrentProcess(), 0, szApplication, &cchLength);
	CString regist;
	CString unregist;
	CString tmp;
	for(int i = 0;i<m_file.GetItemCount();i++)
	{
	/*	LVITEM lvItem;
		m_list.GetItemText(0,0)*/
		if(m_file.GetItemState(i) == ZChecked)
		{
			tmp = m_file.GetItemText(i,0);
			regist.AppendFormat(L"%s|",tmp.Right(tmp.GetLength()-1));
		}
		else
		{
			tmp = m_file.GetItemText(i,0);
			unregist.AppendFormat(L"%s,",tmp.Right(tmp.GetLength()-1));
		}
	}
	tmp.Format(L"/reg%s%s",regist,unregist);
	CZElevation ze;
	if(ze.StartElevatedProcess(szApplication,tmp))
	{
	}
#endif
	this->EndDialog(0);
}


void CZOptionsDlg::OnBnClickedCheck2()
{
	// TODO: Add your control notification handler code here
	bool cover = (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK2));
	if(cover)
	{
		//m_imgPath.EnableWindow(TRUE);
		m_imgPath.SetWindowTextW(L"Click the button to choose image...");
		((CButton*)(GetDlgItem( IDC_BUTTON1 )))->EnableWindow(TRUE);
	}
	else
	{
		m_imgPath.SetWindowTextW(L"Click the checkbox to enable this option...");
		//m_imgPath.EnableWindow(FALSE);
		((CButton*)(GetDlgItem( IDC_BUTTON1 )))->EnableWindow(FALSE);
	}
}


void CZOptionsDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	CString strFile = _T("");

    CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("PNG (*.png)|*.png|JPG (*.jpg)|*.jpg|JPEG (*.jpeg)|*.jpeg|Bitmap (*.bmp)|*.bmp||"), NULL);

    if (dlgFile.DoModal())
    {
		m_imgPath.SetWindowTextW( dlgFile.GetPathName());
    }
}
