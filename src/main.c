#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>
#include <omp.h>

#include "utils.h"

/* OpenMP related */
#define OMP_THREADS (8u)

/* MPI related */
#define NUM_PROCESSES (4u)
#define MASTER (0u)

/* Game related */
#define GAME_ITERATIONS (3u)
#define DIMENSION (400u)

/* Board sizes */
#define TOTAL_ROWS (2u * (DIMENSION))
#define TOTAL_COLS (2u * (DIMENSION))
#define TOTAL_SIZE ((TOTAL_ROWS) * (TOTAL_COLS))
#define SQUARE_SIZE ((TOTAL_SIZE) / (NUM_PROCESSES))

void play_game(int pid, char* main_board)
{
    int i;
    int tag;

    MPI_Request request[2];
    MPI_Status status[2];

    /* Neighbor process ids */
    int previous_process = (pid == MASTER) ? (NUM_PROCESSES - 1) : (pid - 1);
    int next_process = (pid == NUM_PROCESSES - 1) ? 0 : (pid + 1);

    /* Board arrays */
    char* sub_board = malloc(sizeof(*sub_board) * SQUARE_SIZE);
    char* temp_sub_board = malloc(sizeof(*temp_sub_board) * (SQUARE_SIZE));
    char* top_combined_rows = malloc(sizeof(*top_combined_rows) * (3 * TOTAL_COLS));
    char* bottom_combined_rows = malloc(sizeof(*bottom_combined_rows) * (3 * TOTAL_COLS));
    char* temp_combined_rows = malloc(sizeof(*temp_combined_rows) * (3 * TOTAL_COLS));

    /* Validate memory allocations */
    if ((sub_board == NULL) || (temp_sub_board == NULL) || (top_combined_rows == NULL) ||
        (bottom_combined_rows == NULL) || (temp_combined_rows == NULL))
    {
        printf("Error while allocating space!\n");
        exit(-1);
    }

    /* Distribute board to processes */
    MPI_Scatter(main_board, SQUARE_SIZE, MPI_CHAR, sub_board, SQUARE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);

    /* Play game! */
    for (i = 0; i < GAME_ITERATIONS; i++)
    {
        /* Send subboard's data to neighbors */
        MPI_Isend(&sub_board[SQUARE_SIZE - TOTAL_COLS], TOTAL_COLS,
            MPI_CHAR, next_process, tag, MPI_COMM_WORLD, &request[0]);
        MPI_Isend(&sub_board[0], TOTAL_COLS,
            MPI_CHAR, previous_process, tag, MPI_COMM_WORLD, &request[1]);

        /* Play using current data for latency hiding */
        memcpy(&top_combined_rows[1 * TOTAL_COLS], &sub_board[0 * TOTAL_COLS],
            sizeof(char) * (2 * TOTAL_COLS));
        memcpy(&bottom_combined_rows[0 * TOTAL_COLS], &sub_board[SQUARE_SIZE - (2 * TOTAL_COLS)],
            sizeof(char) * (2 * TOTAL_COLS));

        evolve_board(sub_board, temp_sub_board, TOTAL_ROWS / 4, TOTAL_COLS);

        /* Receive the neighbor's data and finish playing */
        MPI_Recv(&top_combined_rows[0 * TOTAL_COLS], TOTAL_COLS,
            MPI_CHAR, previous_process, tag, MPI_COMM_WORLD, &status[0]);
        MPI_Recv(&bottom_combined_rows[2 * TOTAL_COLS], TOTAL_COLS,
            MPI_CHAR, next_process, tag, MPI_COMM_WORLD, &status[1]);

        evolve_board(top_combined_rows, temp_combined_rows, 3, TOTAL_COLS);
        evolve_board(bottom_combined_rows, temp_combined_rows, 3, TOTAL_COLS);

        /* Align edge rows if there are many processes */
        memcpy(&sub_board[0 * TOTAL_COLS], &top_combined_rows[1 * TOTAL_COLS],
            sizeof(char) * (1 * TOTAL_COLS));
        memcpy(&sub_board[SQUARE_SIZE - (1 * TOTAL_COLS)], &bottom_combined_rows[1 * TOTAL_COLS],
            sizeof(char) * (1 * TOTAL_COLS));
    }

    /* Collect subboards from processes to form the final board */
    MPI_Gather(sub_board, SQUARE_SIZE, MPI_CHAR, main_board, SQUARE_SIZE, MPI_CHAR, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv)
{
    int pid;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

    /* Allocate main board */
    char* main_board = malloc(sizeof(*main_board) * TOTAL_SIZE);

    if (main_board == NULL)
    {
        printf("Error while allocating space!\n");
        exit(-1);
    }

    /* Initialize OpenMP */
    omp_set_num_threads(OMP_THREADS);

    /* Initialize board */
    if (pid == MASTER)
    	generate_board(main_board, TOTAL_SIZE);

    play_game(pid, main_board);

    /* MPI done! */
    MPI_Finalize();

    return 0;
}
