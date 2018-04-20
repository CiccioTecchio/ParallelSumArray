# ParallelSumArray

Un MPI program with k+1 process in with the MASTER subdivide an array of N int between k process slave and receive from each SLAVE the sum of their portion of int.
The SLAVE send to MASTER the sum of their portion and the MASTER sum it.

### Run instructions
- Install OpenMPI;
- Compile source;
```
mpicc -o ArrayPartition ArrayPartition.c
```
- Run `ArrayPartition` the first parameter is the length of array, the second is the seeds of the random number generator (e.g. ``` mpirun -np 4 -hostfile hostfile.txt ArrayPartition 20 100```).


