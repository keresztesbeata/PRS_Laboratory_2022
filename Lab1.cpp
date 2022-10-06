
#include "stdafx.h"
#include "Util.h"

const char LEAST_SQUARE_POINTS_PATH[6][PATH_SIZE] = {
	"points_LeastSquares\\points0.txt",
	"points_LeastSquares\\points1.txt",
	"points_LeastSquares\\points2.txt",
	"points_LeastSquares\\points3.txt",
	"points_LeastSquares\\points4.txt",
	"points_LeastSquares\\points5.txt"
};

void model1(int k);
void model1_closed_form(int k);
void model2(int k);
void model3(int k);
void gradient_descent_1(int k, float lr);

int main1()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Model 1\n");
		printf(" 2 - Model 2\n");
		printf(" 3 - Model 3\n");
		printf(" 4 - Model 1 (closed normal form)\n");
		printf(" 5 - Model 1 Gradient descent\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
			case 1: {
				int k;
				printf("k = ");
				scanf("%d", &k);
				model1(k);
				break;
			}
			case 4: {
				int k;
				printf("k = ");
				scanf("%d", &k);
				model1_closed_form(k);
				break;
			}
			case 2: {
				int k;
				printf("k = ");
				scanf("%d", &k);
				model2(k);
				break;
			}
			case 3: {
				int k;
				printf("k = ");
				scanf("%d", &k);
				model3(k);
				break;
			}
			case 5: {
				int k;
				float lr;
				printf("k = ");
				scanf("%d", &k);
				printf("learning_rate = ");
				scanf("%f", &lr);
				gradient_descent_1(k, lr);
				break;
			}
		}
	} while (op != 0);
	return 0;
}

void model1(int k) {
	// read the points
	std::vector<Point2f> points = readPoints(LEAST_SQUARE_POINTS_PATH[k]);
	int N = points.size();

	// dimension of the image
	const int height = 500;
	const int width = 500;

	Mat img = initializeCanvas(height, width);
	drawPoints(img, points);

	float teta_0 = 0;
	float teta_1 = 0;

	float sxy = 0, sx = 0, sy = 0, sx2 = 0;

	for (int i = 0; i < N; i++) {
		sxy += points[i].x * points[i].y;
		sx += points[i].x;
		sx2 += points[i].x * points[i].x;
		sy += points[i].y;
	}

	teta_1 = (N * sxy - sx * sy) / (N * sx2 - sx * sx);
	teta_0 = (sy - teta_1 * sx) / N;

	int x1 = 0;
	int x2 = width - 1;
	int y1 = teta_0 + x1 * teta_1;
	int y2 = teta_0 + x2 * teta_1;

	line(img, Point(x1, y1), Point(x2, y2), RED);

	imshow("Model 1", img);
	waitKey();
	getchar();
}

void model1_closed_form(int k) {
	// read the points
	std::vector<Point2f> points = readPoints(LEAST_SQUARE_POINTS_PATH[k]);
	int N = points.size();

	// dimension of the image
	const int height = 500;
	const int width = 500;

	Mat img = initializeCanvas(height, width);
	drawPoints(img, points);

	Mat A(N, 2, CV_32FC1);
	for (int i = 0; i < N; i++) {
		A.at<float>(i, 0) = 1;
		A.at<float>(i, 1) = points[i].x;
	}

	Mat b(N, 1, CV_32FC1);

	for (int i = 0; i < N; i++) {
		b.at<float>(i, 0) = points[i].y;
	}

	Mat teta_opt = (A.t() * A).inv() * A.t() * b;
	float teta_0 = teta_opt.at<float>(0, 0);
	float teta_1 = teta_opt.at<float>(1, 0);

	int x1 = 0;
	int x2 = width - 1;

	int y1 = teta_0 + x1 * teta_1;
	int y2 = teta_0 + x2 * teta_1;

	line(img, Point(x1, y1), Point(x2, y2), RED);

	imshow("Model 1: closed normal form", img);
	waitKey();
}

void model2(int k) {
	// read the points
	std::vector<Point2f> points = readPoints(LEAST_SQUARE_POINTS_PATH[k]);
	int N = points.size();

	// dimension of the image
	const int height = 500;
	const int width = 500;

	Mat img = initializeCanvas(height, width);
	drawPoints(img, points);

	float sxy = 0, sx = 0, sy = 0, sx2 = 0, sy2 = 0;
	for (int i = 0; i < N; i++) {
		sxy += points[i].x * points[i].y;
		sx += points[i].x;
		sy += points[i].y;
		sx2 += points[i].x * points[i].x;
		sy2 += points[i].y * points[i].y;
	}
	
	float betaY = 2 * sxy - 2 * sx * sy / N;
	float betaX = sy2 - sx2 + (sx * sx - sy * sy) / N;

	float beta = - atan2(betaY, betaX) / 2;
	float ro = (cos(beta) * sx + sin(beta) * sy) / N;

	int x1 = 0;
	int x2 = width - 1;

	int y1 = (ro - x1 * cos(beta)) / sin(beta);
	int y2 = (ro - x2 * cos(beta)) / sin(beta);

	line(img, Point(x1, y1), Point(x2, y2), RED);

	imshow("Model 2", img);
	waitKey();
}

void model3(int k) {
	// read the points
	std::vector<Point2f> points = readPoints(LEAST_SQUARE_POINTS_PATH[k]);
	int N = points.size();

	// dimension of the image
	const int height = 500;
	const int width = 500;

	Mat img = initializeCanvas(height, width);
	drawPoints(img, points);

	Mat A(N, 3, CV_32FC1);
	for (int i = 0; i < N; i++) {
		A.at<float>(i, 0) = points[i].x;
		A.at<float>(i, 1) = points[i].y;
		A.at<float>(i, 2) = 1;
	}

	Mat eigValues;
	Mat eigVectors;
	eigen(A.t() * A, eigValues, eigVectors);

	float min = eigValues.at<float>(0, 0);
	int minIdx = 0;
	for (int i = 1; i < eigValues.rows; i++) {
		if (eigValues.at<float>(i, 0) < min) {
			min = eigValues.at<float>(i, 0);
			minIdx = i;
		}
	}

	Mat beta = eigVectors.col(minIdx);

	float a = beta.at<float>(0, 0);
	float b = beta.at<float>(1, 0);
	float c = beta.at<float>(2, 0);

	std::cout << a << " " << b << " " << c << std::endl;
	getchar();

	int x1 = 0;
	int x2 = width - 1;

	int y1 = - (c + a * x1) / b;
	int y2 = - (c + a * x2) / b;

	line(img, Point(x1, y1), Point(x2, y2), RED);

	char title[200];
	sprintf(title, "Model 3: (a, b, c) = (%f, %f, %f)", a, b, c);
	imshow(title, img);
	waitKey();
}

void gradient_descent_1(int k, float lr) {
	// read the points
	std::vector<Point2f> points = readPoints(LEAST_SQUARE_POINTS_PATH[k]);
	int N = points.size();

	// dimension of the image
	const int height = 500;
	const int width = 500;

	float sxy = 0, sx = 0, sy = 0, sx2 = 0;
	for (int i = 0; i < N; i++) {
		sxy += points[i].x * points[i].y;
		sx += points[i].x;
		sx2 += points[i].x * points[i].x;
		sy += points[i].y;
	}
	// final (optimal) parameters for the line
	float teta_1 = (N * sxy - sx * sy) / (N * sx2 - sx * sx);
	float teta_0 = (sy - teta_1 * sx) / N;

	int x1 = 0;
	int x2 = width - 1;
	int y1 = teta_0 + x1 * teta_1;
	int y2 = teta_0 + x2 * teta_1;

	// endpoints of the true line
	Point start = Point(x1, y1);
	Point end = Point(x2, y2);

	float e = 10 ^ -4;
	Mat teta(2, 1, CV_32FC1);
	Mat new_teta(2, 1, CV_32FC1);
	new_teta.at<float>(0, 0) = rand() % 100;
	new_teta.at<float>(1, 0) = rand() % 100;

	int step = 0;
	do {
		// display the line at each step
		std::cout << "step: " << step << std::endl;

		teta = new_teta.clone();
		//std::cout << "teta = (" << teta.at<float>(0, 0) << ", " << teta.at<float>(1, 0) << ")" << std::endl;
		//getchar();

		// compute the loss
		Mat grad(2, 1, CV_32FC1);
		grad.at<float>(0, 0) = 0;
		grad.at<float>(1, 0) = 0;
		for (int i = 0; i < N; i++) {
			float f = teta.at<float>(0, 0) + teta.at<float>(1, 0) * points[i].x;
			grad.at<float>(0, 0) += f - points[i].y;
			grad.at<float>(1, 0) += (f - points[i].y) * points[i].x;
		}
		new_teta = teta - lr * grad;

		int x1 = 0;
		int x2 = width - 1;
		int y1 = new_teta.at<float>(0, 0) + x1 * new_teta.at<float>(1, 0);
		int y2 = new_teta.at<float>(0, 0) + x2 * new_teta.at<float>(1, 0);

		Mat img = initializeCanvas(height, width);
		drawPoints(img, points);

		line(img, Point(x1, y1), Point(x2, y2), RED);

		// final line equation
		line(img, start, end, GREEN);

		char title[200];
		sprintf(title, "Grad desc, Model 1: step %d teta=[%f, %f]", step, new_teta.at<float>(0, 0), new_teta.at<float>(1, 0));
		imshow(title, img);
		waitKey();

		step++;
	} while ((teta.at<float>(0, 0) - new_teta.at<float>(0, 0) >= e) || (teta.at<float>(1, 0) - new_teta.at<float>(1, 0) >= e));
}