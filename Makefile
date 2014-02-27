all:
	gcc -std=c99 -lm -o gps gps.c bst.c -Wall -Werror -fpack-struct 
	chmod +x gps

clean:
	rm -rf gps
