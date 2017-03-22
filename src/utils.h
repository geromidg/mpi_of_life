/**
  * @file utils.h
  * @brief Contains the declarations of functions defined in utils.c.
  *
  * @author Dimitris Geromichalos <geromidg@gmail.com>
  * @date December, 2016
  */

#ifndef UTILS_H
#define UTILS_H

/***************************** Public Functions ******************************/

/**
  * @brief Generates a new board.
  * @details Randomly initializes the board with 0 or 1 values.
  * @param board The game's board.
  * @param size The board's size.
  * @return Void.
  */
void generate_board(char* board, int size);

/**
  * @brief Evolves a board.
  * @details Plays the game for one iteration according to the game's rules.
  * @param board The game's board.
  * @param temp_board A temporary board needed to store the game's next state.
  * @param rows The board's number of rows.
  * @param cols The board's number of columns.
  * @return Void.
  */
void evolve_board(char* board, char* temp_board, int rows, int cols);

#endif  /* UTILS_H */
