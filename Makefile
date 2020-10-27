all:
	gcc f.c -o frac -lm -lX11 -fopenmp
	gcc fimg.c -o fracimg -lm
