
// ZLPlayerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "..\ZLControls\ZLabel.h"
#include "..\ZLControls\ZSliderCtrl.h"
#include "..\ZLControls\ZPlayListCtrl.h"
#include "..\ZLControls\ZImageCtrl.h"
#include "..\ZLControls\ZButton.h"
#include "..\ZLControls\ZMButton.h"
#include "ZTaskBarHelper.h"
#include "afxcmn.h"
#include "ZManager.h"
#include "ZUITools.h"
#include "HotKeyHelper.h"
#include "ZLyricDlg.h"
#include "ZSetting.h"
#include <Windows.h>
// CZLPlayerDlg dialog
class CZLPlayerDlg : public CDialogEx
{
// Construction
public:
	CZLPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ZLPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCreateThumbToolBar( WPARAM, LPARAM );
	DECLARE_MESSAGE_MAP()
private:
	CZLabel m_play_duration;
	CZTaskBarHelper *zTaskbar;
	CZSliderCtrl m_volume_ctrl;
	CZPlayListCtrl m_playList_ctrl;
	CImageList m_imageList;
	CZImageCtrl m_coverImg;
	CZButton m_stopBtn;
	CZMButton m_playPauseBtn;
	CZSliderCtrl m_timeCtrl;
	CZLabel m_musicNum;
	CZMButton m_previousBtn;
	CZMButton m_nextBtn;
	CZManager manager;
	CZUITools uiTools;
	CBitmap bmp;
	CHotKeyHelper *m_hotKey;
	CZLabel m_waveInfo;	
	CZLyricDlg *lyric;	
	CZSetting m_setting;
	bool playOnAddFile;
	bool lockCover;
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	
	afx_msg void OnDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnBnClickedPrevious();
	afx_msg void OnBnClickedPp();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedPlist();
	void PlayBtn();
	afx_msg void OnNMDblclkPlaylist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRclickPlaylist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPlaylistOpenfilelocation();
	afx_msg void OnPlaylistDelete();
	afx_msg void OnPlaylistClear();
	afx_msg void OnPlaylistProperty();
	afx_msg void OnPlaymodeOrder();
	afx_msg void OnPlaymodeSingle();
	afx_msg void OnPlaymodeCircle();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	void LoadCoverImage();
	afx_msg void OnMaindlgOptions();
	afx_msg void OnPlaymodeRandom();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL SetTipText( UINT id, NMHDR * pTTTStruct, LRESULT * pResult ); 
	CString FormatInfoText(CString src);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
	void UpdateBkColor(DWORD);

	
	afx_msg void OnPlaylistCopytitle();
	afx_msg void OnMaindlgAboutzlplayer();
	afx_msg void OnPlaylistConvertto();
	afx_msg void OnPlaylistSearchlyric();
protected:
	afx_msg LRESULT PlayControlMessage(WPARAM wParam, LPARAM lParam);
};
