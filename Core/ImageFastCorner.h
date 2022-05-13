#ifndef _IMAGEFASTCORNER_H_
#define _IMAGEFASTCORNER_H_

void FastCorner_16_9(float* gray, int width, int height, std::vector<Coordinate_i>& oCornerCoordiantes, float threshold, bool nonmax_suppression, int unSuppressionRadius, float* score);

#endif