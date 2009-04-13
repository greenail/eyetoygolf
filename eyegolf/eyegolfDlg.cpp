// eyegolfDlg.cpp : implementation file
//

#include "stdafx.h"
#include "eyegolf.h"
#include "eyegolfDlg.h"

#include <iostream>
#include <windows.h>
#include <sys/timeb.h>
#include <atlimage.h>
#include <windows.h>

//#include <Gdiplusimaging.h>

//#pragma comment(lib,"cv.lib")
//#pragma comment(lib,"cxcore.lib")
//#pragma comment(lib,"highgui.lib")
#pragma comment(lib,"PS3EyeLib.lib")


#define mag_sqrd(re,im) (re*re+im*im)
#define Decibels(re,im) ((re == 0 && im == 0) ? (0) : 10.0 * log10(double(mag_sqrd(re,im))))
#define Amplitude(re,im,len) (GetFrequencyIntensity(re,im)/(len))
#define AmplitudeScaled(re,im,len,scale) ((int)Amplitude(re,im,len)%scale)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

BOOL Process(void* lpData, LPWAVEHDR pwh)
{
#define FFT_LEN 512
	double finleft[FFT_LEN/2],finright[FFT_LEN/2],fout[FFT_LEN],foutimg[FFT_LEN],fdraw[FFT_LEN/2];

	DWORD nCount = 0;
	for (DWORD dw = 0; dw < FFT_LEN; dw++)
	{
		{
			//copy audio signal to fft real component for left channel
			finleft[nCount] = (double)((short*)pwh->lpData)[dw++];
			//copy audio signal to fft real component for right channel
			finright[nCount++] = (double)((short*)pwh->lpData)[dw];
		}
	}

	// Perform FFT on right channel
	fft_double(FFT_LEN/2,0,finright,NULL,fout,foutimg);
	float re,im,fmax=-99999.9f,fmin=99999.9f;
	fdraw[0] = fdraw[FFT_LEN/4] = 0;
	double decibelLevel = 0;
	double decibelTotal =0;
	int decibelCount = 1;
	for(int i=1;i < FFT_LEN/4;i++)//Use FFT_LEN/4 since the data is mirrored within the array.
	{
		re = (float)fout[i];
		im = (float)foutimg[i];
		//get Decibels in 0-110 range
		double decibels = Decibels(re,im);
		decibelTotal = decibelTotal + decibels;
		decibelLevel = decibelTotal/decibelCount;
		decibelCount++;
		fdraw[i] = Decibels(re,im);
		if (fdraw[i] > fmax)
		{
			fmax = (float)fdraw[i];
		}
		if (fdraw[i] < fmin)
		{
			fmin = (float)fdraw[i];
		}
	}
	//cout << "Decibel Level "<<decibelLevel<<endl;
	// Detect Decibel level above threashold
	if (decibelLevel > 70)
	{
		cout<< "Decible Hit!!!!!!!!!!!!!!!"<<endl;
		//CeyegolfDlg::soundTrigger = true;
		return FALSE;
		
	}
	return TRUE;
}

// CAboutDlg dialog used for App About



class CAboutDlg : public CDialog
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
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CeyegolfDlg dialog




CeyegolfDlg::CeyegolfDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CeyegolfDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CeyegolfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cameraOption);
	DDX_Control(pDX, IDC_RECORD_TOGGLE_BTN, m_recordToggle);
	DDX_Control(pDX, IDC_SET_CAM_OPTION_BTN, m_wndSetCamera);
	DDX_Control(pDX, IDC_FPS_EDIT, FPS_EDIT);
	DDX_Control(pDX, IDC_REPLAY_BTN, m_Replay_BTN);
	DDX_Control(pDX, IDC_COMBO2, m_Fps_Option);
}

BEGIN_MESSAGE_MAP(CeyegolfDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_CAM_OPTIONS_TXT, &CeyegolfDlg::OnStnClickedCamOptionsTxt)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CeyegolfDlg::OnCbnSelchangeCameraOptions)
	ON_BN_CLICKED(IDC_SET_CAM_OPTION_BTN, &CeyegolfDlg::OnBnClickedSetCamOptionBtn)
	ON_BN_CLICKED(IDC_RECORD_TOGGLE_BTN, &CeyegolfDlg::OnBnClickedRecordToggleBtn)
	ON_BN_CLICKED(IDC_Play_Buffer_BTN, &CeyegolfDlg::OnBnClickedPlayBufferBtn)
	ON_EN_CHANGE(IDC_FPS_EDIT, &CeyegolfDlg::OnEnChangeFpsEdit)
	ON_BN_CLICKED(IDC_REPLAY_BTN, &CeyegolfDlg::OnBnClickedReplayBtn)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CeyegolfDlg::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// CeyegolfDlg message handlers

BOOL CeyegolfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	//ShowWindow(SW_MAXIMIZE);

	// TODO: Add extra initialization here
	
	int formats = IPS3EyeLib::GetNumFormats();
	CStringArray formatList;
	displayAvailableFormats(&formatList,formats);
	for (int i = 0;i< formatList.GetSize();i++)
	{
		m_cameraOption.AddString((LPCTSTR)formatList[i]);
	}
	for (int i = 10;i <= 100;i=i+10)
	{	
		CString s;
		s.Format(_T("Playback FPS: %d"),i);
		m_Fps_Option.AddString(s);
	}
	m_Fps_Option.SetCurSel(2);
	pRecordToggle = false;
	m_cameraOption.SetCurSel(5);
	pCam = IPS3EyeLib::Create();
	bufSize = 200;
	//hBitmap=CreateBitmap(320,240,24,1);
	FPS_Playback = 30;
	replay = true;

	DWORD dwSoundRecordThreadID;
	soundRecordRunning = true;
	cout<<"Starting Sound Trigger Thread "<<endl;
	soundTrigger = false;
	hSoundRecordThread = CreateThread(0,0,ThreadSoundRecordStub,this,true,&dwSoundRecordThreadID);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}
void CeyegolfDlg::displayAvailableFormats(CStringArray* FormatList,int formats)

{
	cout <<"[Available Formats]" <<endl;
	for(int i=0; i<formats; i++)
	{
		int width, height, rate;
		char* description;
		width       = IPS3EyeLib::GetFormats()[i].width;
		height      = IPS3EyeLib::GetFormats()[i].height;
		rate        = IPS3EyeLib::GetFormats()[i].rate;
		description = IPS3EyeLib::GetFormats()[i].formatTxt;
		CString d(description);
		FormatList->Add(d);
		cout <<description <<endl;
	}
} 

void CeyegolfDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
HBITMAP CeyegolfDlg::CreateBitmap(int w,int h,WORD bpp,int nSize)
{
  HDC         hDC = ::CreateCompatibleDC(0);
  BYTE        tmp[sizeof(BITMAPINFO)+255*4];
  BITMAPINFO *bmi = (BITMAPINFO*)tmp;
  HBITMAP     hBmp;
  int i;
  memset(bmi,0,sizeof(BITMAPINFO));
  bmi->bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
  bmi->bmiHeader.biWidth    = w;
  bmi->bmiHeader.biHeight   = h;
  bmi->bmiHeader.biPlanes   = nSize;
  bmi->bmiHeader.biBitCount = bpp;
  bmi->bmiHeader.biCompression = BI_RGB;
  bmi->bmiHeader.biSizeImage  = w*h*1;
  bmi->bmiHeader.biClrImportant  =0 ;
  switch(bpp)
  {
  case 8 :
      for(i=0 ; i < 256 ; i++)
      {
        bmi->bmiColors[i].rgbBlue = i;
        bmi->bmiColors[i].rgbGreen= i;
        bmi->bmiColors[i].rgbRed= i;
      }
      break;
  case 32:
  case 24:
	  ((DWORD*) bmi->bmiColors)[2] = 0x00FF0000; /* red mask */
      ((DWORD*) bmi->bmiColors)[1] = 0x0000FF00; /* green mask */
      ((DWORD*) bmi->bmiColors)[0] = 0x000000FF; /* blue mask  */
      //((DWORD*) bmi->bmiColors)[0] = 0x00FF0000; /* red mask */
      //((DWORD*) bmi->bmiColors)[1] = 0x0000FF00; /* green mask */
      //((DWORD*) bmi->bmiColors)[2] = 0x000000FF; /* blue mask  */
      break;
  }
  hBmp =
::CreateDIBSection(hDC,bmi,DIB_RGB_COLORS,NULL,0,0);
  ::DeleteDC(hDC);
  return hBmp;
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CeyegolfDlg::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
	CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CeyegolfDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CeyegolfDlg::OnStnClickedCamOptionsTxt()
{
	// TODO: Add your control notification handler code here
}

void CeyegolfDlg::OnCbnSelchangeCameraOptions()
{
	// TODO: Add your control notification handler code here
}

void CeyegolfDlg::OnBnClickedSetCamOptionBtn()
{
	cout << "Trying to Initialize Camera"<<endl;
	if(pRecordToggle == true)
	{
		pRecordToggle = false;
	}
	cout << "pCam found"<<endl;
	cout << "Stopping current Capture to reset resolution"<<endl;
	if (pCam->IsCapturing())
	{
		pCam->StopCapture();
	}
	int selected = m_cameraOption.GetCurSel();
	cout << "Setting format index to: " << selected <<endl;
	if (selected)
	{
		pCam->SetFormat(selected);
	}
	else
	{
		pCam->SetFormat(0);
	}
	// start capture and setup bitmap and buffer
	pCam->StartCapture();
	width       = IPS3EyeLib::GetFormats()[selected].width;
	height      = IPS3EyeLib::GetFormats()[selected].height;
	cout << "setting Width Height to: "<<width<<"x"<<height<<endl;
	pBuffer=new BYTE[(width*height*24)/8];
	pTempBuf=new BYTE[(width*height*24)/8];
	hBitmap = CreateBitmap(width,height,24,1);
	DWORD dwThreadID;
	DWORD dwSoundRecordThreadID;
	pRecordToggle = true;
	
	// setup ringbuffer
	cout<< "Creating ring buffer "<<endl;
	bufSize = 200;
	//CCircularBuffer< unsigned char*, 200 > ringBuf;
	cout<< "Ring Buffer size is: "<< bufSize<<" "<<endl;
	cout << "Pre allocating memory for video" << endl;
	for (int i = 0; i < bufSize;i++)
	{
		PBYTE tempBuf=new BYTE[(width*height*24)/8]; 
		ringBuf.Write(tempBuf);
	}
	for (int i = 0; i < bufSize;i++)
	{
		PBYTE tempBufPlayback=new BYTE[(width*height*24)/8]; 
		ringBufPlayback.Write(tempBufPlayback);
	}
	cout<<"Starting Video Buffer Thread "<<endl;
	hThread = CreateThread(0,0,ThreadStub,this,true,&dwThreadID);
	/* moved to init
	soundRecordRunning = true;
	cout<<"Starting Sound Trigger Thread "<<endl;
	hSoundRecordThread = CreateThread(0,0,ThreadSoundRecordStub,this,true,&dwThreadID);
	*/
}
void CeyegolfDlg::ThreadPlaybackProc()
{
PBYTE tempBuf;
while(replay)
{
	int countUsed = ringBufPlayback.countUsed;
	for (int i = 0 ; i < countUsed ; i++)
	{
		// copy data 
		//cout<< "Frames Left: "<<i<<endl;
		ringBufPlayback.Read(tempBuf);
		//ringBuf.
		//cout << "Read: "<< ringBuf.get_countUsed() <<"of "<< bufCount<<"..";
		HBITMAP hbmp=CreateBitmap(width,height,24,1);
		int l_bytesSet = SetBitmapBits(hbmp,(height*width*24)/8,tempBuf);
		ShowBitmap(hbmp, width+50,0+50);
		ringBufPlayback.Write(tempBuf);
		// go into sleep loop to detect impact
		int sleepTime = 1000/FPS_Playback;
		for (i = 0;i<sleepTime;i=i+4)
			{
			Sleep(4);
			if (!replay)
				{
				i = 100000;
				}
			}
		}
}
cout << "Replay Thread Exiting"<<endl;
}

// name sucks need to change it
void CeyegolfDlg::ThreadProc()
{
cout<< "starting thread"<<endl;
// setup FPS CRAP
double		m_timePrevFrameStamp = 0;
double		m_timeCurrFrameStamp = 0;
double		m_nFps = 0.1;
double		m_nFpsAlpha = 0.1;
// counter to print fps every 60 frames
int stupidCounter = 0;

struct timeb timeStamp;
while (pRecordToggle)
{
if(pCam->GetFrame(pBuffer, 24, false,true))
		{
		// FPS Calulation
		ftime( &timeStamp );
		m_timeCurrFrameStamp = (double) timeStamp.time*1000 + timeStamp.millitm;
		if( m_nFps < 0 )
		{
			m_nFps = 1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
		} else
		{	
			m_nFps = ( 1 - m_nFpsAlpha ) * m_nFps + m_nFpsAlpha * 1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
		}
		// set current time stamp as previuos
		m_timePrevFrameStamp = m_timeCurrFrameStamp;
		//sprintf(buff, "Fps: %g \0", m_nFps );
		//cout << "Frame Grabbed"<<endl;

		//Create empty bitmap
		hBitmap = CreateBitmap(width,height,24,1);
		
		
		ringBuf.Read(pTempBuf);
		int imageSize = (width*height*24)/8;
		memcpy(pTempBuf,pBuffer,imageSize);
		//delete[] pTempBuf;
		ringBuf.Write(pTempBuf);
		//cout << "USED BUFFER" << ringBuf.countUsed<<endl;
		stupidCounter++;
		if (stupidCounter == 60)
		{
			cout << m_nFps<<endl;
			stupidCounter =0;
		}
		//cout << l_bytesSet << " Bytes Set"<<endl;
		int l_bytesSet = SetBitmapBits(hBitmap,(height*width*24)/8,pBuffer);
		ShowBitmap(hBitmap,0,50);
		/*  Saving image screws it up, deletes it from mem or something.
		CImage image;
		image.Attach(hBitmap);
		image.Save(_T("C:\\test.bmp"));
		cout<< "Image Saved" <<endl;
		*/
			
		}
	else
	{
		cout<<" Failed to Grab Frame"<<endl;
		bool c = pCam->IsCapturing();
		if (!c)
		{
			cout << "Capture reset, sleeping"<<endl;
			Sleep(200);
			
		}
	}

}
cout<< "Record Thread Exiting"<<endl;

}
void CeyegolfDlg::ThreadSoundRecordProc()
{

bool started = false;
struct DataHolder
{
	void* pData;
	void* pData2;
};
DataHolder m_data;
Recorder m_rec;
bool recording = true;
while (soundRecordRunning)
	{
	Sleep(4);
	
	if (started == false)
		{
		cout<< "Starting Sound Trigger main"<<endl;
		m_rec.Open();
		m_rec.SetBufferFunction((void*)&m_data,Process);
		cout<< "Starting Record Buffer"<<endl;
		m_rec.Start();
		started = true;
		}
	else
		{
		//cout<< ".";	
		recording = m_rec.IsRecording();
		if (recording)
			{
			m_rec.SetBufferFunction((void*)&m_data,Process);
			m_rec.Start();
			}
		else
			{
			cout<<"Sound Trigger detected."<<endl;
			//m_rec.Stop();
			PlaybackBuffer();
			//recording = true;
			//m_rec.Open();
			m_rec.Close();
			m_rec.Open();
			m_rec.SetBufferFunction((void*)&m_data,Process);
			m_rec.Start();
			Sleep(200);
			//soundRecordRunning = FALSE;
			}
		
		}
	
	}

cout<<"Sound Trigger stopped"<<endl;
m_rec.Stop();

}
DWORD WINAPI CeyegolfDlg::ThreadStub(LPVOID p)
{
	cout<<"Video Buffer Stub Loading"<<endl;
	((CeyegolfDlg*)p)->ThreadProc();
	return 0;

}
DWORD WINAPI CeyegolfDlg::ThreadPlaybackStub(LPVOID p)
{
	cout<<"Video Playback Stub Loading"<<endl;
	((CeyegolfDlg*)p)->ThreadPlaybackProc();
	return 0;

}
DWORD WINAPI CeyegolfDlg::ThreadSoundRecordStub(LPVOID p)
{

	cout<<"SoundRecordStub...";
	((CeyegolfDlg*)p)->ThreadSoundRecordProc();
	return 0;

}	


bool CeyegolfDlg::ShowBitmap(HBITMAP hbmp,int x, int y)
{
	int memBmpWidth =0;
	int memBmpHeight =0;
	long l_bytesSet =0;

	CDC dcMem,*pDC;
	pDC = this->GetDC();
	dcMem.CreateCompatibleDC(pDC);
	
	CImage MemBitmap;
	MemBitmap.Attach(hbmp);
	memBmpWidth = MemBitmap.GetWidth();
	memBmpHeight = MemBitmap.GetHeight();
	//cout<< "select bitmap"<<endl;
	HGDIOBJ pOldBitmap = dcMem.SelectObject(MemBitmap);
	//dcMem.SelectObject(MemBitmap);
	//cout<< "bitblit"<<endl;
	pDC->BitBlt(x,y,width,height,&dcMem, 0,0,SRCCOPY);
	// Put old bitmap back
	//cout<< "but old bitmap back"<<endl;
	dcMem.SelectObject(pOldBitmap);
	//delete hbmp;
	//MemBitmap.Destroy();
	//delete[] pOldBitmap;
	ReleaseDC(pDC);
	ReleaseDC(&dcMem);
	//delete s;
	return true;
}





void CeyegolfDlg::OnBnClickedRecordToggleBtn()
{
	// TODO: Add your control notification handler code here
	pRecordToggle = false;
	// stop recording
	pCam->StopCapture();
}
void CeyegolfDlg::PlaybackBuffer()
{

cout<< "Stopping Playback Thread"<<endl;
replay = false;
DWORD ret = WaitForSingleObject(hPlaybackThread,INFINITE);
cout<< "Stopping Camera Capture"<<endl;
replay = true;
// stop recording
pCam->StopCapture();
DWORD dwPlaybackThreadID;
cout<< "Writing Playback Buffer"<<endl;
int count = ringBuf.countUsed;
PBYTE tempBuf;
PBYTE tempBufPlayback;
int imageSize = (width*height*24)/8;
for (int i = 0; i < count;i++)
{
	
	ringBuf.Read(tempBuf);
	ringBufPlayback.Read(tempBufPlayback);
	//cout << "Size of Buffer: "<< count<< " "  << sizeof(*tempBuf)<< " .. " << sizeof(&tempBuf)<<endl;
	
	memcpy(tempBufPlayback,tempBuf,imageSize);
	ringBuf.Write(tempBuf);
	ringBufPlayback.Write(tempBufPlayback);
}
cout<< "Starting Playback Thread"<<endl;
hPlaybackThread = CreateThread(0,0,ThreadPlaybackStub,this,true,&dwPlaybackThreadID);
cout<<"Starting Camera Capture"<<endl;
pCam->StartCapture();
}
void CeyegolfDlg::OnBnClickedPlayBufferBtn()
{
	// need to make sure we are not already playing back
	
	PlaybackBuffer();
}

void CeyegolfDlg::OnEnChangeFpsEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	cout<<"FPS Changed"<<endl;
	CString s;
	FPS_EDIT.GetWindowTextW(s);
	FPS_Playback = _ttoi(s);
}

void CeyegolfDlg::OnBnClickedReplayBtn()
{
	// TODO: Add your control notification handler code here
	if (soundRecordRunning)
		{
		soundRecordRunning = false;
		
		m_Replay_BTN.SetWindowText(_T("Sound Trigger Off"));
		
		}
	else
	{
		soundRecordRunning = true;
		m_Replay_BTN.SetWindowText(_T("Sound Trigger On"));
	}
}

void CeyegolfDlg::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
	int selected = m_Fps_Option.GetCurSel();
	if (selected != 0)
	{
		FPS_Playback = selected * 10;
	}
	else
	{
		FPS_Playback = 5;
	}
}
