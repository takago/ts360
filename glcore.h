#ifndef GLCORE_H_
#   define GLCORE_H_
#   include <opencv/cv.h>
#   include <opencv/highgui.h>
#   include <GL/freeglut.h>

#   define UPDATE_PRERIOD_MS 10 // テクスチャを張り替えるときの間隔[ms]

extern IplImage *pimage; // 読みだしたパノラマ画像
extern CvCapture *capture;
extern CvVideoWriter *VideoWriter;

enum playmode {
	JPG_PANO,     // JPEGファイル（パノラマ）
	VFILE_PANO,   // ビデオファイル（パノラマ）
	VFILE_FEYES,  // ビデオファイル（魚眼）, MJPEGストリーマ
	USBCAM_FEYES, // ライブカメラ(魚眼)
};




void MainLoop(enum playmode pm);

#endif
