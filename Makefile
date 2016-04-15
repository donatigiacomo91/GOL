sequential:
	icc sequential.cpp -std=c++11 -O3 -o seq.exe

m_sequential:
	icc sequential.cpp -std=c++11 -O3 -o m_seq.exe -mmic

pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -o pthread.exe

m_pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -o m_pthread.exe -mmic

omp:
	icc omp.cpp -std=c++11 -O3 -fopenmp -o omp.exe

m_omp:
	icc omp.cpp -std=c++11 -O3 -fopenmp -o m_omp.exe -DNO_DEFAULT_MAPPING -mmic

fastflow:
	icc fastflow.cpp -std=c++11 -O3 -pthread -I /home/spm1501/fastflow -o ff.exe

m_fastflow:
	icc fastflow.cpp -std=c++11 -O3 -pthread -I /home/spm1501/fastflow -o m_ff.exe -DNO_DEFAULT_MAPPING -mmic

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
