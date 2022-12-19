#include "stdafx.h"
#include "Util.h"

#define MAXT 100

typedef struct _weaklearner {
	int feature_i;
	int threshold;
	int class_label;
	float error;
	int classify(Mat X) {
		if (X.at<double>(0, feature_i) < threshold)
			return class_label;
		else
			return - class_label;
	}
}WeeklearnerT;


typedef struct _classifier {
	int T;
	float alphas[MAXT];
	WeeklearnerT hs[MAXT];
	int classify(Mat X) {
		double vote = 0;
		for (int i = 0; i < T; i++) {
			vote += alphas[i] * hs[i].classify(X);
		}
		return vote > 0 ? 1 : -1;
	}
}ClassifierT;


ClassifierT adaboost(Mat img, Mat X, Mat y, Mat& w, int T);
WeeklearnerT findWeekLearner(Mat X, Mat y, Mat w, int img_size);
void drawBoundary(Mat img, ClassifierT cl);

int main()
{
	// allocate feature matrix and label vector
	int nrImages = 6;
	int trainIdx = 0;

	while (1) {
		std::cout << "image index = ";
		std::cin >> trainIdx;
		if (trainIdx > nrImages) {
			std::cout << "There are only " << nrImages << " images!" << std::endl;
			return 1;
		}
		char fname[PATH_SIZE];
		sprintf(fname, "images_AdaBoost/points%d.bmp", trainIdx);
		Mat img = imread(fname);
		int width = img.cols;
		int height = img.rows;

		int n = 0;
		std::vector<Vec2b> x_train;
		std::vector<int> y_train;

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (img.at<Vec3b>(i, j) == RED) {
					x_train.push_back(Vec2b(j, i));
					y_train.push_back(1);
					n++;
				}
				else if (img.at<Vec3b>(i, j) == BLUE) {
					x_train.push_back(Vec2b(j, i));
					y_train.push_back(-1);
					n++;
				}
			}
		}

		Mat X(n, 2, CV_64FC1);
		Mat y(n, 1, CV_32SC1);

		for (int i = 0; i < n; i++) {
			X.at<double>(i, 0) = x_train[i][0];
			X.at<double>(i, 1) = x_train[i][1];
			y.at<int>(i, 0) = y_train[i];
		}

		Mat w(n, 1, CV_64FC1);

		int T;
		std::cout << "T=";
		std::cin >> T;

		ClassifierT cl = adaboost(img, X, y, w, T);

		drawBoundary(img, cl);

		int ans;
		std::cout << "Again? (Yes=1/No=0)\n>> ";
		std::cin >> ans;
		if (ans == 0) {
			break;
		}
	}
	return 0;
}

WeeklearnerT findWeekLearner(Mat X, Mat y, Mat w, int img_size) {
	WeeklearnerT best;
	best.error = INT_MAX;
	// nr of samples
	int n = X.rows;
	// nr of features
	int d = X.cols;
	Mat z(n, 1, CV_32SC1);
	z.setTo(0);
	for (int j = 0; j < d; j++) {
		for (int threshold = 0; threshold < img_size; threshold++) {
			int class_labels[] = { -1,1 };
			for (int l = 0; l < 2; l++) {
				double e = 0;
				for (int i = 0; i < n; i++) {
					if (X.at<double>(i, j) < threshold) {
						z.at<int>(i, 0) = class_labels[l];
					}
					else {
						z.at<int>(i, 0) = -class_labels[l];
					}
					if (z.at<int>(i, 0) * y.at<int>(i, 0) < 0) {
						e += w.at<double>(i, 0);
					}
				}
				if (e < best.error) {
					best.error = e;
					best.feature_i = j;
					best.threshold = threshold;
					best.class_label = class_labels[l];
				}
			}
		}
	}
	return best;
}

ClassifierT adaboost(Mat img, Mat X, Mat y, Mat& w, int T) {
	int n = X.rows;
	w.setTo(1.0 / float(n));
	ClassifierT cl;
	cl.T = T;

	for (int t = 0; t < T; t++) {
		WeeklearnerT wl = findWeekLearner(X, y, w, img.rows);
		cl.hs[t] = wl;
		cl.alphas[t] = 0.5 * log((1 - wl.error) / wl.error);

		std::cout << "Week learner nr " << t << ":" << std::endl;
		std::cout << " - label = " << wl.class_label << std::endl;
		std::cout << " - error = " << wl.error << std::endl;
		std::cout << " - feature = " << wl.feature_i << std::endl;
		std::cout << " - Threshold = " << wl.threshold << std::endl;
		std::cout << " - alpha = " << cl.alphas[t] << std::endl;
		std::cout << "------------------------------------------\n";
		double s = 0;

		for (int i = 0; i < n; i++) {
			w.at<double>(i, 0) *= exp(-cl.alphas[t] * y.at<int>(i, 0) * cl.hs[t].classify(X.row(i)));
			s += w.at<double>(i, 0);
		}

		for (int i = 0; i < n; i++) {
			w.at<double>(i, 0) /= s;
		}
	}
	return cl;
}


void drawBoundary(Mat img, ClassifierT cl) {
	Mat outImg = img.clone();
	int height = img.rows;
	int width = img.cols;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (img.at<Vec3b>(i, j) == Vec3b({ 255, 255, 255 })) {
				Mat x(1, 2, CV_64FC1);
				x.at<double>(0, 0) = j;
				x.at<double>(0, 1) = i;
				int label = cl.classify(x);
				if (label > 0) {
					outImg.at<Vec3b>(i, j) = Vec3b({ 0, 255, 255 });
				}
				else {
					outImg.at<Vec3b>(i, j) = Vec3b({ 255, 255, 0 });
				}
			}
		}
	}

	imshow("output", outImg);
	waitKey(0);
}