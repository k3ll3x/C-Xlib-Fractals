# C-Xlib-Fractals

Fractals

![Screenshot](frac.jpg)

This is a simple C program that uses complex.h for manipulating complex numbers for the fractal calculation

	fimg.c			-	Same algorithm but saves the calculation into a ppm image
	f.c			-	Uses Xlib for displaying a window with the actual calculation, mouse wheel is implemented for a lame zoom, arrow keys to move the complex plane and finally left/right button for changing te value of c
	createAnimation.sh	-	Simple Bash script for using fragimg (compiled fimg.c) several times to create multiple images and then render them into a gif image, this has some bugs into the generation of the gif image

Keys:

	left	-	moves Plane
	right	-	moves Plane
	up	-	moves Plane
	down	-	moves Plane
	R	-	Renders current position, calles calculatePixels, usage for manual render
	W	-	Enable/Disable auto render
	S	-	Save Image as f.ppm
	ESC	-	Exit

For building the binaries just do

	make
