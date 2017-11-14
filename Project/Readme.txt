=========================
	README 
=========================

Team :- Ramkumar Rajabaskaran 85241493 rrajabas@uci.edu	
	Sindhuri Rayavaram 28562782 srayavar@uci.edu

+++++++++++++++++++++++++
	STEPS 
+++++++++++++++++++++++++

* To compile the OpenMP file use the command :- g++ -fopenmp bmh_omp.cpp -o bmh_omp
* To run the OpenMP file use the command :- ./bmh_omp ../gene.txt catgtag 64  

The ../gene.txt is the file to be read(searched)
catgtag is the pattern
64 is the number of threads

* You can also submit the test.sh file to the queue like :- qsub test.sh

* For the MPI File, use the command :- mpiCC bmh_mpi.cpp -o bmh_mpi
* To run it use the command mpirun -np 64 ./bmh_mpi ../gene.txt catgtag
* You can also submit the test.sh file to the queue as :- qsub test.sh
=======================================================================================

