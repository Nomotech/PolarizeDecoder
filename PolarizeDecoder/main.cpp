#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;


int main() {
	cv::Mat image;
	image = cv::imread("./images/201907120716.bmp", cv::IMREAD_GRAYSCALE);
	cout << " ( " << image.rows << " , " << image.cols << " ) " << endl;

	const int w = image.cols;
	const int h = image.rows;

	cv::Mat pol;
	cv::Mat minMat = cv::Mat::zeros(h, w, CV_8UC1);
	
	vector<cv::Mat> pols;
	pols.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	pols.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	pols.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	pols.push_back(cv::Mat::zeros(h, w, CV_8UC1));

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			int num = y % 2 * 2 + x % 2;
			pols[num].at<char>(y, x) = image.at<char>(y, x);
		}
	}

	for (int i = 0; i < 4; i++) {
		cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
			0.25,	0.5,	0.25,
			0.5,	1.0,	0.5,
			0.25,	0.5,	0.25);
		cv::filter2D(pols[i], pols[i], -1, kernel);
	}

	vector<cv::Mat> temp;
	temp.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	temp.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	temp.push_back(cv::Mat::zeros(h, w, CV_8UC1));



	pols[3] = 255;
	cv::merge(pols, pol);

	vector<unsigned char> phase;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			double a = pols[0].at<char>(y, x) - pols[2].at<char>(y, x);
			double b = pols[3].at<char>(y, x) - pols[1].at<char>(y, x);
			double c = (b != 0) ? atan(a / b) * 180 / 3.141592 : 90;
			c = c + 90;
			phase.push_back((unsigned char)c % 180);
		}
	}
	
	vector<cv::Mat> po;
	po.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	po.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	po.push_back(cv::Mat::zeros(h, w, CV_8UC1));

	memcpy(po[0].data, phase.data(), phase.size() * sizeof(unsigned char));
	po[0] += 80;
	po[1] = 255;
	po[2] = 255;

	cv::merge(po, pol);
	cv::cvtColor(pol, pol, cv::COLOR_HSV2RGB);
	cv::imwrite("./images/p_201907120716.png", pol);
	resize(pol, pol, cv::Size(), 0.3, 0.3);
	cv::imshow("phase", pol);


	resize(image, image, cv::Size(), 0.3, 0.3);
	//cv::imshow("image", image);
//	cv::imwrite("./images/image.png", image);



	pols[3] = 255;
	cv::merge(pols, pol);
	resize(pol, pol, cv::Size(), 0.3, 0.3);
	cv::imshow("pol", pol);
	//cv::imwrite("./images/pol_180.png", pol);

	cv::waitKey(0);
}