#include "base_station.h"
#include "message.h"
#include <mpi.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

pthread_mutex_t availability_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_file_mutex = PTHREAD_MUTEX_INITIALIZER;

BaseStation* create_base_station(int mpi_size, int dims_no, int* dims) {
    BaseStation* baseStation =
        malloc(sizeof(BaseStation) + mpi_size * sizeof(bool));
    for (int i = 0; i < mpi_size; i++) {
        baseStation->is_node_available[i] = true;
    }
    baseStation->num_of_nodes = mpi_size;
    baseStation->log_file = fopen("log.txt", "w");
    baseStation->row_number = dims[0];
    baseStation->column_number = dims[1];
    return baseStation;
}

void free_base_station(BaseStation* baseStation) {
    if (baseStation->log_file != NULL) {
        fclose(baseStation->log_file);
    }
    free(baseStation);
}

void* respond_message(void* arg);

int run_base_station(BaseStation* baseStation) {
    pthread_t threads[MAX_THREADS];
    bool is_thread_active[MAX_THREADS];
    atomic_bool has_thread_ended[MAX_THREADS]; // atomic bool avoids race
                                               // conditions among threads
    NodeReport node_reports[MAX_THREADS];
    ThreadArgs thread_args[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) {
        is_thread_active[i] = false;
        atomic_init(&has_thread_ended[i], false);
    }

    int next_thread_id = 0;
    clock_t start_time = clock();
    clock_t current_time = clock();
    int alerts_num = 0;
    int events_num = 0;
    double time_elapsed = (double)(current_time - start_time) / CLOCKS_PER_SEC;
    while ((time_elapsed <= 20)) {
        int flag = 0;
        MPI_Status status;

        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        // There is an incoming message and there is a free thread
        if (flag == 1 && next_thread_id != -1) {
            // receive message
            int message[2];
            MPI_Status rStatus;
            MPI_Recv(message, 2, MPI_INT, status.MPI_SOURCE, status.MPI_TAG,
                     MPI_COMM_WORLD, &rStatus);

            // update counter
            if (status.MPI_TAG == MPI_FULL_TAG) {
                alerts_num++;
            } else {
                events_num++;
            }

            // create thread
            NodeReport* report = &node_reports[next_thread_id];
            report->nodeRank = message[0];
            report->special_arguement = message[1];
            current_time = clock();
            report->timeReceived =
                (double)(current_time - start_time) / CLOCKS_PER_SEC;
            ThreadArgs* args = &thread_args[next_thread_id];
            args->report = report;
            args->baseStation = baseStation;
            args->mpi_send_dest = status.MPI_SOURCE;
            args->has_ended = &has_thread_ended[next_thread_id];
            args->mpi_flag = status.MPI_TAG;

            int err_code = pthread_create(&threads[next_thread_id], NULL,
                                          &respond_message, args);
            if (err_code == 0) {
                is_thread_active[next_thread_id] = true;
                next_thread_id = -1; // flag id as used
            }
        }
        // check for any finished threads
        for (int i = 0; i < MAX_THREADS; i++) {
            if (is_thread_active[i] &&
                atomic_load(&has_thread_ended[i]) == true) {
                pthread_join(threads[i], NULL);
                atomic_init(&has_thread_ended[i], false);
                is_thread_active[i] = false;
            }
        }
        // get unused thread id for next loop
        if (next_thread_id == -1) {
            for (int i = 0; i < MAX_THREADS; i++) {
                if (is_thread_active[i] == false) {
                    next_thread_id = i;
                    break;
                }
            }
        }
        current_time = clock();
        time_elapsed = (double)(current_time - start_time) / CLOCKS_PER_SEC;
    }
    // exit gracefully
    // Wait for all threads to finish
    for (int i = 0; i < MAX_THREADS; i++) {
        if (is_thread_active[i]) {
            pthread_join(threads[i], NULL);
        }
    }
    // tell all nodes to exit

    // clean up base station

    fprintf(baseStation->log_file, "Simulation time: %.2f Seconds.\n",
            time_elapsed);
    fprintf(baseStation->log_file, "Number of Alerts Detected: %d Alerts.\n",
            alerts_num);
    fprintf(baseStation->log_file, "Total Number of Messages: %d Messages.\n",
            events_num);
    free_base_station(baseStation);
    return 0;
}

void _get_neighbour(int curr, int row_number, int column_number, int res[4]) {
    // rank of right neighbour
    if ((curr + 1) % column_number == 0) {
        res[0] = -1; // right neighbour is none
    } else {
        res[0] = curr + 1;
    }
    // rank of left neighbour
    if ((curr % column_number) == 0) {
        res[1] = -1; // left neighbour is none
    } else {
        res[1] = curr - 1;
    }
    // rank of neighbour above
    if (curr - column_number < 0) {
        res[2] = -1;
    } else {
        res[2] = curr - column_number;
    }
    // rank of neighbour below
    if (curr + column_number >= (column_number * row_number)) {
        res[3] = -1;
    } else {
        res[3] = curr + column_number;
    }
}

void* respond_message(void* arg) {
    ThreadArgs* a = (ThreadArgs*)arg;
    NodeReport report = *(a->report);
    int mpi_dest = a->mpi_send_dest;
    BaseStation* baseStation = a->baseStation;

    // Charging node reporting that it and its neighbours are all full
    if (a->mpi_flag == MPI_FULL_TAG) {
        // find nearest available nodes
        pthread_mutex_lock(&availability_mutex);
        bool* is_node_available = baseStation->is_node_available;
        int row_no = baseStation->row_number;
        int column_no = baseStation->column_number;
        int neigh[4];
        _get_neighbour(report.nodeRank, row_no, column_no, neigh);
        is_node_available[report.nodeRank] = false;
        for (int i = 0; i < 4; i++) {
            if (neigh[i] != -1) { // check for valid neighbours
                is_node_available[neigh[i]] = false;
            }
        }
        // a max of 8 nodes are exactly 2 steps away from current node,
        // given that current node and all its immediate neighbours are full
        int nearest_available_nodes[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
        int node_counter = 0;
        for (int i = 0; i < 4; i++) {
            if (neigh[i] != -1) { // check for valid neighbours
                int neigh2[4];
                _get_neighbour(neigh[i], row_no, column_no, neigh2);
                for (int i = 0; i < 4; i++) {
                    int rank = neigh2[i];
                    if (rank != -1 && is_node_available[rank]) {
                        nearest_available_nodes[node_counter] = rank;
                        node_counter++;
                    }
                }
            }
        }
        pthread_mutex_unlock(&availability_mutex);

        // formatting log string
        char log_str[150]; // string buffer size
        snprintf(log_str, sizeof(log_str),
                 "%.2f: Node %d is full; Nearest available nodes(s): ",
                 report.timeReceived, report.nodeRank);
        if (node_counter == 0) {
            strcat(log_str, "None.\n");
        } else {
            bool first_term = true;
            for (int i = 0; i < node_counter; i++) {
                char tmp[10];
                if (nearest_available_nodes[i] != -1) {
                    if (first_term) {
                        snprintf(tmp, sizeof(tmp), "%d",
                                 nearest_available_nodes[i]);
                        strcat(log_str, tmp);
                        first_term = false;
                        continue;
                    }
                    snprintf(tmp, sizeof(tmp), ", %d",
                             nearest_available_nodes[i]);
                    strcat(log_str, tmp);
                }
            }
            strcat(log_str, ".\n");
        }

        // writing log entry into file
        FILE* f = baseStation->log_file;
        if (f != NULL) {
            // ensure only one entry is added at a time
            pthread_mutex_lock(&log_file_mutex);
            fprintf(f, "%s", log_str);
            pthread_mutex_unlock(&log_file_mutex);
        }

        // send response of all available charging nodes, 8 is the maximum
        // possible charging nodes at a distance of exactly 2 from any node.
        MPI_Send(nearest_available_nodes, 8, MPI_INT, mpi_dest, 0,
                 MPI_COMM_WORLD);
        atomic_store(a->has_ended, true);
        return NULL;
    }

    // changing station status update message
    else if (a->mpi_flag == MPI_UPDATE_TAG) {
        // update current database
        pthread_mutex_lock(&availability_mutex);
        baseStation->is_node_available[report.nodeRank] = true;
        pthread_mutex_unlock(&availability_mutex);

        // formatting log string
        char log_str[150]; // string buffer size
        snprintf(log_str, sizeof(log_str),
                 "%.2f: Node %d is Availabile: %d Empty Spots.\n",
                 report.timeReceived, report.nodeRank,
                 report.special_arguement);

        // writing log entry into file
        FILE* f = baseStation->log_file;
        if (f != NULL) {
            // ensure only one entry is added at a time
            pthread_mutex_lock(&log_file_mutex);
            fprintf(f, "%s", log_str);
            pthread_mutex_unlock(&log_file_mutex);
        }

        atomic_store(a->has_ended, true);
        return NULL;

        // charging station is full but some neighbouring nodes aren't
    } else if (a->mpi_flag == MPI_LOW_AVAIL_TAG) {
        // update current database
        pthread_mutex_lock(&availability_mutex);
        baseStation->is_node_available[report.nodeRank] = false;
        pthread_mutex_unlock(&availability_mutex);

        // formatting log string
        char log_str[150]; // string buffer size
        snprintf(log_str, sizeof(log_str),
                 "%.2f: Node %d is full; Nearest Neighbouring Node: %d.\n",
                 report.timeReceived, report.nodeRank,
                 report.special_arguement);

        // writing log entry into file
        FILE* f = baseStation->log_file;
        if (f != NULL) {
            // ensure only one entry is added at a time
            pthread_mutex_lock(&log_file_mutex);
            fprintf(f, "%s", log_str);
            pthread_mutex_unlock(&log_file_mutex);
        }

        atomic_store(a->has_ended, true);
        return NULL;
    }
}
