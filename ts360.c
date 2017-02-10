/* 2017/01/13 Takago Lab ts360.c ver:1.03

鷹合研究室
指導教員：鷹合 大輔
製作者：
砂田 峻志　b1366879@planet.kanazawa-it.ac.jp
福田 凌平　b1321344@planet.kanazawa-it.ac.jp
涌嶋 和希　b1302947@planet.kanazawa-it.ac.jp

*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <opencv/cv.h>

#include "ts360.h"
#include "glcore.h"

#include "translate_movie.h"
#include "init.h"

int resolution_size = 1;//画像の倍率

static void manual()
{
	printf("*パノラマ画像を切り出し表示する.\n");
	printf("  $./ts360 [入力ファイル名].jpg\n\n");

	printf
	  ("*デュアル魚眼動画をパノラマ動画に変換する(表示はしない).　\n");
	printf
	  ("  $./ts360 [入力ファイル名].avi -o [出力ファイル名].avi\n\n");

	printf
	  ("*撮影済みのデュアル魚眼動画を切り出し表示する.\n");
	printf("  $./ts360 [入力ファイル名].avi \n\n");

	printf
	  ("*撮影済みのパノラマ動画を切り出し表示する.\n");
	printf("  $./ts360 -t [入力ファイル名].avi \n\n");

	printf
	  ("*カメラでキャプチャしたライブデュアル魚眼画像をパノラマ変換して切り出し表示する.\n");
	printf("  $./ts360 -c [カメラ番号]\n\n");

	printf
	  ("*ストリームしたライブデュアル魚眼画像をパノラマ変換して切り出し表示する.\n");
	printf("  $./ts360 -s [IPアドレス]:[ポート番号] \n\n");
}



int main(int argc, char **argv)
{

	char option = '\0';

	int result;
	int cam_num = 0;

	char *in_filename = NULL;
	char *out_filename = NULL;
	char *IP_PORT = NULL;
	
	char URL[256];	


	if (argc < 2) {
		printf
		  ("使い方を参照したい場合は,以下のコマンドを入力.\n $ ./ts360 help\n");
		return 0;
	}

	while ((result = getopt(argc, argv, "o:c:s:t:k:")) != -1) {
		switch (result) {
		case 'o':
			out_filename = optarg;
			option = 'o';
			break;
		case 'c':
            cam_num= atoi(optarg);            
			option = 'c';
			break;
		case 's':
			IP_PORT = optarg;
			option = 's';
			break;
		case 't':
			in_filename = optarg;
			option = 't';
			break;
		case 'k':
		    resolution_size = atoi(optarg);
		    break;
		default:
			printf("Error 1\n");
			return 1;
		}
	}

	for (; optind < argc; optind++) {
		in_filename = argv[optind];
	}

	if (option == 'o' && strstr(in_filename, ".avi") != NULL ) {

        printf("...now converting\n\tIN %s [Dual FishEyes ]\n\tOUT %s [Panorama]\n",in_filename, out_filename);
		translate_movie(in_filename, out_filename);        
	}

	else if (option == 'c') {
		theta_live(cam_num, NULL);
	}

	else if (option == 's' && IP_PORT != NULL) {
		sprintf(URL, "http://%s/?action=stream&amp;type=.mjpg", IP_PORT);
		printf("Connection URL:%s\n", URL);
		theta_stream(URL);

	}

	else if ( option == 't') {
        printf("パノラマ動画を再生します\n");        
		theta_movie(in_filename);
	}
	
	else if ( (strstr(in_filename, ".MP4") !=NULL) || (strstr(in_filename, ".avi") !=NULL) ) {
        printf("魚眼動画を再生します\n");        
		theta_convert_movie(in_filename);
	}

	else if ( (strstr(in_filename, ".JPG") != NULL) ||  (strstr(in_filename, ".jpg") != NULL))  {
        printf("パノラマ静止画像を表示します\n");       
		theta_image(in_filename);
	}

	else if (strcmp(in_filename, "help") == 0) {
		manual();
	}

	else {
		printf("Error 2\n");
	}



	return 0;
}
