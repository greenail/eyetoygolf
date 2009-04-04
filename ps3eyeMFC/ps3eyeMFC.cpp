
//----------------------------------------------------------------------
// Date      : 3/20/09
//----------------------------------------------------------------------
// credits, started with blatent rip of dandilion's blog post.  also used plenty of opencv examples.

//TODO:
// Figure out how to make ring buffer
// figure out how to display FPS
// figure out audio queue to start recording, and pull from ringbuffer.  May want to just start a 2nd buffer and stitch 
// frames together as mem alloc may be slow.




#include "stdafx.h"
//#include "Ring_Buffer.h"
#include "CircularBuffer.h"
#include <iostream>
#include <vector>
#include <cv.h>
#include <cxcore.h>

#include <windows.h>
#include <sys/timeb.h>

#include <highgui.h>
#include "IPS3EyeLib.h"

// put in wave fft crap

//#include "waveInFFT.h"
#include "waveInFFTDlg.h"
#include "fourier.h"
#include <math.h>
#include ".\waveinfftdlg.h"


#pragma comment(lib,"cv.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"highgui.lib")




#pragma comment(lib,"PS3EyeLib.lib")



#define CAPTURE_WIDTH  320 
#define CAPTURE_HEIGHT 240 
#define FPS             100 
#define COLOR_DEPTH     24 

/*
#define CAPTURE_WIDTH  640 
#define CAPTURE_HEIGHT 480 
#define FPS             50
#define COLOR_DEPTH     24 
*/


using namespace std;

#define mag_sqrd(re,im) (re*re+im*im)
#define Decibels(re,im) ((re == 0 && im == 0) ? (0) : 10.0 * log10(double(mag_sqrd(re,im))))
#define Amplitude(re,im,len) (GetFrequencyIntensity(re,im)/(len))
#define AmplitudeScaled(re,im,len,scale) ((int)Amplitude(re,im,len)%scale)





// function prototypes
BOOL Process(void* lpData, LPWAVEHDR pwh);
void displayAvailableFormats(void);
DWORD WINAPI ThreadProc(LPVOID lpParameter);


int main(int argc, char *argv[])

{
	// key to toggle recording
	int key = 0;
	int lastkey = 0;
	int rate = 0;       
	// opencv window
	char* window_name = "Capture using IPS3EyeLib";
	cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);
	// FPS draw crap
	CvFont font;
	double hScale=1.0;
	double vScale=1.0;
	int    lineWidth=1;
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
	// control stuff
	bool recordStarted = false;
	bool movInit = false;
	bool threadStarted = false;
	bool record = false;
	// thread stuff
	DWORD dwThreadID;
	HANDLE hThread = NULL;

	cout<< "Creating ring buffer "<<endl;
	int bufSize = 200;
	CCircularBuffer< uchar*, 200 > ringBuf;
	cout<< "Ring Buffer size is: "<< bufSize<<" "<<endl;
	cout << "Pre allocating memory for video" << endl;
	for (int i = 0; i < bufSize;i++)
	{
		PBYTE tempBuf=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8]; 
		ringBuf.Write(tempBuf);
	}
	// Initialize our image, destroyed after main loop exits.
	IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);
	
	// FPS stuff
	double		m_timePrevFrameStamp = 0;
	double		m_timeCurrFrameStamp = 0;
	double		m_nFps = 0.1;
	double		m_nFpsAlpha = 0.1;
	struct timeb timeStamp;
	// buffer for printing FPS
	char buff[128];
	// create copy buffer
	PBYTE pBuffer = NULL;
	// fire up the eyetoy
	IPS3EyeLib *pCam = IPS3EyeLib::Create();
	displayAvailableFormats();
	pCam->SetFormat(IPS3EyeLib::GetFormatIndex(CAPTURE_WIDTH,CAPTURE_HEIGHT,FPS));
	pCam->StartCapture();
	//ESC to quit
	while(key != 0x1b)
	{
		// for FPS display
		DWORD dwTimeStart=GetTickCount();
		if(pCam->GetFrame((PBYTE)image->imageData, COLOR_DEPTH, false, true))
		{
			// calculate FPS
			ftime( &timeStamp );
			m_timeCurrFrameStamp = (double) timeStamp.time*1000 + timeStamp.millitm;
			if( m_nFps < 0 )
			{
				m_nFps = 1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
			} else
			{	
				m_nFps = ( 1 - m_nFpsAlpha ) * m_nFps + m_nFpsAlpha * 
							1000 / ( m_timeCurrFrameStamp - m_timePrevFrameStamp );
			}
			// set current time stamp as previuos
			m_timePrevFrameStamp = m_timeCurrFrameStamp;
			sprintf(buff, "FPS: %5.1f", m_nFps );
			// do recording fun stuff
			if (record == true )
			{
				if (movInit == true)
				{
					if (hThread == NULL)
						{
						// start thread
						cout<< "Create thread to listen for sound trigger"<<endl;
						LPVOID pRecord = &record;
						hThread = CreateThread(0,0,ThreadProc,pRecord,record,&dwThreadID);
						threadStarted = true;
						}
					else
					{
						//check to see if the listener thread stopped because it found a sound
						DWORD ret = WaitForSingleObject(hThread,0);
						if (ret != WAIT_TIMEOUT )
							{
							key = 32;
							threadStarted = false;
							}
					}
					

					// for some reason need to read to pop off of ring buffer, may be a better way to do this
					//cout<< "?";
					ringBuf.Read(pBuffer);
					//if ((int)ringBuf.get_countUsed() < (int)ringBuf.get_maxItems()){cout << "-" ;}
					// copy data from imageData pointer to pBuffer
					memcpy(pBuffer,image->imageData,image->imageSize);
					//push onto our ring buffer
					//cout<< "+";
					ringBuf.Write(pBuffer);
					
				}
				else
				{
					// not sure why I have this, maybe so I can setup the video writer each time.
					movInit = true;
				}
			}
			// SPace key to record, can be driven by sound
			if (key == 32)
			{
				record = true;
				if (recordStarted == false)
				{
					recordStarted = true;
				}
				else
				{
					// Pull data out and save it to file
					record = false;
					recordStarted = false;
					movInit = false;
					threadStarted = false;
					// close thread handle
					CloseHandle(hThread);
					// make sure it is null
					hThread = NULL;
					cout << "Attemting to create video writer"<<endl;
					// create ffmpeg video writer
					CvVideoWriter* aviOut = cvCreateVideoWriter("output.avi", CV_FOURCC('P','I','M','1'),30, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 
					cout << "Trying to write video from ring buffer, buffer size is: "<< ringBuf.get_countUsed() <<"  ..." <<endl;
					// create image buffer
					IplImage *img2 = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);;
					int bufCount = ringBuf.get_countUsed();	
					for (int i = 0 ; i < bufCount ; i++)
					{
						// copy data 
						uchar * iPointer = NULL;
						ringBuf.Read(iPointer);
						//cout << "Read: "<< ringBuf.get_countUsed() <<"of "<< bufCount<<"..";
						memcpy(img2->imageData,iPointer,image->imageSize);
						// write frame to video
						cvWriteFrame(aviOut, img2);
						// free memory
						delete [] iPointer;
					}	
					// hack to refill buffer
					for (int i = 0; i < bufSize;i++)
						{
							PBYTE tempBuf=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8]; 
							ringBuf.Write(tempBuf);
						}

					cvReleaseImage(&img2);
					cvReleaseVideoWriter(&aviOut);
					const char * mboxMsg = "Do you want to save and view this swing?";
					CStringW wName( mboxMsg ); 
					/* Need to do this in a thread... PITA
					DWORD mRes =  MessageBox(NULL, wName, wName, MB_OK);
					if (mRes == IDOK)
						{
						// button ok has been pressed
						}
					*/
					//start recording again....
					record = true;
				}
			}
			/* printing key codes since I don't know them
			if (key != lastkey)
			{
				cout << lastkey<<endl;
			}
			*/
			// record last key stroke for next loop
			lastkey = key;
			// Draw FPS data on preview image
			cvPutText (image,buff,cvPoint(30,30), &font, cvScalar(255,255,0));
			// draw preview image
			cvShowImage(window_name, image);
			// check for next key stroke 1ms delay
			key = cvWaitKey(1);
		}
	}
	// clean up
	cvReleaseImage(&image);
	pCam->StopCapture();
	delete pCam;
	cvDestroyWindow(window_name);
	// exit
	return 0;
}




void displayAvailableFormats(void)

{
	cout << "version .02" <<endl;
	cout <<"[Available Formats]" <<endl;

	for(int i=0; i<IPS3EyeLib::GetNumFormats(); i++)

	{

		int width, height, rate;
		char *description;



		width       = IPS3EyeLib::GetFormats()[i].width;

		height      = IPS3EyeLib::GetFormats()[i].height;

		rate        = IPS3EyeLib::GetFormats()[i].rate;

		description = IPS3EyeLib::GetFormats()[i].formatTxt;

		cout <<description <<endl;



	}
} 

BOOL Process(void* lpData, LPWAVEHDR pwh)
{
#define FFT_LEN 2048
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
	for(int i=1;i < FFT_LEN/4;i++)//Use FFT_LEN/4 since the data is mirrored within the array.
	{
		re = (float)fout[i];
		im = (float)foutimg[i];
		//get Decibels in 0-110 range
		decibelLevel = Decibels(re,im);
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
	// Detect Decibel level above threashold
	if (decibelLevel > 70)
	{
		cout<< "Decible Hit!!!!!!!!!!!!!!!"<<endl;
		Sleep(500);
		return FALSE;
		
	}
	return TRUE;
}
DWORD WINAPI ThreadProc(LPVOID lpParameter) 
{
bool running = TRUE;
bool started = false;
DataHolder m_data;
Recorder m_rec;
while (running)
{
	Sleep(4);
	if (lpParameter)
	{
	if (started == false)
		{
		cout<< "Starting recorder"<<endl;
		m_rec.Open();
		m_rec.SetBufferFunction((void*)&m_data,Process);
		m_rec.Start();
		started = true;
		}
	else
		{
		bool recording = m_rec.IsRecording();
		if (recording)
			{
			m_rec.SetBufferFunction((void*)&m_data,Process);
			m_rec.Start();
			}
		else
			{
			cout<<"Sound listener stopped"<<endl;
			m_rec.Stop();
			running = FALSE;
			}
		}
	}
	else
	{
		cout<<"Sound listener stopped"<<endl;
		m_rec.Stop();
		running = FALSE;
	}
}
  return 0;
}