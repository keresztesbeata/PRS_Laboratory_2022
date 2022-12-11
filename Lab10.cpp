#include "stdafx.h"
#include "Util.h"
#include <random>


int NR_IMAGES = 7;
void perceptron(Mat img, Mat X, Mat y, Mat& w, double lr, double errorLimit, int maxIter);
void showDecisionBoundary(Mat img, Mat w);

int main()
{
	// allocate feature matrix and label vector

	int trainIdx = 0;
	char fname[PATH_SIZE];
	sprintf(fname, "images_Perceptron/test0%d.bmp", trainIdx);
	Mat img = imread(fname);
	int width = img.cols;
	int height = img.rows;

	int k = 0;
	std::vector<Vec3b> x_train;
	std::vector<int> y_train;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (img.at<Vec3b>(i, j) == RED) {
				x_train.push_back(Vec3b(1, j, i));
				y_train.push_back(1);
				k++;
			}
			else if (img.at<Vec3b>(i, j) == BLUE) {
				x_train.push_back(Vec3b(1, j, i));
				y_train.push_back(-1);
				k++;
			}
		}
	}

	Mat X_train(k, 3, CV_8UC3);
	Mat Y_train(k, 1, CV_32SC1);

	for (int i = 0; i < k; i++) {
		X_train.at<uchar>(i, 0) = x_train[i][0];
		X_train.at<uchar>(i, 1) = x_train[i][1];
		X_train.at<uchar>(i, 2) = x_train[i][2];
		Y_train.at<int>(i, 0) = y_train[i];
	}
	double errorLimit = pow(10, -5);
	double learningRate = pow(10, -4);
	Mat w(1, 3, CV_64FC1);
	w.setTo(0.1);
	int maxIter = pow(10, 5);

	perceptron(img, X_train, Y_train, w, learningRate, errorLimit, maxIter);
	showDecisionBoundary(img, w);
	
	return 0;
}

void showDecisionBoundary(Mat img, Mat w) {
	Mat outImg = img.clone();
	int x1 = 0;
	int x2 = img.cols - 1;
	int y1 = x1 * w.at<double>(0, 2) + x1 * w.at<double>(0, 1) + w.at<double>(0, 0);
	int y2 = x2 * w.at<double>(0, 2) + x2 * w.at<double>(0, 1) + w.at<double>(0, 0);
	line(outImg, Point2d(x1, -y1), Point2d(x2, -y2), GREEN);
	imshow("output", outImg);
	waitKey(0);
}

void perceptron(Mat img, Mat X, Mat y, Mat& w, double lr, double errorLimit, int maxIter) {
	int N = X.rows;
	int d = X.cols;
	Mat z(N, 1, CV_64FC1);
	z.setTo(0);
	for(int iter = 0; iter < maxIter; iter++) {
		std::cout << "Iteration " << iter << std::endl;
		double error = 0;
		double loss = 0;
		Mat grad(1, 3, CV_64FC1);
		grad.setTo(0);
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < d; j++) {
				z.at<double>(i, 0) += w.at<double>(0,j) * X.at<uchar>(i, j);
			}
			if (z.at<double>(i, 0) * y.at<int>(i, 0) <= 0) {
				// update grad of loss
				grad.at<double>(0, 0) -= y.at<int>(i, 0) * X.at<uchar>(i, 0);
				grad.at<double>(0, 1) -= y.at<int>(i, 0) * X.at<uchar>(i, 1);
				grad.at<double>(0, 2) -= y.at<int>(i, 0) * X.at<uchar>(i, 2);
				error++;
				loss -= y.at<int>(i, 0) * z.at<double>(i, 0);
				// update weights
				w.at<double>(0, 0) += lr * y.at<int>(i, 0) * X.at<uchar>(i, 0);
				w.at<double>(0, 1) += lr * y.at<int>(i, 0) * X.at<uchar>(i, 1);
				w.at<double>(0, 2) += lr * y.at<int>(i, 0) * X.at<uchar>(i, 2);
				
				std::cout << "wrong" << std::endl;
				std::cout << "update w0 = w0 " << (y.at<int>(i, 0) < 0 ? "-" : "+") <<lr <<"*" << w.at<double>(0, 0) << ", w1 = w1 " << (y.at<int>(i, 0) < 0 ? "-" : "+") << lr << "*" << w.at<double>(0, 1) << ", w2 = w2 " << (y.at<int>(i, 0) < 0 ? "-" : "+") << lr << " * " << w.at<double>(0, 2) << std::endl;
			}
			std::cout << "i=" << i << ": w=[" << w.at<double>(0, 0) << "," << w.at<double>(0, 1) << "," << w.at<double>(0, 2) << "]";
			std::cout << " xi=[" << int(X.at<uchar>(i, 0)) << "," << int(X.at<uchar>(i, 1)) << "," << int(X.at<uchar>(i, 2)) << "] yi=" << y.at<int>(i, 0) << " zi=" << z.at<double>(i, 0) << std::endl;
			showDecisionBoundary(img, w);
		}
		error /= (double)lr;
		loss /= (double)lr;
		grad /= (double)lr;
		std::cout << "error: " << error << std::endl;
		
		if (error < errorLimit) {
			std::cout << "All classified correctly" << std::endl;
			break;
		}
		// update weights
		w.at<double>(0, 0) -= lr * grad.at<double>(0, 0);
		w.at<double>(0, 1) -= lr * grad.at<double>(0, 1);
		w.at<double>(0, 2) -= lr * grad.at<double>(0, 2);
	}
}
