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
Line* computeLineEq(Point2f p1, Point2f p2) {
	Line* l = new Line();
	l->a = p1.y - p2.y;
	l->b = p2.x - p1.x;
	l->c = p1.x * p2.y - p2.x * p1.y;
	return l;
}

float distPointToLine(Point2f p, Line l) {
	return abs(l.a * p.x + l.b * p.y + l.c) / sqrt(l.a * l.a + l.b * l.b);
}

void drawLine(Mat& img, Line l) {
	int x1 = 0;
	int x2 = img.cols - 1;
	int y1 = (-l.a * x1 - l.c) / l.b;
	int y2 = (-l.a * x2 - l.c) / l.b;

	// draw a green line
	line(img, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 0));
}

Line* cloneLine(Line l) {
	Line* copy = new Line();
	copy->a = l.a;
	copy->b = l.b;
	copy->c = l.c;
	return copy;
}
