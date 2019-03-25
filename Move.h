
/* The Move class family encodes a move made by one player, including its type
   (PASS, EXCHANGE, PLACE), the tiles used (and use for blanks),
   start square, and direction (if needed).
   Call Move::parseMove() to turn an entered string into its subclass of
   Move, then call apply() on that returned move to have it execute.*/

#ifndef MOVE_H_
#define MOVE_H_

#include <vector>
#include <string>
#include "Tile.h"
#include "Player.h"
#include "Bag.h"
#include "Dictionary.h"

// forward declaration to prevent circular includes
class Board;

class Move
{
	

public:


	/* Parses the given move m, and constructs the corresponding move subclass.
	   m is assumed to be in the format described on the assignment sheet for a move command.
	   The player passed in is used to verify that the proposed tiles are
	   in fact in the player's hand.
	   It can handle all three types of move (PASS, EXCHANGE, PLACE).
	   Coordinates start with 1.
	   Can throw MoveException, with the following messages:
	   - "EMPTY": The player proposed to place/exchange 0 tiles.
	   - "MALFORMED": Format error in expression, e.g., missing direction or garbage characters.
	   - "UNKNOWN": Unknown type of command (other than PASS, EXCHANGE, PLACE)
	   - "WRONGTILES": The player did not have all of the proposed tiles.
	*/
	static Move * parseMove(std::string moveString, Player &p);


	/* Returns true iff the move is a PASS move */
	virtual bool isPass () const
	{
		return false;
	}

	/* Returns true iff the move is an EXCHANGE move */
	virtual bool isExchange () const
	{
		return false;
	}

	/* Returns true iff the move is a PLACE move */
	virtual bool isWord () const
	{
		return false;
	}

	/* Executes this move, whichever type it is.
	   This may throw exceptions; students: it's up to you to
	   decide (and document) what exactly it throws*/
	virtual void execute(Board & board, Bag & bag, Dictionary & dictionary) = 0;

	virtual ~Move();

protected:

	Player * _player;

	// constructor -- only accessible to subclasses
	Move(Player * player);
};

// Represents a pass move, where the player takes no action
class PassMove : public Move
{
	

public:

	/* Constructs a pass move. */
	PassMove(Player * player);

	/* Returns true iff the move is a PASS move */
	bool isPass () const
	{
		return true;
	}

	/* Executes this move, whichever type it is.
	   This may throw exceptions; students: it's up to you to
	   decide (and document) what exactly it throws*/
	void execute(Board & board, Bag & bag, Dictionary & dictionary);
};

// represents an exchange move, were a player replaces certain tiles
class ExchangeMove : public Move
{
	

	std::vector<Tile*> _tiles;

public:
	/* Creates an EXCHANGE move, exchanging the tiles listed in the
	   string (formatted according to the EXCHANGE command description)
	   with new tiles from the bag.

	   Can throw MoveException, with the following messages:
	   - "EMPTY": The player proposed to exchange 0 tiles.
	   - "WRONGTILES": The player did not have all of the proposed tiles.
	   */
	ExchangeMove(std::string tileString, Player * p);

	/* Returns true iff the move is an EXCHANGE move */
	bool isExchange () const
	{
		return true;
	}

	/* Executes this move, whichever type it is.
	   This may throw exceptions; students: it's up to you to
	   decide (and document) what exactly it throws*/
	void execute(Board & board, Bag & bag, Dictionary & dictionary);
};

// represents a place move, where a player places one or more tiles onto the board.
class PlaceMove : public Move
{
	

	size_t _x, _y;
	bool _horizontal;
	std::vector<Tile *> _tiles;

public:
	/* Creates a PLACE move, starting at row y, column x, placing the tiles
	   described by the string tileString. If "horizontal" is true, then the tiles
	   are placed horizontally, otherwise vertically.
	   Coordinates start with 1.
	   The string m is in the format described in HW4; in particular, a '?'
	   must be followed by the letter it is to be used as.
	   Can throw MoveException, with the following messages:
	   - "EMPTY": The player proposed to place 0 tiles.
	   - "WRONGTILES": The player did not have all of the proposed tiles.

	*/
	PlaceMove (size_t x, size_t y, bool horizontal, std::string tileString, Player * p);

	/* Returns true iff the move is a PLACE move */
	bool isWord () const
	{
		return true;
	}

	/* Returns the value of the x coordinate for a PLACE move.
	   Coordinates start with 1. */
	size_t x () const;

	/* Returns the value of the y coordinate for a PLACE move.
	   Coordinates start with 1. */
	size_t y () const;

	/* Returns the direction (true for horizontal) for a PLACE move. */
	bool isHorizontal () const;

	/* Returns the vector of tiles associated with a PLACE/EXCHANGE move.
	   Return value could be arbitrary for PASS moves. */
	std::vector<Tile*> const & tileVector () const;

	/* Executes this place move.  Places the tiles
	 * onto the game board and updates this player's score.
	 *
	 * Can throw the same exceptions as Board::getPlaceMoveResults(),
	 * plus a new MoveException("INVALIDWORD:<word>"), which indicates that
	 * one of the words formed by the move was not in the dictionary (where the
	 * text after the colon is the invalid word).*/
	void execute(Board & board, Bag & bag, Dictionary & dictionary);
};



#endif /* MOVE_H_ */
