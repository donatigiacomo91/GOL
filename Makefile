sequential:
	icc -std=c++11 -O3 sequential.cpp -o seq.exe

threads:
	icc threads.cpp -std=c++11 -O3 -o threads.exe

pthread:
	icc pthread.cpp -std=c++11 -O3 -pthread -o pthread.exe

clean:
	rm *.exe
