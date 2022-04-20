#include <iostream>
#include "TwoImageAdd.h"

int main(int argc, char* argv[]){

    if(argc != 5){
        std::cout << "./TwoImageAddTest image1_path image2_path out_image_path weight" << std::endl;
        return 0;
    }
    
    return 0;
}