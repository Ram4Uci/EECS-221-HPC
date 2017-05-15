#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

void my_bcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator) {
  int rank, size;
  MPI_Comm_rank(communicator, &rank);
  MPI_Comm_size(communicator, &size);

  if (rank == root) {
    // If I am root, send data to everyone
    int i;
    for (i = 0; i < size; ++i)
      if (i != rank)
        MPI_Send(data, count, datatype, i, 0, communicator);
  } else {
    // If I'm a receiver, receive data from root
    MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
  }
}

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: compare_bcast num_elements num_trials\n");
    exit(1);
  }

  int num_elements = atoi(argv[1]);
  int num_trials = atoi(argv[2]);

  MPI_Init(NULL, NULL);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double my_bcast_time = 0.0;
  double mpi_bcast_time = 0.0;
  int i;
  int* data = (int*) malloc(sizeof(int) * num_elements);
  assert(data != NULL);

  for (i = 0; i < num_trials; i++) {
    // Time my_bcast
    // Synchronize before starting timing
    MPI_Barrier(MPI_COMM_WORLD);
    my_bcast_time -= MPI_Wtime();
    my_bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
    // Synchronize again before obtaining final time
    MPI_Barrier(MPI_COMM_WORLD);
    my_bcast_time += MPI_Wtime();

    // Time MPI_Bcast
    MPI_Barrier(MPI_COMM_WORLD);
    mpi_bcast_time -= MPI_Wtime();
    MPI_Bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    mpi_bcast_time += MPI_Wtime();
  }

  // Print timing information
  if (rank == 0) {
    printf("Data size = %d, Trials = %d\n", num_elements * (int) sizeof(int),
           num_trials);
    printf("Avg my_bcast time = %lf\n", my_bcast_time / num_trials);
    printf("Avg MPI_Bcast time = %lf\n", mpi_bcast_time / num_trials);
  }

  MPI_Finalize();
}
