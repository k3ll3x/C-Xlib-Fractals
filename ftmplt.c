/*
 * Author: Siegfried Keller
 * Date: 2020-10-22 00:50:00
 *
 * Title: fractals
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <math.h>
#include <time.h>
#include <omp.h>

/*Functions Signature*/
int main(int argc, char ** argv);
long double complex mandelbrot(long double complex * z, long double * c);
long double map(int * s, int * a1, int * a2, long double * b1, long double * b2);
unsigned long _RGB(unsigned char r, unsigned char g, unsigned char b);
void calculatePixels(Display * d, Window * w, int * s, int * x);
void zoom(char * c, int * x, int * y);

//not good practice to make global variables

//PI
float pi = 3.14159f;

//color changer
int clrset = 0;
int nclrset = 13;

//complex numbers
long double complex z;

//parts of complex numbers for mapping
long double real;
long double imag;

//complex functions configuration
int fflag = 0;
int ncfunc = 12;

//Complex Plane Limits
long double cminx = -2.0f;
long double cmaxx = 2.0f;

long double cminy = -2.0f;
long double cmaxy = 2.0f;

//Real Plane limits / window
int rminx = 0;
int rmaxx = 800;

int rminy = 0;
int rmaxy = 800;

//save image counter
int count = 0;

//moving variable
long double movestepx, movestepy;

//max iterations
int maxiter = 2;

void zoom(char * c, int * x, int * y){
	long double xf = map(x, &rminx, &rmaxx, &cminx, &cmaxx);
	long double yf = map(y, &rminy, &rmaxy, &cminy, &cmaxy);
	//printf("%Lf %Lf\n", xf, yf);
	
	if(*c == '-'){
		//movestep += movestep/3;
		cminx = (cminx * 2 - xf);
		cmaxx = (cmaxx * 2 - xf);
		cminy = (cminy * 2 - yf);
		cmaxy = (cmaxy * 2 - yf);
	}else if(*c == '+'){
		//movestep -= movestep/3;
		cminx = (cminx + xf) / 2;
		cmaxx = (cmaxx + xf) / 2;
		cminy = (cminy + yf) / 2;
		cmaxy = (cmaxy + yf) / 2;
	}
	movestepx = (cmaxx - cminx) / 16;
	movestepy = (cmaxy - cminy) / 16;
}

void calculatePixels(Display * d, Window * w, int * s, int * x){
	int t = time(NULL);
	
	long double a = -1.0f;
	long double b = 1.0f;
	long double c = map(x, &rminx, &rmaxx, &a, &b);
	//printf("%Lf\n", c);
	float r = 2.0f;

	int i, j;
	int iter = 0;
	unsigned long color;
	#pragma omp parallel shared(d,s,w) private(z, j, i, real, imag, color, iter)
	{
	#pragma omp for
		for(j = 0; j < rmaxy; j++){
			for(i = 0; i < rmaxx; i++){
				real = map(&i, &rminx, &rmaxx, &cminx, &cmaxx);
				imag = map(&j, &rminy, &rmaxy, &cminy, &cmaxy);
				
				z = real + imag * I;
				z = mandelbrot(&z, &c);

				iter = 0;
				while(cabs(z) <= r){
					z = mandelbrot(&z, &c);
					if(iter >= maxiter) break;
					iter++;
				}

				//print pixel by iterations
				switch(clrset){
					case 0:
					//color = _RGB(iter*iter%256, iter*iter*iter%256, iter%256);
					color =  _RGB((int)(RED) % 256, (int)(GREEN) % 256, (int)(BLUE) % 256);
					break;
					case 1:
					color = _RGB((int)(iter*creal(z))%256, (int)(iter*cimag(z))%256, (int)(cos(z)*iter)%256);
					break;
					case 2:
					color = _RGB(iter%256, (int)(iter*cimag(z))%256, (int)(iter*creal(z))%256);
					//color = _RGB((int)(iter*iter)%256, (int)(iter*iter*iter)%256, (int)(iter*iter*iter)%256);
					break;
					case 3:
					color = _RGB(((int)cabsl(z)*iter)%256, ((int)cargl(z)*iter)%256, ((int)cimagl(z)*iter)%256);
					break;
					case 4:
					color = _RGB((int)(iter*iter + z)%256, (int)(z*z*iter / z)%256, (int)(z*iter - z)%256);
					break;
					case 5:
					color = _RGB(((int)cargl(z*z)*iter)%256, ((int)cimagl(z*z*z)*iter)%256, ((int)cos(z)*iter)%256);
					break;
					case 6:
					color = _RGB(iter%256, t*iter%256, t*t*iter%256);
					break;
					case 7:
					color = _RGB((int)(iter*cos(t*iter))%256, (int)(iter*iter*iter*sin(t))%256, (int)(iter*iter*iter*tan(t*iter))%256);
					break;
					case 8:
					color = _RGB((int)(iter*iter*cos(iter))%256, (int)(iter*iter*iter*sin(iter))%256, (int)(iter*iter*iter*tan(iter))%256);
					break;
					case 9:
					color = _RGB((iter*iter*iter)%256, (iter*iter)%256, iter%256);
					break;
					case 10:
					color = _RGB((int)(iter*iter*t)%256, (int)(iter*iter*iter*t)%256, (int)(iter*iter*iter*t)%256);
					break;
					case 11:
					color = _RGB(0, (int)(iter*iter*iter*t)%256, 0);
					break;
					case 12:
					color = _RGB(iter%256, (int)(tan(iter)*255)%256, (int)(sin(iter)*255)%256);
					break;
				}

				#pragma omp critical
				{
					XSetForeground(d, DefaultGC(d, *s), color);
					XDrawPoint(d, *w, DefaultGC(d, *s), i, j);
				}
			}
		}
	}
	char txt[20];
	snprintf(txt, 20, "c = %Lf", c);
	XSetForeground(d, DefaultGC(d, *s), _RGB(255,255,255));
	XDrawString(d, *w, DefaultGC(d, *s), 0, 10, txt, strlen(txt));
}

unsigned long _RGB(unsigned char r, unsigned char g, unsigned char b){
	return b + (g<<8) + (r<<16);
}

long double map(int * s, int * a1, int * a2, long double * b1, long double * b2){
	return *b1 + (*s-*a1)*(*b2-*b1)/(*a2-*a1);
}

long double complex mandelbrot(long double complex * z, long double * c){
	switch(fflag){
		case 0:
		//user input
		return FUNCTION;
		break;
		case 1:
		//z*z*sqrt(z*c) - sinh(z+c)
		return *z**z*csqrtl(*z**c) - csinhl(*z+*c);
		break;
		case 2:
		//cos(z - c) * c - z-z*z*z*-z / z * asin(c)*z*atanl(c / z) - c
		return ccoshl(*z-*c) * *c - *z-*z**z**z*-*z / *z*casinhl(*c)**z*catanl(*c / *z) - *c;
		break;
		case 3:
		return ccoshl(*z) * csinhl(*z) * ctanhl(*z) * cpowl(*z, *z) + *c;
		break;
		case 4:
		//z* (cimag(z) - creal(z) * I) + c * cos(z * c)
		return *z* (cimag(*z) - creal(*z) * I) + *c * cos(*z * *c);
		break;
		case 5:
		return *z * *z * (1 / *z) + sin(*c);
		break;
		case 6:
		return *z + *z**z + *z**z**z + *z**z**z**z + *c;
		break;
		case 7:
		return *z**z**z + cos(*z**z**z**c) - sin(*c**c**c**z);
		break;
		case 8:
		return (*z * pi * *c) - *z**z - *c;
		break;
		case 9:
		return *z**z**z - *c;
		break;
		case 10:
		return *z * (creal(*z) - tan(cimag(*z) * *c + *c) * I);
		break;
		case 11:
		return ctanhl(*z) + ccoshl(*z) - *c;
		break;
		default:
		//z + c
		return *z + *c;
		break;
	}
}

int main(int argc, char ** argv){	
	//random seed, time
	srand(time(NULL));

	//set moving step for x and y
	movestepx = (cmaxx - cminx) / 16;
	movestepy = (cmaxy - cminy) / 16;
	
	Display *d;
	Window w;
	XEvent e;
	int s;

	d = XOpenDisplay(NULL);
	if(d == NULL) {
		fprintf(stderr, "Cannot open display\n");
		exit(1);
	}

	s = DefaultScreen(d);
	w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, rmaxx, rmaxy, 1, BlackPixel(d, s), WhitePixel(d, s));
	XSelectInput(d, w, ExposureMask | KeyPressMask | ButtonPressMask);
	XMapWindow(d, w);
	XStoreName(d, w, "Fractals");

	int x, y;
	unsigned short ar = 1;

	while(1) {
		XNextEvent(d, &e);
		if(e.type == ButtonPressMask) {
			switch(e.xbutton.button){
				case Button1:
					x=e.xbutton.x;
					y=e.xbutton.y;
				break;
				case Button3:
					x=e.xbutton.x;
					y=e.xbutton.y;
				break;
				case Button4:
				{
					char zc = '+';
					zoom(&zc, &e.xbutton.x, &e.xbutton.y);
				}
				break;
				case Button5:
				{
					char zc = '-';
					zoom(&zc, &e.xbutton.x, &e.xbutton.y);
				}
				break;
				default:
				break;
			}
		}
		if(e.type == KeyPress){
			printf("%x\n", e.xkey.keycode);
			switch(e.xkey.keycode){
				case 0x26://a - change color set
				clrset++;
				clrset %= nclrset;
				break;
				case 0x71://left
				cminx -= movestepx;
				cmaxx -= movestepx;
				break;
				case 0x72://right
				cminx += movestepx;
				cmaxx += movestepx;
				break;
				case 0x6f://up
				cminy -= movestepy;
				cmaxy -= movestepy;
				break;
				case 0x74://down
				cminy += movestepy;
				cmaxy += movestepy;
				break;
				case 0x1b://r - render
				calculatePixels(d, &w, &s, &x);
				break;
				case 0x19://w - enable auto render (slower)
				ar = ar ^ 1;
				break;
				case 0x27://s - save image
				{
					XSetForeground(d, DefaultGC(d, s), _RGB(255,255,255));
					char * txt = "Saving image, please wait...";
					XDrawString(d, w, DefaultGC(d, s), 0, 90, txt, strlen(txt));
					//save to ppm image
					FILE *fp;
					char filename[20];
					snprintf(filename, 20, "img/frac_%d.ppm", count);
					count++;
					XWindowAttributes attr;
					Status status = XGetWindowAttributes(d, w, &attr);
					fp = fopen(filename, "w+");
					fprintf(fp, "P3\n%d %d\n255\n", attr.width, attr.height);
					XImage *image;
					image = XGetImage(d, w, 0, 0, attr.width, attr.height, AllPlanes, XYPixmap);
					int i, j;
					for(j = 0; j < attr.height; j++){
						for(i = 0; i < attr.width; i++){
							XColor color;
							color.pixel = XGetPixel(image, i, j);
							XQueryColor(d, XDefaultColormap(d, XDefaultScreen(d)), &color);
							fprintf(fp, "%d %d %d\n", color.red/256, color.green/256, color.blue/256);
						}
					}
					XFree(image);
					fclose(fp);
				}
				break;
				case 0x36://c - change return function +
				fflag++;
				fflag = fflag%ncfunc;
				break;
				case 0x35://x - change return function -
				fflag--;
				if(fflag <= 0) fflag = ncfunc - 1;
				fflag = fflag%ncfunc;
				break;
				case 0x28://d - restore complex plane to default
				cminx = -2.0f;
				cmaxx = 2.0f;
				cminy = -2.0f;
				cmaxy = 2.0f;
				break;
				case 0x29://f - increment max iterations per pixel
				maxiter++;
				break;
				case 0x2A://g - decrease max iterations per pixel
				maxiter--;
				break;
				case 0x09://esc
				return 0;
				break;
			}
		}
		if(ar){
			calculatePixels(d, &w, &s, &x);
		}else{
			XSetForeground(d, DefaultGC(d, s), _RGB(0,0,0));
			XFillRectangle(d, w, DefaultGC(d, s), 0, 0, 200, 60);
		}
		char txt[100];
		snprintf(txt, 100, "R limits: %Lf %Lf", cminx, cmaxx);
		XSetForeground(d, DefaultGC(d, s), _RGB(255,255,255));
		XDrawString(d, w, DefaultGC(d, s), 0, 30, txt, strlen(txt));
		snprintf(txt, 100, "C limits: %Lf %Lf", cminy, cmaxy);
		XDrawString(d, w, DefaultGC(d, s), 0, 50, txt, strlen(txt));
		if(fflag == 0){
			snprintf(txt, 100, "PFUNCTION");
			XDrawString(d, w, DefaultGC(d, s), 0, 70, txt, strlen(txt));
		}
		
		//printf("function %d\n", fflag);
	}

	getchar();
	XCloseDisplay(d);
	return 0;
}
