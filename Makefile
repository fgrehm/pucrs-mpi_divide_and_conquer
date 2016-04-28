default:
	@echo 'No default task'

divide-and-conquer: divide_and_conquer.c bubble_sort.o
	mpicc $^ -Wall -o $@

single-process: single_process.c bubble_sort.o
	mpicc $^ -Wall -o $@

bubble_sort.o: bubble_sort.c bubble_sort.h
	mpicc -c $< -Wall -o $@

clean:
	rm -rf *.o single-process divide-and-conquer

hack:
	docker build -t mpi-divide-and-conquer . && docker run -ti --rm -v `pwd`:/code mpi-divide-and-conquer
