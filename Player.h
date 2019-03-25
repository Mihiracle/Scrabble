
/* Encodes the state of a player, mostly the name, tiles, points. 
   The main non-trivial ability of this class is to produce a vector
   of Tile* from a string describing tiles to place/exchange. */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <string>
#include <set>
#include <map>
#include <vector>

#include "Tile.h"


class Player
{
	
	const std::string _name;
	const size_t _maxTiles;
	unsigned int _points;

	// stores the player's tiles indexed by character
	std::multimap<char, Tile *> _hand;

	
	typedef std::pair<std::multimap<char, Tile *>::iterator, std::multimap<char, Tile *>::iterator> HandSearchResult;
	typedef std::pair<std::multimap<char, Tile *>::const_iterator, std::multimap<char, Tile *>::const_iterator> ConstHandSearchResult;


public:
	/* Constructor giving the player the given name, and setting their points to 0.
	   Does not give the player any tiles.
	*/
	Player (std::string const & name, size_t maxTiles);


	/* Destructor for a player. Deletes all the tiles the player still has. */
	~Player ();

	/* Increments the player's score by the given amount. */
	void addPoints (unsigned int points);

	/* Decrements the player's score by the given amount.
	   If this subtraction would cause the player's score to go below 0,
	   the player's score is set to 0.
   	*/
	void subtractPoints(unsigned int pointsToSubtract);

	/* Returns the player's name. */
	std::string getName () const;

	/* Returns the player's current score. */
	unsigned int getPoints () const;

	/* Returns the number of tiles the player currently has. */
	size_t getNumTiles () const;

	/* Returns the maxiumum amount of tiles that the player may hold. */
	size_t getMaxTiles() const;

	/* Returns the total value of all tiles the player still has.
	   Used in the final subtraction of the game. */
	unsigned int remainingPoints () const;

	/* Returns the set of tiles the player currently holds. */
	std::set<Tile*> getHandTiles() const;


	/* Reads a move string and confirms that the player has the tiles necessary to
	   execute this move.

	   The move string is case-insensitive.

	   The boolean resolveBlanks states whether this is a PLACE or EXCHANGE move.
	   If resolveBlanks is true, then when the string contains a '?',
	   the next letter is interpreted as the letter to use the blank as.

	   By definition, if this function returns true, then takeTiles() would
	   succeed without throwing any exceptions.
	 */

	bool hasTiles(std::string const & move, bool resolveBlanks) const;


	/* Reads a move string, finds the corresponding tiles in the player's hand, and
	   removes and returns those tiles in the order they appear in the move string.

	   The move string is case-insensitive.

	   The boolean resolveBlanks states whether this is a PLACE or EXCHANGE move.
	   If resolveBlanks is true, then when the string contains a '?',
	   the next letter is interpreted as the letter to use the blank as,
	   and the "use" field of the tile is set accordingly.

	   This function may throw a MoveException with message "WRONGTILES"
	   if the player does not have the tiles described by move.

	   The move string is assumed to have correct syntax.
	*/
	std::vector<Tile*> takeTiles (std::string const & move, bool resolveBlanks);

	/* Adds all the tiles in the vector to the player's hand.

	   If adding the given tiles would push the player over
	   the maximum tile limit, a MoveException with message
	   "TILELIMIT" is thrown.  */
	void addTiles (std::vector<Tile*> const & tilesToAdd);


};


#endif /* PLAYER_H_ */
