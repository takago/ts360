/* 2017/01/13 Takago Lab common.c ver:1.02

鷹合研究室
指導教員：鷹合 大輔
製作者：
砂田 峻志　b1366879@planet.kanazawa-it.ac.jp
福田 凌平　b1321344@planet.kanazawa-it.ac.jp
涌嶋 和希　b1302947@planet.kanazawa-it.ac.jp

*/
#include"ts360.h"
#include"glcore.h"
#include"panorama.h"
#include "fish_translate.h"


static int ww, hh;

static float pan, tilt;				// 回転量
static float zoom = 80;

static GLuint mFontTextureId;
static GLUquadric *sphere;


IplImage *pimage; // 読みだしたパノラマ画像
CvCapture *capture;
CvVideoWriter *VideoWriter;



/* 動作モード */
enum playmode pmode;

static int grid_flag;
static int rec_flag;
static int disp_flag_v;
static int disp_flag_p;
static int flip_flag;

static int mouse_x, mouse_y;
static void gridline(int grid_space_x, int grid_space_y, IplImage *source_img);
static void saveImage(int imageWidth, int imageHeight);

static void Keyboard(unsigned char key, int x, int y);
static void LoadTexture(int val);

// コールバック
static void mouse_func(int button, int state, int x, int y);
static void motion_func(int x, int y);
static void skey_func(int skey, int x, int y);

// 描画ルーチン
static void drawScene();

static void handleResize(int w, int h);
static void time_char(char*);

static void Rendering(IplImage *img);

// マウスを押した時のコールバック
static void mouse_func(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;

	switch (button) {	// ホイール
	case 2:
		zoom = 80;
		break;
	case 3:
		zoom--;
		break;
	case 4:
		zoom++;
		break;
	default:
		break;
	}
	glutPostRedisplay();	// 描画コールバックを呼び出す  
}

// マウスドラッグした時のコールバック
static void motion_func(int x, int y)
{
	pan += x - mouse_x;
	tilt += y - mouse_y;
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();	// 描画コールバックを呼び出す  
}

// 特殊キー操作時のコールバック
static void skey_func(int skey, int x, int y)
{
	switch (skey) {
	case GLUT_KEY_LEFT:
		pan -= 1;
		break;
	case GLUT_KEY_RIGHT:
		pan += 1;
		break;
	case GLUT_KEY_UP:
		tilt -= 1;
		break;
	case GLUT_KEY_DOWN:
		tilt += 1;
		break;
	case GLUT_KEY_PAGE_UP:
		zoom--;
		break;
	case GLUT_KEY_PAGE_DOWN:
		zoom++;
		break;
	}
	glutPostRedisplay();	// 描画コールバックを呼び出す
}

// 描画ルーチン
static void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// クリア

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(zoom, (float)ww / (float)hh, 0.5, 100.0);

	glMatrixMode(GL_MODELVIEW);	// 以降の処理はモデルを対象とする
	//////////////////////////////////////////////////////////////////////////
	glLoadIdentity();	// 単位行列
	gluLookAt(0, 0, 0, 0, 1, 0, 0.0, 0.0, 1.0);	// カメラ（固定）

	///////////////////////////////////////////////////////////////////////////
	// チルトしてからパンしないとうまく思ったように動かない・・・なぜ？
	glRotatef(tilt, 1, 0, 0);	// (回転角，回転軸)
	glRotatef(pan, 0, 0, 1);	// (回転角，回転軸)

	// 球
	glBegin(GL_QUAD_STRIP);	// STRIPをつけないと地球のそこに穴があく
	glColor3f(1, 1, 1);
	gluSphere(sphere, 2, 30, 30);	// xxxx, 半径， 経度方向分割数，緯度方向分割数
	glEnd();

	glutSwapBuffers();	// 実際の描画(ここで上述の演算を纏めて実施する)
}

static void handleResize(int w, int h)
{
	ww = w;
	hh = h;
	glViewport(0, 0, ww, hh);
}

static void saveImage(int imageWidth, int imageHeight)	//スクリーンショット
{
	IplImage *buf =
	  cvCreateImage(cvSize(imageWidth, imageHeight), IPL_DEPTH_8U, 3);
	IplImage *outImage =
	  cvCreateImage(cvSize(imageWidth, imageHeight), IPL_DEPTH_8U, 3);


	char outputimage[256];
	
	time_char(outputimage);

	strcat(outputimage, ".jpg");

	printf("Snapshot:%s\n", outputimage);

	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
				 buf->imageData);
	glFlush();
	cvConvertImage(buf, outImage, CV_CVTIMG_FLIP + CV_CVTIMG_SWAP_RB);


	cvSaveImage(outputimage, outImage, 0);
	cvReleaseImage(&buf);
	cvReleaseImage(&outImage);

}

static void time_char(char *name)
{
	time_t timer;
	struct tm *timeptr;
	timer = time(NULL);
	timeptr = localtime(&timer);
	strftime(name, 256, "%Y%m%d%H%M%S", timeptr);

}

static void gridline(int grid_space_x, int grid_space_y, IplImage *img)
{

	int n=0;
	
	// 経線
	for(n = (img->height/2) - (grid_space_y/2);n< img->height;n+=grid_space_y){ 
		cvLine(img, cvPoint(0, n), cvPoint(img->width, n), cvScalar(0,204,0), GRID_LINE_WIDTH, CV_AA);
	}
	for(n = (img->height/2) + (grid_space_y/2);n > 0;n-=grid_space_y){
		cvLine(img, cvPoint(0, n), cvPoint(img->width, n), cvScalar(0,204,0), GRID_LINE_WIDTH, CV_AA);
	}

	cvLine(img, cvPoint(0, img->height/2), cvPoint(img->width, img->height/2), cvScalar(204,0,0), GRID_LINE_WIDTH*2, CV_AA); // 赤道

    // 緯線
	for(n=0;n< img->width;n+=grid_space_x){ 
		cvLine(img, cvPoint(n, 0), cvPoint(n,img->height), cvScalar(0,204,0), GRID_LINE_WIDTH, CV_AA);
	}	

	
}

static void grid_reset(IplImage* img, int g_flg)//JPGにグリッド線を引く関数
{
   	IplImage *img_tmp = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);//グリッド用画像データ
		if (g_flg == 1){
		     cvCopy(img, img_tmp);
		     gridline(GRID_X, GRID_Y, img_tmp);	//グリッド線を引く
		}
		else{
		cvCopy(img, img_tmp);//元の画像に差し替える
		}
	Rendering(img_tmp);
	cvReleaseImage(&img_tmp);
}

/* 球に新しいテクスチャを貼る */
static void Rendering(IplImage *img)
{
    

	if (grid_flag == 1 && pmode != JPG_PANO){
		gridline(GRID_X, GRID_Y, img);	//グリッド線を引く
    }
    
    if (flip_flag == 1){
        cvFlip(img,0);
    }


	glActiveTexture(GL_TEXTURE0);

	glDeleteTextures(1, &mFontTextureId);	// 古いテクスチャを消す
	glGenTextures(1, &mFontTextureId);

	glBindTexture(GL_TEXTURE_2D, mFontTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );	// GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );	// GL_NEAREST
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,img->width, img->height, 0, GL_RGB,
				 GL_UNSIGNED_BYTE, img->imageData);

	// 球の生成
	if (sphere != NULL) {	// 古いオブジェクトを消す       
		gluDeleteQuadric(sphere);
	}
	sphere = gluNewQuadric();	// 曲面オブジェクトの生成

	gluQuadricTexture(sphere, GL_TRUE);	// テクスチャ座標を生成を有効化（重要）

    glutPostRedisplay(); // 再表示
}


static int num;
static char **str;

void MainLoop(enum playmode pm)
{
	// 変換マップの生成
	fish_translate(SZ*resolution_size);

	pmode=pm;	

	glutInit(&num, str);	//ダミー
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(DISP_WIDTH, DISP_HEIGHT);
	glutCreateWindow("PANORAMTIC IMAGE VIEWER (TAKAGO LAB.)");
	glClearColor(0.3, 0.3, 0.3, 1.0);	/* 背景色 */

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);	// 2Dテクスチャ有効化
	LoadTexture(0);	// テクスチャを貼る
    
	switch(pmode){ // 静止画以外のときは定期的に画像を差し替える
	case JPG_PANO:
		break;
	default:
		glutTimerFunc(UPDATE_PRERIOD_MS, LoadTexture, 0); //画像の差し替え
		break;
    }
    
	glutDisplayFunc(drawScene);
	glutReshapeFunc(handleResize);


	// カリングで高速化
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);	// 表面除去(球体内から見るので)
	glutKeyboardFunc(Keyboard); // キーが押されたら
	glutSpecialFunc(skey_func);	// 特殊キーが押されたら
	glutMouseFunc(mouse_func);	// マウスが押されたら
	glutMotionFunc(motion_func);	// マウスドラッグを検知したら

	// GLループ
	glutMainLoop();

	// 変換マップの削除
	cvReleaseMat(&mat_u);
	cvReleaseMat(&mat_v);
}

static void Keyboard(unsigned char key, int x, int y)
{
	char outputVideo[256];
	

	switch (key) {
	case 'q': //Qキー:終了
		exit(1);
	case 'g': //Gキー:グリッド線ON/OFF
		grid_flag = 1 - grid_flag;
		if (grid_flag )
			printf("グリッド表示 ON\n");
		else
			printf("グリッド表示 OFF\n");
		switch (pmode){
		case JPG_PANO:
		    grid_reset(pimage, grid_flag);		
			break;
		default:
			break;
		}		
		break;			
	case 's': //Sキー:スクリーンショット
		saveImage(DISP_WIDTH, DISP_HEIGHT);
		break;
	case 'r': //Rキー:録画ON/OFF
		rec_flag = 1 - rec_flag;
		switch (pmode){
		case USBCAM_FEYES:
		case VFILE_FEYES:
		case VFILE_PANO:
			if (rec_flag) {
				time_char(outputVideo);
				strcat(outputVideo, ".avi");
				VideoWriter =
					cvCreateVideoWriter(outputVideo, CV_FOURCC('X', 'V', 'I', 'D'),
										FPS, cvSize(WIDTH * resolution_size, HEIGHT * resolution_size), 1);
				printf("REC start:%s\n", outputVideo);
			}
			else {
				printf("REC stop\n");
				cvReleaseVideoWriter(&VideoWriter);
			}
			break;
		}		
		break;
	case 'v': //Vキー:パノラマ動画表示ON/OFF
		disp_flag_v = 1 - disp_flag_v;		
		switch (pmode){
		case USBCAM_FEYES:
		case VFILE_FEYES:
		case VFILE_PANO:		
			if(disp_flag_v ){
				cvNamedWindow("Stereo_Fisheye", CV_WINDOW_NORMAL);
				cvMoveWindow( "Stereo_Fisheye", 0, 0 );
				cvResizeWindow( "Stereo_Fisheye",DISP_WIDTH , DISP_HEIGHT );
			}
			else{
				cvDestroyWindow("Stereo_Fisheye");
			}			
			break;
		}		
		break;
	case 'p'://Pキー:デュアル魚眼動画表示ON/OFF
		disp_flag_p = 1 - disp_flag_p;		
		switch (pmode){
		case USBCAM_FEYES:
		case VFILE_FEYES:
		case VFILE_PANO:		
			if(disp_flag_p ){
				cvNamedWindow("Panorama", CV_WINDOW_NORMAL);
				cvMoveWindow( "Panorama", 0, 720 );
				cvResizeWindow( "Panorama",DISP_WIDTH , DISP_HEIGHT );
			}
			else{
				cvDestroyWindow("Panorama");
			}			
			break;
		default:
			break;			
		}
	case 'f':
		switch (pmode){
		case USBCAM_FEYES:
		case VFILE_FEYES:
		case VFILE_PANO:
			if( flip_flag ){
		            flip_flag = 1 - flip_flag;	
			}
			break;
		default:
			break;
		}
	}
}



/* 球体に貼り付けるためのテクスチャのロード */
static void LoadTexture(int val)
{
	IplImage *frame=NULL; // 魚眼
	IplImage *pano=NULL; // パノラマ

	switch(pmode)
	{
	case JPG_PANO:
		cvCvtColor(pimage, pimage, CV_BGR2RGB);	
		cvFlip(pimage,NULL,-1);		
		//cvFlip(pimage, NULL, 0);
		//cvFlip(pimage, NULL, 1);
		Rendering(pimage); // テクスチャの差し替え
		break;
	case VFILE_PANO:
		pimage = cvQueryFrame(capture);
		cvCvtColor(pimage, pimage, CV_BGR2RGB);
		cvFlip(pimage, NULL, 0);
		cvFlip(pimage, NULL, 1);
		Rendering(pimage); // テクスチャの差し替え
		glutTimerFunc(10, LoadTexture, 0);  // タイマーを再登録
		break;
	case VFILE_FEYES:
	case USBCAM_FEYES:
	    //動画の画像データ;
		frame = cvCreateImage(cvSize(BEFORE_WIDTH * resolution_size, BEFORE_HEIGHT * resolution_size), IPL_DEPTH_8U, 3);
		
		//パノラマ画像	
		pano = cvCreateImage(cvSize(WIDTH * resolution_size, HEIGHT * resolution_size), IPL_DEPTH_8U, 3);

		cvResize(cvQueryFrame(capture), frame, CV_INTER_CUBIC);	//キャプチャした魚眼画像をリサイズ		
		panorama(frame, pano); // 魚眼をパノラマ画像に変換

		if (rec_flag == 1)
			cvWriteFrame(VideoWriter, pano);
		
		if (disp_flag_v == 1){
			cvShowImage ("Stereo_Fisheye", frame);    	
			cvWaitKey (1); // 描画実行			
		}
		if (disp_flag_p == 1){
			cvShowImage ("Panorama", pano);    	
			cvWaitKey (1); // 描画実行			
		}
		
		cvCvtColor(pano, pano, CV_BGR2RGB);
		cvFlip(pano, NULL, 0);
		cvFlip(pano, NULL, 1);   
		Rendering(pano); // テクスチャの差し替え
		cvReleaseImage(&frame);
		cvReleaseImage(&pano);
		glutTimerFunc(UPDATE_PRERIOD_MS, LoadTexture, 0);  // タイマーを再登録
		break;		
	}    
}
