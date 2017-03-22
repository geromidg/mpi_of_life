/**
  * @file utils.c
  * @brief Provides utilities to assist in the implementation of Game of Life.
  *
  * @author Dimitris Geromichalos <geromidg@gmail.com>
  * @date December, 2016
  */

/******************************** Inclusions *********************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>

#include "utils.h"

/************************ Static Function Prototypes *************************/

/**
  * @brief Adds a number to a board's index in one dimension.
  * @details Given the board's dimension, a check for overflow is done.
  * @param i The board's index.
  * @param a The integer to be added.
  * @param N The board's dimension.
  * @return The new index.
  */
static int add_index(int i, int a, int N);

/**
  * @brief Calculates the next state of a board's cell.
  * @details Following the game's rules, a cell's neighbors are counted
             in order to determine its next value.
  * @param board The game's board.
  * @param i The index of the cell in the first dimension.
  * @param j The index of the cell in the second dimension.
  * @param rows The board's number of rows.
  * @param cols The board's number of columns.
  * @return The new value of the cell.
  */
static char get_next_state(char* board, int i, int j, int rows, int cols);

/***************************** Static Functions ******************************/

int add_index(int i, int a, int N)
{
    int new_i = i + a;

    if (new_i < 0)
        new_i += N;
    else if (new_i >= N) 
        new_i -= N;
    
    return new_i;
}

char get_next_state(char* board, int i, int j, int rows, int cols)
{
    int k, l;
    char next_state;
    int count = 0;

    for (k = -1; k <= 1; k++)
        for (l = -1; l <= 1; l++)
            if ((k || l) && board[(add_index(i, k, rows) * cols + add_index(j, l, cols))])
                count++;

    if (count == 2)
        next_state = board[i * cols + j];
    else if (count == 3)
        next_state = 1;
    else
        next_state = 0;

    return next_state;
}

/******************************* Public Functions ****************************/

void evolve_board(char* board, char* temp_board, int rows, int cols)
{
    int i, j;

    #pragma omp parallel for private(i, j) if((rows * rows) > cols)
    for (i = 0; i < rows; i++)
        for (j = 0; j < cols; j++)
            temp_board[i * cols + j] = get_next_state(board, i, j, rows, cols);

    memcpy(board, temp_board, sizeof(char) * (rows * cols));
}

void generate_board(char* board, int size)
{
    int i;

    srand(time(NULL));

    for (i = 0; i < size; i++)
        board[i] = ((float)rand() / (float)RAND_MAX) < 0.5;
}
