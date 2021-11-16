#include <iostream>
#include <string>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
using namespace cv;
using namespace std;
void bilinear_interpolation(Mat *orgImg, Mat *resImg);


int main() {

	int target_m,target_n;
	//변환시킬 사이즈를 입력받기
	cin >> target_m >> target_n;

	//기존의 사진을 불러온다
	Mat original;
	original = imread("sleep.png", IMREAD_COLOR);

	cout << original.rows <<endl; 
	cout << original.cols << endl;

	//기존의 사진의 창을 띄운다
	imshow("Original \"sleep.png\"", original);
	waitKey(0);
	
	//bilinear interpolation을 통한 resize
	Mat result = Mat::zeros(target_n, target_m, CV_8UC3);
	bilinear_interpolation(&original, &result);

	cout << result.rows << endl;
	cout << result.cols << endl;

	//변환된 사진을 띄운다
	imshow("Result \"sleep.png\"", result);
	waitKey(0);
}


void bilinear_interpolation(Mat *orgImg, Mat *resImg) {
	//각 축에대한 확대 또는 축소 비율 구하기
	double x_rate = (double)(*resImg).cols / (*orgImg).cols;
	double y_rate = (double)(*resImg).rows / (*orgImg).rows;

	//새로 만들어진 이미지의 각 픽셀값을 모두 새로 계산
	for (int x = 0; x < (*resImg).cols; x++) {
		for (int y = 0; y < (*resImg).rows; y++) {
			//새 x,y의 원래 x,y 위치를 구한다
			int x_org = (int)(x / x_rate);
			int y_org = (int)(y / y_rate);

			//각 사각형(4등분된 픽셀)의 넓이를 구한다
			double e_x1 = (double)x / x_rate - x_org;
			double e_x2 = 1 - e_x1;
			double e_y1 = (double)y / y_rate - y_org;
			double e_y2 = 1 - e_y1;

			double width_1 = e_x1 * e_y2;
			double width_2 = e_x2 * e_y2;
			double width_3 = e_x1 * e_y1;
			double width_4 = e_x2 * e_y1;

			//범위에서 벗어나면 out
			if (y_org + 1 >= (*orgImg).rows || x_org + 1 >= (*orgImg).cols) continue;

			//이전의 사진에서의 각 픽셀 색상값
			Vec3b pixel_1 = (*orgImg).at<Vec3b>(y_org, x_org);
			Vec3b pixel_2 = (*orgImg).at<Vec3b>(y_org, x_org + 1);
			Vec3b pixel_3 = (*orgImg).at<Vec3b>(y_org + 1, x_org);
			Vec3b pixel_4 = (*orgImg).at<Vec3b>(y_org + 1, x_org + 1);

			//새 픽셀의 값을 구해서 넣어준다.
			(*resImg).at<Vec3b>(y, x) =
				width_1 * pixel_1 + width_2 * pixel_2 +
				width_3 * pixel_3 + width_4 * pixel_4;

		}
	}

}