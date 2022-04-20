
void TwoImageAdd(unsigned char* rgb0, unsigned char* rgb1, unsigned char* orgb, int width, int height, float weight){
    int size = width * height * 3;
    float weight1 = 1.f - weight;

    for(int i = 0; i < size; ++ i){
        float v = rgb0[i] * weight + rgb1[i] * weight1;
        orgb[i] = v > 255.f ? (unsigned char)255 : (v < 0.f ? (unsigned char)0 : (unsigned char)v);
    }
}