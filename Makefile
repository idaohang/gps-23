all:
	gcc -std=c99 -lm -o gps gps.c bst.c -Wall -Werror -fpack-struct 
	chmod +x gps

test: clean all
	python test_gen.py 11 test
	./gps test

clean:
	rm -rf gps
