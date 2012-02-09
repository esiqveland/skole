#include <math.h>
#include "mylibrary.h"

//Compute cos of angle between two vectors
float angle(float v1x, float v1y, float v2x, float v2y)
{
    //Normalize
    float len1 = sqrt(v1x*v1x+v1y*v1y);
    float len2 = sqrt(v2x*v2x+v2y*v2y);

    return acos(dot_product(v1x/len1, v1y/len1, v2x/len2, v2y/len2));
}

