/*
 * Author: Siegfried Keller
 * Date: 2020-10-20 16:45:35.693038
 *
 * Title: fractals
 */

#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

/*Functions Signature*/
int main(int argc, char ** argv);
float complex mandelbrot(float complex z, float c);
float map(int s, int a1, int a2, float b1, float b2);

float map(int s, int a1, int a2, float b1, float b2){
	return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

float complex mandelbrot(float complex z, float c){
	return z*z*z + c;
	//return z*z + 1/z + c;
}

int main(int argc, char ** argv){
	//random seed, time
	int t = time(NULL);
	srand(t);
	
	//Real Plane limits
	int rminx = 0;
	int rmaxx = 1000;
	
	int rminy = 0;
	int rmaxy = 1000;

	//Complex Plane Limits
	float cminx = -2.0f;
	float cmaxx = 2.0f;

	float cminy = -2.0f;
	float cmaxy = 2.0f;
	
	//float c = 0.245f;
	float c = map(rand(), 0, RAND_MAX, -1.0f, 1.0f);
	printf("%f\n", c);
	float r = 2.0f;

	int maxiter = 30;

	//save to ppm image
	FILE *fp;
	fp = fopen("frac.ppm", "w+");
	fprintf(fp, "P3\n%d %d\n255\n", rmaxx, rmaxy);

	int i, j;
	for(j = 0; j < rmaxy; j++){
		for(i = 0; i < rmaxx; i++){
			float real = map(i, rminx, rmaxx, cminx, cmaxx);
			float imag = map(j, rminy, rmaxy, cminy, cmaxy);
			
			//printf("%f + %fi\n", real, imag);
			
			float complex z = real + imag * I;
			z = mandelbrot(z, c);

			int iter = 0;
			while(cabs(z) <= r){
				z = mandelbrot(z, c);
				if(iter >= maxiter) break;
				iter++;
				//printf("%f + %fi\titer:\t%d\n", creal(z), cimag(z), iter);
			}

			//print pixel by iterations
			//printf("%f + %fi\titer:\t%d\n", creal(z), cimag(z), iter);
			//fprintf(fp, "%d %d %d\n", abs(iter%256), abs((int)(cos(iter)*255)%256), abs((int)(sin(iter)*255)%256));
			fprintf(fp, "%d %d %d\n", (int)(iter*iter*t)%256, (int)(iter*iter*iter*t)%256, (int)(iter*iter*iter*t)%256);
			//fprintf(fp, "%d %d %d\n", iter%256, iter%256, iter%256);
			iter = 0;
		}
	}
	
	fclose(fp);

	//system("feh frac.ppm&");
	
	return 0;
}
