#ifndef _IMAGEFASTCORNER_H_
#define _IMAGEFASTCORNER_H_

#include <vector>
#include "Common.h"

void FastCorner_16_9(float* gray, int width, int height, std::vector<Vector2i>& oCornerCoordiantes, float threshold, bool nonmax_suppression, int unSuppressionRadius, float* score);

#endif