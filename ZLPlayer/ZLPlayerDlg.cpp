
// ZLPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZLPlayer.h"
#include "ZLPlayerDlg.h"
#include "afxdialogex.h"
#include "Helper.h"
#include "..\ZLControls\ZAeroHelper.h"
#include "..\Utility\DirectoryHelper.h"
#include "common.h"
#include "..\ZLPCore\TagHelper.h"
#include "ZOptionsDlg.h"
#include "..\ZLControls\ZSingleton.h"
#include "ZParseCommandLine.h"
#include "afxcmn.h"
#include "..\Utility\ZFileVersion.h"
#include "ZSetting.h"
#include "..\Utility\FileHelper.h"
#include "..\ZLControls\ZDImageHelper.h"
using namespace DDImage;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_versionInfo;
	virtual BOOL OnInitDialog();
	CZMButton m_ok;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_versionInfo);
	DDX_Control(pDX, IDOK, m_ok);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

LRESULT CAboutDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(message == WM_DWMCOLORIZATIONCOLORCHANGED)
	{
		CZAeroHelper cz;
		m_versionInfo.SetBackgroundColor(FALSE,  wParam); 
		return FALSE;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CZAeroHelper cz;
	if(cz.IsAeroEnabled())
	{
		cz.EnableAero(this);
	}
	m_ok.LoadStdPngImage(IDB_APPLY);
	wchar_t exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	/*int index = wcslen(exeFullPath) - 1;
	while(L'\\' != exeFullPath[index])
		index--;
	exeFullPath[index + 1] = L'\0';*/
	PathRemoveFileSpec(exeFullPath);
	wcscat(exeFullPath,L"\\");
	CString verInfo;
	CString tmp;

	DWORD color = 0;
	BOOL opaque = FALSE;
	cz.GetColorizationColor(&color,&opaque);


	CZFileVersion fileVersion;
	CZVersionInfo zverInfo;
	m_versionInfo.SetBackgroundColor(FALSE,  color!=0?color: RGB(12,34,23)); 
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(0,101,255);
	m_versionInfo.SetDefaultCharFormat(cf);

	verInfo.Append(L"ZLPlayer.exe = ");
	fileVersion.GetVersionInfo(exeFullPath,L"ZLPlayer.exe",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

	verInfo.Append(_T("\r\nZLPCore.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"ZLPCore.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

	verInfo.Append(_T("\r\nZLControls.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"ZLControls.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

	verInfo.Append(_T("\r\nZLP.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"ZLP.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

	verInfo.Append(_T("\r\nUtility.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"Utility.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);
#ifdef _DEBUG
	verInfo.Append(_T("\r\nAudio.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"Audio_d.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

	//verInfo.Append(_T("\r\nZPlayer_d.dll = "));
	//fileVersion.GetVersionInfo(exeFullPath,L"ZPlayer_d.dll",&zverInfo);
	//tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
	//	zverInfo.productSecondRight,zverInfo.productRightMost);
	//verInfo.Append(tmp);

	verInfo.Append(_T("\r\nTagLibrary_d.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"TagLibrary_d.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

#else
	verInfo.Append(_T("\r\nAudio.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"Audio.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

	//verInfo.Append(_T("\r\nZPlayer.dll = "));
	//fileVersion.GetVersionInfo(exeFullPath,L"ZPlayer.dll",&zverInfo);
	//tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
	//	zverInfo.productSecondRight,zverInfo.productRightMost);
	//verInfo.Append(tmp);

	verInfo.Append(_T("\r\nTagLibrary.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"TagLibrary.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);

#endif


	verInfo.Append(_T("\r\nOptimFROG.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"OptimFROG.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);


	verInfo.Append(_T("\r\ntak_deco_lib.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"tak_deco_lib.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);


	verInfo.Append(_T("\r\nwavpackdll.dll = "));
	fileVersion.GetVersionInfo(exeFullPath,L"wavpackdll.dll",&zverInfo);
	tmp.Format(L"%d.%d.%d.%d",zverInfo.productLeftMost,zverInfo.productSecondLeft,
		zverInfo.productSecondRight,zverInfo.productRightMost);
	verInfo.Append(tmp);


	verInfo.Append(L"\r\n\r\nCopyright(C) 2011-2012 of ZhaoJun,All Rights Reserved.");
	m_versionInfo.SetWindowTextW(verInfo);


	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CZLPlayerDlg dialog


UINT g_uTBBC = RegisterWindowMessage(L"TaskbarButtonCreated");

CZLPlayerDlg::CZLPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZLPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	zTaskbar = new CZTaskBarHelper(IDI_TPRED,IDI_TPREN,IDI_TPLAYD,
		IDI_TPLAYN,IDI_TPAUSEN,IDI_TNEXTD,IDI_TNEXTN);

#ifdef _DEBUG
	AllocConsole();
	freopen("CON", "r", stdin );
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
#endif
}

void CZLPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAY_DURATION, m_play_duration);
	DDX_Control(pDX, IDC_VOLUME, m_volume_ctrl);
	DDX_Control(pDX, IDC_LIST2, m_playList_ctrl);
	DDX_Control(pDX, IDC_COVER, m_coverImg);
	DDX_Control(pDX, IDC_PLIST, m_stopBtn);
	DDX_Control(pDX, IDC_PP, m_playPauseBtn);
	DDX_Control(pDX, IDC_TIME, m_timeCtrl);
	DDX_Control(pDX, IDC_MUSICNUM, m_musicNum);
	DDX_Control(pDX, IDC_PREVIOUS, m_previousBtn);
	DDX_Control(pDX, IDC_NEXT, m_nextBtn);
	DDX_Control(pDX, IDC_WAVEINFO, m_waveInfo);
}

BEGIN_MESSAGE_MAP(CZLPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
	ON_REGISTERED_MESSAGE(g_uTBBC,CZLPlayerDlg::OnCreateThumbToolBar)
	//ON_REGISTERED_MESSAGE( g_uTBBC, CZLPlayerDlg::OnCreateThumbToolBar )
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_PREVIOUS, &CZLPlayerDlg::OnBnClickedPrevious)
	ON_BN_CLICKED(IDC_PP, &CZLPlayerDlg::OnBnClickedPp)
	ON_BN_CLICKED(IDC_NEXT, &CZLPlayerDlg::OnBnClickedNext)
	ON_BN_CLICKED(IDC_PLIST, &CZLPlayerDlg::OnBnClickedPlist)
	ON_NOTIFY(NM_DBLCLK, IDC_PLAYLIST, &CZLPlayerDlg::OnNMDblclkPlaylist)
	ON_WM_TIMER()
	ON_NOTIFY(NM_RCLICK, IDC_PLAYLIST, &CZLPlayerDlg::OnRclickPlaylist)
	ON_COMMAND(ID_PLAYLIST_OPENFILELOCATION, &CZLPlayerDlg::OnPlaylistOpenfilelocation)
	ON_COMMAND(ID_PLAYLIST_DELETE, &CZLPlayerDlg::OnPlaylistDelete)
	ON_COMMAND(ID_PLAYLIST_CLEAR, &CZLPlayerDlg::OnPlaylistClear)
	ON_COMMAND(ID_PLAYLIST_PROPERTY, &CZLPlayerDlg::OnPlaylistProperty)
	ON_COMMAND(ID_PLAYMODE_ORDER, &CZLPlayerDlg::OnPlaymodeOrder)
	ON_COMMAND(ID_PLAYMODE_SINGLE, &CZLPlayerDlg::OnPlaymodeSingle)
	ON_COMMAND(ID_PLAYMODE_CIRCLE, &CZLPlayerDlg::OnPlaymodeCircle)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_MAINDLG_OPTIONS, &CZLPlayerDlg::OnMaindlgOptions)
	ON_COMMAND(ID_PLAYMODE_RANDOM, &CZLPlayerDlg::OnPlaymodeRandom)
	ON_WM_COPYDATA()
	ON_WM_CREATE()
	ON_NOTIFY_EX( TTN_NEEDTEXT, 0, SetTipText )
	ON_WM_HOTKEY()
	ON_COMMAND(ID_PLAYLIST_COPYTITLE, &CZLPlayerDlg::OnPlaylistCopytitle)
	ON_COMMAND(ID_MAINDLG_ABOUTZLPLAYER, &CZLPlayerDlg::OnMaindlgAboutzlplayer)
	ON_COMMAND(ID_PLAYLIST_CONVERTTO, &CZLPlayerDlg::OnPlaylistConvertto)
	ON_COMMAND(ID_PLAYLIST_SEARCHLYRIC, &CZLPlayerDlg::OnPlaylistSearchlyric)
	ON_MESSAGE(1204, &CZLPlayerDlg::PlayControlMessage)
END_MESSAGE_MAP()


// CZLPlayerDlg message handlers

BOOL CZLPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CZAeroHelper cz;
	DWORD color = GetSysColor(COLOR_WINDOW);
	if(cz.IsAeroEnabled())
	{
		cz.EnableAero(this);
		BOOL opaque = FALSE;
		
		cz.GetColorizationColor(&color,&opaque);
		
	}
	UpdateBkColor(color);
	//SetWindowTheme(m_playList_ctrl.GetSafeHwnd(), L"explorer", NULL);
	m_stopBtn.LoadPngBitmap(IDB_LISTN,IDB_LISTH,IDB_LISTP,IDB_LISTN);
	m_playPauseBtn.LoadStdPngImage(IDB_PLAY);
	m_playPauseBtn.LoadAtlPngImage(IDB_PAUSE);
	m_previousBtn.LoadStdPngImage(IDB_PREVIOUS);
	m_nextBtn.LoadStdPngImage(IDB_NEXT);
	m_playList_ctrl.SetExtendedStyle( m_playList_ctrl.GetExtendedStyle()| LVS_EX_FULLROWSELECT|LVS_OWNERDRAWFIXED);
	m_imageList.Create(32,32,ILC_MASK|ILC_COLOR32, 1, 1);
	AddFileImage(&m_imageList,L".flac");
	AddFileImage(&m_imageList,L".ape");
	AddFileImage(&m_imageList,L".wav");
	AddFileImage(&m_imageList,L".mp3");
	AddFileImage(&m_imageList,L".ogg");
	AddFileImage(&m_imageList,L".tta");
	AddFileImage(&m_imageList,L".aac");
	AddFileImage(&m_imageList,L".mp4");
	AddFileImage(&m_imageList,L".m4a");
	AddFileImage(&m_imageList,L".ofr");
	AddFileImage(&m_imageList,L".tak");
	AddFileImage(&m_imageList,L".wv");
	m_imageList.Add(m_hIcon);
	m_playList_ctrl.SetImageList(&m_imageList,LVSIL_SMALL);
	m_playList_ctrl.InsertColumn(0,L"Title",LVCFMT_LEFT);
	m_playList_ctrl.SetHeight(50);
	m_volume_ctrl.SetRange(0,10000);
	m_volume_ctrl.SetPos(manager.GetPlayerConfig().volume);
	m_timeCtrl.EnableWindow(FALSE);
	CZSetting set;
	lockCover = set.ReadLockCoverImage();
	CFileHelper fHelper;
	CString bmpFile ;
	set.ReadCoverImage(bmpFile);
	if(fHelper.FileExist(bmpFile))
	{
		CImage imgTemp;
		imgTemp.Load(bmpFile);
		if ( bmp.m_hObject ) 
			bmp.Detach();
		 bmp.Attach(imgTemp.Detach());				
	}
	else 
	{
		//bmp.LoadBitmapW(IDB_COVER);
		ZDImageHelper::ZDLoadImage(bmp,AfxGetResourceHandle(),IDB_COVER,L"PNG");
		/*HRSRC hResource = ::FindResource(0, L"IDB_PLAY", L"PNG");
		DWORD imageSize = ::SizeofResource(0, hResource);
		const void* pResourceData = ::LockResource(::LoadResource(0, hResource));
		HGLOBAL m_hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
		if (m_hBuffer)
		{
			void* pBuffer = ::GlobalLock(m_hBuffer);
			if (pBuffer)
			{
				CopyMemory(pBuffer, pResourceData, imageSize);

				IStream* pStream = NULL;
				if (::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
				{
					CImage imgTemp;
					imgTemp.Load(pStream);
					if ( bmp.m_hObject ) 
						bmp.Detach();
					bmp.Attach(imgTemp.Detach());			
				}
				::GlobalUnlock(m_hBuffer);
			}
		}
		::GlobalFree(m_hBuffer);		*/
	}
	m_coverImg.SetBitmap((HBITMAP)bmp);
	playOnAddFile = set.ReadAddFilePlayMode();
	CZParseCommandLine zc;
	zc.ParseCommand();
	for(int i=0;i< zc.GetCount();i++)
	{
		manager.AddFile(zc.GetAt(i));
	}
	EnableToolTips(TRUE); 
	this->SetFocus();

	lyric = new CZLyricDlg;
	lyric->Create(CZLyricDlg::IDD,GetDesktopWindow());
	lyric->SetZManager(&manager);
	//lyric->ShowWindow(TRUE);
	lyric->EnableLyric(set.ReadLyricStatus());
	::SetForegroundWindow(lyric->m_hWnd);

	m_hotKey = new CHotKeyHelper(this->GetSafeHwnd());
	m_hotKey->Register();
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CZLPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZLPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
		
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CZLPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CZLPlayerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == ZSLIDERMESSAGE)// slider Message
	{
		HWND srcHwnd = (HWND)lParam;
		if(m_volume_ctrl.GetSafeHwnd() == srcHwnd)
		{
			//manager.player->SetVolume(m_volume_ctrl.GetPos());
			manager.SetVolume(m_volume_ctrl.GetPos());
		}
		else if(m_timeCtrl.GetSafeHwnd() == srcHwnd)
		{
			// kill timer
			//printf("%d\n",m_timeCtrl.GetPos());

			manager.player->Seek(m_timeCtrl.GetPos());
		}
		return FALSE;
	}
	else if(message == WM_UPDATE_PLAYLIST)
	{
		Media *m = (Media*)wParam;
		if(0 != m)
		{
			uiTools.AddMediaItem(&m_playList_ctrl,m);
		}
		return FALSE;
	}
	//else if(message == WM_UPDATE_UI)
	//{
	//	// don't use ZPLAY message,it's a default message
	//	CZButtonStatus *bs = (CZButtonStatus*)wParam;
	//	if(0 != bs)
	//	{
	//		if(ZOPEN == bs->mes)
	//		{
	//			lyric->ReadLRC(manager.player->GetAt(manager.GetCurrentPlay()));
	//		}
	//		lyric->PlayControlMessage((ZMessage)bs->mes);
	//		//printf("mes for Lyric:%d\n",bs->mes);
	//		if(ZUSERSTOP == bs->mes || ZSTOP == bs->mes)
	//		{
	//			m_timeCtrl.EnableWindow(FALSE);
	//			// kill timer
	//			KillTimer(3001);
	//			this->SetWindowTextW(L"ZLPlayer");
	//			if(ZUSERSTOP == bs->mes)
	//			{
	//				m_waveInfo.SetWindowTextW(L"");
	//				m_waveInfo.Invalidate();
	//			}
	//		}
	//		else if(ZPLAY == bs->mes)
	//		{
	//			// start timer
	//			
	//			m_timeCtrl.EnableWindow(TRUE);
	//			SetTimer(3001,800,0);
	//			POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	//			while(0 != pos)
	//			{
	//				int  index   = m_playList_ctrl.GetNextSelectedItem(pos);
	//				m_playList_ctrl.SetItemState(index,~LVNI_SELECTED,LVNI_SELECTED);
	//			}
	//			m_playList_ctrl.SetItemState(manager.GetCurrentPlay(),LVNI_SELECTED,LVNI_SELECTED);	
	//			m_playList_ctrl.EnsureVisible(manager.GetCurrentPlay(),TRUE);
	//			this->SetWindowTextW(manager.player->GetAt(manager.GetCurrentPlay()).title);
	//			CString mes;
	//			double b = manager.player->GetWaveInfo().bitrate;
	//			if(b > 0.0 && b < 5000.0)
	//				mes.Format(L"%.1fK",b);
	//			else
	//				mes.Format(L"%.1fKhz",manager.player->GetWaveInfo().nSamplesPerSec / 1000.0);
	//			m_waveInfo.SetWindowTextW(mes);
	//			m_waveInfo.Invalidate();
	//			mes.Empty();
	//			
	//			LoadCoverImage();
	//		}
	//		else if(ZPAUSE == bs->mes)
	//		{
	//			// kill timer
	//			KillTimer(3001);
	//		}
	//		else if(ZRESUM == bs->mes)
	//		{
	//			// start timer
	//			SetTimer(3001,800,0);
	//		}

	//		if(ZOPEN == bs->mes /*|| ZPLAY == bs->mes*/)
	//		{
	//			m_timeCtrl.SetRangeMax(bs->time);
	//			//printf("total time 2:%d\n",bs->time);
	//			m_timeCtrl.EnableWindow(TRUE);
	//			CString mes;
	//			int min = bs->time / 60;
	//			mes.Format(L"00:00/%02d:%02d",min,bs->time-min*60);
	//			m_play_duration.SetWindowTextW(mes);
	//			m_play_duration.Invalidate();
	//			
	//			
	//			mes.Empty();
	//		}
	//		else
	//		{	
	//			if(ZSTOP == bs->mes)
	//			{
	//				manager.PlayerControl(MPlay,-1);
	//			}
	//			zTaskbar->UpdateThumbToolBar(bs->tBarPreious,bs->tBarPlayPause,bs->tBarNext);
	//			m_previousBtn.EnableWindow(bs->previous?TRUE:FALSE);
	//			m_nextBtn.EnableWindow(bs->next?TRUE:FALSE);
	//			if(0 == bs->playPause)
	//				m_playPauseBtn.EnableWindow(FALSE);
	//			else
	//			{
	//				m_playPauseBtn.SetAtlCommand(2==bs->playPause);
	//				m_playPauseBtn.EnableWindow(TRUE);
	//			}
	//		}
	//		CString mes;
	//		mes.Format(L"%d/%d",manager.GetMediaCount()>0?manager.GetCurrentPlay()+1:0,manager.GetMediaCount());
	//		m_musicNum.SetWindowTextW(mes);
	//		m_musicNum.Invalidate();
	//		mes.Empty();
	//		return FALSE;
	//	}		
	//}
	else if(message == WM_DWMCOMPOSITIONCHANGED )
	{
		CZAeroHelper cz;
		DWORD color = GetSysColor(COLOR_WINDOW);
		if(cz.IsAeroEnabled())
		{
			cz.EnableAero(this);
			BOOL opaque = FALSE;
			cz.GetColorizationColor(&color,&opaque);
		}
		else
			cz.DisableAero(this); 
		UpdateBkColor(color);
		return FALSE;
	}
	else if (message == WM_DWMCOLORIZATIONCOLORCHANGED)
	{
		UpdateBkColor(wParam);		
		return FALSE;
	}
	else if(message == WM_UPDATE_SETTING)
	{
		CZSetting set;
		lyric->EnableLyric(set.ReadLyricStatus());
		lyric->ProcessFontSize();
		playOnAddFile = set.ReadAddFilePlayMode();
		CString path;
		set.ReadCoverImage(path);
		lockCover = set.ReadLockCoverImage();
		path = path.Trim();
		if(!path.IsEmpty())
		{
			CImage imgTemp;
			imgTemp.Load(path);
			if ( bmp.m_hObject ) 
				bmp.Detach();
			bmp.Attach(imgTemp.Detach());	
			m_coverImg.SetBitmap((HBITMAP)bmp);
		}
		else
		{
			if ( bmp.m_hObject ) 
				bmp.Detach();
			//bmp.LoadBitmapW(IDB_COVER);
			ZDImageHelper::ZDLoadImage(bmp,AfxGetResourceHandle(),IDB_COVER,L"PNG");
			m_coverImg.SetBitmap((HBITMAP)bmp);
		}
		return FALSE;
	}
	return  CDialogEx::WindowProc(message,   wParam,   lParam);  ;
}

void CZLPlayerDlg::LoadCoverImage()
{
	if(lockCover)
		return;
	Media m = manager.player->GetAt(manager.GetCurrentPlay());
	if(MP3 == m.mediaFormat || AAC == m.mediaFormat ||
		MP4 == m.mediaFormat)
	{
		/*CTagHelper tag;
		tag.Link(m.fileName);*/
		if(manager.player->GetDecoder()->GetID3Info()->id3Bitmap->hbm)
		{
			m_coverImg.SetBitmap(manager.player->GetDecoder()->GetID3Info()->id3Bitmap->hbm);
		}
		else
		{
			m_coverImg.SetBitmap((HBITMAP)bmp);
		}
	}
	else// load cover image
	{
		CString coverPath = m.fileName.Left(m.fileName.ReverseFind('\\')+1);
		CString coverImage;
		coverImage.Format(L"%s%s",coverPath,L"cover.jpg");
		CFileStatus   status ; 
		if(!CFile::GetStatus(coverImage,status)) 
		{
			coverImage.Format(L"%s%s",coverPath,L"cover.png");
			if(!CFile::GetStatus(coverImage,status)) 
			{
				coverImage.Empty();
			}
		}
		if(!coverImage.IsEmpty())
		{
			BSTR bstr = coverImage.AllocSysString();
			m_coverImg.LoadFromFile(coverImage.AllocSysString());
			SysFreeString(bstr);
			coverImage.Empty();
		}
		else
		{
			m_coverImg.SetBitmap((HBITMAP)bmp);
		}
	}
}

void CZLPlayerDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	bool ctrlKey = false;
	if(GetAsyncKeyState(VK_CONTROL)&0x8000)
	{
		manager.PlayerControl(PlayCommand::MStop);
		m_playList_ctrl.DeleteAllItems();
		manager.Clear();
	}

	int DropCount=DragQueryFile(hDropInfo,-1,NULL,0);
	//CStringArray fileList;
	wchar_t *pName = NULL;
	int allocSize = 0;
	for(int i = 0; i < DropCount; i++)
	{
		int NameSize=DragQueryFile(hDropInfo,i,NULL,0);
		if(NameSize >= allocSize)
		{
			if(pName)
				free(pName);
			pName = NULL;
		}
		if(!pName)
		{
			pName=(LPWSTR)malloc( (NameSize+1)*sizeof(pName));
			allocSize = NameSize + 1;
		}

		if (pName==NULL) 
		{

			return;
		}
		DragQueryFile(hDropInfo,i,pName,allocSize/*NameSize+1*/);
		//CString fileName(pName);
		//fileList.Add(fileName);  
		CDirectoryHelper dirHelper;
		if(!dirHelper.IsDirectory(pName))
			manager.AddFile(pName);
		else
			manager.AddDirectory(pName);
	}
	if(pName)
		free(pName);
	DragFinish(hDropInfo); 
	CDialogEx::OnDropFiles(hDropInfo);
}

LRESULT CZLPlayerDlg::OnCreateThumbToolBar( WPARAM, LPARAM )
{	
	//bool r = zTaskbar->AddThumbToolBar();
	
	/*if(r)
		return S_OK;
	return S_FALSE;*/
	return 0;
}


void CZLPlayerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	if(zTaskbar)
		delete zTaskbar;
	if(lyric)
	{
		lyric->EndDialog(0);
		delete lyric;
		lyric = 0;
	}

	PLAYER_STATE ps = manager.GetPlayerState();
	if(!ps.stop)
	{
		KillTimer(3001);
		manager.PlayerControl(MStop);
	}
	CZSetting set;
	set.SaveAddFilePlayMode(playOnAddFile);
	set.SavePlayMode(manager.GetPlayerConfig().pm);
	set.SaveVolume(manager.GetPlayerConfig().volume);
	/*if(clipbuffer)
		GlobalFree(clipbuffer);*/
#ifdef _DEBUG
	FreeConsole();
#else
	CZSingleton zs;
	zs.RemoveDrop();
#endif
	bmp.DeleteObject();
	m_hotKey->UnRegister();
	delete m_hotKey;


}


BOOL CZLPlayerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch(zTaskbar->OnCommand(wParam,lParam))
	{
		case MPlay:PlayBtn();return TRUE;
		case MPrevious:OnBnClickedPrevious();return TRUE;
		case MNext:OnBnClickedNext();return TRUE;
		default:break;
	}
	return CDialogEx::OnCommand(wParam,lParam);
}



void CZLPlayerDlg::OnBnClickedPrevious()
{
	// TODO: Add your control notification handler code here
	PLAYER_STATE ps = manager.GetPlayerState();
	if(this->m_timeCtrl.GetPos() >= 5 && ps.play && !ps.pause  )
	{
		manager.PlayerControl(MSeek,0);
	}
	else		
		manager.PlayerControl(PlayCommand::MPrevious);
}


void CZLPlayerDlg::OnBnClickedPp()
{
	// TODO: Add your control notification handler code here
	bool isAtlCommand = m_playPauseBtn.IsAtlCommand();
	PLAYER_STATE ps = manager.GetPlayerState();
	if(ps.stop)
		manager.PlayerControl(isAtlCommand?PlayCommand::MPause:PlayCommand::MPlay,
		 manager.GetCurrentPlay());
	else
		manager.PlayerControl(isAtlCommand?PlayCommand::MPause:PlayCommand::MResume,
		manager.GetCurrentPlay());
}


void CZLPlayerDlg::OnBnClickedNext()
{
	// TODO: Add your control notification handler code here	
	manager.PlayerControl(PlayCommand::MNext);	
}


void CZLPlayerDlg::OnBnClickedPlist()
{
	// TODO: Add your control notification handler code here
}

void CZLPlayerDlg::PlayBtn()
{
	m_playPauseBtn.SetAtlCommand(m_playPauseBtn.IsAtlCommand());
	OnBnClickedPp();
}

void CZLPlayerDlg::OnNMDblclkPlaylist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(pNMItemActivate->iItem >=0)
	{
		PLAYER_STATE ps = manager.GetPlayerState();
		if(manager.GetCurrentPlay() == pNMItemActivate->iItem && ps.pause)
			manager.PlayerControl(MResume);
		else
			manager.PlayerControl(MPlay,pNMItemActivate->iItem);
	}
}


void CZLPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(3001 == nIDEvent)
	{
		
		int time = manager.player->GetPlayDuration(true);
		/*printf("%d\n",time);*/
		CString mes;
		int tmin = m_timeCtrl.GetRangeMax() / 60;
		int min = time/60;
		mes.Format(L"%02d:%02d/%02d:%02d",min,time-min*60,tmin,m_timeCtrl.GetRangeMax()-tmin*60);
		m_play_duration.SetWindowTextW(mes);
		m_play_duration.Invalidate();
		m_timeCtrl.SetPos(time);
		mes.Empty();
		if(manager.player->GetPlayerState().stop)
		{
			KillTimer(nIDEvent);
			m_timeCtrl.EnableWindow(FALSE);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CZLPlayerDlg::OnRclickPlaylist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	if(pos)
	{
		CPoint point;   
		::GetCursorPos(&point); 
		CMenu   pMenu; 
		pMenu.LoadMenu(IDR_PL_MENU); 
		CMenu*  pPopup = pMenu.GetSubMenu(0);		
		pPopup-> TrackPopupMenu(TPM_RIGHTBUTTON,point.x,point.y,this); 
	}	
}


void CZLPlayerDlg::OnPlaylistOpenfilelocation()
{
	// TODO: Add your command handler code here
	POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	if(pos)
	{
		int index = m_playList_ctrl.GetNextSelectedItem(pos);
		
		CString pa;
		pa.Format(L"explorer /n,/select,%s",manager.player->GetAt(index).fileName);
		CCharWCharHelper ccw;
		ccw.ZWideCharToMultiByte(pa);

		WinExec(ccw.GetMultiChar(), SW_SHOW);
		Sleep(100);
		pa.Empty();
	}
}


void CZLPlayerDlg::OnPlaylistDelete()
{
	// TODO: Add your command handler code here
	vector<unsigned int>itemIndex;
	POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	while(pos)
	{
		int index = m_playList_ctrl.GetNextSelectedItem(pos);
		itemIndex.push_back(index);
	}
	for(int i = itemIndex.size()-1;i>0;i--)
	{
		m_playList_ctrl.DeleteItem(itemIndex[i]);
		manager.RemoveMedia(itemIndex[i],false);
	}
	if(itemIndex.size() > 0)
	{
		m_playList_ctrl.DeleteItem(itemIndex[0]);
		manager.RemoveMedia(itemIndex[0],true);
	}
	m_playList_ctrl.Invalidate();
}


void CZLPlayerDlg::OnPlaylistClear()
{
	// TODO: Add your command handler code here
	POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	if(pos)
	{
		m_playList_ctrl.DeleteAllItems();
		manager.Clear();
	}
	m_playList_ctrl.Invalidate();
}


void CZLPlayerDlg::OnPlaylistProperty()
{
	// TODO: Add your command handler code here
	POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	if(pos)
	{
		int index = m_playList_ctrl.GetNextSelectedItem(pos);
		SHELLEXECUTEINFO sei; 

		sei.hwnd = this->GetSafeHwnd(); 
		sei.lpVerb =  L"properties"; 
		sei.lpFile = manager.player->GetAt(index).fileName;
		sei.lpDirectory = NULL; 
		sei.lpParameters = NULL; 
		sei.nShow = SW_SHOWNORMAL; 
		sei.fMask = SEE_MASK_INVOKEIDLIST; 
		sei.lpIDList = NULL; 
		sei.cbSize = sizeof(SHELLEXECUTEINFO); 
		ShellExecuteEx(&sei);
	}
}


void CZLPlayerDlg::OnPlaymodeOrder()
{
	// TODO: Add your command handler code here
	manager.SetPlayMode(PMOrder);
}


void CZLPlayerDlg::OnPlaymodeSingle()
{
	// TODO: Add your command handler code here
	manager.SetPlayMode(PMSingle);
}


void CZLPlayerDlg::OnPlaymodeCircle()
{
	// TODO: Add your command handler code here
	manager.SetPlayMode(PMCircle);
}

void CZLPlayerDlg::OnPlaymodeRandom()
{
	// TODO: Add your command handler code here
	manager.SetPlayMode(PMRandom);
}

void CZLPlayerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	if(pWnd->GetSafeHwnd() != m_playList_ctrl.GetSafeHwnd())
	{
		CMenu   pMenu; 
		pMenu.LoadMenu(IDR_PL_MENU); 
		CMenu*  pPopup = pMenu.GetSubMenu(1);
		PlayMode pm = manager.GetPlayerConfig().pm;
		if(PMOrder == pm)
		{
			pPopup->GetSubMenu(0)->CheckMenuItem(ID_PLAYMODE_ORDER,MF_CHECKED|MF_BYCOMMAND);
		}
		else if(PMSingle == pm)
		{
			pPopup->GetSubMenu(0)->CheckMenuItem(ID_PLAYMODE_SINGLE,MF_CHECKED|MF_BYCOMMAND);
		}
		else if(PMCircle == pm)
		{
			pPopup->GetSubMenu(0)->CheckMenuItem(ID_PLAYMODE_CIRCLE,MF_CHECKED|MF_BYCOMMAND);
		}
		else
		{
			pPopup->GetSubMenu(0)->CheckMenuItem(ID_PLAYMODE_RANDOM,MF_CHECKED|MF_BYCOMMAND);
		}
		pPopup-> TrackPopupMenu(TPM_RIGHTBUTTON,point.x,point.y,this); 
	}
}


void CZLPlayerDlg::OnMaindlgOptions()
{
	// TODO: Add your command handler code here
	CZOptionsDlg dlg;
	dlg.DoModal();
}


BOOL CZLPlayerDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	// TODO: Add your message handler code here and/or call default
	CString files   ((LPWSTR)pCopyDataStruct->lpData);
	files = files.Left(pCopyDataStruct->cbData/sizeof(wchar_t));
	int index = 0;
	int last = 0;
	//files = "d:\\better in time.flac|d:\\test1.mp3|d:\\1.tta|";
	int playIndex = manager.GetMediaCount();
	while(-1 != (index = files.Find('|',last)))
	{
		CString tmp = files.Mid(last,index - last);
		manager.AddFile(tmp);
		last = index + 1;
		tmp.Empty();
	}
	files.Empty();
	if(playOnAddFile && playIndex < manager.GetMediaCount())
		manager.PlayerControl(MPlay,playIndex);
	return TRUE;
	//return CDialogEx::OnCopyData(pWnd, pCopyDataStruct);
}


int CZLPlayerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
#ifndef _DEBUG
	CZSingleton zs;
	zs.SetDrop();
#endif
	return 0;
}


BOOL CZLPlayerDlg::SetTipText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult )
{
	//printf("before setTipText %d,%d\n",id,TTN_NEEDTEXT);


	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct; 

	UINT nID =pTTTStruct->idFrom; //得到相应窗口ID，有可能是HWND 

	if (pTTT->uFlags & TTF_IDISHWND) //表明nID是否为HWND   
	{ 
		CString text;
		nID = ::GetDlgCtrlID((HWND)nID);//从HWND得到ID值，当然你也可以通过HWND值来判断 

		if(nID == IDC_TIME)
		{
			if(m_timeCtrl.IsWindowEnabled())
			{
				int pos = m_timeCtrl.GetPostionCursor();
				int min = pos/60;
				int sec = pos - min * 60;
				int tmin = m_timeCtrl.GetRangeMax() / 60;
				int tsec = m_timeCtrl.GetRangeMax() - tmin * 60;
				text.Format(L"%02d:%02d-%02d:%02d",min,sec,tmin,tsec);
			}
			else
				text.Append(L"");
			wcscpy(pTTT->lpszText,FormatInfoText(text));//.AllocSysString()
		}
		else if(nID == IDC_VOLUME)
		{

			//text.Format(L"Volume(%d%s:%d%s)",m_volume.GetPostionCursor()*100/10000,L"%",m_volume.GetPos()*100 / 10000,L"%");
			text.Format(L"Volume(%d%s)",m_volume_ctrl.GetPos()*100/10000,L"%");
			wcscpy(pTTT->lpszText,FormatInfoText(text));//.AllocSysString()
		}
		else if(nID == IDC_STOP)
		{
			wcscpy(pTTT->lpszText,L"Stop");
		}
		else if(nID == IDC_PP)
		{
			if(m_playPauseBtn.IsAtlCommand())
				wcscpy(pTTT->lpszText,L"Play");
			else
				wcscpy(pTTT->lpszText,L"Pause");
		}
		else if(nID == IDC_PREVIOUS)
		{
			wcscpy(pTTT->lpszText,L"Previous");
		}
		else if(nID == IDC_NEXT)
		{
			wcscpy(pTTT->lpszText,L"Next");
		}
		else
		{
			this->GetWindowTextW(text);
			//printf("length of text:%d\n",text.GetLength());

			wcscpy(pTTT->lpszText,FormatInfoText(text));//.AllocSysString()
		}
	}
	// printf("after setTipText %d\n",id);
	return TRUE;
}


CString CZLPlayerDlg::FormatInfoText(CString src)
{
	if(src.GetLength() <= 80)
		return src;
	return src.Left(79);
}

BOOL CZLPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_KEYDOWN == pMsg->message)
	{
		if(VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam)
			return FALSE;
		else if('L' == pMsg->wParam &&  (GetKeyState(VK_CONTROL)&0x8000))// open lrc folder
		{
			wchar_t exeFullPath[MAX_PATH];
			GetModuleFileName(NULL, exeFullPath, MAX_PATH);
			PathRemoveFileSpec(exeFullPath);
			wcscat(exeFullPath,L"\\ZDLRC\0");
			ShellExecute(NULL,L"explore",exeFullPath,NULL,NULL,SW_SHOWNORMAL);
		}
	}
	else if(WM_MOUSEWHEEL == pMsg->message)
	{
		short zDelta = (short)HIWORD(pMsg->wParam);
		CPoint point,pt;
		CRect rect;
		GetCursorPos(&point);
		GetCursorPos(&pt);
		GetWindowRect(&rect);
		CRect plRect;
		//printf("1 x:%d,y:%d\n",point.x,point.y);
		m_playList_ctrl.GetClientRect(plRect);
		m_playList_ctrl.ScreenToClient(&pt);
		//printf("2 x:%d,y:%d\n",point.x,point.y);
		if(!this->IsWindowVisible() || !rect.PtInRect(point))
		{
			if( plRect.PtInRect(pt))
			{
				m_playList_ctrl.SetFocus();
				return CDialogEx::PreTranslateMessage(pMsg);
			}
		}
		
		m_volume_ctrl.SetFocus();
		
		//printf("mouse wheel delta:%d\n",zDelta);
		if(zDelta >= -120)
		{	
			m_volume_ctrl.SetPos(100+m_volume_ctrl.GetPos());
		}
		else
			m_volume_ctrl.SetPos(m_volume_ctrl.GetPos()-100);
		manager.player->SetVolume(m_volume_ctrl.GetPos());
		return FALSE;
	}
	else if(pMsg->message == g_uTBBC)
	{
		//printf("%d,%d\n",pMsg->message,g_uTBBC);
		bool r = zTaskbar->AddThumbToolBar();
		if(manager.GetMediaCount() > 0 )
			PlayBtn();
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CZLPlayerDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: Add your message handler code here and/or call default
	if(MEDIA_NEXT == nHotKeyId && m_nextBtn.IsWindowEnabled())
	{
		OnBnClickedNext();
	}
	else if(MEDIA_PREV == nHotKeyId && m_previousBtn.IsWindowEnabled())
	{
		OnBnClickedPrevious();
	}
	else if(MEDIA_STOP == nHotKeyId && m_stopBtn.IsWindowEnabled())
	{
		PlayBtn();
	}
	else if(MEDIA_PLAY_PAUSE == nHotKeyId && m_playPauseBtn.IsWindowEnabled())
	{
		PlayBtn();
	}
	else
		CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

void CZLPlayerDlg::UpdateBkColor(DWORD color)
{
	m_timeCtrl.SetBkColor(color);
	m_volume_ctrl.SetBkColor(color);
	//m_stopBtn.SetBkColor(color);
	//m_playPauseBtn.SetBkColor(color);
	//m_nextBtn.SetBkColor(color);
	//m_previousBtn.SetBkColor(color);
	m_playList_ctrl.SetBkColor(color);
}



void CZLPlayerDlg::OnPlaylistCopytitle()
{
	// TODO: Add your command handler code here
	POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	int index = -1;
	if(pos)
	{
		index = m_playList_ctrl.GetNextSelectedItem(pos);
	}
	if(OpenClipboard() && -1 != index)
	{
		wchar_t * buffer;
		EmptyClipboard();
		CString title = manager.player->GetAt(index).title;
	
		HGLOBAL clipbuffer = GlobalAlloc(GHND, sizeof(wchar_t) + title.GetLength()*2+1);
		buffer = (wchar_t *)GlobalLock(clipbuffer);
		wcscpy(buffer,title);
		buffer[title.GetLength()] = L'\0';
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_UNICODETEXT,clipbuffer);
		CloseClipboard();
	}
}


void CZLPlayerDlg::OnMaindlgAboutzlplayer()
{
	// TODO: Add your command handler code here
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

void CZLPlayerDlg::OnPlaylistConvertto()
{
	// TODO: Add your command handler code here
}


void CZLPlayerDlg::OnPlaylistSearchlyric()
{
	// TODO: Add your command handler code here
	POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
	int index = -1;
	if(pos)
	{
		index = m_playList_ctrl.GetNextSelectedItem(pos);
		CString title = manager.player->GetAt(index).title;
		CString artist = manager.player->GetAt(index).aritst;
		CString file ;
		wchar_t exeFullPath[512];
		GetModuleFileName(NULL, exeFullPath, 512);
		PathRemoveFileSpec(exeFullPath);
		wcscat(exeFullPath,L"\\LRC.exe");
		file.Format(L"%s%s",title,L".lrc");
		title.Format(L"\"%s|%s|%s\"",title.Trim()==L""?L"0xFFFF":title,artist.Trim()==""?L"0xFFFF":artist,file);
		ShellExecute(0,L"",exeFullPath,title,L"",SW_SHOW);
	}
}




afx_msg LRESULT CZLPlayerDlg::PlayControlMessage(WPARAM wParam, LPARAM lParam)
{
	ZMessage mes = (ZMessage)wParam;
	manager.UpdateUI(mes);
	//printf("%d\n",mes);
	if(ZPLAY == mes)
	{
		manager.zbStatus.tBarPlayPause = 2;
		manager.zbStatus.playPause = 2;


		lyric->ReadLRC(manager.player->GetAt(manager.GetCurrentPlay()));
		m_timeCtrl.EnableWindow(TRUE);
		SetTimer(3001,800,0);
		POSITION pos = m_playList_ctrl.GetFirstSelectedItemPosition();
		while(0 != pos)
		{
			int  index   = m_playList_ctrl.GetNextSelectedItem(pos);
			m_playList_ctrl.SetItemState(index,~LVNI_SELECTED,LVNI_SELECTED);
		}
		m_playList_ctrl.SetItemState(manager.GetCurrentPlay(),LVNI_SELECTED,LVNI_SELECTED);	
		m_playList_ctrl.EnsureVisible(manager.GetCurrentPlay(),TRUE);
		this->SetWindowTextW(manager.player->GetAt(manager.GetCurrentPlay()).title);
		CString mes;
		double b = manager.player->GetWaveInfo().bitrate;
		if(b > 0.0 && b < 5000.0)
			mes.Format(L"%.1fK",b);
		else
			mes.Format(L"%.1fKhz",manager.player->GetWaveInfo().nSamplesPerSec / 1000.0);
		m_waveInfo.SetWindowTextW(mes);
		m_waveInfo.Invalidate();
		mes.Empty();
				
		LoadCoverImage();

	}
	else if(ZPAUSE == mes)
	{
		manager.zbStatus.tBarPlayPause = 1;
		manager.zbStatus.playPause = 1;
		KillTimer(3001);
	}
	else if(ZSTOP == mes || ZUSERSTOP == mes)
	{
		//manager->zbStatus.tBarPlayPause = 1;
		//manager->zbStatus.playPause = 1;
		// play next
		if(manager.player->GetMediaCount() <= 0)
		{
			manager.zbStatus.tBarPlayPause = 0;
			manager.zbStatus.playPause = 0;
		}
		else
		{
			manager.zbStatus.tBarPlayPause = 1;
			manager.zbStatus.playPause = 1;
		}

		m_timeCtrl.EnableWindow(FALSE);
				// kill timer
		KillTimer(3001);
		this->SetWindowTextW(L"ZLPlayer");
		if(ZUSERSTOP == mes)
		{
			m_waveInfo.SetWindowTextW(L"");
			m_waveInfo.Invalidate();
		}
		else
			manager.PlayerControl(MPlay,-1);

		m_play_duration.SetWindowTextW(L"00:00/00:00");
		m_play_duration.Invalidate();
	}
	

	//else if(ZUSERSTOP == mes)
	//{		
	//	if(manager.player->GetMediaCount() <= 0)
	//	{
	//		manager.zbStatus.tBarPlayPause = 0;
	//		manager.zbStatus.playPause = 0;
	//	}
	//	else
	//	{
	//		manager.zbStatus.tBarPlayPause = 1;
	//		manager.zbStatus.playPause = 1;
	//	}
	//}
	else if(ZRESUM == mes)
	{
		manager.zbStatus.tBarPlayPause = 2;
		manager.zbStatus.playPause = 2;
		SetTimer(3001,800,0);
	}
	else if(ZOPEN == mes)
	{		
		manager.zbStatus.time = (int)lParam;
		//printf("total times:%d\n",manager->zbStatus.time);
		m_timeCtrl.SetRangeMax(manager.zbStatus.time);
				//printf("total time 2:%d\n",bs->time);
		m_timeCtrl.EnableWindow(TRUE);
		CString mes;
		int min = manager.zbStatus.time / 60;
		mes.Format(L"00:00/%02d:%02d",min,manager.zbStatus.time-min*60);
		m_play_duration.SetWindowTextW(mes);
		m_play_duration.Invalidate();
				
				
		mes.Empty();
	}
	lyric->PlayControlMessage(mes);

	if(ZOPEN != mes)
	{
		zTaskbar->UpdateThumbToolBar(manager.zbStatus.tBarPreious,manager.zbStatus.tBarPlayPause,manager.zbStatus.tBarNext);
		m_previousBtn.EnableWindow(manager.zbStatus.previous?TRUE:FALSE);
		m_nextBtn.EnableWindow(manager.zbStatus.next?TRUE:FALSE);
		if(0 == manager.zbStatus.playPause)
			m_playPauseBtn.EnableWindow(FALSE);
		else
		{
			m_playPauseBtn.SetAtlCommand(2==manager.zbStatus.playPause);
			m_playPauseBtn.EnableWindow(TRUE);
		}
	}

	CString mesStr;
	mesStr.Format(L"%d/%d",manager.GetMediaCount()>0?manager.GetCurrentPlay()+1:0,manager.GetMediaCount());
	m_musicNum.SetWindowTextW(mesStr);
	m_musicNum.Invalidate();
	mesStr.Empty();
	return 0;
}
