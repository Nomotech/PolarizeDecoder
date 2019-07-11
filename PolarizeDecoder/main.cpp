#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;

void demosaic() {

}

int main() {
	cv::Mat image;
	image = cv::imread("./images/y00.bmp", cv::IMREAD_GRAYSCALE);
	cout << " ( " << image.rows << " , " << image.cols << " ) " << image.depth() << "bit" << endl;

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


	for (int i = 0; i < 3; i++) {
		stringstream ss;
		ss << "./images/" << i << "-" << i + 1 << "-" << i + 2 << "-180.png";

		minMat = cv::min(pols[i], pols[(i + 1) % 4]);
		minMat = cv::min(minMat, pols[(i + 2) % 4]);

		temp[0] = (pols[i		   ] - minMat) * 10;
		temp[1] = (pols[(i + 1) % 4] - minMat) * 10;
		temp[2] = (pols[(i + 2) % 4] - minMat) * 10;
		cv::merge(temp, pol);
		resize(pol, pol, cv::Size(), 0.3, 0.3);
		cv::imshow(ss.str(), pol);
		//cv::imwrite(ss.str(), pol);
		cv::waitKey(10);
	}

	pols[3] = 255;
	cv::merge(pols, pol);


//	cv::imshow("", image);
//	cv::imwrite("./images/image.png", image);

	pols[3] = 255;
	cv::merge(pols, pol);
//	cv::imshow("pol", pol);
	//cv::imwrite("./images/pol.png", pol);

	cv::waitKey(0);
}