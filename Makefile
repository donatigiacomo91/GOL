sequential:
	icc sequential.cpp -std=c++11 -O3 -o seq.exe

m_sequential:
	icc sequential.cpp -std=c++11 -O3 -mmic -o m_seq.exe

pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -o pthread.exe

m_pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -mmic -o m_pthread.exe

omp:
	icc omp.cpp -std=c++11 -O3 -fopenmp -o omp.exe

m_omp:
	icc omp.cpp -std=c++11 -O3 -fopenmp -mmic -o m_omp.exe

fastflow:
	icc fastflow.cpp -std=c++11 -O3 -pthread -DNO_DEFAULT_MAPPING -I /home/spm1501/fastflow -o ff.exe

m_fastflow:
	icc fastflow.cpp -std=c++11 -O3 -pthread -mmic -DNO_DEFAULT_MAPPING -I /home/spm1501/fastflow -o ff.exe

clean:
	rm *.exe
