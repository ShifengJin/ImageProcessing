#ifndef _IMAGESUSANCORNER_H_
#define _IMAGESUSANCORNER_H_

#include <vector>
#include "Common.h"

void SusanCorner(float* gray, int width, int height, std::vector<Vector2i>& oCornerCoordiantes, int maskRadius, int unSuppressionRadius, float threshold/*two pixels diff min value*/, float* tmpR/*Corner Corresponding value*/);

#endif