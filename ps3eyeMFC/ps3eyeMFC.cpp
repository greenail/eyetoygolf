
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



using namespace std;

#define mag_sqrd(re,im) (re*re+im*im)
#define Decibels(re,im) ((re == 0 && im == 0) ? (0) : 10.0 * log10(double(mag_sqrd(re,im))))
#define Amplitude(re,im,len) (GetFrequencyIntensity(re,im)/(len))
#define AmplitudeScaled(re,im,len,scale) ((int)Amplitude(re,im,len)%scale)

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
	//Perform FFT on left channel
	/*  don't need left channel for decible measure...

	fft_double(FFT_LEN/2,0,finleft,NULL,fout,foutimg);
	float re,im,fmax=-99999.9f,fmin=99999.9f;
	for(int i=1;i < FFT_LEN/4;i++)//Use FFT_LEN/4 since the data is mirrored within the array.
	{
		re = (float)fout[i];
		im = (float)foutimg[i];
		//get amplitude and scale to 0..256 range
		//fdraw[i]=AmplitudeScaled(re,im,FFT_LEN/2,256);
		fdraw[i] = ((int)mag_sqrd(re,im))%256;
		if (fdraw[i] > fmax)
		{
			fmax = (float)fdraw[i];
		}
		if (fdraw[i] < fmin)
		{
			fmin = (float)fdraw[i];
		}
	}
	//Use this to send the average band amplitude to something
	int nAvg, nBars=16, nCur = 0;
	for(int i=1;i < FFT_LEN/4;i++)
	{
		nAvg = 0;
		for (int n=0; n < nBars; n++)
		{
			nAvg += (int)fdraw[i];
		}
		nAvg /= nBars;
		//Send data here to something,
		//nothing to send it to so we print it.
		TRACE("Average for Bar#%d is %d\n",nCur++,nAvg);
		i+=nBars-1;
	}
	*/
	
	
	// Perform FFT on right channel
	//fmax=-99999.9f,fmin=99999.9f;
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
		
	}
	return TRUE;
	/*else
	{
		return false;
	}
	*/
}



void displayAvailableFormats(void);



int main(int argc, char *argv[])

{

	int key = 0;
	int lastkey = 0;

	int rate = 0;       

	

	char* window_name = "Capture using IPS3EyeLib";

	cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);


	IPS3EyeLib *pCam = IPS3EyeLib::Create();



	displayAvailableFormats();
	pCam->SetFormat(IPS3EyeLib::GetFormatIndex(CAPTURE_WIDTH,CAPTURE_HEIGHT,FPS));
	//PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8];

	//CvVideoWriter* aviOut2 = NULL;
	//CvVideoWriter* aviOut2 = cvCreateVideoWriter("output2.avi", CV_FOURCC('D', 'I', 'V', 'X'),FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 

	pCam->StartCapture();

	bool record = false;
	//start time
	//count << "Increment for GetTick " <<GetSystemTimeAdjustment()<<endl;


	// setup fps string
	CvFont font;
	double hScale=1.0;
	double vScale=1.0;
	int    lineWidth=1;
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);




	
	bool recordStarted = false;
	bool movInit = false;

	// expiriment create buffer to store 300 frames, or 3 seconds

	
	// note copy each flat is true, may be slow...

	//CCircularBuffer< IplImage, 300,true > ringBuf;

	cout<< "Creating ring buffer "<<endl;
	//
	// my attempt at a "ring buffer"
	int bufSize = 200;
	//vector<uchar*> ringBuf(0);
	CCircularBuffer< uchar*, 200 > ringBuf;

	cout<< "Ring Buffer size is: "<< bufSize<<" "<<endl;

	cout << "Pre allocating memory for video" << endl;
	for (int i = 0; i < bufSize;i++)
	{
		PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8]; 
		ringBuf.Write(pBuffer);
	}
	// end expiriment
	

	//ESC to quit
	int damnCount = 0;
	
	//int maxX = (CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8;
	IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);
	DataHolder m_data;
	Recorder m_rec;
	while(key != 0x1b)
	{
		DWORD dwTimeStart=GetTickCount();
		//IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);
		if(pCam->GetFrame((PBYTE)image->imageData, COLOR_DEPTH, false, true))
		{
			
			
			

			long fTimeDifference=GetTickCount()-dwTimeStart;         
			char buff[128];
			int nFramesPerSecond =(int)(1000.f/fTimeDifference);
			//int nFramesPerSecond=1000/fTimeDifference;
			sprintf( buff, "%d", nFramesPerSecond );
			if (record == true )
			{

				/* need to create ring buffer not short circuit static one
				if (damnCount >= bufSize)
				{
					key = 32;
					cout << " -BUFFER FULL- ";
				}*/
				if (movInit == true)
				{
					//int iSize = sizeof *image;
					//cout << "SizeOf image is: " << iSize <<endl;
					
					m_rec.Open();
					m_rec.SetBufferFunction((void*)&m_data,Process);
					m_rec.Start();

					// Create new memory location for image, need to prealocate this in a ring buffer
					//PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8]; 
					
					PBYTE pBuffer = NULL;
					
					ringBuf.Read(pBuffer);
					if ((int)ringBuf.get_countUsed() < (int)ringBuf.get_maxItems()){cout << "-" ;}

					// copy data from imageData pointer to pBuffer
					memcpy(pBuffer,image->imageData,image->imageSize);
					//push onto our ring buffer
					
					//ringBuf.push_back(pBuffer);
					ringBuf.Write(pBuffer);
					if ((int)ringBuf.get_countUsed() < (int)ringBuf.get_maxItems()){cout << "." ;}


					// write other avi and clean up
					//cvWriteFrame(aviOut2, image);
					//cout << "."<<damnCount<<".";

					//delete pointer?
					//delete pBuffer;
					damnCount++;
				}
				else
				{
					
					movInit = true;
				}
			}
			// SPace key to record
			if (key == 32)
			{
				record = true;
				if (recordStarted == false)
				{
					recordStarted = true;
				}
				else
				{
					// Need some way to sleep .5 seconds or so to get complete swing
					//
					record = false;
					recordStarted = false;
					movInit = false;
					m_rec.Stop();
					cout << "Attemting to create video writer"<<endl;

					// create ffmpeg video writer

					CvVideoWriter* aviOut = cvCreateVideoWriter("output.avi", CV_FOURCC('D', 'I', 'V', 'X'),30, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 
	
					// try to pull images out of ring buffer
					
					//IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);
					

					cout << "Trying to write video from ring buffer, buffer size is: "<< ringBuf.get_countUsed() <<"  ..." <<endl;

					//for (unsigned int i = 0;i < ringBuf.size();i++)
					int bufCount = ringBuf.get_countUsed();
					for (int i = 0 ; i < bufCount ; i++)
					{
						//cout << "Writing frame: " << i << " buffer size is: "<< ringBuf.size() <<"  ..." <<endl;
						//ringBuf.Read((PBYTE &)image->imageData);
						//int rSize = sizeof ringBuf[i];
						//cout << "SizeOf vector object is: " << rSize << endl;
							
						// Create image, not sure if I need to dealocate this each iteration or not, may be faster not to.
						IplImage *img2 = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);;
							
						// copy data 
						uchar * iPointer = NULL;
						ringBuf.Read(iPointer);
						cout << "Read: "<< &iPointer<< " with count: "<< ringBuf.get_countUsed() <<"  ..." <<endl;
						memcpy(img2->imageData,iPointer,image->imageSize);
							
						// write frame to video
						cvWriteFrame(aviOut, img2);
						//delete [] iPointer;
						//release image, may not be nessisary
						cvReleaseImage(&img2);
							
						
						
					}	
					// clear vector, need to change to ring buffer
					//ringBuf.clear();
					damnCount = 0;
					//cout << endl << "Releasing video writer, should flush file write, buffer size is: "<< damnCount <<"  ..." << endl;
					cvReleaseVideoWriter(&aviOut);
					//cvReleaseVideoWriter(&aviOut2);
				}
			}



			
			if (key != lastkey)
			{
				cout << lastkey<<endl;
			}
			lastkey = key;
			cvPutText (image,buff,cvPoint(30,30), &font, cvScalar(255,255,0));
			cvShowImage(window_name, image);
			
			
			
			key = cvWaitKey(1);
			//cvReleaseImage(&image);
		}
	}
	//cout << ringBuf.get_countUsed() <<endl;
	cvReleaseImage(&image);

	// pause for a while

	//key = cvWaitKey(5000);

	//cvReleaseVideoWriter(&aviOut);
	pCam->StopCapture();

	delete pCam;




	cvDestroyWindow(window_name);

	//cvReleaseImage(&image);



	return 0;

}




void displayAvailableFormats(void)

{
	cout << "version .01" <<endl;
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



