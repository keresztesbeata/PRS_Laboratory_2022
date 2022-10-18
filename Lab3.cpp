
#include "stdafx.h"
#include "Util.h"

#include <opencv2/imgcodecs/legacy/constants_c.h>

const char HOUGH_IMAGES_PATH[4][PATH_SIZE] = {
	"images_Hough\\edge_simple.bmp",
	"images_Hough\\edge_complex.bmp",			
	"images_Hough\\image_simple.bmp",
	"images_Hough\\image_complex.bmp"
};

typedef struct peak {
	int theta, ro, hval;
	bool operator < (const peak& o) const {
		return hval > o.hval;
	}
}Peak;


Mat houghAccumulator(Mat img, std::vector<Point2f> edgePoints);
std::vector<Point2f> getEdgePoints(Mat img, bool doComputeEdges);
/*
* Extract the local peaks from the hough accumulator (H) from a (n x n) neighbourhood.
* Return the list of local peak points.
*/
std::vector<Peak> getLocalPeaks(Mat H, int n);
Mat drawPeakLines(Mat img, std::vector<Peak> peaks);

int lab3()
{
	int stop = 0;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Hough transform for line detection:\n");
		printf("k = 0 => edge simple\n");
		printf("k = 1 => edge complex\n");
		printf("k = 2 => image simple\n");
		printf("k = 3 => image complex\n");
		int k;
		printf("k = ");
		scanf("%d", &k);
		Mat img = imread(HOUGH_IMAGES_PATH[k], CV_LOAD_IMAGE_GRAYSCALE);
		std::vector<Point2f> edgePoints = getEdgePoints(img, (k>2));
		Mat houghAcc = houghAccumulator(img, edgePoints);

		Mat houghImg;
		// normalize the hough accumulator before displaying it
		//normalize(houghAcc, houghImg, 0, 1, NORM_MINMAX, -1);
		double minVal, maxVal;
		minMaxLoc(houghAcc, &minVal, &maxVal);
		houghAcc.convertTo(houghImg, CV_8UC1, 255.f / maxVal);
		imshow("Hough accumulator", houghImg);
		waitKey();
		getchar();

		int n;
		printf("Filter size?\n n = ");
		scanf("%d", &n);
		std::vector<Peak> localPeaks = getLocalPeaks(houghAcc, n);

		// get max peak
		std::sort(localPeaks.begin(), localPeaks.end());

		int N = localPeaks.size();

		printf("Largest local maxima: %d\n", localPeaks[N-1].hval);

		Mat outOrig = drawPeakLines(img, localPeaks);
		imshow("Peak lines (original img)", outOrig);

		Mat outH = drawPeakLines(houghImg, localPeaks);
		imshow("Peak lines (hough)", outH);

		waitKey();
		getchar();

		printf("Again? (Continue = 1, Exit = 0)\n");
		scanf("%d", &stop);
	} while (stop != 0);
	return 0;
}

Mat houghAccumulator(Mat img, std::vector<Point2f> edgePoints) {
	int width = img.cols;
	int height = img.rows;
	int D = sqrt(width * width + height * height); // image diagonal
	Mat H(D + 1, 360, CV_32SC1);
	H.setTo(0); // initialize accumulator
	int dt = 1; // teta accuracy
	int roMax = D;
	
	for (Point2f e : edgePoints) {
		for (int theta = 0; theta < 360; theta += dt) {
			float ro = e.x * cos(theta) + e.y * sin(theta);
			if (ro >= 0 && ro <= roMax) {
				H.at<int>(ro, theta)++;
			}
		}
	}

	return H;
}

std::vector<Point2f> getEdgePoints(Mat img, bool doComputeEdges) {
	/*
	Black and white image, where the edges are coloured white.
	*/
	int width = img.cols;
	int height = img.rows;
	std::vector<Point2f> edgePoints;

	Mat edges = img.clone();

	if (doComputeEdges) {
		// extract edges from the image using Canny edge detector
		Canny(img, edges, 10, 350);
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (edges.at<uchar>(i, j) == 255) {
				// white point belongs to an edge
				edgePoints.push_back(Point2f(i, j));
			}
		}
	}
	return edgePoints;
}


std::vector<Peak> getLocalPeaks(Mat H, int n) {
	int width = H.cols;
	int height = H.rows;
	std::vector<Peak> localPeaks;

	if (n > width || n > height) {
		return localPeaks;
	}

	for (int i = n; i < height-n; i++) {
		for (int j = n; j < width-n; j++) {
			int ro = i;
			int theta = j;
			int hval = H.at<int>(i, j);
			for (int u = -n; u < n; u++) {
				for (int v = -n; v < n; v++) {
					if (H.at<int>(i + u, j + v) > hval) {
						ro = i + u;
						theta = j + v;
						hval = H.at<int>(i + u, j + v);
					}
				}
			}
			localPeaks.push_back({ theta, ro, hval });
		}
	}
	return localPeaks;
}

Mat drawPeakLines(Mat img, std::vector<Peak> peaks) {
	int height = img.rows;
	int width = img.cols;
	Mat out;
	cvtColor(img, out, cv::COLOR_GRAY2RGB);

	for (auto p : peaks) {
		// draw a line
		Line line(p.ro, p.theta);
		line.drawPolar(out);
	}

	return out;
}