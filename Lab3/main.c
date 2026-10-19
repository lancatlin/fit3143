#include "charging_node.h"
#include "context.h"
#include "my_mpi.h"
#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int mpi_rank;
    int mpi_size;
    MPI_Comm comm2D;
    int dims[NDIMS] = {2, 4};
    int wrap_around[NDIMS] = {0, 0};
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    if (mpi_size != 8) {
        perror("MPI size is not 8, exiting...");
        MPI_Finalize();
        return -1;
    }

    MPI_Dims_create(mpi_size, NDIMS, dims);
    if (mpi_rank == 0) {
        printf("PW[%d]/[%d]: PEdims = [%d x %d]\n", mpi_rank, mpi_size, dims[0],
               dims[1]);
    }

    int reorder = 1;
    int ierr = MPI_Cart_create(MPI_COMM_WORLD, NDIMS, dims, wrap_around,
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
    sleep(20);
    context_shutdown(&ctx);

    pthread_join(node.thread_id, NULL);
    pthread_join(recv_id, NULL);
    pthread_join(send_id, NULL);

    MPI_Finalize();
    context_destroy(&ctx);

    return 0;
}
