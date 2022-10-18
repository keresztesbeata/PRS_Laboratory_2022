
#include "stdafx.h"
#include "Util.h"

const char DT_IMAGES_PATH[3][PATH_SIZE] = {
	"images_DT_PM\\DT\\contour1.bmp",
	"images_DT_PM\\DT\\contour2.bmp",
	"images_DT_PM\\DT\\contour3.bmp"
};

const char PM_IMAGES_PATH[3][PATH_SIZE] = {
	"images_DT_PM\\PatternMatching\\template.bmp",
	"images_DT_PM\\PatternMatching\\unknown_object1.bmp",
	"images_DT_PM\\PatternMatching\\unknown_object2.bmp"
};

Mat distanceTransform(Mat img);
float matchingScore(Mat templ, Mat img);
float matchingScoreWithAlignImageCenter(Mat templ, Mat img);
Point2f findCenterOfMass(Mat img);

int main()
{
	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf("1 - Chamfer based Distance Transform\n");
		printf("2 - Pattern Matching using Distance Transform\n");
		printf("3 - Pattern Matching using Distance Transform (align cneters)\n");
		printf("0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
		case 1: {
			printf("k = 0 => contour 1\n");
			printf("k = 1 => contour 2\n");
			printf("k = 2 => contour 3\n");
			int k;
			printf("k = ");
			scanf("%d", &k);
			Mat img = imread(DT_IMAGES_PATH[k], IMREAD_GRAYSCALE);
			Mat DT = distanceTransform(img);
			imshow("DT", DT);
			waitKey();
			break;
		}
		case 2: {
			Mat templ = imread(PM_IMAGES_PATH[0], IMREAD_GRAYSCALE);
			Mat obj1 = imread(PM_IMAGES_PATH[1], IMREAD_GRAYSCALE);
			Mat obj2 = imread(PM_IMAGES_PATH[2], IMREAD_GRAYSCALE);
			float msc1 = matchingScore(templ, obj1);
			printf("Matching score between <template, obj1> = %.2f\n", msc1);
			float msc2 = matchingScore(templ, obj2);
			printf("Matching score between <template, obj2> = %.2f\n", msc2);
			printf("--------------------Reverse---------------------\n");
			float msc1R = matchingScore(obj1, templ);
			printf("Matching score between <obj1, template> = %.2f\n", msc1R);
			float msc2R = matchingScore(obj2, templ);
			printf("Matching score between <obj2, template> = %.2f\n", msc2R);
			getchar();
			getchar();
			break;
		}

		case 3: {
			Mat templ = imread(PM_IMAGES_PATH[0], IMREAD_GRAYSCALE);
			Mat obj1 = imread(PM_IMAGES_PATH[1], IMREAD_GRAYSCALE);
			Mat obj2 = imread(PM_IMAGES_PATH[2], IMREAD_GRAYSCALE);
			float msc1 = matchingScoreWithAlignImageCenter(templ, obj1);
			printf("Matching score between <template, obj1> = %.2f\n", msc1);
			float msc2 = matchingScoreWithAlignImageCenter(templ, obj2);
			printf("Matching score between <template, obj2> = %.2f\n", msc2);
			printf("--------------------Reverse---------------------\n");
			float msc1R = matchingScoreWithAlignImageCenter(obj1, templ);
			printf("Matching score between <obj1, template> = %.2f\n", msc1R);
			float msc2R = matchingScoreWithAlignImageCenter(obj2, templ);
			printf("Matching score between <obj2, template> = %.2f\n", msc2R);
			getchar();
			getchar();
			break;
		}

		default: break;
		}
	} while (op);
	return 0;
}

Mat distanceTransform(Mat img) {
	int width = img.cols;
	int height = img.rows;

	Mat DT = img.clone();

	int di[8] = { -1,-1,-1,0,0,1,1,1 };
	int dj[8] = { -1,0,1,-1,1,-1,0,1 };
	int weight[8] = { 0,1,0,1,1,0,1,0 };

	for (int i = 1; i < height-1; i++) {
		for (int j = 1; j < width-1; j++) {
			int min = 255;
			for (int k = 0; k < 8; k++) {
				uchar pixel = img.at<uchar>(i + di[k], j + dj[k]);
				if (pixel + weight[k] < min) {
					min = pixel + weight[k];
				}
			}
			DT.at<uchar>(i, j) = min;
		}
	}

	return DT;
}

float matchingScore(Mat templ, Mat img) {
	Mat DT = distanceTransform(templ);
	int width = img.cols;
	int height = img.rows;

	float matchingScore = 0;
	int nrMatches = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (img.at<uchar>(i, j) == 0) {
				matchingScore += DT.at<uchar>(i, j);
				nrMatches++;
			}
		}
	}
	
	return matchingScore / (float)nrMatches;
}

Point2f findCenterOfMass(Mat img) {
	Mat binary;
	threshold(img, binary, 127, 255, THRESH_BINARY);
	std::vector<Mat> contours;
	findContours(binary, contours, RETR_LIST, CONTOURS_MATCH_I2);

	Moments M = moments(contours[0]);

	// center of contours
	int cx = int(M.m10 / M.m00);
	int cy = int(M.m01 / M.m00);

	return Point2f(cx, cy);
}


float matchingScoreWithAlignImageCenter(Mat templ, Mat obj) {
	// translate the img w.r.t the template's center of mass
	Point2f templCenter = findCenterOfMass(templ);
	Point2f objCenter = findCenterOfMass(obj);

	Point2f srcTri[3];
	srcTri[0] = Point2f(0.f, 0.f);
	srcTri[1] = Point2f(obj.cols - 1.f, 0.f);
	srcTri[2] = Point2f(0.f, obj.rows - 1.f);
	Point2f dstTri[3];
	dstTri[0] = Point2f(0.f, obj.rows * 0.33f);
	dstTri[1] = Point2f(obj.cols * 0.85f, obj.rows * 0.25f);
	dstTri[2] = Point2f(obj.cols * 0.15f, obj.rows * 0.7f);
	Mat warp_mat = getAffineTransform(srcTri, dstTri);

	Mat warp_dst = Mat::zeros(obj.rows, obj.cols, obj.type());
	warpAffine(obj, warp_dst, warp_mat, warp_dst.size());

	return matchingScore(templ, obj);
}