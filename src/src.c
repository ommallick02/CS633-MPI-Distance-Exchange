/*
===============================================================================
Assignment 1
File: src.c

Description:
    This program implements an iterative distance-based data exchange
    pattern using MPI point-to-point communication.

    Each rank communicates with neighbors located D1 and D2 positions
    to its right (rank + D1, rank + D2). A rank simultaneously plays
    two roles:

        1. Sender:
           Sends its local buffer to right neighbors and receives
           processed data back.

        2. Receiver:
           Receives data from left neighbors (rank - D1, rank - D2),
           performs the required computation, and returns the result.

    The D1 path performs element-wise squaring.
    The D2 path performs element-wise logarithm followed by scaling.

    This procedure is repeated for T iterations.

Input:
    M      : Number of doubles
    D1     : First communication distance
    D2     : Second communication distance
    T      : Number of iterations
    seed   : Random seed

Output:
    <maximumD1> <maximumD2> <execution_time>

Compilation:
    mpicc -o execute src.c -lm

Execution:
    mpirun -np <P> -ppn 16 ./execute <M> <D1> <D2> <T> <SEED>

NOTE: run command `mpirun` before compilation
===============================================================================
*/


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>


// //  Job ID = Job ID = 3026771
// // [0] 99936.000000 1416360.081584 16.386152

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, P;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &P);

    if (argc < 6) {
        if (rank == 0)
            printf("Usage: ./program M D1 D2 T seed\n");
        MPI_Finalize();
        return 0;
    }

    int M       = atoi(argv[1]);
    int D1      = atoi(argv[2]);
    int D2      = atoi(argv[3]);
    int T       = atoi(argv[4]);
    int seed    = atoi(argv[5]);

    double *data_D1 = malloc(M * sizeof(double));
    double *data_D2 = malloc(M * sizeof(double));
    double *recv_buffer_D1 = malloc(M * sizeof(double));
    double *recv_buffer_D2 = malloc(M * sizeof(double));

    MPI_Status status;

    /* ================= INITIALIZATION ================= */

    //random initialisation generated using the given seed
    srand(seed); 
    for (int i=0; i<M; i++){
            data_D1[i] = (double)rand()*(rank+1)/10000.0;
    }

    srand(seed); 
    for (int i=0; i<M; i++){
            data_D2[i] = (double)rand()*(rank+1)/10000.0;
    }

    double start_time = MPI_Wtime();

    /* ================= ITERATIONS ================= */

    for (int iter = 0; iter < T; iter++) {

        /* ================= D1 ================= */

        //Valid sender sends

        if (rank + D1 < P)
            MPI_Send(data_D1, M, MPI_DOUBLE, rank + D1, 1, MPI_COMM_WORLD);

        
        //Receiver receives + computes + sends back
        if (rank - D1 >= 0) {

            MPI_Recv(recv_buffer_D1, M, MPI_DOUBLE, rank - D1, 1, MPI_COMM_WORLD, &status);

            for (int i = 0; i < M; i++)
                recv_buffer_D1[i] *= recv_buffer_D1[i];

            MPI_Send(recv_buffer_D1, M, MPI_DOUBLE, rank - D1, 2, MPI_COMM_WORLD);
        }

        //Sender receives back + updates for next iteration

        if (rank + D1 < P) {

            MPI_Recv(recv_buffer_D1, M, MPI_DOUBLE, rank + D1, 2, MPI_COMM_WORLD, &status);

            for (int i = 0; i < M; i++)
                data_D1[i] = (unsigned long long)recv_buffer_D1[i] % 100000;
        }



        /* ================= D2 ================= */

        //Valid sender sends
        if (rank + D2 < P)
            MPI_Send(data_D2, M, MPI_DOUBLE, rank + D2, 3, MPI_COMM_WORLD);


        //Receiver receives + computes + sends back
        if (rank - D2 >= 0) {

            MPI_Recv(recv_buffer_D2, M, MPI_DOUBLE, rank - D2, 3, MPI_COMM_WORLD, &status);

            for (int i = 0; i < M; i++) {
                if (recv_buffer_D2[i] <= 0)
                    recv_buffer_D2[i] = 0.0;
                else
                    recv_buffer_D2[i] = log(recv_buffer_D2[i]);
            }

            MPI_Send(recv_buffer_D2, M, MPI_DOUBLE, rank - D2, 4, MPI_COMM_WORLD);
        }
        
        //Sender receives back + updates for next iteration
        if (rank + D2 < P) {

            MPI_Recv(recv_buffer_D2, M, MPI_DOUBLE, rank + D2, 4, MPI_COMM_WORLD, &status);

            for (int i = 0; i < M; i++)
                data_D2[i] = recv_buffer_D2[i] * 100000.0;
        }
    }

    /* ================= LOCAL MAX ================= */

    double local_max_D1 = -DBL_MAX;
    double local_max_D2 = -DBL_MAX;

    int valid_D1 = (rank + D1 < P);
    int valid_D2 = (rank + D2 < P);

    if (valid_D1) {
        for (int i = 0; i < M; i++)
            if (data_D1[i] > local_max_D1)
                local_max_D1 = data_D1[i];
    }

    if (valid_D2) {
        for (int i = 0; i < M; i++)
            if (data_D2[i] > local_max_D2)
                local_max_D2 = data_D2[i];
    }
    

    

    /* ================= SEND MAX TO RANK 0 ================= */

    int is_sender = (rank + D1 < P);

    double global_max_D1 = local_max_D1;
    double global_max_D2 = local_max_D2;
    
    /* Non-senders should not contribute */
    if (!is_sender) {
        global_max_D1 = -DBL_MAX;
        global_max_D2 = -DBL_MAX;
    }
    
    //hierarchical pairwise scheme in log(P) stages
    for (int step = 1; step < P; step <<= 1) {
    
        /* RECEIVER */
        if (rank % (2 * step) == 0) {
    
            int src = rank + step;
    
            if (src < P) {
    
                // Only receive if sender is valid 
                if (src + D1 < P) {
    
                    double temp_D1, temp_D2;
    
                    MPI_Recv(&temp_D1, 1, MPI_DOUBLE, src, 200, MPI_COMM_WORLD, &status);
                    MPI_Recv(&temp_D2, 1, MPI_DOUBLE, src, 201, MPI_COMM_WORLD, &status);
    
                    if (temp_D1 > global_max_D1)
                        global_max_D1 = temp_D1;
    
                    if (temp_D2 > global_max_D2)
                        global_max_D2 = temp_D2;
                }
            }
    
        }
        /* SENDER */
        else {
            int dst = rank - step;
    
            if (is_sender) {
                MPI_Send(&global_max_D1, 1, MPI_DOUBLE, dst, 200, MPI_COMM_WORLD);
                MPI_Send(&global_max_D2, 1, MPI_DOUBLE, dst, 201, MPI_COMM_WORLD);
            }
            break;  // sender exits after sending 
        }
    }


    double total_time = MPI_Wtime() - start_time;
    

    /* ================= FINAL OUTPUT ================= */

    if (rank == 0) {
        printf("%lf %lf %lf\n",global_max_D1, global_max_D2, total_time);
    }

    free(data_D1);
    free(data_D2);
    free(recv_buffer_D1);
    free(recv_buffer_D2);

    

    MPI_Finalize();
    return 0;
}