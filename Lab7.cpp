
#include "stdafx.h"
#include "Util.h"
#include <random>


const char KMEANS_POINTS_PATH[5][PATH_SIZE] = {
	"images_Kmeans\\points1.bmp",
	"images_Kmeans\\points2.bmp",
	"images_Kmeans\\points3.bmp",
	"images_Kmeans\\points4.bmp",
	"images_Kmeans\\points5.bmp"
};

const char KMEANS_IMAGES_PATH[6][PATH_SIZE] = {
	"images_Kmeans\\img01.jpg",
	"images_Kmeans\\img02.jpg",
	"images_Kmeans\\img03.jpg",
	"images_Kmeans\\img04.jpg",
	"images_Kmeans\\img05.jpg",
	"images_Kmeans\\img06.jpg"
};

void kmeans(std::vector<std::vector<float>> x, std::vector<std::vector<float>>& m, std::vector<int>& labels, int K, int maxIt);
Mat voronoi(Mat img, std::vector<std::vector<float>> x, std::vector<std::vector<float>> m, std::vector<int> labels, std::vector<Vec3b> colors, int K);

int main()
{
	int op = 0;
	do {
		std::cout << "Menu:" << std::endl;
		std::cout << "1 - K-means clustering on 2D points (D = 2)" << std::endl;
		std::cout << "2 - K-means clustering on grayscale image (D = 1)" << std::endl;
		std::cout << "3 - K-means clustering on color image (D = 3)" << std::endl;
		std::cout << "0 - Exit" << std::endl;
		std::cin >> op;
		switch (op) {
			case 1: {
				int k;
				std::cout << "k = ";
				std::cin >> k;
				int f;
				std::cout << "input points file[1->5]: f = ";
				std::cin >> f;
				int maxIt;
				std::cout << "max iterations = ";
				std::cin >> maxIt;
				Mat img = imread(KMEANS_POINTS_PATH[f - 1], IMREAD_GRAYSCALE);
				std::vector<std::vector<float>> x;
				int width = img.cols;
				int height = img.rows;
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						if (img.at<uchar>(i, j) == 0) {
							std::vector<float> p;
							// 2D points
							p.push_back(i);
							p.push_back(j);
							x.push_back(p);
						}
					}
				}
				std::random_device r;
				std::default_random_engine gen{ r() };
				std::uniform_int_distribution<int> dist_color(0, 255);
				std::vector<Vec3b> colors(k);
				for (int i = 0; i < k; i++) {
					colors[i] = { (uchar)dist_color(gen), (uchar)dist_color(gen), (uchar)dist_color(gen) };
					std::cout << i << " " << colors[i] << std::endl;
				}

				int N = x.size();
				std::vector<int> labels(N, 0);
				std::vector<std::vector<float>> m(k);
				kmeans(x, m, labels, k, maxIt);
				Mat out(height,width, CV_8UC3);
				out.setTo(Vec3b({ 255, 255, 255 }));
				for (int l = 0; l < N; l++) {
					int i = x[l][0];
					int j = x[l][1];
					out.at<Vec3b>(i, j) = colors[labels[l]];
				}
				imshow("Labeled points", out);
				Mat V = voronoi(img, x, m, labels, colors, k);
				imshow("Voronoi", V);
				waitKey(0);
				break;
				}
			case 2: {
				int k;
				std::cout << "k = ";
				std::cin >> k;
				int f;
				std::cout << "input images file[1->6]: img = ";
				std::cin >> f;
				int maxIt;
				std::cout << "max iterations = ";
				std::cin >> maxIt;
				Mat img = imread(KMEANS_IMAGES_PATH[f - 1], IMREAD_GRAYSCALE);
				std::vector<std::vector<float>> x;
				int width = img.cols;
				int height = img.rows;
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						std::vector<float> p;
						// 1D points
						p.push_back(img.at<uchar>(i, j));
						x.push_back(p);
					}
				}
				int N = x.size();
				std::vector<int> labels(N, 0);
				std::vector<std::vector<float>> m(k);
				kmeans(x, m, labels, k, maxIt);
				Mat out = img.clone();
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						int idx = i * width + j;
						out.at<uchar>(i, j) = (uchar) m[labels[idx]][0];
					}
				}
				imshow("Labeled points", out);
				waitKey(0);
				break;
			}
			case 3: {
				int k;
				std::cout << "k = ";
				std::cin >> k;
				int f;
				std::cout << "input images file[1->6]: img = ";
				std::cin >> f;
				int maxIt;
				std::cout << "max iterations = ";
				std::cin >> maxIt;
				Mat img = imread(KMEANS_IMAGES_PATH[f - 1], IMREAD_COLOR);
				std::vector<std::vector<float>> x;
				int width = img.cols;
				int height = img.rows;
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						std::vector<float> p;
						// 3D points
						p.push_back(img.at<Vec3b>(i, j)[0]);
						p.push_back(img.at<Vec3b>(i, j)[1]);
						p.push_back(img.at<Vec3b>(i, j)[2]);
						x.push_back(p);
					}
				}
				int N = x.size();
				std::vector<int> labels(N, 0);
				std::vector<std::vector<float>> m(k);
				kmeans(x, m, labels, k, maxIt);
				Mat out = img.clone();
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						int idx = i * width + j;
						out.at<Vec3b>(i, j)[0] = (uchar)m[labels[idx]][0];
						out.at<Vec3b>(i, j)[1] = (uchar)m[labels[idx]][1];
						out.at<Vec3b>(i, j)[2] = (uchar)m[labels[idx]][2];
					}
				}
				imshow("Labeled points", out);
				waitKey(0);
				break;
			}
			case 0: break;
		}
	} while (op != 0);

	return 0;
}

Mat voronoi(Mat img, std::vector<std::vector<float>> x, std::vector<std::vector<float>> m, std::vector<int> labels, std::vector<Vec3b> colors, int K) {
	int height = img.rows;
	int width = img.cols;
	Mat out = Mat(height, width, CV_8UC3);

	// find the closest mean for each of the points (also background)
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			float minDist = INT_MAX;
			int label = 0;
			for (int k = 0; k < K; k++) {
				// Euclidean dist
				float dist = sqrt((i - m[k][0]) * (i - m[k][0]) + (j - m[k][1]) * (j - m[k][1]));
				if (dist < minDist) {
					minDist = dist;
					label = k;
				}
			}

			out.at<Vec3b>(i, j) = colors[label];
		}
	}

	return out;
}

void kmeans(std::vector<std::vector<float>> x, std::vector<std::vector<float>>& m, std::vector<int>& labels, int K, int maxIt) {
	int N = x.size();
	int d = x[0].size();
	// initialization: randomly select k centers
	std::random_device r;
	std::default_random_engine gen{ r() };
	std::uniform_int_distribution<int> dist_center(1, N);
	for (int i = 0; i < K; i++) {
		m[i] = x[dist_center(gen)];
		std::cout << m[i][0] << " ";
	}
	std::cout<<std::endl;

	for (int it = 0; it < maxIt; it++) {	
		// assignment of points to closest cluster center
		bool change = false;
		for (int i = 0; i < N; i++) {
			float minDist = INT_MAX;
			int minK = 0;
			for (int k = 0; k < K; k++) {
				float sum = 0;
				for (int j = 0; j < d; j++) {
					sum += (x[i][j] - m[k][j]) * (x[i][j] - m[k][j]);
				}
				float dist = sqrt(sum);
				if (dist < minDist) {
					minDist = dist;
					minK = k;
				}
			}

			if (labels[i] != minK) {
				change = true;
				labels[i] = minK;
			}
		}

		// halting condition
		if (!change) {
			return;
		}

		// update: recalculate cluster centers
		for (int k = 0; k < K; k++) {
			std::vector<float> sum(d, 0);
			int count = 0;
			for (int i = 0; i < N; i++) {
				if (labels[i] == k) {
					count++;
					for (int j = 0; j < d; j++) {
						sum[j] += x[i][j];
					}
				}
			}
			for (int j = 0; j < d; j++) {
				m[k][j] = sum[j] / count;
			}
		}
	}
}