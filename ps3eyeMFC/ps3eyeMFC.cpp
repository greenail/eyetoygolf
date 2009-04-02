
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

	

	char* window_name = "Capture using IPS3EyeLib";

	cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);


	IPS3EyeLib *pCam = IPS3EyeLib::Create();



	displayAvailableFormats();
	pCam->SetFormat(IPS3EyeLib::GetFormatIndex(CAPTURE_WIDTH,CAPTURE_HEIGHT,FPS));
	//PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8];

	//CvVideoWriter* aviOut2 = NULL;
	CvVideoWriter* aviOut2 = cvCreateVideoWriter("output2.avi", CV_FOURCC('D', 'I', 'V', 'X'),FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 

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
	//CCircularBuffer< PBYTE, 50 > ringBuf;

	// my attempt at a "ring buffer"
	int bufSize = 200;
	vector<uchar*> ringBuf(0);

	cout<< "Ring Buffer size is: "<< bufSize<<" "<<endl;
	// end expiriment
	

	//ESC to quit
	int damnCount = 0;
	PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8]; 
	while(key != 0x1b)
	{
		DWORD dwTimeStart=GetTickCount();
		IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);
		if(pCam->GetFrame((PBYTE)image->imageData, COLOR_DEPTH, false, true))
		{
			
			
			

			long fTimeDifference=GetTickCount()-dwTimeStart;         
			char buff[128];
			int nFramesPerSecond =(int)(1000.f/fTimeDifference);
			//int nFramesPerSecond=1000/fTimeDifference;
			sprintf( buff, "%d", nFramesPerSecond );
			if (record == true )
			{
				if (damnCount >= bufSize)
				{
					key = 32;
					cout << " -BUFFER FULL- ";
				}
				if (movInit == true)
				{
					// move write frame to end when buffer is full.
					
					
					
					// expiriment to write 3 seconds of video to ring buffer then quit
			
					//damnCount = ringBuf.Write((PBYTE &)image->imageData,ringBuf.get_countUsed()+1);
					//memcpy(image->imageData, pBuffer, image->imageSize); 
					//memcpy(pBuffer,image->imageData,image->imageSize);
					int iSize = sizeof *image;
					cout << "SizeOf image is: " << iSize <<endl;
					//IplImage img2 = cvCloneImage(image);
					//char *foo = image->imageData;
					char *id = image->imageData;
					ringBuf.push_back(*id.data());
					//cvReleaseImage(&img2);
					cvWriteFrame(aviOut2, image);
					cout << "."<<damnCount<<".";
					damnCount++;
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
					cout << "Attemting to create video "<<endl;
					CvVideoWriter* aviOut = cvCreateVideoWriter("output.avi", CV_FOURCC('D', 'I', 'V', 'X'),FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 
	
					// try to pull images out of ring buffer
					
					//IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);
					

					cout << "Trying to write video from ring buffer, buffer size is: "<< ringBuf.size() <<"  ..." <<endl;

					for (unsigned int i = 0;i < ringBuf.size();i++)
					{
						cout << "Writing frame: " << i << " buffer size is: "<< ringBuf.size() <<"  ..." <<endl;
						//ringBuf.Read((PBYTE &)image->imageData);
						int rSize = sizeof ringBuf[i];
						if (rSize > 0)
						{
							cout << "SizeOf vector object is: " << rSize << endl;
							//IplImage img2 = *ringBuf[i];
							//cout << "actual ring buffer object: " << img2->imageSize << endl;
							//cout << "actual ring buffer object: " << ringBuf[i] << endl;
							IplImage *img2 = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);;
							memcpy(img2->imageData,ringBuf[i],image->imageSize);
							cvWriteFrame(aviOut, img2);

							//should break
							cvReleaseImage(&img2);
							//cvShowImage(window_name, &ringBuf[i]);
							//cvReleaseImage(img2);
						}
						// should break
						//cvReleaseImage(&image);
					}	
					ringBuf.clear();
					damnCount = 0;
					cout << endl << "Releasing video writer, should flush file write, buffer size is: "<< damnCount <<"  ..." << endl;
					cvReleaseVideoWriter(&aviOut);
					cvReleaseVideoWriter(&aviOut2);
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
			cvReleaseImage(&image);
		}
	}
	//cout << ringBuf.get_countUsed() <<endl;
	

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


