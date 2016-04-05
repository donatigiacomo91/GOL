sequential:
	icc sequential.cpp -std=c++11 -O3 -o seq.exe

m_sequential:
	icc sequential.cpp -std=c++11 -O3 -mmic -o m_seq.exe

threads:
	icc threads.cpp -std=c++11 -O3 -o threads.exe

pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -o pthread.exe

m_pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -mmic -o m_pthread.exe

openmp:
	icc openmp.cpp -std=c++11 -O3 -fopenmp -o openmp.exe

m_openmp:
	icc openmp.cpp -std=c++11 -O3 -fopenmp -mmic -o m_openmp.exe

clean:
	rm *.exe
