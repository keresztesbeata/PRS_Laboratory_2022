#pragma once

#include "stdafx.h"
#include "Line.h"

Line::Line() {
	this->a = 0;
	this->b = 0;
	this->c = 0;
}

Line::Line(Point2f p1, Point2f p2) {
	this->a = p1.y - p2.y;
	this->b = p2.x - p1.x;
	this->c = p1.x * p2.y - p2.x * p1.y;
}

float Line::distPointToLine(Point2f p) {
	return abs(this->a * p.x + this->b * p.y + this->c) / sqrt(this->a * this->a + this->b * this->b);
}

void Line::draw(Mat& img) {
	int x1 = 0;
	int x2 = img.cols - 1;
	int y1 = (-this->a * x1 - this->c) / this->b;
	int y2 = (-this->a * x2 - this->c) / this->b;

	// draw a green line
	line(img, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 0));
}

Line* Line::clone() {
	Line* copy = new Line();
	copy->a = this->a;
	copy->b = this->b;
	copy->c = this->c;
	return copy;
}