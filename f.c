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

/*Functions Signature*/
int main(int argc, char ** argv);
float complex mandelbrot(float complex z, float c);
float map(int s, int a1, int a2, float b1, float b2);
unsigned long _RGB(int r,int g, int b);
void calculatePixels(Display *d, Window w, int s, int x);
void zoom(char c, int x, int y);

//not good practice to make global variables
//Complex Plane Limits
float cminx = -2.0f;
float cmaxx = 2.0f;

float cminy = -2.0f;
float cmaxy = 2.0f;

//Real Plane limits
int rminx = 0;
int rmaxx = 800;

int rminy = 0;
int rmaxy = 800;

void zoom(char c, int x, int y){
	//xf and yf should be used to 'auto'-zoom
	float xf = map(x, rminx, rmaxx, cminx, cmaxx);
	float yf = map(y, rminy, rmaxy, cminy, cmaxy);

	float zoomstep = 0.1f;
	
	if(c == '-'){
		cminx -= zoomstep;
		cmaxx += zoomstep;
		cminy -= zoomstep;
		cmaxy += zoomstep;
	}else if(c == '+'){
		cminx += zoomstep;
		cmaxx -= zoomstep;
		cminy += zoomstep;
		cmaxy -= zoomstep;
	}
}

void calculatePixels(Display *d, Window w, int s, int x){
	int t = time(NULL);
	
	//float c = 0.245f;
	//float c = map(rand(), 0, RAND_MAX, -1.0f, 1.0f);
	float c = map(x, rminx, rmaxx, -1.0f, 1.0f);
	printf("%f\n", c);
	float r = 2.0f;

	int maxiter = 30;

	int i, j;
	for(j = 0; j < rmaxy; j++){
		for(i = 0; i < rmaxx; i++){
			float real = map(i, rminx, rmaxx, cminx, cmaxx);
			float imag = map(j, rminy, rmaxy, cminy, cmaxy);
			
			float complex z = real + imag * I;
			z = mandelbrot(z, c);

			int iter = 0;
			while(cabs(z) <= r){
				z = mandelbrot(z, c);
				if(iter >= maxiter) break;
				iter++;
			}

			//print pixel by iterations
			unsigned long color;
			//color = _RGB((int)(iter*cos(t*iter))%256, (int)(iter*iter*iter*sin(t))%256, (int)(iter*iter*iter*tan(t*iter))%256);
			//color = _RGB((int)(iter*iter*cos(iter))%256, (int)(iter*iter*iter*sin(iter))%256, (int)(iter*iter*iter*tan(iter))%256);
			//color = _RGB((int)(iter*iter)%256, (int)(iter*iter*iter)%256, (int)(iter*iter*iter)%256);
			color = _RGB((int)(iter*iter*t)%256, (int)(iter*iter*iter*t)%256, (int)(iter*iter*iter*t)%256);
			//color = _RGB(iter%256, (int)(tan(iter)*255)%256, (int)(sin(iter)*255)%256);
			XSetForeground(d, DefaultGC(d, s), color);
			XDrawPoint(d, w, DefaultGC(d, s), i, j);
			iter = 0;
		}
	}
	char txt[20];
	snprintf(txt, 20, "c = %f", c);
	XSetForeground(d, DefaultGC(d, s), _RGB(255,255,255));
	XDrawString(d, w, DefaultGC(d, s), 0, 10, txt, strlen(txt));
}

unsigned long _RGB(int r,int g, int b){
	return b + (g<<8) + (r<<16);
}

float map(int s, int a1, int a2, float b1, float b2){
	return b1 + (s-a1)*(b2-b1)/(a2-a1);
}

float complex mandelbrot(float complex z, float c){
	//return z*z + c
	return z*z*z + c;
	//return z*z + 1/z + c;
}

int main(int argc, char ** argv){	
	//random seed, time
	srand(time(NULL));
	
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
					zoom('+', e.xbutton.x, e.xbutton.y);
				break;
				case Button5:
					zoom('-', e.xbutton.x, e.xbutton.y);
				break;
				default:
				break;
			}
		}
		if(e.type == KeyPress){
			//printf("%x\n", e.xkey.keycode);
			float movestep = 0.01f;
			switch(e.xkey.keycode){
				case 0x71://left
				cminx += movestep;
				cmaxx += movestep;
				break;
				case 0x72://right
				cminx -= movestep;
				cmaxx -= movestep;
				break;
				case 0x6f://up
				cminy += movestep;
				cmaxy += movestep;
				break;
				case 0x74://down
				cminy -= movestep;
				cmaxy -= movestep;
				break;
				case 0x1b://r - render
				calculatePixels(d, w, s, x);
				break;
				case 0x19://w - enable auto render (slower)
				ar = ar ^ 1;
				break;
				case 0x27://s - save image
				{
					//save to ppm image
					FILE *fp;
					fp = fopen("f.ppm", "w+");
					fprintf(fp, "P3\n%d %d\n255\n", rmaxx, rmaxy);
					XImage *image;
					XMapRaised(d, w);
					XWindowAttributes attr;
					Status status = XGetWindowAttributes(d, w, &attr);
					image = XGetImage(d, w, 0, 0, attr.width, attr.height, AllPlanes, ZPixmap);
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
				case 0x09://esc
				return 0;
				break;
			}
		}
		if(ar){
			calculatePixels(d, w, s, x);
		}else{
			XSetForeground(d, DefaultGC(d, s), _RGB(0,0,0));
			XFillRectangle(d, w, DefaultGC(d, s), 0, 0, 200, 60);
		}
		char txt[100];
		snprintf(txt, 100, "R limits: %f %f", cminx, cmaxx);
		XSetForeground(d, DefaultGC(d, s), _RGB(255,255,255));
		XDrawString(d, w, DefaultGC(d, s), 0, 30, txt, strlen(txt));
		snprintf(txt, 100, "C limits: %f %f", cminy, cmaxy);
		XDrawString(d, w, DefaultGC(d, s), 0, 50, txt, strlen(txt));
	}

	getchar();
	XCloseDisplay(d);
	return 0;
}
