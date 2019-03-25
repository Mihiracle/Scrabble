
/* The class Board encodes the state of the Scrabble board.
   It provides functions for displaying the board,
   and for returning the words (and score) that would be obtained by a move.
*/

#ifndef BOARD_H_
#define BOARD_H_

#include <string>
#include <set>
#include <map>
#include <vector>
#include "Tile.h"
#include "Square.h"
#include "Move.h"
#include "Dictionary.h"

class Board
{

	std::vector<std::vector<Square *>> _squares;

	size_t _numRows;
	size_t _numColumns;

	// true if no other tiles have been placed yet
	bool _isFirstMove;

	// minimum length for a sequence of character to be considered a word
	const size_t minWordLength = 2;

	// stores a set of square coordinates (0-indexed)
	struct SquareCoords
	{
		size_t row;
		size_t col;

		SquareCoords(size_t _row, size_t _col): row(_row), col(_col) {}

		// default equality operator
		bool operator==(SquareCoords const & other)
		{
			return row == other.row && col == other.col;
		}
	};

	// Used when you want to iterate over the squares on a board that a word being placed will occupy.
	// Pass in the start square and direction, the direction, and how long the word is.
	// Will return a list of coordinates that the letters in the word will be placed on, skipping over any squares that are already occupied
	// Will throw a MoveException("OUTOFBOUNDS") if the word goes out of bounds of the board.
	// Assumes that startSquare is in bounds.
	std::vector<SquareCoords> getWordSquares(SquareCoords startSquare, size_t wordLength, bool horizontal) const;

	// Investigate a word that runs normal (geometry term for "at a 90 degree angle") to the main word that is being placed.
	// Returns a pair of the letters in the word as a string, and the points earned by the word.
	std::pair<std::string, unsigned int> getNormalWord(SquareCoords placedTileCoords, Tile * placedTile, bool normalIsHorizontal) const;

	// If you are placing a letter at firstLetterCoords, this function looks up or left to
	// find the coordinates of the actual first square in the sequence of letters that the letter is placed in.
	SquareCoords findStartOfSequence(SquareCoords firstLetterCoords, bool horizontal) const;

	// overload to get a square by SquareCoords
	Square * getSquare(SquareCoords const & coords) const;

public:
	/* constructor that initializes the board state with an empty board
           and the size and bonuses and start square based on the given file.

	   If the given file cannot be opened, will throw a FileException
	   with message "BOARD".
	   If the file contains unexpected characters, will throw a FileException
	   with message "BOARDCONTENT".
	*/
	Board (std::string board_file_namey);

	~Board ();

	/* returns a vector of all words that would be formed by executing the
           given move. The first element of the pair is a string containing the word
           formed, and the second element is the score that that word earns
           (with all multipliers, but not the 50-point bonus for all letters).

       Words returned are all in uppercase.

	   The last entry of the vector is always the "main" word formed
	   in the direction chosen by the user; the others could be in arbitrary
	   order. (This is helpful for backtracking search.)

	   This function does not check that the words formed are actually in the dictionary.
	   The words returned from this function must be checked against the dictionary to
	   determine if the move is legal.

	   This function throws MoveException objects to signal illegal moves.
	   Specifically, the following messages could be included in the
	   MoveException:
		- "OUTOFBOUNDS": One or more tile would have to be placed out of bounds of the board.
		- "OCCUPIED": The proposed starting square was occupied by a tile already.
		- "NOSTART": During the very first move, the player did not use the start square.
		- "NOWORDS": The player did not produce any possible words (placed one letter as the first move, or no letters on a subsequent move).
		- "NONEIGHBOR": The proposed move is not adjacent to any previously placed tile.
	*/
	std::vector<std::pair<std::string, unsigned int>> getPlaceMoveResults(const PlaceMove &m) const;

	/* Executes the given move by taking tiles and placing them on the board.
	   This function does not check for correctness of the move, so could
	   segfault or cause other errors if called for an incorrect move.
	   When a blank tile '?' is placed on the board, it is treated as a letter,
	   i.e., the corresponding square has that letter (with score 0) placed on it.
	*/
	void executePlaceMove (const PlaceMove & m);

	/* returns a map, giving for each character (letter or '?')
	   how many copies of that letter have been placed by players
	   on the board so far.
	*/
	std::map<char,int> getTileCount () const;

	/* Returns a pointer to the Square object representing the
       (y,x) position of the board. Indexing starts at 1 here.
	   This is needed only to display the board. */
	Square * getSquare (size_t x, size_t y) const;

	/* Returns the number of rows of the board.*/
	size_t getRows() const;

	/* Returns the number of columns of the board.*/
	size_t getColumns() const;

};


#endif /* BOARD_H_ */
