# ImageProcessing

./TwoImageAddTest image1_path image2_path out_image_path weight

./ImageRotateTest image_path out_image_path angle

./ImageBlurTest image_path out_image_path type ksize otherParam

./ImageFastCornerTest image_path out_image_path

./ImageHarrisCornerTest image_path out_image_path

./ImageInterpolationTest image_path out_image_path scale type otherparam

./ImageHarrisCornerTest image_path out_image_path

./ImageEdgeTest image_path out_image_path EdgeType MagType;
"EdgeType: "
"    EDGE_SOBEL = 0"
"    EDGE_ROBERTS = 1"
"    EDGE_PREWITT = 2"
"    EDGE_LAPLACIAN = 3"
"    EDGE_CANNY = 4"
"MagType: (EdgeType={EDGE_SOBEL, EDGE_ROBERTS, EDGE_PREWITT})"
"    MAG_L1 = 0"
"    MAG_L2 = 1"
"    MAG_LInfinite = 2"
"MagType: (EdgeType={EDGE_LAPLACIAN})"
"    X_0_Y_0 = 0"
"    X_45_Y_45 = 1"
"    X_0_45_Y_0_45 = 2"