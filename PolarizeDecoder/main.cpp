#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

using namespace std;


double dopToNormal(double p) {
	double a = sqrt(1 - 2 * p / (1 + p));
	double b = sqrt((1 + a) / 2);
	double c = sqrt((1 - a) / 2);
	double theta = asin(b);
	return theta;
}

double dopToTheta(double x) {
	double p1, p2, p3, p4, p5, q1, q2, q3;
	p1 = 1.306;
	p2 = 1.549;
	p3 = 0.04275;
	p4 = 7.135e-05;
	p5 = 1.57e-09;
	q1 = 0.08599;
	q2 = 0.0006094;
	q3 = 1.7e-07;
	return (p1 * pow(x, 4) + p2 * pow(x, 3) + p3 * pow(x, 2) + p4 * x + p5) / (pow(x, 3) + q1 * pow(x, 2) + q2 * x + q3);
}

double calcDop(unsigned char I[4]) {
	double I_m = sqrt((I[3] - I[1]) * (I[3] - I[1]) + (I[0] - I[2]) * (I[0] - I[2])) / 2;
	double I_d = (I[0] + I[1] + I[2] + I[3]) / 4;
	//double I_d = (I[0] + I[2]) / 2;
	double dop = I_m / I_d;
	return dop;
}

int main() {
	string filename = "y_000";
//	string filename = "201907120639";
//	string filename = "201907120651";
	stringstream ss;
	ss << "./images/" << filename << ".bmp";
	cv::Mat image;
	image = cv::imread(ss.str() , cv::IMREAD_GRAYSCALE);
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
			int num = 0;
			switch (y % 2 * 2 + x % 2) {
				case 0: num = 0; break;
				case 1: num = 1; break;
				case 2: num = 3; break;
				case 3: num = 2; break;
			}
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

	cv::merge(pols, pol);

	double coef = 2.0;
	vector<unsigned char> phase;
	vector<unsigned char> normal;
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			double a = pols[3].at<char>(y, x) - pols[1].at<char>(y, x);
			double b = pols[0].at<char>(y, x) - pols[2].at<char>(y, x);
			double c = (b != 0) ? atan(a / b) * 180 / 3.141592 : 90;
			c /= 2;
			phase.push_back((unsigned char)c % 180);

			unsigned char I[4];
			I[0] = pols[0].at<char>(y, x);
			I[1] = pols[1].at<char>(y, x);
			I[2] = pols[2].at<char>(y, x);
			I[3] = pols[3].at<char>(y, x);
			double dop = calcDop(I);
			normal.push_back((unsigned char)(dopToTheta(dop) * 180 / 3.141592) % 180);
		}
	}
	
	// phi
	vector<cv::Mat> phaseMat;
	cv::Mat phaseMat_;
	phaseMat.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	phaseMat.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	phaseMat.push_back(cv::Mat::zeros(h, w, CV_8UC1));

	memcpy(phaseMat[0].data, phase.data(), phase.size() * sizeof(unsigned char));
	phaseMat[1] = 150;
	phaseMat[2] = 255;

	ss.str("");
	ss << "./images/" << filename << "_phi.png";
	cv::merge(phaseMat, phaseMat_);
	cv::cvtColor(phaseMat_, phaseMat_, cv::COLOR_HSV2RGB);
	cv::imwrite(ss.str(), phaseMat_);
	resize(phaseMat_, phaseMat_, cv::Size(), 0.3, 0.3);
	cv::imshow("phase", phaseMat_);

	// theta
	vector<cv::Mat> normalMat;
	cv::Mat normalMat_;
	normalMat.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	normalMat.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	normalMat.push_back(cv::Mat::zeros(h, w, CV_8UC1));
	memcpy(normalMat[0].data, normal.data(), normal.size() * sizeof(unsigned char));
	normalMat[1] = 150;
	normalMat[2] = 255;
	
	ss.str("");
	ss << "./images/" << filename << "_theta.png";
	cv::merge(normalMat, normalMat_);
	cv::cvtColor(normalMat_, normalMat_, cv::COLOR_HSV2RGB);
	cv::imwrite(ss.str(), normalMat_);
	resize(normalMat_, normalMat_, cv::Size(), 0.3, 0.3);
	cv::imshow("normal", normalMat_);
	cv::waitKey(0);
}