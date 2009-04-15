// eyegolfDlg.h : header file
//

#pragma once
//#include <highgui.h>
#include "afxwin.h"
#include "IPS3EyeLib.h"
#include "CircularBuffer.h"
#include "AviFile.h"

//#include "waveInFFTDlg.h"
#include "fourier.h"
#include "Recorder.h"
#include <math.h>
// CeyegolfDlg dialog

class CeyegolfDlg : public CDialog
{
// Construction
public:
	CeyegolfDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EYEGOLF_DIALOG };

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
	HBITMAP CeyegolfDlg::CreateBitmap(int w,int h,WORD bpp,int nSize);
	void displayAvailableFormats(CStringArray* FormatList,int formats);
	DECLARE_MESSAGE_MAP()
	//my crap
	CDC m_dcMem;
public:
	afx_msg void OnStnClickedCamOptionsTxt();
	CComboBox m_cameraOption;
	IPS3EyeLib *pCam;
	HBITMAP hBitmap;
	PBYTE pBuffer;
	PBYTE pTempBuf;
	int width;
	int height;
	// pRecord Toggle stops recording thread
	bool pRecordToggle;
	HANDLE hThread;
	//bufSize is the ammount of frames we want to pull from buffer, max defined below
	int bufSize;
	CCircularBuffer< unsigned char *, 200 > ringBuf;
	CCircularBuffer< unsigned char *, 200 > ringBufPlayback;
	CButton m_recordToggle;
	CButton m_wndSetCamera;
	afx_msg void OnCbnSelchangeCameraOptions();
	afx_msg void OnBnClickedSetCamOptionBtn();
	
	bool ShowBitmap(HBITMAP hbmp,int x, int y);
	void ThreadProc();
	void ThreadPlaybackProc();
	void ThreadSoundRecordProc();
	void CeyegolfDlg::CopyBuffer();
	static DWORD WINAPI ThreadStub(LPVOID p);
	static DWORD WINAPI ThreadPlaybackStub(LPVOID p);
	static DWORD WINAPI ThreadSoundRecordStub(LPVOID p);
	bool replay;
	afx_msg void OnBnClickedReplaybufferbtn();
	afx_msg void OnBnClickedRecordToggleBtn();
	afx_msg void OnBnClickedPlayBufferBtn();
	CEdit FPS_EDIT;
	int FPS_Playback;
	HANDLE hPlaybackThread;
	HANDLE hSoundRecordThread;
	afx_msg void OnEnChangeFpsEdit();
	afx_msg void OnBnClickedReplayBtn();
	void CeyegolfDlg::PlaybackBuffer();
	CButton m_Replay_BTN;
	bool soundRecordRunning;
	bool soundTrigger;
	CComboBox m_Fps_Option;
	afx_msg void OnCbnSelchangeCombo2();
};
