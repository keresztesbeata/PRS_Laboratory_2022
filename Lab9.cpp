#include "stdafx.h"
#include "Util.h"
#include <random>


void computePriors(Mat X, Mat y, Mat& priors);
void computeLikelihood(Mat X, Mat y, Mat T, Mat priors, Mat& likelihood);
int classifyBayes(Mat X, Mat priors, Mat likelihood);
void calcConfusionMatrix(Mat y, Mat y_pred, Mat& M);
float calcErrorRate(Mat M);

typedef struct data {
	std::string file;
	int label;
}Data;

int main()
{
	const int C = 2;// 3;
	// allocate feature matrix and label vector
	int nrTrainInst = 200; //60000;
	int img_size = 28;
	int d = img_size*img_size;
	Mat X_train(nrTrainInst, d, CV_8UC1);
	Mat y_train(nrTrainInst, 1, CV_8UC1);

	// load the train set
	std::vector<Data> train_set(nrTrainInst);

	int nrFiles = 0;
	for (int c = 0; c < C; c++) {
		int rowX = 0;
		while (rowX < nrTrainInst/2) {
			char fname[PATH_SIZE];
			sprintf(fname, "images_Bayes/test/%d/%06d.png", c, rowX);
			Mat img = imread(fname);
			if (img.cols == 0) {
				break;
			}
			// binarize the image
			Mat binary_img;
			threshold(img, binary_img, 128, 255, THRESH_BINARY);
			// flatten pixels and add to feature matrix X
			for (int i = 0; i < img_size; i++) {
				for (int j = 0; j < img_size; j++) {
					X_train.at<uchar>(nrFiles, i*img_size+j) = binary_img.at<uchar>(i,j);
				}
			}
			y_train.at<uchar>(nrFiles, 0) = c;
			std::string str(fname);
			train_set[nrFiles] = { str, c };
			rowX++;
			nrFiles++;
		}
	}

	int nrTestInst = 100;
	Mat X_test(nrTestInst, d, CV_8UC1);
	Mat y_test(nrTestInst, 1, CV_8UC1);

	// load the test set
	std::vector<Data> test_set(nrTestInst);

	int nrTestFiles = 0;
	for (int c = 0; c < C; c++) {
		int rowX = 0;
		while (rowX < nrTestInst/2) {
			char fname[PATH_SIZE];
			sprintf(fname, "images_Bayes/test/%d/%06d.png", c, nrTrainInst+rowX);
			Mat img = imread(fname);
			if (img.cols == 0) {
				break;
			}
			// binarize the image
			Mat binary_img(nrTestInst, d, CV_8UC1);
			threshold(img, binary_img, 128, 255, THRESH_BINARY);
			// flatten pixels and add to feature matrix X
			for (int i = 0; i < img_size; i++) {
				for (int j = 0; j < img_size; j++) {
					X_test.at<uchar>(nrTestFiles, i * img_size + j) = binary_img.at<uchar>(i, j);
				}
			}
			y_test.at<uchar>(nrTestFiles, 0) = c;
			std::string str(fname);
			test_set[nrTestFiles] = { str, c };
			rowX++;
			nrTestFiles++;
		}
	}

	Mat priors(C, 1, CV_64FC1);
	Mat likelihood(C, d, CV_64FC1);

	// train the model
	computePriors(X_train, y_train, priors);
	Mat T(1, d, CV_8UC1);
	T.setTo(255);
	computeLikelihood(X_train, y_train, T, priors, likelihood);

	int op;
	do {
		int rand_sample = rand() % nrTrainInst;
		computeLikelihood(X_train, y_train, X_train.row(rand_sample), priors, likelihood);
		int pred_label = classifyBayes(X_train, priors, likelihood);
		std::cout << "Sample:" << std::endl;
		std::cout << "True label = " << (int) y_train.at<uchar>(rand_sample, 0) << std::endl;
		std::cout << "Predicted label = " << pred_label << std::endl;
		Mat sample_img = imread(train_set[rand_sample].file);
		imshow("Sample image", sample_img);
		waitKey(0);

		std::cout << "Next sample (1) / Done (0) ?\n>>";
		std::cin >> op;
	} while (op != 0);

	Mat y_pred(nrTestInst, 1, CV_8UC1);
	// evaluate the model on the whole test set
	for (int i = 0; i < nrTestInst; i++) {
		computeLikelihood(X_test, y_test, X_test.row(i), priors, likelihood);
		y_pred.at<uchar>(i, 0) = classifyBayes(X_test, priors, likelihood);
	}

	// compute the confusion matrix
	Mat M(C, C, CV_32SC1);
	calcConfusionMatrix(y_test, y_pred, M);

	// error rate (nr of misclassified samples), complementary to Accuracy
	float errorRate = calcErrorRate(M);
	std::cout << "Error rate = " << errorRate << std::endl;
	std::cout << "Accuracy = " << 1 - errorRate << std::endl;

	return 0;
}

void computePriors(Mat X, Mat y, Mat& priors) {
	int n = X.rows;
	int C = priors.rows;
	priors.setTo(0); 

	// calculate priors of each class
	for (int c = 0; c < C; c++) {
		double count = 0;
		for (int k = 0; k < n; k++) {
			if (y.at<uchar>(k, 0) == c) {
				count++;
			}
		}
		priors.at<double>(c, 0) = count / (double)n;
	}
}


void computeLikelihood(Mat X, Mat y, Mat T, Mat priors, Mat& likelihood) {
	int n = X.rows;
	int d = X.cols;
	int C = priors.rows;
	likelihood.setTo(0);

	// calculate the likelihood of each feature from X being equal with T[j] given class c
	for (int c = 0; c < C; c++) {
		for (int j = 0; j < d; j++) {
			double count = 1;
			double ni = 0;
			for (int k = 0; k < n; k++) {
				if (y.at<uchar>(k, 0) == c) {
					ni++;
					if (X.at<uchar>(k, j) == T.at<uchar>(0, j)) {
						count++;
					}
				}
			}
			// apply Laplace smoothing to avoid 0 values
			likelihood.at<double>(c, j) = count / (double)(ni + C);
		}
	}
}

int classifyBayes(Mat X, Mat priors, Mat likelihood) {
	int C = priors.rows;
	int n = X.rows;
	int d = X.cols;
	Mat posterior(C, d, CV_64FC1);
	posterior.setTo(0);

	// calculate the posterios probabilities
	for (int c = 0; c < C; c++) {
		for (int i = 0; i < d; i++) {
			double sum = 0;
			for (int j = 0; j < d; j++) {
				sum += log(likelihood.at<double>(c, j));
			}
			posterior.at<double>(c, i) = log(priors.at<double>(c, 0)) + sum;
		}
	}

	// find class with highest posterior probability
	int label = -1;
	double max = INT_MIN;
	for (int i = 0; i < d; i++) {
		for (int c = 0; c < C; c++) {
			//
			if (posterior.at<double>(c, i) > max) {
				max = posterior.at<double>(c, i);
				label = c;
			}
		}
	}
	return label;
}

void calcConfusionMatrix(Mat y, Mat y_pred, Mat& M) {
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
		std::cout << (int)y.at<uchar>(i, 0) << " ";
	}
	std::cout << std::endl;
	std::cout << "Predicted values: ";
	for (int i = 0; i < nrInst; i++) {
		std::cout << (int)y_pred.at<uchar>(i, 0) << " ";
	}
	std::cout << std::endl;
	std::cout << "Confusion matrix:" << std::endl;
	// display the confusion matrix
	for (int i = 0; i < M.rows; i++) {
		for (int j = 0; j < M.cols; j++) {
			std::cout << M.at<int>(i, j) << " ";
		}
		std::cout << std::endl;
	}
}

float calcErrorRate(Mat M) {
	int nr_classes = M.rows;
	int correct = 0;
	int total = 0;
	for (int i = 0; i < nr_classes; i++) {
		correct += M.at<int>(i, i);
		for (int j = 0; j < nr_classes; j++) {
			total += M.at<int>(i, j);
		}
	}
	return 1 - correct / (float)total;
}