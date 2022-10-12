#pragma once

#include "stdafx.h"
#include "Util.h"

std::vector<Point2f> readPoints(const char* path) {
	std::ifstream fin(path, std::iostream::in);
	int N;
	fin >> N;

	// allocate memory for the points
	std::vector<Point2f> points;

	// read the pairs of (x,y) coordinates
	for (int i = 0; i < N; i++) {
		float x, y;
		fin >> x >> y;
		points.push_back(Point2f(x, y));
	}
	fin.close();

	return points;
}

Mat initializeCanvas(int height, int width) {
	// create the image matrix and initialize it with a white background
	Mat img(height, width, CV_8UC3);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			img.at<Vec3b>(i, j) = WHITE;
		}
	}
	return img;
}

void drawPoints(Mat& img, std::vector<Point2f> points) {
	const int height = img.rows;
	const int width = img.cols;

	int N = points.size();
	for (int i = 0; i < N; i++) {
		Point2f p = points[i];
		int x = (int)p.x;
		int y = (int)p.y;
		// check that the points are inside the image boundary
		if (x >= 0 && x < width && y >= 0 && y < height) {
			// draw a small circle for each point
			circle(img, Point(x, y), 1, Scalar(0, 0, 0), 2);
		}
	}
}

std::vector<Point2f> getPointsFromImage(Mat img) {
	std::vector<Point2f> points;
	int height = img.rows;
	int width = img.cols;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (img.at<char>(i, j) == 0) {
				// black point (i, j) corresponds to coord (y, x)
				points.push_back(Point2f(j, i));
			}
		}
	}
	return points;
}


