#pragma once

#include "stdafx.h"
#include "Algorithms.h"

void leastMeanSquares(std::vector<Point2f> points, float& teta_0, float& teta_1) {
	teta_0 = 0;
	teta_1 = 0;

	float sxy = 0, sx = 0, sy = 0, sx2 = 0;
	int N = points.size();

	for (int i = 0; i < N; i++) {
		sxy += points[i].x * points[i].y;
		sx += points[i].x;
		sx2 += points[i].x * points[i].x;
		sy += points[i].y;
	}

	teta_1 = (N * sxy - sx * sy) / (N * sx2 - sx * sx);
	teta_0 = (sy - teta_1 * sx) / N;
}