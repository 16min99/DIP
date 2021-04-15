#include <iostream>
#include "opencv2/core/core.hpp" // Mat class와 각종 data structure 및 산술 루틴을 포함하는 헤더
#include "opencv2/highgui/highgui.hpp" // GUI와 관련된 요소를 포함하는 헤더(imshow 등)
#include "opencv2/imgproc/imgproc.hpp" // 각종 이미지 처리 함수를 포함하는 헤더
#include "myfunc.h"

using namespace cv;
using namespace std;

int myKernelConv3x3(uchar* arr, int kernel[][3], int x, int y, int w, int h) {
	int sum = 0;
	int sumKernel = 0;

	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			if ((y + j) >= 0 && (y + j) < h && (x + i) >= 0 && (x + i) < w) {
				sum += arr[(y + j) * w + (x + i)] * kernel[i + 1][j + 1];
				sumKernel +=(kernel[i + 1][j + 1]);
			}
		}
	}
	if (sumKernel != 0)return sum / sumKernel;
	else return sum;
}



Mat mySobelFilter(Mat srcImg) {
				     //이전값   다음값
	int kernelX[3][3] = { -1,0,1,//
						  -2,0,2,
						  -1,0,1 };//가로방향 sobel 마스크
	int kernelY[3][3] = { -1,-2,-1,//이전값
						   0, 0, 0,
						   1, 2, 1 };//다음값 ,세로방향 sobel 마스크

	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;
	int width = srcImg.cols;
	int height = srcImg.rows;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernelX, x, y, width, height))
				+ abs(myKernelConv3x3(srcData, kernelY, x, y, width, height))) / 2;
		}
	}
	return dstImg;
}


Mat SobelFilter45(Mat srcImg) {//동쪽기준 45도
	int kernel45[3][3] = { 0, 1, 2,
						  -1, 0, 1,
						  -2,-1, 0 };
	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;
	int width = srcImg.cols;
	int height = srcImg.rows;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernel45, x, y, width, height)));
		}
	}
	return dstImg;
}

Mat SobelFilter135(Mat srcImg) { //동쪽기준 135도
	int kernel135[3][3] = { 2, 1, 0,
						   1, 0,-1,
						   0,-1,-2 };
	Mat dstImg(srcImg.size(), CV_8UC1);
	uchar* srcData = srcImg.data;
	uchar* dstData = dstImg.data;
	int width = srcImg.cols;
	int height = srcImg.rows;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			dstData[y * width + x] = (abs(myKernelConv3x3(srcData, kernel135, x, y, width, height)));
		}
	}
	return dstImg;
}



int main() {
	Mat src_img, dst_img45, dst_img135 ;
	src_img = imread("image/gear.jpg", 0);;//grayscale;

	dst_img45 = SobelFilter45(src_img);
	dst_img135 = SobelFilter135(src_img);

	Mat img;
	hconcat(dst_img45, dst_img135, img);
	imshow("hw4_img", img);
	waitKey(0);
	destroyWindow("*");

	return 0;

}

