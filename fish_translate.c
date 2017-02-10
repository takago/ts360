/* 2017/01/13 Takago Lab fish_translate.c ver:1.02

鷹合研究室
指導教員：鷹合 大輔
製作者：
砂田 峻志　b1366879@planet.kanazawa-it.ac.jp
福田 凌平　b1321344@planet.kanazawa-it.ac.jp
涌嶋 和希　b1302947@planet.kanazawa-it.ac.jp

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opencv/cv.h>
#include <ctype.h>

#include "ts360.h"
#include "fish_translate.h"
#include "glcore.h"

/*変換マップ用CvMat*/
CvMat *mat_u;
CvMat *mat_v;

void fish_translate(int sz);

static void meshgrid(double lin_x[], double lin_y[], CvMat * mat_x,
					 CvMat * mat_y )
{
	int row, col;


	for (col = 0; col < mat_x->cols; col++) {
		for (row = 0; row < mat_x->rows; row++) {
			cvmSet(mat_x, row, col, lin_x[col]);
		}
	}

	for (row = 0; row < mat_y->rows; row++) {
		for (col = 0; col < mat_y->cols; col++) {
			cvmSet(mat_y, row, col, lin_y[row]);
		}
	}
}

static void linspace(int x, int y, int z, double tmp[])
{
	int i;
	double ans, n;
	n = y - x;
	ans = n / (z - 1);
	tmp[0] = x;
	tmp[z - 1] = y;
	for (i = 1; i <= z - 2; i++) {
		tmp[i] = tmp[i - 1] + ans;
	}
}

/* 足し算 */
static void matrix_adder(CvMat * mat, double add, CvMat * ans)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			cvmSet(ans, row, col, add + cvmGet(mat, row, col));
		}
	}
}

/* スカラー倍 */
static void matrix_scalar(CvMat * mat, double add, CvMat * ans)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			cvmSet(ans, row, col, add * cvmGet(mat, row, col));
		}
	}
}

/* cos */
static void matrix_cos(CvMat * mat, CvMat * ans)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			cvmSet(ans, row, col, cos(cvmGet(mat, row, col)));
		}
	}
}

/* sin */
static void matrix_sin(CvMat * mat, CvMat * ans)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			cvmSet(ans, row, col, sin(cvmGet(mat, row, col)));
		}
	}
}

/* atan2 */
static void matrix_atan2(CvMat * mat_y, CvMat * mat_x, CvMat * ans)
{
	int row, col;

	for (row = 0; row < ans->rows; row++) {
		for (col = 0; col < ans->cols; col++) {
			cvmSet(ans, row, col,
				   atan2(cvmGet(mat_y, row, col), (cvmGet(mat_x, row, col))));
		}
	}
}


/* acos */
static void matrix_acos(CvMat * mat, CvMat * ans)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			cvmSet(ans, row, col, acos(cvmGet(mat, row, col)));
		}
	}
}

/* 割り算 */
static void matrix_division(CvMat * mat, double div, CvMat * ans)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			cvmSet(ans, row, col, cvmGet(mat, row, col) / div);
		}
	}
}

/* floor */
static void mat_floor(CvMat * mat)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			cvmSet(mat, row, col, cvFloor(cvmGet(mat, row, col)));
		}
	}
}

/*行列の数値をsz以下にする関数*/
static void mat_sz(CvMat * mat, int sz)
{
	int row, col;

	for (row = 0; row < mat->rows; row++) {
		for (col = 0; col < mat->cols; col++) {
			if (cvmGet(mat, row, col) > sz) {
				cvmSet(mat, row, col, sz);
			}
		}
	}
}

/*変換マップを整数補正*/
static void mat_integer(CvMat * mat, CvMat * out_mat, int sz)
{
	matrix_adder(mat, 1, mat);
	matrix_scalar(mat, sz / 2, mat);
	mat_floor(mat);
	matrix_adder(mat, 0, out_mat);
	mat_sz(out_mat, sz - 1);
}



void fish_translate(int sz)
{
	int NX, NY;
	double PID2;
	double xx[sz], yy[sz];
    double tmp[sz];			//linespace用配列
   
	NX = sz;
	NY = sz;
	PID2 = M_PI / 2;

	CvMat *cm_x = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *cm_y = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *theta = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *phi = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *p_x = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *p_y = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *p_z = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *m_tmp1 = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *m_tmp2 = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *r = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *uu = cvCreateMat(NX, NY, CV_64FC1);
	CvMat *vv = cvCreateMat(NX, NY, CV_64FC1);

	mat_u = cvCreateMat(NX, NY, CV_64FC1);
	mat_v = cvCreateMat(NX, NY, CV_64FC1);

	linspace(-1, 1, NX, tmp);
	memcpy(xx, tmp, sizeof(double) * NX);
	linspace(-1, 1, NY, tmp);
	memcpy(yy, tmp, sizeof(double) * NY);

	meshgrid(xx, yy, cm_x, cm_y);

	//theta 
	matrix_adder(cm_x, 1, m_tmp1);
	matrix_scalar(m_tmp1, PID2, theta);

	// phi
	matrix_scalar(cm_y, PID2, phi);

	// p_x
	matrix_cos(phi, m_tmp1);
	matrix_cos(theta, m_tmp2);
	cvMul(m_tmp1, m_tmp2, p_x, 1);

	// p_y
	matrix_cos(phi, m_tmp1);
	matrix_sin(theta, m_tmp2);
	cvMul(m_tmp1, m_tmp2, p_y, 1);

	// p_z
	matrix_sin(phi, p_z);

	// theta mk2
	matrix_atan2(p_z, p_x, theta);

	// phi mk2 
	matrix_acos(p_y, phi);

	// r 
	matrix_division(phi, PID2, r);

	// u 
	matrix_scalar(r, -1, m_tmp1);
	matrix_cos(theta, m_tmp2);
	cvMul(m_tmp1, m_tmp2, uu, 1);

	// v 
	matrix_sin(theta, m_tmp1);
	cvMul(r, m_tmp1, vv, 1);

	// floor 
	mat_integer(uu, mat_u, sz);
	mat_integer(vv, mat_v, sz);

	//cvMat開放
	cvReleaseMat(&theta);
	cvReleaseMat(&phi);
	cvReleaseMat(&p_x);
	cvReleaseMat(&p_y);
	cvReleaseMat(&p_z);
	cvReleaseMat(&m_tmp1);
	cvReleaseMat(&m_tmp2);
	cvReleaseMat(&r);
	cvReleaseMat(&uu);
	cvReleaseMat(&vv);
	cvReleaseMat(&cm_x);
	cvReleaseMat(&cm_y);

}
