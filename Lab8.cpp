#include "stdafx.h"
#include "Util.h"
#include <random>
#include <algorithm>

void calcHist(Mat img, int nr_bins, int * hist);
void calcColorHist(Mat img, int nr_bins, int* hist);
float calcDist(Mat x, int a, int b);
int knn(int nr_classes, Mat X, Mat y, int unknown, int K);
void confusionMatrix(Mat y, Mat y_pred, Mat& M);
float calcAccuracy(Mat M);

typedef struct distance {
	float dist;
	int label;
	bool operator < (const distance& o) const {
		return dist < o.dist;
	}
}Distance;

typedef struct data {
	std::string file;
	int label;
}Data;

int main8()
{
	const int nrclasses = 6;
	char classes[nrclasses][10] = { "beach", "city", "desert", "forest", "landscape", "snow" };

	int hist_size;
	std::cout << "hist_size = ";
	std::cin >> hist_size;
	// allocate feature matrix and label vector
	int nrinst = 672;
	int feature_dim = 3 * hist_size;
	Mat X_train(nrinst, feature_dim, CV_32FC1);
	Mat y_train(nrinst, 1, CV_8UC1);

	// load the train set
	std::vector<Data> train_set(nrinst);

	int nrFiles = 0;
	for (int i = 0; i < nrclasses; i++) {
		int rowX = 0;
		while (1) {
			char fname[PATH_SIZE];
			sprintf(fname, "images_KNN/train/%s/%06d.jpeg", classes[i], rowX);
			Mat img = imread(fname);
			if (img.cols == 0) {
				break;
			}
			// calculate the histogram in hist
			int* hist = (int*)malloc(sizeof(int) * feature_dim);
			if (hist == NULL) {
				return -1;
			}
			calcColorHist(img, hist_size, hist);
			for (int d = 0; d < feature_dim; d++) {
				X_train.at<float>(rowX, d) = hist[d];
			}

			free(hist);
			y_train.at<uchar>(nrFiles, 0) = i;
			std::string str(fname);
			train_set[nrFiles] = { str, nrFiles };
			rowX++;
			nrFiles++;
		}
	}

	int nrTestinst = 85;
	Mat X_test(nrTestinst, feature_dim, CV_32FC1);
	Mat y_test(nrTestinst, 1, CV_8UC1);

	// load the test set
	std::vector<Data> test_set(nrTestinst);

	int nrTestFiles = 0;
	for (int i = 0; i < nrclasses; i++) {
		int rowX = 0;
		while (1) {
			char fname[PATH_SIZE];
			sprintf(fname, "images_KNN/test/%s/%06d.jpeg", classes[i], rowX);
			Mat img = imread(fname);
			if (img.cols == 0) {
				break;
			}
			// calculate the histogram in hist
			int* hist = (int*)malloc(sizeof(int) * feature_dim);
			if (hist == NULL) {
				return -1;
			}
			calcColorHist(img, hist_size, hist);
			for (int d = 0; d < feature_dim; d++) {
				X_test.at<float>(rowX, d) = hist[d];
			}

			free(hist);
			y_test.at<uchar>(nrTestFiles, 0) = i;
			std::string str(fname);
			test_set[nrTestFiles] = {str, nrTestFiles};
			rowX++;
			nrTestFiles++;
		}
	}

	while(1) {
		int K;
		std::cout << "K = ";
		std::cin >> K;

		
		int rand_sample = rand() % nrinst;
		int pred_label = knn(nrclasses, X_train, y_train, rand_sample, K);
		std::cout << "Sample:" << std::endl;
		std::cout << "True label = " << classes[y_train.at<uchar>(rand_sample, 0)] << std::endl;
		std::cout << "Predicted label = " << classes[pred_label] << std::endl;
		Mat sample_img = imread(train_set[rand_sample].file);
		imshow("Sample image", sample_img);
		waitKey(0);

		Mat y_pred(nrTestinst, 1, CV_8UC1);
		y_pred.setTo(0);
		// evaluate the model on the whole test set
		for (int i = 0; i < nrTestinst; i++) {
			y_pred.at<uchar>(i, 0) = knn(nrclasses, X_test, y_test, i, K);
		}

		// compute the confusion matrix
		Mat M(nrclasses, nrclasses, CV_32SC1);
		confusionMatrix(y_test, y_pred, M);

		// calculate the accuracy
		float acc = calcAccuracy(M);
		std::cout << "Accuracy = " << acc << std::endl;

		std::cout << "Continue (1) / Exit (0) ?" << std::endl;
		std::cout << ">> ";
		int op;
		std::cin >> op;
		if (op == 0) {
			break;
		}
	}

	return 0;
}


void calcHist(Mat img, int nr_bins, int * hist) {
	int range = 256 / nr_bins;
	int width = img.cols;
	int height = img.rows;

	for (int i = 0; i < nr_bins; i++) {
		hist[i] = 0;
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int bin = img.at<uchar>(i, j) / range;
			hist[bin]++;
		}
	}
}

void calcColorHist(Mat img, int nr_bins, int * hist) {
	int width = img.cols;
	int height = img.rows;

	for (int i = 0; i < 3*nr_bins; i++) {
		hist[i] = 0;
	}

	int range = 256 / nr_bins;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			// offset of RGB histograms
			int r_offset = 0;
			int g_offset = nr_bins;
			int b_offset = 2 * nr_bins;
			// compute the index of the bin for each channel
			int r_idx = img.at<Vec3b>(i, j)[2] / range;
			int g_idx = img.at<Vec3b>(i, j)[1] / range;
			int b_idx = img.at<Vec3b>(i, j)[0] / range;
			// increment the value in the bin for each RGB channel
			hist[r_offset + r_idx]++;
			hist[g_offset + g_idx]++;
			hist[b_offset + b_idx]++;
		}
	}
}

float calcDist(Mat x, int a, int b) {
	// compute the Euclidean dist between feature a and b
	float dist = 0;
	for (int i = 0; i < x.cols; i++) {
		dist += pow(x.at<float>(a, i) - x.at<float>(b, i), 2);
	}
	return sqrt(dist);
}

int knn(int nr_classes, Mat X, Mat y, int unknown, int K) {
	int n = X.rows;
	int d = X.cols;
	std::vector<Distance> distances;
	for (int i = 0; i < n; i++) {
		if (i != unknown) {
			float dist = calcDist(X, unknown, i);
			distances.push_back({ dist, y.at<uchar>(i, 0) });
		}
	}
	std::sort(distances.begin(), distances.end());
	// vote histogram Cx1
	int C = nr_classes;
	std::vector<int> votes(C);
	// initialize votes to 0
	for (int i = 0; i < C; i++) {
		votes[i] = 0;
	}
	int maxH = 0;
	int label = -1;
	// count the votes
	std::cout << "------------------ DIST --------------------" << std::endl;
	for (int j = 0; j < K; j++) {
		std::cout << distances[j].dist << " ";
		votes[distances[j].label]++;
	}
	std::cout << std::endl;
	for (int i = 0; i < C; i++) {
		if (votes[i] > maxH) {
			maxH = votes[i];
			label = i;
		}
	}
	std::cout << "\n------------------ VOTES --------------------" << std::endl;
	for (int i = 0; i < C; i++) {
		std::cout << votes[i] << " ";
	}
	std::cout << std::endl;
	return label;
}

void confusionMatrix(Mat y, Mat y_pred, Mat& M) {
	// square matrix of size (nrclasses x nrclasses)
	int nrInst = y.rows;
	M.setTo(0);
	for (int c = 0; c < nrInst; c++) {
		// nr of classes classified as class i while having true class j
		int i = y_pred.at<uchar>(c, 0);
		int j = y.at<uchar>(c, 0);
		M.at<int>(i, j)++;
	}
	std::cout << "True values: ";
	for (int i = 0; i < nrInst; i++) {
		std::cout << (int) y.at<uchar>(i,0) << " ";
	}
	std::cout << std::endl;
	std::cout << "Predicted values: ";
	for (int i = 0; i < nrInst; i++) {
		std::cout << (int) y_pred.at<uchar>(i, 0) << " ";
	}
	std::cout << std::endl;
	std::cout << "Confusion matrix:"<<std::endl;
	// display the confusion matrix
	for (int i = 0; i < M.rows; i++) {
		for (int j = 0; j < M.cols; j++) {
			std::cout << M.at<int>(i, j) << " ";
		}
		std::cout << std::endl;
	}
}

float calcAccuracy(Mat M) {
	int nr_classes = M.rows;
	int correct = 0;
	float total = 0;
	for (int i = 0; i < nr_classes; i++) {
		correct += M.at<int>(i, i);
		for (int j = 0; j < nr_classes; j++) {
			total += M.at<int>(i, j);
		}
	}
	return correct / total;
}