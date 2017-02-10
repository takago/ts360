/* 2017/01/13 Takago Lab translate_movie.c ver:1.02

鷹合研究室
指導教員：鷹合 大輔
製作者：
砂田 峻志　b1366879@planet.kanazawa-it.ac.jp
福田 凌平　b1321344@planet.kanazawa-it.ac.jp
涌嶋 和希　b1302947@planet.kanazawa-it.ac.jp

*/


// icompatible with output codec id
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "ts360.h"
#include "glcore.h"
#include "translate_movie.h"
#include "fish_translate.h"
#include "panorama.h"

//#define DEBUG_CONVERT // 変換中の画面を表示するときはコメントアウト

void translate_movie(char *in_filename, char *out_filename)
{

	CvCapture *capture;
	CvVideoWriter *vw;

	IplImage *frame;			//動画の画像データ
	IplImage *pano = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);	//パノラマ画像

	//読み込み 
	if ((capture = cvCaptureFromFile(in_filename)) == NULL) {
        printf("ファイルが見つかりませんでした。\n");
		return;
	}
	fish_translate(SZ*resolution_size);//変換マップの生成


	//書き込み準備
	vw =
	  cvCreateVideoWriter(out_filename, CV_FOURCC('X', 'V', 'I', 'D'), 30,
						  cvSize(WIDTH, HEIGHT), 1);
    if(vw==NULL){
        printf("ファイルを出力できません\n");        
        return;
    }
    

#ifdef DEBUG_CONVERT
	//動画再生用の窓を作成
	cvNamedWindow("CaptureFromFile", CV_WINDOW_AUTOSIZE);
#endif
   
	while (1) {
		//動画ファイルからフレーム画像の取込
		frame = cvQueryFrame(capture);

		if (frame == NULL) {
			break;
		}

		//パノラマ画像生成
		panorama(frame, pano);
		//画像の書き込み    
		cvWriteFrame(vw, pano);
#ifdef DEBUG_CONVERT
        //画像の表示
        cvShowImage ("CaptureFromFile", pano);

        //キー入力待ち（Escキーで終了）
        if (cvWaitKey (2) == '\x1b'){
            break;
        }
#endif

	}


	//解放
	cvReleaseImage(&pano);
	cvReleaseCapture(&capture);
#ifdef DEBUG_CONVERT
    cvDestroyWindow("CaptureFromFile");
#endif
	cvReleaseVideoWriter(&vw);

}
