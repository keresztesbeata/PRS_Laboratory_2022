#pragma once

#include "stdafx.h"
#include "common.h"

// UTIL
#define PATH_SIZE 100
#define WHITE Vec3b(255, 255, 255)
#define BLACK Vec3b(0, 0, 0)
#define RED Vec3b(0, 0, 255)
#define GREEN Vec3b(0, 255, 0)

// line utils
typedef struct _line {
	float a;
	float b;
	float c;
}Line;

/*
* Compute the equation of the line (params a, b, c) which passes through the 2 given points, p1 and p2.
* ax + by + c = 0
*/
Line* computeLineEq(Point2f p1, Point2f p2);
float distPointToLine(Point2f p, Line l);
Line* cloneLine(Line l);

// reading utils
std::vector<Point2f> readPoints(const char* path);
std::vector<Point2f> getPointsFromImage(Mat img);

// drawing utils
Mat initializeCanvas(int height, int width);
void drawPoints(Mat& img, std::vector<Point2f> points);
void drawLine(Mat& img, Line l);