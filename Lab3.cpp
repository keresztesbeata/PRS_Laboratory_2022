
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
	bool operator > (const peak& o) const {
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
Mat drawFirstKPeakLines(Mat img, std::vector<Peak> peaks, int k);

int main3()
{
	int stop = 0;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Hough transform for line detection:\n");
		printf("k = 1 => edge simple\n");
		printf("k = 2 => edge complex\n");
		printf("k = 3 => image simple\n");
		printf("k = 4 => image complex\n");
		int k;
		printf("k = ");
		scanf("%d", &k);
		Mat img = imread(HOUGH_IMAGES_PATH[k-1], CV_LOAD_IMAGE_GRAYSCALE);
		std::vector<Point2f> edgePoints = getEdgePoints(img, (k>2));
		Mat houghAcc = houghAccumulator(img, edgePoints);

		Mat houghImg;
		// normalize the hough accumulator before displaying it
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
		// sort the peaks in desc order
		std::sort(localPeaks.begin(), localPeaks.end(), std::greater<Peak>());

		int N = localPeaks.size();

		int K;
		printf("K = \n");
		scanf("%d", &K);


		// get K max peaks
		printf("The K largest local maxima : \n");
		for (int i = 0; i < K; i++) {
			printf(" %d, ", localPeaks[i].hval);
		}
		printf("\n");

		Mat outOrig = drawFirstKPeakLines(img, localPeaks, K);
		imshow("Peak lines (original img)", outOrig);

		Mat outHough = drawFirstKPeakLines(houghImg, localPeaks, K);
		imshow("Peak lines (hough)", outHough);

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
	int dt = 1; // theta accuracy
	int roMax = D;
	
	for (Point2f e : edgePoints) {
		for (int theta = 0; theta < 360; theta += dt) {
			float thetaRad = theta * PI / 180.0;
			float ro = e.x * cos(thetaRad) + e.y * sin(thetaRad);
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
				// reverse coordinates (x, y) <=> (j,i) when rendering
				edgePoints.push_back(Point2f(j, i));
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

	int w = n / 2;

	// consider also the pixels on the image borders when computing the local maximums in order to not miss the line on the main diagonal with ro = 0
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int hval = H.at<int>(i, j);
			bool ok = true;
			for (int u = -w; u <= w; u++) {
				for (int v = -w; v <= w; v++) {
					if (i+u >= 0 && i+u < height && j + v >= 0 && j + v < width && H.at<int>(i + u, j + v) > hval) {
						ok = false;
					}
				}
			}
			if (ok && hval > 0) {
				int ro = i;
				int theta = j;
				localPeaks.push_back({ theta, ro, hval });
			}
		}
	}
	return localPeaks;
}

Mat drawFirstKPeakLines(Mat img, std::vector<Peak> peaks, int k) {
	int height = img.rows;
	int width = img.cols;
	Mat out;
	cvtColor(img, out, cv::COLOR_GRAY2RGB);
	int N = peaks.size();
	
	for (int i = 0; i < k && i < N; i++) {
		// draw a line
		Peak p = peaks[i];
		Line line(p.ro, p.theta * PI / 180);
		line.drawPolar(out);
	}

	return out;
}