#include <math.h>
#include <algorithm>
#include "ImageTransformation.h"
#include "ImageInterpolation.h"
#include "SIFT.h"

SIFT::SIFT(unsigned int width, unsigned int height, unsigned int maxNum, unsigned int Octave, unsigned int Level, float sigma)
: m_width(width), m_height(height), m_maxNum(maxNum), m_octave(Octave), m_level(Level), m_sigma(sigma){

    std::cout << "SIFT Initting" << std::endl;
    
    if(m_octave == -1){
        m_octave = (unsigned int)(log2f((m_width > m_height ? m_height : m_width)) - 3);
    }
    std::cout << "Octave: " << m_octave << std::endl;
    // Gauss sigma
    float sigma0 = sqrtf(m_sigma * sigma - 4.f * SIFT_INIT_SIGMA * SIFT_INIT_SIGMA);
    int kernelWidth0 = 2 * roundf(SIFT_GAUSS_KERNEL_RATION * sigma0) + 1;
    GaussFilter* gaussFilter0 = new GaussFilter(kernelWidth0, sigma0);
    m_Level_GaussFilters.push_back(gaussFilter0);
    float k = powf(2.f, 1.f / m_level);
    for(unsigned int i = 1; i < m_level + 3; ++ i){
        float prev_sigma = powf(k, i - 1) * m_sigma;
        float curr_sigma = k * prev_sigma;
        int kernelWidthi = 2 * roundf(SIFT_GAUSS_KERNEL_RATION * curr_sigma) + 1;
        GaussFilter* gaussFilteri = new GaussFilter(kernelWidthi, curr_sigma);
        m_Level_GaussFilters.push_back(gaussFilteri);
    }

    for(unsigned int i = 0; i < m_Level_GaussFilters.size(); ++i){
        std::cout << "GaussFilter: " << m_Level_GaussFilters[i]->kernel_width << ", " << m_Level_GaussFilters[i]->sigma << std::endl;
    }


    // compute octave img size
    ImgSize imgSize;
    imgSize.x = m_width;
    imgSize.y = m_height;

    std::cout << "m_O_Size:" << std::endl;
    m_O_Size.push_back(imgSize);
    std::cout << "m_octave: " << m_octave << std::endl;
    for(unsigned int o = 1; o < m_octave; ++o){
        ImgSize imgSize;
        imgSize.x = (int)(m_O_Size[o - 1].x * 0.5f + 0.5f);
        imgSize.y = (int)(m_O_Size[o - 1].y * 0.5f + 0.5f);
        m_O_Size.push_back(imgSize);
    }

    for(unsigned int o = 0; o < m_octave; ++o){
        std::cout << m_O_Size[o].x << ", " << m_O_Size[o].y << std::endl;
    }

    // calloc memory
    m_srcImg = (float*)calloc(m_width * m_height, sizeof(float));

    for(unsigned int o = 0; o < m_octave; ++ o){
        std::vector<float*> s_gaussImgs;
        for(unsigned int s = 0; s < m_level + 3; ++ s){
            float* gaussImg = (float*)calloc(m_O_Size[o].x * m_O_Size[o].y, sizeof(float));
            s_gaussImgs.push_back(gaussImg);
            std::cout << o << "  " << s << std::endl;
        }
        m_O_L_GaussImgs.push_back(s_gaussImgs);
    }

    for(unsigned int o = 0; o < m_octave; ++ o){
        std::vector<float*> s_dogImgs;
        for(unsigned int s = 0; s < m_level + 2; ++ s){
            float* dogImg = (float*)calloc(m_O_Size[o].x * m_O_Size[o].y, sizeof(float));
            s_dogImgs.push_back(dogImg);
        }
        m_O_L_DOGImgs.push_back(s_dogImgs);
    }
    
    std::cout << "SIFT Initted" << std::endl;
}

bool featureResponseCmp(SIFT::Feature& in0, SIFT::Feature& in1){
    return in0.m_response < in1.m_response;
}

void SIFT::Run(float* gray){

    std::cout << "SIFT Running" << std::endl;

    // clear pre result
    // for(int o = 0; o < m_octave; ++ o){
    //     m_O_Coordiantes[o].clear();
    // }
    
    std::cout << "Creatting Gauss Image pyramid." << std::endl;
    // build gauss image pyramid
    GaussPyramidImgs(gray);

    // build DOG pyramid
    DOGPyramidImgs();

    // m_O_Coordiantes.clear();
    // check extreme point
    FindExtremePoint();
    
    // choose Feature
    std::sort(m_Coordiantes.begin(), m_Coordiantes.end(), featureResponseCmp);
    if(m_Coordiantes.size() > m_maxNum){
        m_Coordiantes.resize(m_maxNum);
    }


    // ori and desc

}
    
void SIFT::GaussPyramidImgs(float* gray){

    for(unsigned int o = 0; o < m_octave; ++ o){

        for(unsigned int s = 0; s < m_level + 3; ++ s){
            std::cout << "GaussImage: " << o << "   " << s << std::endl;
            if(o == 0 && s == 0){
                
                Utily::ImageConvolution2(gray, m_O_Size[o].x, m_O_Size[o].y, m_O_L_GaussImgs[o][s], m_Level_GaussFilters[s]->kernel, m_Level_GaussFilters[s]->kernel_width);
            }else if(o > 0 && s == 0){
                ImageInterpolation_Nearest(m_O_L_GaussImgs[o - 1][m_level], m_O_Size[o - 1].x, m_O_Size[o - 1].y, 1, m_O_L_GaussImgs[o][s], m_O_Size[o].x, m_O_Size[o].y);
            }else{

                Utily::ImageConvolution2(m_O_L_GaussImgs[o][s - 1], m_O_Size[o].x, m_O_Size[o].y, m_O_L_GaussImgs[o][s], m_Level_GaussFilters[s]->kernel, m_Level_GaussFilters[s]->kernel_width);
            }
        }
    }

#if 0
    {
        for(int o = 0; o < m_octave; ++ o){
            for(int s = 0; s < m_level + 3; ++ s){
                std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.jpg";
                Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_GaussImgs[o][s]);
                // std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.txt";
                // std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_gaussImg.txt";
                // Utily::SaveBuffer(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_GaussImgs[o][s]);
            }
        }
    }
#endif

}

void SIFT::DOGPyramidImgs(){
    for(unsigned int o = 0; o < m_octave; ++ o){
        for(unsigned int s = 0; s < m_level + 2; ++ s){
            Utily::TwoImageDiff(m_O_L_GaussImgs[o][s + 1], m_O_L_GaussImgs[o][s], m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_DOGImgs[o][s]);
        }
    }

#if 0
    {
        for(int o = 0; o < m_octave; ++ o){
            for(int s = 0; s < m_level + 2; ++ s){
                std::string savePath = std::to_string(o) + "_" + std::to_string(s) + "_DOGImg.jpg";
                Utily::SaveImage<float>(savePath, m_O_Size[o].x, m_O_Size[o].y, 1, m_O_L_DOGImgs[o][s]);
            }
        }
    }
#endif
}

void SIFT::FindExtremePoint(){

    float scaleOffset[10] = { 0.f, 0.5f, 1.5f, 3.5f, 7.5f, 15.5f, 31.5f, 63.5f, 127.5f, 255.5f };
    float octaveScale[10] = { 1.f, 2.f, 4.f, 8.f, 16.f, 32.f, 64.f, 128.f, 256.f, 512.f };
    float SubZThreshold = m_octave * m_level + 1.f;

#define EXTREMEPOINTSTATISTICS 0

#if EXTREMEPOINTSTATISTICS
    int findNum = 0;
    int extremeNum = 0;

    int detHOutNum = 0;
    int EDGOutNum = 0;
    int detMOutNum = 0;
    int xOffsetOutNum = 0;
    int yOffsetOutNum = 0;
    int sigmaOffsetOutNum = 0;
    int responseOutNum = 0;
    int edgeOutNum = 0;
#endif
    m_Coordiantes.clear();

    for(unsigned int o = 0; o < m_octave; ++ o){
        
        float coordinateScale = octaveScale[o];
        float coordinateOffset = scaleOffset[o];
        
        for(unsigned int s = 1; s < m_level + 1; ++ s){

            float pyramidLevel = o * m_level + s;

            float* downImg = m_O_L_DOGImgs[o][s - 1];
            float* curImg  = m_O_L_DOGImgs[o][s];
            float* upImg   = m_O_L_DOGImgs[o][s + 1];

            unsigned int height = m_O_Size[o].y;
            unsigned int width  = m_O_Size[o].x;

            std::cout << "Oct: " << o << "    level: " << s << "    width: " << width << "    height: " << height << std::endl;
            for(unsigned int i = SIFT_IMG_BORDER; i < height - SIFT_IMG_BORDER; ++ i){
                int preRow = (i - 1) * width;
                int curRow = i * width;
                int nxtRow = (i + 1) * width;
                for(unsigned int j = SIFT_IMG_BORDER; j < width - SIFT_IMG_BORDER; ++ j){

                    float B11 = downImg[preRow + j - 1];
                    float B12 = downImg[preRow + j];
                    float B13 = downImg[preRow + j + 1];

                    float B21 = downImg[curRow + j - 1];
                    float B22 = downImg[curRow + j];
                    float B23 = downImg[curRow + j + 1];

                    float B31 = downImg[nxtRow + j - 1];
                    float B32 = downImg[nxtRow + j];
                    float B33 = downImg[nxtRow + j + 1];
                    

                    float A11 = curImg[preRow + j - 1];
                    float A12 = curImg[preRow + j];
                    float A13 = curImg[preRow + j + 1];

                    float A21 = curImg[curRow + j - 1];
                    float A22 = curImg[curRow + j];
                    float A23 = curImg[curRow + j + 1];

                    float A31 = curImg[nxtRow + j - 1];
                    float A32 = curImg[nxtRow + j];
                    float A33 = curImg[nxtRow + j + 1];


                    float C11 = upImg[preRow + j - 1];
                    float C12 = upImg[preRow + j];
                    float C13 = upImg[preRow + j + 1];

                    float C21 = upImg[curRow + j - 1];
                    float C22 = upImg[curRow + j];
                    float C23 = upImg[curRow + j + 1];

                    float C31 = upImg[nxtRow + j - 1];
                    float C32 = upImg[nxtRow + j];
                    float C33 = upImg[nxtRow + j + 1];

#if EXTREMEPOINTSTATISTICS
                    findNum ++;
#endif

                    int extremePointType = 0;
                    // find extreme point
                    if( A22 > B11 && A22 > B12 && A22 > B13 && A22 > B21 && A22 > B22 && A22 > B23 && A22 > B31 && A22 > B32 && A22 > B33 && 
                        A22 > A11 && A22 > A12 && A22 > A13 && A22 > A21 &&              A22 > A23 && A22 > A31 && A22 > A32 && A22 > A33 && 
                        A22 > C11 && A22 > C12 && A22 > C13 && A22 > C21 && A22 > C22 && A22 > C23 && A22 > C31 && A22 > C32 && A22 > C33)
                    {
                        extremePointType = 1;
                    }
                    else if (A22 < B11 && A22 < B12 && A22 < B13 && A22 < B21 && A22 < B22 && A22 < B23 && A22 < B31 && A22 < B32 && A22 < B33 && 
                             A22 < A11 && A22 < A12 && A22 < A13 && A22 < A21 &&              A22 < A23 && A22 < A31 && A22 < A32 && A22 < A33 && 
                             A22 < C11 && A22 < C12 && A22 < C13 && A22 < C21 && A22 < C22 && A22 < C23 && A22 < C31 && A22 < C32 && A22 < C33)
                    {
                        extremePointType = 2;
                    }else
                    {
                        continue;
                    }
#if EXTREMEPOINTSTATISTICS
                    extremeNum ++;
#endif

                    float Dxx = A21 + A23 - 2.f * A22;
                    float Dyy = A12 + A32 - 2.f * A22;
                    float Dxy = (A33 + A11 - (A13 + A31)) * 0.25f;

                    float detH = Dxx * Dyy - Dxy * Dxy;
                    if(fabsf(detH) < FLT_EPSILON){
#if EXTREMEPOINTSTATISTICS
                        detHOutNum ++;
#endif
                        continue;
                    }

                    float TrH = Dxx + Dyy;
                    float r_1_2__r_2 = (TrH * TrH) / detH;
                    if(fabsf(r_1_2__r_2) > SIFT_EDGERESPONSE){
#if EXTREMEPOINTSTATISTICS
                        EDGOutNum ++;
#endif
                        continue;
                    }

                    //     | Dxx        Dxy        DSigmax     |
                    // M = | Dyx        Dyy        DSigmay     |
                    //     | DSigmax    DSigmay    DsigmaSigma |
                    float DSigmaSigma = C22 + B22 - 2.f * A22;
                    float DSigmaX     = (B21 + C23 - (B23 + C21)) * 0.25f;
                    float DSigmaY     = (B12 + C32 - (B32 + C12)) * 0.25f;
                    float detM = Dxx * Dyy * DSigmaSigma + Dxy * DSigmaY * DSigmaX + DSigmaX * Dxy * DSigmaY - (Dxx * DSigmaY * DSigmaY + Dxy * Dxy * DSigmaSigma + DSigmaX * Dyy * DSigmaX);
                    if(fabsf(detM) < FLT_EPSILON){
#if EXTREMEPOINTSTATISTICS
                        detMOutNum ++;
#endif
                        continue;
                    }

                    detM = 1.f / detM;
                    float Dy = (A32 - A12) * 0.5f;
                    float Dx = (A23 - A21) * 0.5f;
                    float DSigma = (C22 - B22) * 0.5f;

                    float MInv_11 = Dyy * DSigmaSigma - DSigmaY * DSigmaY;
                    float MInv_12 = DSigmaX * DSigmaY - Dxy * DSigmaSigma;
                    float MInv_13 = Dxy * DSigmaY - Dyy * DSigmaX;

                    float xOffset = (MInv_11 * (-Dx) + MInv_12 * (-Dy) + MInv_13 * (-DSigma)) * detM;
                    float xyOffsetTh = 4.f / (o + 1);
                    if(fabsf(xOffset) > xyOffsetTh){
#if EXTREMEPOINTSTATISTICS
                        xOffsetOutNum ++;
#endif
                        continue;
                    }


                    float MInv_21 = DSigmaX * DSigmaY - Dxy * DSigmaSigma;
                    float MInv_22 = Dxx * DSigmaSigma - DSigmaX * DSigmaX;
                    float MInv_23 = Dxy * DSigmaX - Dxx * DSigmaY;
                    float yOffset = (MInv_21 * (-Dx) + MInv_22 * (-Dy) + MInv_23 * (-DSigma)) * detM;
                    if(fabsf(yOffset) > xyOffsetTh){
#if EXTREMEPOINTSTATISTICS
                        yOffsetOutNum ++;
#endif
                        continue;
                    }

                    float MInv_31 = Dxy * DSigmaY - Dyy * DSigmaX;
                    float MInv_32 = Dxy * DSigmaX - Dxx * DSigmaY;
                    float MInv_33 = Dxx * Dyy - Dxy * Dxy;
                    float sigmaOffset = (MInv_31 * (-Dx) + MInv_32 * (-Dy) + MInv_33 * (-DSigma)) * detM;
                    if(fabsf(sigmaOffset) > 1.f){
#if EXTREMEPOINTSTATISTICS
                        sigmaOffsetOutNum ++;
#endif
                        continue;
                    }

                    float response = fabsf(A22 + Dx * xOffset + Dy * yOffset + DSigma * sigmaOffset) * 0.00390625f;
                    if(response < SIFT_RESPONSE_TH)
                    {
#if EXTREMEPOINTSTATISTICS
                        responseOutNum++;
#endif
                        continue;
                    }

                    float subCoordX = (xOffset + j) * coordinateScale + coordinateOffset;
                    float subCoordY = (yOffset + i) * coordinateScale + coordinateOffset;
                    float subLevel = sigmaOffset + pyramidLevel;

                    if(subCoordX < SIFT_IMG_BORDER || subCoordX >= m_width  - SIFT_IMG_BORDER ||
                       subCoordY < SIFT_IMG_BORDER || subCoordY >= m_height - SIFT_IMG_BORDER ||
                       subLevel < 0.f || subLevel >= SubZThreshold){
#if EXTREMEPOINTSTATISTICS
                        edgeOutNum ++;
#endif
                        continue;
                    }

                    Feature feature;
                    feature.m_octave = o;
                    feature.m_level = s;
                    feature.m_subLevel = s + sigmaOffset;
                    feature.m_o_s_coordiante.x = j;
                    feature.m_o_s_coordiante.y = i;
                    feature.m_pyramidLevel = subLevel;
                    feature.m_pyramidScale = powf(2.f, subLevel / m_level);
                    feature.m_octScale = powf(2.f, feature.m_subLevel / m_level);

                    feature.m_coordiante.x = j * coordinateScale + coordinateOffset;
                    feature.m_coordiante.y = i * coordinateScale + coordinateOffset;

                    feature.m_response = response;
                    feature.m_extremePointType = extremePointType;
                    feature.m_subCoordiante.x = subCoordX;
                    feature.m_subCoordiante.y = subCoordY;

                    
                    feature.m_dir = 0.f;
                        
                    // oFeatures.push_back(feature);
                    m_Coordiantes.push_back(feature);
                }
            }
        }
        // m_O_Coordiantes.push_back(oFeatures);
    }

#if EXTREMEPOINTSTATISTICS
    std::cout << "findNum: " << findNum << "    extremeNum: " << extremeNum << "    detHOutNum: " << detHOutNum << 
    "    EDGOutNum: " << EDGOutNum << "    detMOutNum: " << detMOutNum << 
    "    xOffsetOutNum: " << xOffsetOutNum << "    yOffsetOutNum: " << yOffsetOutNum <<
    "    sigmaOffsetOutNum: " << sigmaOffsetOutNum << "    responseOutNum: " << responseOutNum <<
    "    edgeOutNum: " << edgeOutNum << std::endl;
#endif
}

SIFT::~SIFT(){
    
    FREE_Memory(m_srcImg);

    for(int o = 0; o < m_octave; ++ o){        
        for(int s = 0; s < m_level + 3; ++ s){
            FREE_Memory(m_O_L_GaussImgs[o][s]);
        }
        m_O_L_GaussImgs[o].clear();
    }
    m_O_L_GaussImgs.clear();

    for(int o = 0; o < m_octave; ++ o){
        
        for(int s = 0; s < m_level + 2; ++ s){
            FREE_Memory(m_O_L_DOGImgs[o][s]);
        }
        m_O_L_DOGImgs[o].clear();
    }
    m_O_L_DOGImgs.clear();

    for(unsigned int i = 0; i < m_Level_GaussFilters.size(); ++ i){
        delete m_Level_GaussFilters[i];
        m_Level_GaussFilters[i] = NULL;
    }
    m_Level_GaussFilters.clear();
}

SIFT::GaussFilter::GaussFilter(int iKernelWidth, float iSigma) : kernel_width(iKernelWidth), sigma(iSigma)
{
    kernel = new float[kernel_width * kernel_width];
    Utily::GaussFilter(kernel_width, iSigma, kernel);
}

SIFT::GaussFilter::~GaussFilter(){
    if(kernel){
        delete[] kernel;
        kernel = NULL;
    }
}