build: single-process

single-process: single_process.c bubble_sort.o
	gcc $^ -Wall -o $@

bubble_sort.o: bubble_sort.c bubble_sort.h
	gcc -c $< -Wall -o $@

hack:
	docker build -t mpi-divide-and-conquer . && docker run -ti --rm -v `pwd`:/code mpi-divide-and-conquer
