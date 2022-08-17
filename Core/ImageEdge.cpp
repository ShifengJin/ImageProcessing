#include <string.h>
#include <math.h>
#include "ImageTransformation.h"
#include "ImageEdge.h"

static void ImageL1(float* edgeX, float* edgeY, int width, int height, float* out){
    memset(out, 0, width * height * sizeof(float));
    int size = width * height;
    for(int i = 0; i < size; ++ i){
        out[i] = fabs(edgeX[i]) + fabs(edgeY[i]);
    }
}

static void ImageL2(float* edgeX, float* edgeY, int width, int height, float* out){
    memset(out, 0, width * height * sizeof(float));
    int size = width * height;
    for(int i = 0; i < size; ++ i){
        float x = edgeX[i];
        float y = edgeY[i];
        out[i] = sqrtf(x * x + y * y);
    }
}

static void ImageLInfinite(float* edgeX, float* edgeY, int width, int height, float* out){
    memset(out, 0, width * height * sizeof(float));
    int size = width * height;
    for(int i = 0; i < size; ++ i){
        float x = fabs(edgeX[i]);
        float y = fabs(edgeY[i]);
        out[i] = x > y ? x : y;
    }
}

void Sobel::Run(float* inGray, float* out, MAGTYPE magType, float* oEdgeX, float* oEdgeY){
    float* tmpEdgeX = m_edgeX;
    float* tmpEdgeY = m_edgeY;
    if(oEdgeX){
        tmpEdgeX = oEdgeX;
    }
    if(oEdgeY){
        tmpEdgeY = oEdgeY;
    }
    memset(tmpEdgeX, 0, m_width * m_height * sizeof(float));
    memset(tmpEdgeY, 0, m_width * m_height * sizeof(float));
    
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++ x){
            tmpEdgeX[y * m_width + x] = (inGray[(y - 1) * m_width + (x + 1)] + 2.f * inGray[y * m_width + (x + 1)] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + 2.f * inGray[y * m_width + (x - 1)] + inGray[(y + 1) * m_width + (x - 1)]);
            
            tmpEdgeY[y * m_width + y] = (inGray[(y + 1) * m_width + (x - 1)] + 2.f * inGray[(y + 1) * m_width + x] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + 2.f * inGray[(y - 1) * m_width + x] + inGray[(y - 1) * m_width + (x + 1)]);
        }
    }
    switch (magType){
        case MAG_L1:
            ImageL1(tmpEdgeX, tmpEdgeY, m_width, m_height, out);
            break;
        case MAG_L2:
            ImageL2(tmpEdgeX, tmpEdgeY, m_width, m_height, out);
            break;
        case MAG_LInfinite:
            ImageLInfinite(tmpEdgeX, tmpEdgeY, m_width, m_height, out);
            break;
    }

}

void Roberts::Run(float* inGray, float* out, MAGTYPE magType){
    memset(m_edgeX, 0, m_width * m_height * sizeof(float));
    memset(m_edgeY, 0, m_width * m_height * sizeof(float));

    for(int y = 0; y < m_height - 1; ++ y){
        for(int x = 0; x < m_width - 1; ++ x){
            m_edgeX[y * m_width + x] = inGray[y * m_width + x] - inGray[(y + 1) * m_width + x + 1];
            m_edgeY[y * m_width + x] = inGray[y * m_width + x + 1] - inGray[(y + 1) * m_width + x];
        }
    }
    switch (magType){
        case MAG_L1:
            ImageL1(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_L2:
            ImageL2(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_LInfinite:
            ImageLInfinite(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
    }
}

void Prewitt::Run(float* inGray, float* out, MAGTYPE magType){
    memset(m_edgeX, 0, m_width * m_height * sizeof(float));
    memset(m_edgeY, 0, m_width * m_height * sizeof(float));

    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++ x){
            m_edgeX[y * m_width + x] = (inGray[(y - 1) * m_width + (x + 1)] + inGray[y * m_width + (x + 1)] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + inGray[y * m_width + (x - 1)] + inGray[(y + 1) * m_width + (x - 1)]);
            
            m_edgeY[y * m_width + y] = (inGray[(y + 1) * m_width + (x - 1)] + inGray[(y + 1) * m_width + x] + inGray[(y + 1) * m_width + (x + 1)]) - 
                                       (inGray[(y - 1) * m_width + (x - 1)] + inGray[(y - 1) * m_width + x] + inGray[(y - 1) * m_width + (x + 1)]);
        }
    }
    switch (magType){
        case MAG_L1:
            ImageL1(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_L2:
            ImageL2(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
        case MAG_LInfinite:
            ImageLInfinite(m_edgeX, m_edgeY, m_width, m_height, out);
            break;
    }
}

void Laplacian::Run(float* inGray, float* out, Laplacian::Type type){

    switch (type){
        case Laplacian::Type::X_0_Y_0:
            X_0_Y_0(inGray, out);
            break;
        case Laplacian::Type::X_45_Y_45:
            X_45_Y_45(inGray, out);
            break;
        case Laplacian::Type::X_0_45_Y_0_45:
            X_0_45_Y_0_45(inGray, out);
            break;
    }

}

void Laplacian::X_0_Y_0(float* inGray, float* out){
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++x){
            out[y * m_width + x] = fabs( inGray[(y - 1) * m_width + x] + 
                                         inGray[y * m_width + x - 1] + 
                                         inGray[y * m_width + x + 1] + 
                                         inGray[(y + 1) * m_width + x] - inGray[y * m_width + x] * 4.f);
        }
    }
}

void Laplacian::X_45_Y_45(float* inGray, float* out){
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++x){
            out[y * m_width + x] = fabs( inGray[(y - 1) * m_width + x - 1] + 
                                         inGray[(y - 1) * m_width + x + 1] + 
                                         inGray[(y + 1) * m_width + x - 1] +
                                         inGray[(y + 1) * m_width + x + 1] - inGray[y * m_width + x] * 4.f);
        }
    }
}

void Laplacian::X_0_45_Y_0_45(float* inGray, float* out){
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++x){
            out[y * m_width + x] = fabs( inGray[(y - 1) * m_width + x - 1] + 
                                         inGray[(y - 1) * m_width + x] + 
                                         inGray[(y - 1) * m_width + x + 1] + 
                                         inGray[y * m_width + x - 1] + 
                                         inGray[y * m_width + x + 1] +
                                         inGray[(y + 1) * m_width + x - 1] +
                                         inGray[(y + 1) * m_width + x] +
                                         inGray[(y + 1) * m_width + x + 1] - inGray[y * m_width + x] * 8.f);
        }
    }
}

void Canny::Run(float* in, float* out, float TH , float TL, int gaussR, float sigma, MAGTYPE magType){
    memset(out, 0, m_width * m_height * sizeof(float));
    
    int ksize = gaussR * 2 + 1;
    float* gaussKernel = (float*)calloc(ksize * ksize, sizeof(float));
    Utily::GaussFilter(ksize, sigma, gaussKernel);
    Utily::ImageConvolution(in, m_width, m_height, 1, m_gaussImage, gaussKernel, ksize);

    m_pSobel->Run(m_gaussImage, m_edgeMag, magType, m_edgeX, m_edgeY);

    noMax(m_edgeMag, m_edgeX, m_edgeY, m_edgeMag_noMaxsup);

    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++ x){
            float mag = m_edgeMag_noMaxsup[y * m_width + x];

            if(mag > TH){
                trace(m_edgeMag_noMaxsup, out, x, y, TL);
            }else if(mag < TL){
                out[y * m_width + x] = 0.f;
            }
        }
    }

    FREE_Memory(gaussKernel);
}

void Canny::noMax(float* edgeMag, float* edgeX, float* edgeY, float* edgeMag_noMaxsup){

    memset(edgeMag_noMaxsup, 0, m_width * m_height * sizeof(float));

    //    135度    90度      45度
	//      *       *       *
	//        *     *     *
	//          *   *   *
	//            * * *
	// 180度* * * * * * * * *    0度
	//            * * *    
	//          *   *   *
	//        *     *     *
	//      *       *       *
	//   -135度    -90度     -45度
	// 0,180 水平方向的梯度(-22.5 22.5], (157.5 180][-180, -157.5]
	// 45,-135   (22.5, 67.5],   (-157.5, -112.5]
	// 90,-90    (67.5, 112.5],  (-112.5, -67.5]
	// 135,-45   (112.5, 157.5], (-67.5, -22.5]
    for(int y = 1; y < m_height - 1; ++ y){
        for(int x = 1; x < m_width - 1; ++ x){

            float _x = edgeX[y * m_width + x];
            float _y = edgeY[y * m_width + x];

            float angle = atan2f(_y, _x) * ANGLE2RAD;
            float mag = edgeMag[y * m_width + x];

            // 0,180  
			if ((angle > -22.5f && angle <= 22.5) ||
				(angle > 157.5 && angle <= 180.f) ||
				(angle >= -180.f && angle <= -157.5f)) {

				// 非极大值抑制
				float left = edgeMag[y * m_width + x - 1];
				float right = edgeMag[y * m_width + x + 1];
				if (mag >= left && mag >= right) {
					edgeMag_noMaxsup[y * m_width + x] = mag;
				}
			}

			// 45,-135
			if ((angle > 22.5f && angle <= 67.5f) || (angle > -157.5f && angle <= -122.5f)) {
				float left_up = edgeMag[(y - 1) * m_width + x - 1];
				float right_down = edgeMag[(y + 1) * m_width + x + 1];
				if (mag >= left_up && mag >= right_down) {
					edgeMag_noMaxsup[y * m_width + x] = mag;
				}
			}

			// 90,-90
			if ((angle > 67.5f && angle <= 112.5f) || (angle > -112.5f && angle <= -67.5f)) {
				float up = edgeMag[(y - 1) * m_width + x];
				float down = edgeMag[(y + 1) * m_width + x];
				if (mag >= up && mag >= down) {
					edgeMag_noMaxsup[y * m_width + x] = mag;
				}
			}

			//135,-45   (112.5, 157.5], (-67.5, -22.5]
			if ((angle > 112.5f && angle <= 157.5f) || (angle > -67.5f && angle <= -22.5f)) {
				float right_up = edgeMag[(y - 1) * m_width + x + 1];
				float left_down = edgeMag[(y + 1) * m_width + x - 1];
				if (mag >= right_up && mag >= left_down) {
					edgeMag_noMaxsup[y * m_width + x] = mag;
				}
			}
        }
    }
    
}

void Canny::trace(float* edgeMag_noMaxsup, float* out, int x, int y, float TL){
    if(out[y * m_width + x] == 0.f){
        out[y * m_width + x] = 255.f;
        for(int i = -1; i <= 1; ++ i){
            for(int j = -1; j <= 1; ++ j){
                if(!checkInRange(x + j, y + i, m_width, m_height)){
                    continue;
                }
                float mag = edgeMag_noMaxsup[(y + i) * m_width + x + j];
                if(mag >= TL){
                    trace(edgeMag_noMaxsup, out, x + j, y + i, TL);
                }
            }
        }
    }
}