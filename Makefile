all:
	gcc f.c -o frac -lm -lX11 -fopenmp
	gcc fimg.c -o fracimg -lm
fuser:
	gcc fuser.c -o ufrac -lm -lX11 -fopenmp
	./ufrac &
