
#include "stdafx.h"
#include "Util.h"

#include <opencv2/imgcodecs/legacy/constants_c.h>

const char RANSAC_POINTS_PATH[6][PATH_SIZE] = {
	"points_RANSAC\\points1.bmp",			// q = 0.3
	"points_RANSAC\\points1-res.bmp",		// q = 0.8
	"points_RANSAC\\points2.bmp",			// q = 0.5
	"points_RANSAC\\points3.bmp",			// q = 0.9
	"points_RANSAC\\points4.bmp",			// q = 0.7
	"points_RANSAC\\points5.bmp"			// q = 0.6
};

void lab2();
/*
* Apply the RANSAC algorithm for fitting the line to the data points.
* k = index of the input file
* t = distance threshold from any point to the model(line)
* s = nr of points in the sample set
* p = prob that none of the selected samples s contains any outliers
* q = prob that any of the selected points is an inlier
*/
void ransac(int k, int t, int s, float p, float q);
/*
* Calculate N as the nr of trials to find the best model.
* s = nr of points in the sample set
* p = prob that none of the selected samples s contains any outliers
* q = prob that any of the selected points is an inlier
*/
int calculateNrTrials(int s, float p, float q);
/*
* Calculate how large the consensus dataset should be.
* T = n*q.
* n = nr of data points
* q = prob that any of the selected points is an inlier (dist < t)
*/
int calculateConsensusThreshold(int n, float q);


int main()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - RANSAC\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
			case 1: {
				int k;
				int s = 2;
				int t = 10;
				float p = 0.99;
				float q;
				printf("k = ");
				scanf("%d", &k);
				printf("q = ");
				scanf("%f", &q);
				ransac(k, t, s, p, q);
				break;
			}
		}
	} while (op != 0);
	return 0;
}


void ransac(int k, int t, int s, float p, float q) {
	Mat img = imread(RANSAC_POINTS_PATH[k], CV_LOAD_IMAGE_GRAYSCALE);
	// read the points
	std::vector<Point2f> points = getPointsFromImage(img);
	int N = points.size();

	// dimension of the image
	const int height = img.rows;
	const int width = img.cols;

	// choose 2 random points
	Point2f p1 = points[rand() % N];
	Point2f p2 = points[rand() % N];

	Line * optLine = NULL;
	int maxInliers = 0;
	int nrTrials = calculateNrTrials(s, p, q);
	for (int l = 0; l < nrTrials; l++) {
		// compute the line eq
		Line * line = computeLineEq(p1, p2);

		// find the dist of each point to the line and count the nr of inliers
		int inliers = 0;
		for (int i = 0; i < N; i++) {
			if (distPointToLine(points[i], *line) < t) {
				inliers++;
			}
		}
		if (inliers > maxInliers) {
			optLine = cloneLine(*line);
			maxInliers = inliers;
		}

		int consesusThreshold = calculateConsensusThreshold(N, q);
		if (inliers > consesusThreshold) {
			// terminate if the nr of inliers in the consensus set is greater than the treshold
			break;
		}
	}

	Mat dest = initializeCanvas(height, width);
	drawPoints(dest, points);
	if (optLine != NULL) {
		drawLine(dest, *optLine);
	}

	imshow("Ransac method", dest);
	waitKey();
}

int calculateNrTrials(int s, float p, float q) {
	return log(1 - p) / log(1 - pow(q, s));
}

int calculateConsensusThreshold(int n, float q) {
	return n * q;
}
