
      //----------------------------------------------------------------------

      // 概要       : PS3EyeLibを用いたキャプチャサンプル

      // ファイル名 : CapturePS3Eye.cpp

      // 使用ライブラリ: 

      //  OpenCV for MS-Windows 1.1

      //  http://sourceforge.net/projects/opencvlibrary/

      //

      //  PS3EyeLib

      //  http://alexpopovich.wordpress.com/2008/10/20/ps3eye-camera-sdk-for-use-in-your-own-applications/

      //

      // Author    : dandelion

      // Date      : Oct 22, 2008; 20:00

      //----------------------------------------------------------------------


	  //TODO:
	// Figure out how to make ring buffer
	// figure out how to display FPS
	// figure out audio queue to start recording, and pull from ringbuffer.  May want to just start a 2nd buffer and stitch 
	// frames together as mem alloc may be slow.
       

      

      #include "stdafx.h"

       #include <iostream>

      //ヘッダインクルード(OpenCV関連)

      #include <cv.h>

      #include <cxcore.h>

      #include <highgui.h>

       

      //ヘッダインクルード(PS3EyeLib関連)

      #include "IPS3EyeLib.h"

       

      //ライブラリ読み込み(OpenCV関連)

      #pragma comment(lib,"cv.lib")

      #pragma comment(lib,"cxcore.lib")

      #pragma comment(lib,"highgui.lib")

       

      //ライブラリ読み込み(PS3EyeLib関連)

      #pragma comment(lib,"PS3EyeLib.lib")

       

      #define CAPTURE_WIDTH  320 //キャプチャサイズ(幅)

      #define CAPTURE_HEIGHT 240 //キャプチャサイズ(高さ)

      #define FPS             100 //フレームレート

      #define COLOR_DEPTH     24 //depth

       

      using namespace std;

       

      // 利用可能フォーマット表示関数（プロトタイプ宣言）

      void displayAvailableFormats(void);

       

      int main(int argc, char *argv[])

      {

        int key = 0;
		int lastkey = 0;

		int rate = 0;       

        //キャプチャ画像

        IplImage *image = cvCreateImage(cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), IPL_DEPTH_8U, 3);

       

        //ウィンドウ

        char* window_name = "Capture using IPS3EyeLib";

        cvNamedWindow(window_name, CV_WINDOW_AUTOSIZE);

       

        // PS3EyeLibオブジェクト生成

        IPS3EyeLib *pCam = IPS3EyeLib::Create();

       

        displayAvailableFormats();
        pCam->SetFormat(IPS3EyeLib::GetFormatIndex(CAPTURE_WIDTH,CAPTURE_HEIGHT,FPS));
        //PBYTE pBuffer=new BYTE[(CAPTURE_WIDTH*CAPTURE_HEIGHT*COLOR_DEPTH)/8];

        CvVideoWriter* aviOut2 = NULL;
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
			//memcpy(image->imageData, pBuffer, image->imageSize);
			//cvPutText (image,(char*)nFramesPerSecond,cvPoint(200,400), &font, cvScalar(255,255,0));
			long fTimeDifference=GetTickCount()-dwTimeStart;         
			char buff[128];
			int nFramesPerSecond =(int)(1000.f/fTimeDifference);
			//int nFramesPerSecond=1000/fTimeDifference;
			sprintf( buff, "%d", nFramesPerSecond );
			cvPutText (image,buff,cvPoint(30,30), &font, cvScalar(255,255,0));
			cvShowImage(window_name, image);

			}

          if (record == true )
			{
			if (movInit == true)
				{
				cvWriteFrame(aviOut2, image);
				}
			else
				{
				cout << "Creating video Writer "<<endl;
				CvVideoWriter* aviOut2 = cvCreateVideoWriter("output.avi", CV_FOURCC('D', 'I', 'V', 'X'),FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 
				movInit = true;
				}
			//CvVideoWriter* aviOut2 = cvCreateVideoWriter("output.avi", CV_FOURCC_PROMPT,FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1); 
			//CvVideoWriter* aviOut2 = cvCreateVideoWriter("output.avi", CV_FOURCC('D', 'I', 'V', '3'),FPS, cvSize(CAPTURE_WIDTH, CAPTURE_HEIGHT), 1);
        
			 
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
				cvReleaseVideoWriter(&aviOut2);
			  }
			}


          
          key = cvWaitKey(1);
		  if (key != lastkey)
			{
			  cout << lastkey<<endl;
			  }
		  lastkey = key;
		
        }

        pCam->StopCapture();

        delete pCam;

       

        // 後処理（OpenCV関連）

        cvDestroyWindow(window_name);

        cvReleaseImage(&image);
		


        return 0;

      }

       

      // 利用可能フォーマット表示関数

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

