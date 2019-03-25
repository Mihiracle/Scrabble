
#include "Move.h"
#include "Util.h"
#include "Exceptions.h"
#include "Board.h"
#include "rang.h"
#include "Tile.h"

#include <sstream>
#include <iostream>
#include <iomanip>

Move* Move::parseMove(std::string moveString, Player &p)
{
	makeUppercase(moveString);
	std::istringstream moveStream(moveString);

	// pull the first word out of the command
	std::string commandWord;
	moveStream >> commandWord;

	if(commandWord == "PASS")
	{
		return new PassMove(&p);
	}
	else if(commandWord == "EXCHANGE")
	{
		// pull tile string out of command
		std::string tilesToExchange;
		moveStream >> tilesToExchange;

		return new ExchangeMove(tilesToExchange, &p);
	}
	else if(commandWord == "PLACE")
	{
		// pull place args out of command
		char direction;
		size_t row;
		size_t col;
		std::string tilesToExchange;

		moveStream >> direction >> row >> col >> tilesToExchange;

		if(!(direction == '-' || direction == '|'))
		{
			throw MoveException("MALFORMED");
		}

		return new PlaceMove(col, row, direction == '-', tilesToExchange, &p);
	}
	else
	{
		// unknown word
		throw MoveException("UNKNOWN");
	}

}

Move::Move(Player *player):
_player(player)
{

}

Move::~Move()
{

}

PassMove::PassMove(Player *player):
Move(player)
{

}

void PassMove::execute(Board & board, Bag & bag, Dictionary & dictionary)
{
	// silence unused parameter warnings
	(void)board;
	(void)bag;
	(void)dictionary;

	// do nothing at all
}

ExchangeMove::ExchangeMove(std::string tileString, Player *p):
Move(p),
_tiles(p->takeTiles(tileString, false))
{
	if(_tiles.empty())
	{
		throw MoveException("EMPTY");
	}
}

void ExchangeMove::execute(Board & board, Bag & bag, Dictionary & dictionary)
{
	// silence unused parameter warnings
	(void)board;
	(void)dictionary;

	std::cout << std::endl << "Throwing back these tiles:" << rang::fg::green << rang::style::bold;
	for(std::vector<Tile *>::const_iterator sequencesIter = _tiles.cbegin(); sequencesIter != _tiles.end(); ++sequencesIter)
	{
		std::cout << ' ' << static_cast<char>(std::toupper((*sequencesIter)->getLetter()));
	}
	std::cout << rang::style::reset << std::endl;

	bag.addTiles(_tiles);
}

PlaceMove::PlaceMove(size_t x, size_t y, bool horizontal, std::string tileString, Player *p):
Move(p),
_x(x),
_y(y),
_horizontal(horizontal),
_tiles(p->takeTiles(tileString, true))
{
	if(_tiles.empty())
	{
		throw MoveException("EMPTY");
	}
}

size_t PlaceMove::x() const
{
	return _x;
}

size_t PlaceMove::y() const
{
	return _y;
}

bool PlaceMove::isHorizontal() const
{
	return _horizontal;
}

std::vector<Tile *> const &PlaceMove::tileVector() const
{
	return _tiles;
}

void PlaceMove::execute(Board & board, Bag & bag, Dictionary & dictionary)
{
	// silence unused parameter warnings
	(void)bag;

	try
	{
		// first, try this move on the board and see what words are generated
		// (may generate exceptions)
		std::vector<std::pair<std::string, unsigned int>> sequencesFormed = board.getPlaceMoveResults(*this);

		// check that each sequence of tiles in in fact a word
		for(size_t sequenceIndex = 0; sequenceIndex < sequencesFormed.size(); ++sequenceIndex)
		{
			if(!dictionary.isLegalWord(sequencesFormed[sequenceIndex].first))
			{
				throw MoveException("INVALIDWORD:" + sequencesFormed[sequenceIndex].first);
			}
		}

		// print the words that would be created, and add up scores
		unsigned int totalScore = 0;
		std::cout << std::endl << "This move creates these words:";
		for(size_t sequenceIndex = 0; sequenceIndex < sequencesFormed.size(); ++sequenceIndex)
		{
			std::cout << ' ' << rang::fg::cyan << rang::style::bold << sequencesFormed[sequenceIndex].first << rang::style::reset;
			std::cout << " (" << rang::fgB::magenta << rang::style::bold << std::setw(0) << sequencesFormed[sequenceIndex].second << rang::style::reset << ')';

			totalScore += sequencesFormed[sequenceIndex].second;
		}
		std::cout << std::endl;

		// now that we're sure it's going to work,
		// we can execute the move!
		board.executePlaceMove(*this);
		_player->addPoints(totalScore);

		if(_tiles.size() == _player->getMaxTiles())
		{
			std::cout << "You used all tiles in your hand, earning a 50-point bonus!" << std::endl;
			_player->addPoints(50);
		}

	}
	catch(MoveException & moveException)
	{
		// the move execution failed, but the tiles in _tiles have still been removed from
		// the player's hand.  So, we need to put them back.
		// note to self: add unit tests for this mistake

		_player->addTiles(_tiles);

		// now, you can get on with your exception
		throw moveException;
	}
}
