/* 2017/01/06 Takago Lab panorama.c ver:1.00

鷹合研究室
指導教員：鷹合 大輔
製作者：
砂田 峻志　b1366879@planet.kanazawa-it.ac.jp
福田 凌平　b1321344@planet.kanazawa-it.ac.jp
涌嶋 和希　b1302947@planet.kanazawa-it.ac.jp

*/

#include <stdio.h>
#include <opencv/cv.h>

#include "ts360.h"
#include "panorama.h"
#include "fish_translate.h"


//画素移動用
#   define NH_ACCESS_BLUE(IMG,X,Y) ((unsigned char *)((IMG)->imageData + (IMG)->widthStep * (Y)))[(X)*3]
#   define NH_ACCESS_GREEN(IMG,X,Y) ((unsigned char *)((IMG)->imageData + (IMG)->widthStep * (Y)))[(X)*3 + 1]
#   define NH_ACCESS_RED(IMG,X,Y) ((unsigned char *)((IMG)->imageData + (IMG)->widthStep * (Y)))[(X)*3 + 2]





/*変換マップから座標を取り出す関数*/
static void pix(int m, int n, int *fish_u, int *fish_v)
{
	*fish_u = cvmGet(mat_u, m, n);	//グローバル変数の"mat_u"取り出し
	*fish_v = cvmGet(mat_v, m, n);	//グローバル変数の"mat_v"取り出し
}


/*パノラマ画像を生成する関数*/
static void panoLR(IplImage *L_img, IplImage *R_img, IplImage *out_img)
{
	int sz, n, m, pano_sz, u, v;
	sz = L_img->width;
	pano_sz = out_img->width;


	for (m = 0; m < sz; m++) {
		for (n = 0; n < sz; n++) {
			pix(n, m, &u, &v);
			NH_ACCESS_RED(out_img, m, n) = NH_ACCESS_RED(L_img, u, v);
			NH_ACCESS_GREEN(out_img, m, n) = NH_ACCESS_GREEN(L_img, u, v);
			NH_ACCESS_BLUE(out_img, m, n) = NH_ACCESS_BLUE(L_img, u, v);

		}
	}
	
	for (m = 0; m < sz; m++) {
		for (n = 0; n < sz; n++) {
			pix(n, m, &u, &v);
			NH_ACCESS_RED(out_img, m + sz, n) =
			  NH_ACCESS_RED(R_img, u, v);
			NH_ACCESS_GREEN(out_img, m + sz, n) =
			  NH_ACCESS_GREEN(R_img, u, v);
			NH_ACCESS_BLUE(out_img, m + sz, n) =
			  NH_ACCESS_BLUE(R_img, u, v);

		}
	}
}

/*元画像からL画像・R画像を抽出*/
static void LRimg_resize(IplImage *LR_img, IplImage *L_img, IplImage *R_img)
{

	int sz = R_img->width;		
	int k = CUT * resolution_size;
	int m, n, s;


	 /*L*/ for (m = 0; m < sz; m++) {
		for (n = 0; n < sz; n++) {
			NH_ACCESS_RED(L_img, n, m) = NH_ACCESS_RED(LR_img, m + k, n + k);
			NH_ACCESS_GREEN(L_img, n, m) =
			  NH_ACCESS_GREEN(LR_img, m + k, n + k);
			NH_ACCESS_BLUE(L_img, n, m) = NH_ACCESS_BLUE(LR_img, m + k, n + k);

		}
	}

	cvFlip(L_img, NULL, 1);

	 /*R*/ s = (k * 3) + sz;
	for (m = 0; m < sz; m++) {
		for (n = 0; n < sz; n++) {
			NH_ACCESS_RED(R_img, n, m) = NH_ACCESS_RED(LR_img, m + s, n + k);
			NH_ACCESS_GREEN(R_img, n, m) =
			  NH_ACCESS_GREEN(LR_img, m + s, n + k);
			NH_ACCESS_BLUE(R_img, n, m) = NH_ACCESS_BLUE(LR_img, m + s, n + k);

		}
	}
	cvFlip(R_img, NULL, 0);

}

void panorama(IplImage *in_frame, IplImage *out_frame)
{
    int size = resolution_size * SZ;
	IplImage *fish_L_img = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 3);	//抽出したL画像
	IplImage *fish_R_img = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 3);	//抽出したR画像


	LRimg_resize(in_frame, fish_L_img, fish_R_img);
	panoLR(fish_L_img, fish_R_img, out_frame);
	cvReleaseImage(&fish_L_img);
	cvReleaseImage(&fish_R_img);
}
