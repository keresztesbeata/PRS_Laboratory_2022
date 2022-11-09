#pragma once

#include "stdafx.h"
#include "Algorithms.h"
#include <cmath>
#include <fstream>

// UTIL
#define PATH_SIZE 100
#define WHITE Vec3b(255, 255, 255)
#define BLACK Vec3b(0, 0, 0)
#define RED Vec3b(0, 0, 255)
#define GREEN Vec3b(0, 255, 0)

// reading utils
std::vector<Point2f> readPoints(const char* path);
std::vector<Point2f> getPointsFromImage(Mat img);

// drawing utils
Mat initializeCanvas(int height, int width);
void drawPoints(Mat& img, std::vector<Point2f> points);