#pragma once

#include "stdafx.h"
#include "common.h"

class Line {
public:
	float a;
	float b;
	float c;
public:
	// default constructor
	Line();
	Line(int x1, int x2, float teta_0, float teta_1);
	/*
	* Compute the equation of the line (params a, b, c) which passes through the 2 given points, p1 and p2.
	* ax + by + c = 0
	*/
	Line(Point2f p1, Point2f p2);
	float distPointToLine(Point2f p);
	Line* clone();
	void draw(Mat& img);
};