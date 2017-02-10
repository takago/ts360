#ifndef TS360_H_INCLUDED__
#   define TS360_H_INCLUDED_

// 元の画像サイズ・画面のサイズ
#   define BEFORE_WIDTH 1920
#   define BEFORE_HEIGHT 1080
#   define SZ 848//黒い縁の部分を切り取った後の縦横の長さ(L,R: 前960)

//同じく切り取った後の画像のサイズ(パノラマ: 前1920×1080)				
#   define WIDTH 1696					
#   define HEIGHT 848

#   define CUT 56 //【 】切り取る範囲設定

//グリッド線の間隔
#   define GRID_X 53
#   define GRID_Y 53

//表示サイズ

#   define DISP_WIDTH 1280 
#   define DISP_HEIGHT 720

#   define FPS 10 //録画用FPS

#define GRID_LINE_WIDTH 2

extern int resolution_size; //画像の倍率(2k画像 * resolution_size[初期値 1])


#endif


