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
	icc fastflow.cpp -std=c++11 -O3 -pthread -mmic -DNO_DEFAULT_MAPPING -I /home/spm1501/fastflow -o m_ff.exe

compile_all:
	make sequential
	make m_sequential
	make pthread
	make m_pthread
	make omp
	make m_omp
	make fastflow
	make m_fastflow

move_to_mic:
	scp m_seq.exe mic0:
	scp m_pthread.exe mic0:
	scp m_omp.exe mic0:
	scp m_ff.exe mic0:

clean:
	rm *.exe
