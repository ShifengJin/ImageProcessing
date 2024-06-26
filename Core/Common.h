#ifndef _COMMON_H_
#define _COMMON_H_

#include <float.h>

#define PI 3.1415926f
#define CLAMP(x, a, b) ((x) > (b) ? (b) : ((x) < (a) ? (a) : (x)))
#define ANGLE2RAD 0.01745329f

//YUV to RGB formulas
#define CVT_R_255(y, u, v) (1.164f * ((y) - 16.0f) + 1.596f * ((v) - 128.0f))
#define CVT_G_255(y, u, v) (1.164f * ((y) - 16.0f) - 0.813f * ((u) - 128.0f) - 0.392f * ((v) - 128.0f))
#define CVT_B_255(y, u, v) (1.164f * ((y) - 16.0f) + 2.017f * ((u) - 128.0f))

//RGB to YUV formulas
#define CVT_Y_255(r, g, b) ( 0.257f * (r) + 0.504f * (g) + 0.098f * (b) +  16.0f)
#define CVT_U_255(r, g, b) (-0.148f * (r) - 0.291f * (g) + 0.439f * (b) + 128.0f)
#define CVT_V_255(r, g, b) ( 0.439f * (r) - 0.368f * (g) - 0.071f * (b) + 128.0f)

//YUV to RGB formulas
#define CVT_R_Normal(y, u, v) (1.164f * ((y) - 0.0625f) + 1.596f * ((v) - 0.5f))
#define CVT_G_Normal(y, u, v) (1.164f * ((y) - 0.0625f) - 0.813f * ((u) - 0.5f) - 0.392f * ((v) - 0.5f))
#define CVT_B_Normal(y, u, v) (1.164f * ((y) - 0.0625f) + 2.017f * ((u) - 0.5f))

//RGB to YUV formulas
#define CVT_Y_Normal(r, g, b) ( 0.257f * (r) + 0.504f * (g) + 0.098f * (b) + 0.0625f)
#define CVT_U_Normal(r, g, b) (-0.148f * (r) - 0.291f * (g) + 0.439f * (b) + 0.5f)
#define CVT_V_Normal(r, g, b) ( 0.439f * (r) - 0.368f * (g) - 0.071f * (b) + 0.5f)

#define FREE_Memory(ptr) do{if(ptr){free(ptr); ptr = NULL;}}while(0)


typedef struct _Vector2i_{
    int x;
    int y;
}Vector2i;

typedef struct _Vector2U_{
    unsigned int x;
    unsigned int y;
}Vector2U;

typedef Vector2U ImgSize;

typedef struct _Vector2f_{
    float x;
    float y;
}Vector2f;

typedef struct _Vector3UC_{
    unsigned char x;
    unsigned char y;
    unsigned char z;
}Vector3UC;

typedef struct _Vector3f_{
    float x;
    float y;
    float z;
}Vector3f;

#endif