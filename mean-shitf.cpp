#include <iostream>
#include "opencv2/core/core.hpp" 
#include "opencv2/highgui/highgui.hpp" 
#include "opencv2/imgproc/imgproc.hpp" 
using namespace cv;
using namespace std;
#include <ctime>
#include"Point5D.h"
Mat exCvMeanShift();
Mat exMyMeanShift();


void main() {

	Mat cvimg = exCvMeanShift();
	Mat myimg = exMyMeanShift();

	imshow("CvMeanShiftDst", cvimg);
	imshow("MyMeanShiftDst", myimg);
	waitKey();
	destroyAllWindows();
}






class MeanShift {
	//민쉬프트클래스
public:
	float bw_spatial = 8; 
	float bw_color = 16;
	float min_shift_color = 0.1;
	float min_shift_spatial = 0.1;
	int max_steps = 10;
	vector<Mat>img_split;
	MeanShift(float, float, float, float, int);
	void doFiltering(Mat&);
};

MeanShift::MeanShift(float bs, float bc, float msc, float mss, int ms) {
	//생성자
	bw_spatial = bs;
	bw_color = bc;
	max_steps = ms;
	min_shift_color = msc;
	min_shift_spatial = mss;
}

void MeanShift::doFiltering(Mat& img) {
	int h = img.rows;
	int w = img.cols;
	split(img, img_split);
	Point5D pt, pt_prev, pt_cur, pt_sum;

	int pad_left, pad_right, pad_top, pad_bottom;
	size_t n_pt, step;
	
	for (int row = 0; row < h; row++) {
		for (int col = 0; col < w; col++) {
			pad_left = (col - bw_spatial) > 0 ? (col - bw_spatial) : 0;
			pad_right = (col + bw_spatial) < w ? (col + bw_spatial) : w;
			pad_top = (row - bw_spatial) > 0 ? (row - bw_spatial) : 0;
			pad_bottom = (row + bw_spatial) < h ? (row + bw_spatial) : h;


			pt_cur.setPt(row, col,
				(float)img_split[0].at<uchar>(row, col),
				(float)img_split[1].at<uchar>(row, col),
				(float)img_split[2].at<uchar>(row, col));
			step = 0;
			do {
				pt_prev.copyPt(pt_cur);
				pt_sum.setPt(0, 0, 0, 0, 0);
				n_pt = 0;
				for (int hx = pad_top; hx < pad_bottom; hx++) {
					for (int hy = pad_left; hy < pad_right; hy++) {
						pt.setPt(hx, hy,
							(float)img_split[0].at<uchar>(hx, hy),
							(float)img_split[1].at<uchar>(hx, hy),
							(float)img_split[2].at<uchar>(hx, hy));

						if (pt.getColorDist(pt_cur) < bw_color) {
							pt_sum.accumPt(pt);
							n_pt++;
						}
					}
				}
				//축적 결과를 기반으로 현재 픽셀 갱신
				pt_sum.scalePt(1.0 / n_pt);
				pt_cur.copyPt(pt_sum);
				step++;
			}

			while ((pt_cur.getColorDist(pt_prev) > min_shift_color) &&
				(pt_cur.getSpatialDist(pt_prev) > min_shift_spatial) &&
				(step < max_steps));
			//변화량 최소조건을 만족할 때 까지 반복
			//최대 반복횟수 조건도 포함
			//결과 픽셀갱신
			img.at<Vec3b>(row, col) = Vec3b(pt_cur.l, pt_cur.u, pt_cur.v);
		}
	}
}


Mat exCvMeanShift() {
	Mat img = imread("image/fruit.jpg");
	if (img.empty()) exit(-1);//사진없음
	cout << "----- exCvMeanShift()-----" << endl;

	resize(img, img, Size(256, 256), 0, 0, CV_INTER_AREA);
	imshow("Src", img);
	imwrite("exCvMeanShift_src.jpg", img);

	pyrMeanShiftFiltering(img, img, 8, 16);

	return img;
}

Mat exMyMeanShift() {
	Mat img = imread("image/fruit.jpg");
	if (img.empty()) exit(-1);//사진없음
	cout << "----- exMyMeanShift()-----" << endl;

	resize(img, img, Size(256, 256), 0, 0, CV_INTER_AREA);
	//imshow("Src", img);
	//imwrite("exMyMeanShift_src.jpg", img);

	cvtColor(img, img, CV_RGB2Luv);

	MeanShift MSProc(8, 16, 0.1, 0.1, 10);
	MSProc.doFiltering(img);

	cvtColor(img, img, CV_Luv2RGB);

	return img;
}