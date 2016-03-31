sequential:
	icc sequential.cpp -std=c++11 -O3 -o seq.exe

threads:
	icc threads.cpp -std=c++11 -O3 -o threads.exe

pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -o pthread.exe

openmp:
	icc openmp.cpp -std=c++11 -O3 -fopenmp -o openmp.exe

clean:
	rm *.exe
