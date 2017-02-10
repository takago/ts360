/* 2017/01/10 Takago Lab theta_stream.c ver:1.01

鷹合研究室
指導教員：鷹合 大輔
製作者：
砂田 峻志　b1366879@planet.kanazawa-it.ac.jp
福田 凌平　b1321344@planet.kanazawa-it.ac.jp
涌嶋 和希　b1302947@planet.kanazawa-it.ac.jp

*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "ts360.h"
#include "glcore.h"



int theta_convert_movie(char *filename)
{
	//動画読み込み
	if ((capture = cvCaptureFromFile(filename)) == NULL) {
		//ファイルが見つからない時
		printf("ファイルが見つかりませんでした。\n");
        exit(1);        
	}
    
    MainLoop(VFILE_FEYES);

	cvReleaseCapture(&capture);
	cvDestroyWindow("CaptureFromFile");
}


int theta_image(char *filename)
{
	pimage = cvLoadImage(filename, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);	//パノラマ画像読み込み
	if (pimage == NULL) {
		printf("ファイルが見つかりません\n");
        exit(1);        
	}

    MainLoop(JPG_PANO);	
    cvReleaseImage(&pimage);
}



int theta_live(int num, char **str)
{

	//カメラ読み込み
	if ((capture = cvCreateCameraCapture(num)) == NULL) {
		printf("カメラが見つかりませんでした\n");
        exit(1);        
	}

    MainLoop(USBCAM_FEYES);
    
	cvReleaseCapture(&capture);
	cvDestroyWindow("CaptureFromFile");

}


int theta_movie(char *filename)
{

	printf("%s\n", filename);
	//動画読み込み
	if ((capture = cvCaptureFromFile(filename)) == NULL) {
		//ファイルが見つからない時
		printf("ファイルが見つかりませんでした。\n");
        exit(1);        
	}
	MainLoop(VFILE_PANO);

	cvReleaseCapture(&capture);
	cvDestroyWindow("CaptureFromFile");

}


int theta_stream(char *ip_port)
{
	//ストリーミング準備
	if ((capture = cvCaptureFromFile(ip_port)) == NULL) {
		printf("Check Streaming Server!!\n");
        exit(1);        
	}

	
    MainLoop(VFILE_FEYES);

	cvReleaseCapture(&capture);
	cvDestroyWindow("CaptureFromFile");

}



