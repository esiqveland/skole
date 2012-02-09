#include <stdio.h>
#include <stdlib.h>

#include "my_header1.h"
#include "my_header2.h"

int main(int argc, char** argv)
{
    float vx = 2, vy = 3, 
          wx = 1, wy = 9;

    printf("Vector 1: %f %f\nVector 2: %f %f\n", vx, vy, wx,wy);
    printf("Angle between vectors: %f radians.\n", angle(vx,vy,wx,wy));
    printf("Length of vector 1: %f.\n", vector_length(vx,vy));
    printf("Length of vector 2: %f.\n", vector_length(wx,wy));

    return 0;
}
