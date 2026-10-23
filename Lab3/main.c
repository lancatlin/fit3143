#include "charging_node.h"
#include "context.h"
#include "my_mpi.h"
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "base_station.c"

#define BASE_STATION_COLOUR 0
#define NODE_COLOUR 1

#define NO_ROWS 2
#define NO_COLUMNS 4

int main(int argc, char* argv[]) {
    int mpi_rank;
    int mpi_size;
    MPI_Comm comm2D;
    MPI_Comm NODES_COMM;
    int dims[NDIMS] = {NO_ROWS, NO_COLUMNS};
    int wrap_around[NDIMS] = {0, 0};
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE) {
        fprintf(stderr, "MPI_THREAD_MULTIPLE is not supported\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    MPI_Comm_split(MPI_COMM_WORLD, mpi_rank < NO_ROWS * NO_COLUMNS, mpi_rank, &NODES_COMM);

    if (mpi_rank < NO_ROWS * NO_COLUMNS) {
        // if (mpi_size != 8) {
        //     perror("MPI size is not 8, exiting...");
        //     MPI_Finalize();
        //     return -1;
        // }

        MPI_Dims_create(mpi_size - 1, NDIMS, dims);
        if (mpi_rank == 0) {
            printf("PW[%d]/[%d]: PEdims = [%d x %d]\n", mpi_rank, mpi_size, dims[0],
                dims[1]);
        }

        int reorder = 1;
        int ierr = MPI_Cart_create(NODES_COMM, NDIMS, dims, wrap_around,
                                reorder, &comm2D);
        if (ierr != 0) {
            perror("Failed to create cartesion");
        }

        Context ctx;
        context_init(&ctx);

        ChargingNode node = create_charging_node(mpi_rank, 9);
        set_neighbours(&node, &comm2D);

        RecvQueueParams* recv_params =
            (RecvQueueParams*)malloc(sizeof(RecvQueueParams));
        recv_params->node = &node;
        recv_params->comm2D = &comm2D;
        recv_params->ctx = &ctx;
        pthread_t recv_id = start_recv_queue(recv_params);

        SendQueueParams* send_params =
            (SendQueueParams*)malloc(sizeof(SendQueueParams));
        send_params->node = &node;
        send_params->comm2D = &comm2D;
        send_params->ctx = &ctx;
        pthread_t send_id = start_send_queue(send_params);

        RunNodeParams* node_params = (RunNodeParams*)malloc(sizeof(RunNodeParams));
        node_params->node = &node;
        node_params->ctx = &ctx;
        int result = run_node(node_params);
        if (result != 0) {
            perror("run node error");
        }

        // TODO: simulate shutdown
        sleep(15);
        context_shutdown(&ctx);

        pthread_join(node.thread_id, NULL);
        pthread_join(recv_id, NULL);
        pthread_join(send_id, NULL);

        context_destroy(&ctx);
        MPI_Comm_free(&comm2D);
        MPI_Comm_free(&NODES_COMM);
        MPI_Finalize();

        printf("Node %d: exited\n", node.id);

        return 0;
    } else {
        // base station
        BaseStation *baseStation = create_base_station(mpi_size, NDIMS, dims);

        run_base_station(baseStation);
        MPI_Comm_free(&NODES_COMM);
        MPI_Finalize();
        printf("BaseStation has exited.\n");
        return 0;
    }
}
