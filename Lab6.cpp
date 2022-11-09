
#include "stdafx.h"
#include "Util.h"
#include <random>


const char DATA_PCA_PATH[2][PATH_SIZE] = {
	"data_PCA\\pca2d.txt",
	"data_PCA\\pca3d.txt"
};

Mat readInputPoints(const char inputFile[]);
std::vector<double> calculateMean(Mat X);
Mat PCAKthApproximation(Mat X, Mat Q, int k);
double calculateMeanAbsDiff(Mat X, Mat Q);

int main()
{
	int op = 0;
	do {
		std::cout << "Menu:" << std::endl;
		std::cout << "1 - Principal component analysis" << std::endl;
		std::cout << "0 - Exit" << std::endl;
		std::cin >> op;
		switch (op) {
		case 1: {
			int k;
			std::cout << "Select input data:\n";
			std::cout << "2 - 2D points" << std::endl;
			std::cout << "3 - 3D points" << std::endl;
			std::cin >> k;
			Mat X = readInputPoints(DATA_PCA_PATH[k-2]);
			std::vector<double> means = calculateMean(X);
			int N = X.rows;
			int D = X.cols;
			// subtract mean from data points
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < D; j++) {
					X.at<double>(i,j) -= means[j];
				}
			}
			// calculate the covariance matrix
			Mat C = X.t() * X / (N - 1);
			// eigenvalue decomposition on the cov matrix
			Mat Lambda, Q;
			eigen(C, Lambda, Q);
			Q = Q.t();
			for (int i = 0; i < Lambda.rows; i++) {
				std::cout << "L[" << i << "] = ";
				for (int j = 0; j < Lambda.cols; j++) {
					std::cout << Lambda.at<double>(i, j) << " ";
				}
				std::cout << std::endl;
			}
			
			int K = 0;
			std::cout << "K = ";
			std::cin >> K;
			Mat Xk = PCAKthApproximation(X, Q, K);
			std::cout << "mean abs diff = " << calculateMeanAbsDiff(X, Xk) << std::endl;

			Mat X_coeff = X * Q;   // NxD 

			// find min and max on the cols of the coeff matrix
			std::vector<double> minC(D, INT_MAX);
			std::vector<double> maxC(D, 0);

			for (int j = 0; j < D; j++) {
				for (int i = 0; i < N; i++) {
					if (X_coeff.at<double>(i, j) < minC[j]) {
						minC[j] = X_coeff.at<double>(i, j);
					}
					else if (X_coeff.at<double>(i, j) > maxC[j]) {
						maxC[j] = X_coeff.at<double>(i, j);
					}
				}
			}

			for (int i = 0; i < D; i++) {
				std::cout << "min [" << i << "] = " << minC[i] << " | ";
				std::cout << "max [" << i << "] = " << maxC[i] << std::endl;
			}

			int img_width = maxC[0] - minC[0] + 1;
			int img_height = maxC[1] - minC[1] + 1;

			std::cout << "height = " << img_height << std::endl;
			std::cout << "width = " << img_width << std::endl;

			Mat plot(img_height, img_width, CV_8SC1);
			plot.setTo(255);
			for (int i = 0; i < N; i++) {
				int x = X_coeff.at<double>(i, 0) - minC[0];
				int y = X_coeff.at<double>(i, 1) - minC[1];
				if (k == 2) {
					plot.at<uchar>(x, y) = 0;
				}
				else {
					plot.at<uchar>(x, y) = (X_coeff.at<double>(i,2) - minC[2]) * 255 / maxC[2];
				}
			}
			imshow("Visualize points", plot);
			
			waitKey(0);
			break;
		}
		case 0: break;
		}
	} while (op != 0);

	return 0;
}

Mat readInputPoints(const char inputFile[]) {
	std::ifstream f(inputFile);
	int N, D;
	f >> N >> D;
	std::cout << N << " " << D << std::endl;
	Mat X(N, D, CV_64F);
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < D; j++) {
			f >> X.at<double>(i, j);
		}
	}
	return X;
}

std::vector<double> calculateMean(Mat X) {
	int N = X.rows;
	int D = X.cols;
	std::vector<double> means(D);
	for (int i = 0; i < D; i++) {
		double m = 0;
		for (int j = 0; j < N; j++) {
			m += X.at<double>(j,i);
		}
		m /= N;
		means[i] = m;
	}
	return means;
}

double calculateMeanAbsDiff(Mat X, Mat Xk) {
	int N = X.rows;
	int D = X.cols;
	double diff = 0;
	for (int i = 0; i < N; i++) {
		double d = 0;
		for (int j = 0; j < D; j++) {
			d += abs(X.at<double>(i, j) - Xk.at<double>(i, j));
		}
		diff += d / D;
	}
	return diff/N;
}

Mat PCAKthApproximation(Mat X, Mat Q, int k) {
	int N = X.rows;
	int D = X.cols;
	Mat X_coeff = X * Q;   // NxD 
	Mat Qk(D, k, CV_64F);
	Q(Range(0, D), Range(0, k)).copyTo(Qk);
	Mat Xk = X * Qk * Qk.t();  // (N x D) * (D x k) * (k x D) = (N x D)
	return Xk;
}
