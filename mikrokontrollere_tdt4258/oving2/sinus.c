#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define M_PI 3.14159265358979323846  /* pi */

static short* list;
static int size = 0;

void generateTone( int freq, unsigned int oscillator, int amp ) {
	int samples = oscillator / freq;
	size = samples;
	printf( "samples: %d\n", samples);
	list = malloc(sizeof(short)*size);
	for(int i = 1; i <= samples; i++) {
		//list[i-1] = (short)amp*sin(360/i*M_PI/180);
		list[i-1] = (short)amp*sin(i*2*M_PI/size);
	}
}

int main( int argc, char** argv )
{
	/* A=400Hz, osc=32kHz, amp=2^14 */
	generateTone(400, 32000, 16384);
	printf( "A: \n");
	printf( "{ ");
	for(int i = 0; i < size-1; i++) {
		short t = list[i];
		printf( "%d, ", t);
	}
	short t = list[size-1];
	printf( "%d ", t);
    printf( " };\n");
	return 0;
}


/* vim: set ts=4 sw=4: */

