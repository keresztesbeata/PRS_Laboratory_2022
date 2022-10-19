#pragma once

#include "stdafx.h"
#include "Line.h"

Line::Line(float ro, float theta) {
	// create a line using polar coord
	this->ro = ro;
	this->theta = theta;
}

Line::Line(int x1, int x2, float teta_0, float teta_1) {
	int y1 = teta_0 + x1 * teta_1;
	int y2 = teta_0 + x2 * teta_1;

	Point2f p1 = Point(x1, y1);
	Point2f p2 = Point(x2, y2);
	this->a = p1.y - p2.y;
	this->b = p2.x - p1.x;
	this->c = p1.x * p2.y - p2.x * p1.y;
}

Line::Line(Point2f p1, Point2f p2) {
	this->a = p1.y - p2.y;
	this->b = p2.x - p1.x;
	this->c = p1.x * p2.y - p2.x * p1.y;
}

Line::Line() {
	this->a = 0;
	this->b = 0;
	this->c = 0;
	this->ro = 0;
	this->theta = 0;
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

void Line::drawPolar(Mat& img) {
	double a = cos(this->theta), b = sin(this->theta);
	double x0 = a * this->ro, y0 = b * this->ro;
	Point2f p1, p2;
	p1.x = cvRound(x0 + 1000 * (-b));
	p1.y = cvRound(y0 + 1000 * (a));
	p2.x = cvRound(x0 - 1000 * (-b));
	p2.y = cvRound(y0 - 1000 * (a));
	line(img, p1, p2, Scalar(0, 0, 255));
}

Line* Line::clone() {
	Line* copy = new Line();
	copy->a = this->a;
	copy->b = this->b;
	copy->c = this->c;
	copy->ro = this->ro;
	copy->theta = this->theta;
	return copy;
}