
//----------------------------------------------------------------------
// Date      : Oct 22, 2008; 20:00
//----------------------------------------------------------------------


//TODO:
// Figure out how to make ring buffer
// figure out how to display FPS
// figure out audio queue to start recording, and pull from ringbuffer.  May want to just start a 2nd buffer and stitch 
// frames together as mem alloc may be slow.




#include "stdafx.h"

#include <iostream>

#include <cv.h>

#include <cxcore.h>

#include <highgui.h>
#include "IPS3EyeLib.h"
#pragma comment(lib,"cv.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"highgui.lib")




#pragma comment(lib,"PS3EyeLib.lib")



#define CAPTURE_WIDTH  320 

#define CAPTURE_HEIGHT 240 

#define FPS             100 

#define COLOR_DEPTH     24 



using namespace std;
void displayAvailableFormats(void);



int main(int argc, char *argv[])

{

	int key = 0;
	int lastkey = 0;

	int rate = 0;       

	IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);

	char* window_name = "Capture using IPS3EyeLib";

	cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);


	IPS3EyeLib *pCam = IPS3EyeLib::Create();



	displayAvailableFormats();
	pCam->SetFormat(IPS3EyeLib::GetFormatIndex(CAPTURE_WIDTH,CAPTURE_HEIGHT,FPS));
	//PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8];

	//CvVideoWriter* aviOut2 = NULL;
	CvVideoWriter* aviOut = cvCreateVideoWriter("output.avi", CV_FOURCC('D', 'I', 'V', 'X'),FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 

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




	//ESC to quit
	bool recordStarted = false;
	bool movInit = false;
	while(key != 0x1b)
	{
		DWORD dwTimeStart=GetTickCount();
		if(pCam->GetFrame((PBYTE)image->imageData, COLOR_DEPTH, false, true))
		{
			long fTimeDifference=GetTickCount()-dwTimeStart;         
			char buff[128];
			int nFramesPerSecond =(int)(1000.f/fTimeDifference);
			//int nFramesPerSecond=1000/fTimeDifference;
			sprintf( buff, "%d", nFramesPerSecond );
			if (record == true )
			{
				if (movInit == true)
				{
					cvWriteFrame(aviOut, image);
					//cout << ".";
				}
				else
				{
					cout << "Creating video Writer "<<endl;
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
					record = false;
					recordStarted = false;
					movInit = false;
					cout << "Completing video Writer "<<endl;
					cvReleaseVideoWriter(&aviOut);
				}
			}



			key = cvWaitKey(1);
			if (key != lastkey)
			{
				cout << lastkey<<endl;
			}
			lastkey = key;
			cvPutText (image,buff,cvPoint(30,30), &font, cvScalar(255,255,0));
			cvShowImage(window_name, image);

		}
	}

	pCam->StopCapture();

	delete pCam;




	cvDestroyWindow(window_name);

	cvReleaseImage(&image);



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



//----------------------------------------------------------------------
// Date      : Oct 22, 2008; 20:00
//----------------------------------------------------------------------


//TODO:
// Figure out how to make ring buffer
// figure out how to display FPS
// figure out audio queue to start recording, and pull from ringbuffer.  May want to just start a 2nd buffer and stitch 
// frames together as mem alloc may be slow.




#include "stdafx.h"

#include <iostream>

#include <cv.h>

#include <cxcore.h>

#include <highgui.h>
#include "IPS3EyeLib.h"
#pragma comment(lib,"cv.lib")
#pragma comment(lib,"cxcore.lib")
#pragma comment(lib,"highgui.lib")




#pragma comment(lib,"PS3EyeLib.lib")



#define CAPTURE_WIDTH  320 

#define CAPTURE_HEIGHT 240 

#define FPS             100 

#define COLOR_DEPTH     24 



using namespace std;
void displayAvailableFormats(void);



int main(int argc, char *argv[])

{

	int key = 0;
	int lastkey = 0;

	int rate = 0;       

	IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);

	char* window_name = "Capture using IPS3EyeLib";

	cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);


	IPS3EyeLib *pCam = IPS3EyeLib::Create();



	displayAvailableFormats();
	pCam->SetFormat(IPS3EyeLib::GetFormatIndex(CAPTURE_WIDTH,CAPTURE_HEIGHT,FPS));
	//PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8];

	//CvVideoWriter* aviOut2 = NULL;
	CvVideoWriter* aviOut = cvCreateVideoWriter("output.avi", CV_FOURCC('D', 'I', 'V', 'X'),FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 

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




	//ESC to quit
	bool recordStarted = false;
	bool movInit = false;
	while(key != 0x1b)
	{
		DWORD dwTimeStart=GetTickCount();
		if(pCam->GetFrame((PBYTE)image->imageData, COLOR_DEPTH, false, true))
		{
			long fTimeDifference=GetTickCount()-dwTimeStart;         
			char buff[128];
			int nFramesPerSecond =(int)(1000.f/fTimeDifference);
			//int nFramesPerSecond=1000/fTimeDifference;
			sprintf( buff, "%d", nFramesPerSecond );
			if (record == true )
			{
				if (movInit == true)
				{
					cvWriteFrame(aviOut, image);
					//cout << ".";
				}
				else
				{
					cout << "Creating video Writer "<<endl;
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
					record = false;
					recordStarted = false;
					movInit = false;
					cout << "Completing video Writer "<<endl;
					cvReleaseVideoWriter(&aviOut);
				}
			}



			key = cvWaitKey(1);
			if (key != lastkey)
			{
				cout << lastkey<<endl;
			}
			lastkey = key;
			cvPutText (image,buff,cvPoint(30,30), &font, cvScalar(255,255,0));
			cvShowImage(window_name, image);

		}
	}

	pCam->StopCapture();

	delete pCam;




	cvDestroyWindow(window_name);

	cvReleaseImage(&image);



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



