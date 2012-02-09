#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> //for variable argument list types and functions

//Our array type
typedef struct { int* arr; int size; int alloced;} array;

//Initialize the array
void array_init(array* arr)
{
    //Allocate space for array elements
    arr->arr = malloc(sizeof(int)*2);
    //Initially allocated space for 2 elements
    arr->alloced = 2;
    //Initially no elements
    arr->size = 0;
}

void array_free(array* arr)
{
    free(arr->arr);
}

// ... indicates that here's the variable argument list
void array_append(array* arr, int nargs, ...) 
{
    va_list args;

    //While we don't have enough space in our array, double the allocated size
    while (arr->alloced < arr->size+nargs)
    {
        arr->arr = realloc(arr->arr, sizeof(int)*arr->alloced*2);
        arr->alloced *= 2;
    }

    va_start(args, nargs); // initialize args to store all argument after the argument nargs
    for (int i = 0; i < nargs; i++)
        arr->arr[arr->size++] = va_arg(args, int); // Get an int from the argument list, and put it into the array 
    va_end(args); //clean up
}

void array_print(array* arr)
{
    for (int i = 0; i < arr->size; i++)
    {
        if (i > 0)
            printf(", ");
        printf("%d", arr->arr[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    array arr;
    array_init(&arr);

    for (int i = 0; i < 5; i++)
    {
        array_append(&arr, 3, 1, 2, 3); //Append 1,2,3 to the array
        array_print(&arr);
    }

    array_free(&arr);

    return 0;
}
