
#include "stdafx.h"
#include "Util.h"

const char FACE_IMAGES_DIR[PATH_SIZE] = "images_faces";

std::vector<float> computeMeanValues(Mat featureMat);
std::vector<float> computeStdDeviation(Mat featureMat);
Mat computeCovarianceMat(Mat featureMat);
Mat computeCorrelationCoeff(Mat featureMat);

const char MEAN_VALUES_FILE[PATH_SIZE] = "statistical_results/mean_values.csv";
const char COV_MATRIX_FILE[PATH_SIZE] = "statistical_results/covariance_matrix.csv";
const char CORR_MATRIX_FILE[PATH_SIZE] = "statistical_results/correlation_matrix.csv";

void saveResults(std::vector<float> values, const char output_path[]);
void saveResults(Mat values, const char output_path[]);
void displayCorrelationChart(Mat featureMat, int x1, int y1, int x2, int y2);
void pdfForSingleFeature(Mat featureMat, int x);
void pdfFor2Features(Mat featureMat, int i, int j);

int main5()
{
	std::vector<Mat> images;
	char fname[256];

	int p = 400;
	int IMG_SIZE = 19;
	int N = IMG_SIZE * IMG_SIZE;

	Mat featureMat(p, N, CV_8UC1);

	for(int k=0;k<p;k++) 
	{
		// build path to image file
		sprintf(fname, "%s\\face%05d.bmp", FACE_IMAGES_DIR, k+1);
		// read the image matrix of pixels
		Mat img = imread(fname, 0);
		for (int i = 0; i < IMG_SIZE; i++) {
			for (int j = 0; j < IMG_SIZE; j++) {
				featureMat.at<uchar>(k, i*IMG_SIZE + j) = img.at<uchar>(i, j);
			}
		}
		
	} 

	std::vector<float> means = computeMeanValues(featureMat);
	//saveResults(means, MEAN_VALUES_FILE);

	Mat cov = computeCovarianceMat(featureMat);
	//saveResults(cov, COV_MATRIX_FILE);

	Mat corr = computeCorrelationCoeff(featureMat);
	//saveResults(corr, CORR_MATRIX_FILE);

	int op = 0;
	do {
		std::cout << "Menu:" << std::endl;
		std::cout << "1 - Correlation coefficient between 2 features" << std::endl;
		std::cout << "2 - Plot prob density function for a single feature" << std::endl;
		std::cout << "3 - Plot prob density function for 2 features" << std::endl;
		std::cout << "0 - Exit" << std::endl;
		std::cin >> op;
		switch (op) { 
			case 1: {
				int x1, y1, x2, y2;
				std::cout << "Coeff 1: " << std::endl;
				std::cout << "x1 = ";
				std::cin >> x1;
				std::cout << "y1 = ";
				std::cin >> y1;
				std::cout << "Coeff 2: " << std::endl;
				std::cout << "x2 = ";
				std::cin >> x2;
				std::cout << "y2 = ";
				std::cin >> y2;
				std::cout << "Correlation coefficient = " << corr.at<float>(IMG_SIZE * x1 + y1, IMG_SIZE * x2 + y2) << std::endl;
				displayCorrelationChart(featureMat, x1, y1, x2, y2);
				break;
			}
			case 2: {
				int x;
				std::cout << "Feature (= pixel nr): " << std::endl;
				std::cout << "x = ";
				std::cin >> x;
				pdfForSingleFeature(featureMat, x);
				break;
			}
			case 3: {
				int x, y;
				std::cout << "Features (= pixel nr): " << std::endl;
				std::cout << "x = ";
				std::cin >> x;
				std::cout << "y = ";
				std::cin >> y;
				pdfFor2Features(featureMat, x, y);
				break;
			}
			case 0: break;
		}
	} while (op != 0);

	return 0;
}

std::vector<float> computeMeanValues(Mat featureMat) {
	std::vector<float> means;
	// p = nr of images
	int p = featureMat.rows;
	// N = nr of pixels
	int N = featureMat.cols;

	for (int i = 0; i < N; i++) {
		float mean = 0;
		for (int k = 0; k < p; k++) {
			mean += featureMat.at<uchar>(k, i);
		}
		mean /= (float)p;
		means.push_back(mean);
	}
	return means;
}

std::vector<float> computeStdDeviation(Mat featureMat) {
	std::vector<float> stdDeviations;
	// p = nr of images
	int p = featureMat.rows;
	// N = nr of pixels
	int N = featureMat.cols;

	std::vector<float> means = computeMeanValues(featureMat);

	for (int i = 0; i < N; i++) {
		float stdDev = 0;
		for (int k = 0; k < p; k++) {
			stdDev += pow(featureMat.at<uchar>(k, i) - means[i], 2);
		}
		stdDev /= (float)p;
		stdDeviations.push_back(sqrt(stdDev));
	}
	return stdDeviations;
}

Mat computeCovarianceMat(Mat featureMat) {
	// p = nr of images
	int p = featureMat.rows;
	// N = nr of pixels
	int N = featureMat.cols;

	std::vector<float> means = computeMeanValues(featureMat);
	Mat cov(N, N, CV_32FC1);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			float c = 0;
			for (int k = 0; k < p; k++) {
				c += (featureMat.at<uchar>(k, i) - means[i]) * (featureMat.at<uchar>(k, j) - means[j]);
			}
			c /= (float)p;
			cov.at<float>(i,j) = c;
		}
	}
	return cov;
}

Mat computeCorrelationCoeff(Mat featureMat) {
	Mat cov = computeCovarianceMat(featureMat);
	std::vector<float> stdDev = computeStdDeviation(featureMat);

	// N = nr of features
	int N = featureMat.cols;
	Mat corr(N, N, CV_32FC1);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			corr.at<float>(i, j) = cov.at<float>(i, j) / (stdDev[i] * stdDev[j]);
		}
	}

	return corr;
}

void saveResults(std::vector<float> values, const char output_path[]) {
	std::ofstream f(output_path);
	int N = values.size();

	for (int i = 0; i < N; i++) {
		f << values[i];
		if (i < N - 1) {
			f << ",";
		}
	}

	f.close();
}

void saveResults(Mat values, const char output_path[]) {
	std::ofstream f(output_path);

	int width = values.cols;
	int height = values.rows;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			f << values.at<float>(i, j);
			if (i < width - 1) {
				f << ",";
			}
		}
		f << "\n";
	}

	f.close();
}

void displayCorrelationChart(Mat featureMat, int x1, int y1, int x2, int y2) {
	Mat img(256, 256, CV_8UC1);
	img.setTo(255);

	int p = featureMat.rows;
	int N = featureMat.cols;
	int img_size = sqrt(N);

	int i = img_size * x1 + y1;
	int j = img_size * x2 + y2;

	for (int k = 0; k < p; k++) {
		int f1 = featureMat.at<uchar>(k, i);
		int f2 = featureMat.at<uchar>(k, j);
		img.at<uchar>(f1, f2) = 0;
	}
	imshow("Correlation chart", img);
	waitKey();
}

void pdfForSingleFeature(Mat featureMat, int x) {
	Mat img(256, 256, CV_8UC1);
	img.setTo(255);

	std::vector<float> means = computeMeanValues(featureMat);
	std::vector<float> stdDevs = computeStdDeviation(featureMat);

	float mean = means[x];
	float stdDev = stdDevs[x];
	std::vector<float> f; 

	int p = featureMat.rows;
	for (int k = 0; k < p; k++) {
		int curr = featureMat.at<uchar>(k, x);
		f.push_back(exp(-(curr - mean) * (curr - mean) / (2 * stdDev * stdDev)) / (sqrt(2 * PI) * stdDev));
	}

	float peak = 1.0 / (sqrt(2 * PI) * stdDev);

	for (int k = 0; k < p; k++) {
		int curr = featureMat.at<uchar>(k, x);
		f[k] = f[k] * 255 / peak;
		img.at<uchar>(255 - f[k], curr) = 0;
	}

	imshow("Pdf for single feature", img);
	waitKey();
}

void pdfFor2Features(Mat featureMat, int i, int j) {
	Mat img(256, 256, CV_8UC1);
	img.setTo(255);

	std::vector<float> means = computeMeanValues(featureMat);
	Mat cov = computeCovarianceMat(featureMat);

	float mean_i = means[i];
	float mean_j = means[j];

	int N = sqrt(featureMat.cols);
	Mat f(N,N,CV_32FC1);

	int p = featureMat.rows;
	for (int k = 0; k < p; k++) {
		int xi = featureMat.at<uchar>(k, i);
		int xj = featureMat.at<uchar>(k, j);
	}

	imshow("Pdf for a pair of features", img);
	waitKey();
}