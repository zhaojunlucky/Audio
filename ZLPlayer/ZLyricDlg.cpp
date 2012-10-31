// ZLyricDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZLPlayer.h"
#include "ZLyricDlg.h"
#include "afxdialogex.h"
#include <cmath>
#include "ZSetting.h"


// CZLyricDlg dialog

IMPLEMENT_DYNAMIC(CZLyricDlg, CDialogEx)

CZLyricDlg::CZLyricDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CZLyricDlg::IDD, pParent)
{
	::InitializeCriticalSection(&m_cs);
	m_bBack=false;
	/*GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);*/
	wcscpy(m_defaultMes, L"ZLPlayer");
	showDefault = true;
	timerStart = false;
	lastTagTime = 0;
	lastTagIndex = -1; 
	screenX = 1280;
	screenY = 800;
	taskH = 160;
	yOffset = 25;// 30 -25 ,42 - 10

}

CZLyricDlg::~CZLyricDlg()
{
	::DeleteCriticalSection(&m_cs);
}

void CZLyricDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CZLyricDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CZLyricDlg message handlers


BOOL CZLyricDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	
	screenX = GetSystemMetrics(SM_CXSCREEN); 
	screenY = GetSystemMetrics(SM_CYSCREEN);

	taskH = GetSystemMetrics(SM_CYSCREEN)-GetSystemMetrics(SM_CYFULLSCREEN) ;

	CRect rect;
	this->GetWindowRect(rect);

	//this->MoveWindow(rect.left+50,650,rect.Width(),rect.Height());
	::SetWindowPos(this->GetSafeHwnd(),HWND_TOPMOST, 30,screenY-taskH - 90,screenX-50,/*rect.Height()+10*/120, SWP_SHOWWINDOW);
	GetWindowRect(&rct);
	ptWinPos.x = rct.left;
	ptWinPos.y = rct.top;
	//fontSize = 25;//  25 38 42
	fontSize = 0;
	font = NULL;
	
	fontFamily = new FontFamily(L"FZMM");//·½Õýß÷ÎØÌå
	ProcessFontSize();
	font = new Gdiplus::Font(fontFamily,fontSize,FontStyleRegular);

	/*TEXTMETRIC tm;
	
	memset(&tm,0,sizeof(tm));
	this->GetDC()->GetTextMetrics(&tm);*/

	pen = new Pen(Color(155,215,215,215),3);
	linGrBrush = new LinearGradientBrush(
		Point(0,0),Point(0,rct.bottom - rct.top),//110
		Color(255,255,255,255),
		Color(255,30,120,195));

	linGrBrushW = new LinearGradientBrush(
		Point(0,10),Point(0,rct.bottom - rct.top - 50),//60
		Color(255,255,255,255),
		Color(15,1,1,1));
	ptSrc.x = ptSrc.y = 0;
	sizeWindow.cx = screenX - 50;
	sizeWindow.cy = 450;
	showlyric = true;
	UpdateDisplay();

	ModifyStyleEx(WS_EX_APPWINDOW,WS_EX_TRANSPARENT);
	wchar_t exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	PathRemoveFileSpec(exeFullPath);
	wcscat_s(exeFullPath,L"\\ZDLRC\\");

	fileWatch = new CZFileWatch(exeFullPath,MyDIRCallback,this);
	fileWatch->Start();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CZLyricDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	fileWatch->Stop();
	delete fileWatch;
	delete font;
	delete fontFamily;
	delete linGrBrushW;
	delete linGrBrush;
	delete pen;
	KillTimer(1988);
}


int CZLyricDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_Blend.BlendOp=0; //theonlyBlendOpdefinedinWindows2000
	m_Blend.BlendFlags=0; //nothingelseisspecial...
	m_Blend.AlphaFormat=1; //...
	m_Blend.SourceConstantAlpha=255;//AC_SRC_ALPHA
	return 0;
}

void CZLyricDlg::ProcessFontSize()
{

	CZSetting set;
	int fSize = set.ReadFontSize();
	if(fontSize == fSize)
		return ;
	fontSize = fSize;
	EnterCriticalSection(&m_cs);
	bool timeProcess = timerStart;
	bool lyricProcess = showlyric;
	if(lyricProcess)
		showlyric = false;
	if(timeProcess)
		StopTimer();

	if(font)
		delete font;
	switch(fontSize)
	{
	case 20:
	case 21:
	case 22:
	case 23:
		yOffset = 35;break;
	case 24:
	case 25:
	case 26:
		yOffset = 30;break;
	case 27:
	case 28:
	case 29:
	case 30:
		yOffset = 25;break;
	case 31:
	case 32:
	case 33:
	case 34:
		yOffset = 17;break;
	case 35:
	case 36:
	case 37:
	case 38:
		yOffset = 13;break;
	case 39:
	case 40:
	case 41:
	case 42:
		yOffset = 10;break;
	default:break;
	}
	font = new Gdiplus::Font(fontFamily,fontSize,FontStyleRegular);
	if(lyricProcess)
		showlyric = true;
	if(timeProcess)
		StartTimer();
	LeaveCriticalSection(&m_cs);
	this->Invalidate();
}
void CZLyricDlg::EnableLyric(bool enable)
{
	this->ShowWindow(enable?TRUE:FALSE);
	showlyric = enable;
	if(showlyric && lyricer.tagTimes.size() > 0)
		StartTimer();
}
BOOL CZLyricDlg::UpdateDisplay(int Transparent)
{
	HDC hdcTemp=GetDC()->m_hDC;
	m_hdcMemory=CreateCompatibleDC(hdcTemp);
	HBITMAP hBitMap=CreateCompatibleBitmap(hdcTemp,screenX-50,450);
	SelectObject(m_hdcMemory,hBitMap);
	if(Transparent<0||Transparent>100)	Transparent=100;

	m_Blend.SourceConstantAlpha=int(Transparent*2.55);//1~255
	HDC hdcScreen=::GetDC (m_hWnd);

	
	//POINT ptWinPos={rct.left,rct.top};

	Graphics *graph = new Graphics(m_hdcMemory);

	Graphics *graphics = new Graphics(m_hdcMemory);

	graphics->SetSmoothingMode(SmoothingModeAntiAlias);
	graphics->SetInterpolationMode(InterpolationModeHighQualityBicubic);
	

	StringFormat strformat;

	GraphicsPath path;
	
	if(showDefault)
		path.AddString(m_defaultMes,wcslen(m_defaultMes),fontFamily,
			FontStyleRegular,fontSize,Point(10,10+yOffset),&strformat);
	else
	{
		LRCWord lrcWord = lyricer.lrcWords[lyricer.tagTimes[lastTagIndex].lineNum];
		//Gdiplus::Font font(&fontFamily,fontSize,FontStyleRegular);
		BSTR bstr = lrcWord.lrcWord.AllocSysString();
		drawSize = zGraphics.GetTextBounds(*font,strformat,bstr);
		SysFreeString(bstr);
		SizeF orgSize(rct.right-rct.left-20/*+700*/,rct.bottom - rct.top);
		

		if(drawSize.Width <= orgSize.Width)
			path.AddString(lrcWord.lrcWord,lrcWord.lrcWord.GetLength(),fontFamily,
				FontStyleRegular,fontSize,Point(10,10+yOffset),&strformat);
		
		else if(drawSize.Width < orgSize.Width*2)
		{
			CString lyricStr(lrcWord.lrcWord);
			int midIndex = lyricStr.GetLength()/2;
			char c = lyricStr.GetAt(midIndex);
			if(((c >= 'A' && c <= 'Z')||(c>='a'||c<='z')|| c=='\'' ) /*&& midIndex > 20*/)
			{
				int i = 1;
				while(i <= 30 && midIndex > i)
				{
					
					c = lyricStr.GetAt(midIndex-i);
					if(c == ' ' || c == '\t' || c =='\n')
					{
						midIndex = midIndex - i;
						break;
					}		
					i--;
				}
				// if there is ascii chars follow with non-ascii chars
				// the left part's length will smaller the right's part
				// so we will test the length once more,and find the split index
			}
			else if(c >='0' && c <='9'/* && midIndex > 30*/)
			{
				int i = 1;
				while(i <= 30 && midIndex > i)
				{
					
					c = lyricStr.GetAt(midIndex-i);
					if(c < '0' || c>'9'||c == ' ' || c == '\t' || c =='\n')
					{
						midIndex = midIndex - i;
						break;
					}		
					i--;
				}
			}


			CString lyricStrTop = lyricStr.Left(midIndex).Trim();
			CString lyricStrBottom = lyricStr.Right(lyricStr.GetLength() - midIndex).Trim();
			///
			bstr = lyricStrTop.AllocSysString();
			drawSize2 = zGraphics.GetTextBounds(*font,strformat,bstr);
			SysFreeString(bstr);

			if(drawSize2.Width + 100 < drawSize.Width/2)
			{
				midIndex = lyricStr.GetLength()/2;
				char c = lyricStr.GetAt(midIndex+1);
				while((c>='a'&&c<='z')||(c>='A' && c<= 'Z') || (c>='0' && c<='9') || c=='\'' || c==' ' || c == ',' || c == '£¬' || c=='!' || c == '£¡' || c == '.' || c == '¡£'
					|| c=='~' || c == '¡«' )
					c = lyricStr.GetAt(++midIndex);
				//midIndex--;
			}
			drawSize.Height = drawSize.Width = 0;

			

			lyricStrTop = lyricStr.Left(midIndex).Trim();
			lyricStrBottom = lyricStr.Right(lyricStr.GetLength() - midIndex).Trim();
			
			path.AddString(lyricStrTop,lyricStrTop.GetLength(),fontFamily,
				FontStyleRegular,fontSize,Point(10,10+yOffset),&strformat);

			bstr = lyricStrBottom.AllocSysString();
			drawSize = zGraphics.GetTextBounds(*font,strformat,bstr);
			SysFreeString(bstr);

			path.AddString(lyricStrBottom,lyricStrBottom.GetLength(),fontFamily,
				FontStyleRegular,fontSize,Point(orgSize.Width-drawSize.Width,60+yOffset),&strformat);

			lyricStr.Empty();
			lyricStrTop.Empty();
			lyricStrBottom.Empty();
		}
		else
		{
			path.AddString(lrcWord.lrcWord,lrcWord.lrcWord.GetLength(),fontFamily,
				FontStyleRegular,fontSize,Point(10,10+yOffset),&strformat);
		}
		lrcWord.lrcWord.Empty();
	}
	

	graphics->DrawPath(pen,&path);
	

	
	
/**/
   for(int i=1; i<9; i+=1)
    {
        Pen penTmp(Color(62, 0, 2, 2), i);
        penTmp.SetLineJoin(LineJoinRound);
        graphics->DrawPath(&penTmp, &path);
    } 
   
   //SolidBrush brush(Color(25,228,228,228));
   //Pen pen1(Color(155,223,223,223));
   //Pen pen2(Color(55,223,223,223));	 
   ////Image image(L"d:\\ly.png");
   //if(m_bBack)
   //{
	  // graphics.FillRectangle(&brush,3,5,1220,90);
	  // graphics.DrawRectangle(&pen1,2,6,1221,91);
	  // graphics.DrawRectangle(&pen2,1,5,1223,93);

	  // //graphics.DrawImage(&image,600,5);
   //}

	graphics->FillPath(linGrBrush,&path);
	graphics->FillPath(linGrBrushW,&path);
	
	//SIZE sizeWindow={1230,450};

	DWORD dwExStyle=GetWindowLong(m_hWnd,GWL_EXSTYLE);
	if((dwExStyle&0x80000)!=0x80000)
		SetWindowLong(m_hWnd,GWL_EXSTYLE,dwExStyle^0x80000);

	BOOL bRet=FALSE;
	CDC *phdcScreen = CDC::FromHandle(hdcScreen);
	CDC *phdcMemory = CDC::FromHandle(m_hdcMemory);
	bRet= UpdateLayeredWindow(phdcScreen,&ptWinPos,
				&sizeWindow,phdcMemory,&ptSrc,0,&m_Blend,2);
	graph->ReleaseHDC(m_hdcMemory);
	::ReleaseDC(m_hWnd,hdcScreen);
	hdcScreen=NULL;
	::ReleaseDC(m_hWnd,hdcTemp);
	hdcTemp=NULL;
	DeleteObject(hBitMap);
	DeleteDC(m_hdcMemory);
	m_hdcMemory=NULL;

	delete graph;
	delete graphics;
	return bRet;
}

//int j = 0;
void CZLyricDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_bBack=false;
	int playTime = int(pManager->player->GetPlayDuration(true) * 1000.0f);
	if(lyricer.tagTimes.size() > 0 )
	{

		long i = lastTagIndex;
		if(i >= 0 )
			while(i < lyricer.tagTimes.size() - 1  && lyricer.tagTimes[i+1].time+lyricer.offset - playTime<5 )
			{
				i++;
				playTime = int(pManager->player->GetPlayDuration(true) * 1000.0f);
			}
		else if(-1==i)
			i = 0;
		//int delta = i>0&&i<lyricer.tagTimes.size()?(lyricer.tagTimes[i].time - lyricer.tagTimes[i-1].time)/2:5;
		//printf("j:%d,%d\n",j++,i);
		if(i >= lyricer.tagTimes.size())
		{
			lastTagIndex = lyricer.tagTimes.size() - 1;
			Sleep(200);
			StopTimer();
		}
		else if(i != lastTagIndex/* &&  (lyricer.tagTimes[i].time+lyricer.offset - playTime < 30 || playTime - lyricer.tagTimes[i].time -lyricer.offset < 50)*/ )
		{
			//printf("i:%d\n",i);
			lastTagIndex = i;
			this->Invalidate();
		}
	}
	else
	{
		StopTimer();
		this->Invalidate();
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CZLyricDlg::SetZManager(CZManager *pManager)
{
	this->pManager = pManager;
}
void CZLyricDlg::ReadLRC(const Media &m)
{
	this->m = m;
	wcsncpy(m_defaultMes,m.title,79);

	StopTimer();
	lyricer.ReadLRCFile(m);
	if(lyricer.tagTimes.size() > 0)
	{
		//StartTimer();
		lastTagIndex = lyricer.tagTimes[0].lineNum;
		lastTagTime = lyricer.tagTimes[0].time;	
		//printf("has lyric\n");
	}	
}

void CZLyricDlg::PlayControlMessage(ZMessage message)
{
	this->controlMes = message;
	SendMessage(WM_LYRIC_UI_CONTROL,0,0);
}

void CZLyricDlg::StartTimer(int Interval)
{
	StopTimer();
	if(!showlyric)
		return;
	lastTagIndex = 0;
	if(lyricer.tagTimes.size() > 0)
	{
		//printf("Start timmer\n");
		SetTimer(1988,Interval,0);
		showDefault = false;
	}
	timerStart = true;
}

void CZLyricDlg::StopTimer()
{
	//printf("Stop timmer\n");
	if(ZPAUSE != controlMes)
		showDefault = true;
	KillTimer(1988);
	this->Invalidate();
	timerStart = false;
}

LRESULT CZLyricDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if(WM_LYRIC_UI_CONTROL == message )
	{
		if(ZSTOP == controlMes || ZUSERSTOP == controlMes )
		{
			lastTagIndex = -1;
			
			StopTimer();
			wcscpy(m_defaultMes, L"ZLPlayer");
			this->Invalidate();
		}
		else if(ZPAUSE == controlMes)
		{
			StopTimer();
		}
		else if(ZPLAY == controlMes || ZRESUM == controlMes)
		{
			StartTimer();
		}
		else if(ZSEEK == controlMes)
		{
			lastTagIndex = -1;
		}
		return FALSE;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}


void CZLyricDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages
	UpdateDisplay();
}


void  __stdcall  CZLyricDlg::MyDIRCallback( ZDDIRAction act,wchar_t*,wchar_t* ,void *client)
{
	CZLyricDlg *p = (CZLyricDlg*)client;
	if(p->lyricer.tagTimes.size() <= 0)
	{		
		p->ReadLRC(p->m);
		p->StartTimer();
	}
}