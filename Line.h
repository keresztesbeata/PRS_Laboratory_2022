#pragma once

#include "stdafx.h"
#include "common.h"

class Line {
public:
	float a = 0;
	float b = 0;
	float c = 0;
	// polar coord
	float ro = 0;
	float theta = 0;
public:
	// default constructor
	Line();
	Line(int x1, int x2, float teta_0, float teta_1);
	Line(float ro, float theta);
	/*
	* Compute the equation of the line (params a, b, c) which passes through the 2 given points, p1 and p2.
	* ax + by + c = 0
	*/
	Line(Point2f p1, Point2f p2);
	float distPointToLine(Point2f p);
	Line* clone();
	void draw(Mat& img);
	void drawPolar(Mat& img);
};