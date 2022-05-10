#ifndef _IMAGEHARRISCORNER_H_
#define _IMAGEHARRISCORNER_H_

#include <vector>
#include "Common.h"

void HarrisCorner(float* gray, int width, int height, std::vector<Coordinate_i>& oCornerCoordiantes, float k, int ksize, float* GaussFilter, float* Ix2, float* Iy2, float* IxIy, float* Gauss_Ix2, float* Gauss_Iy2, float* Gauss_IxIy, float* MR, int localSize);

#endif