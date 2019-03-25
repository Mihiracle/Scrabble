
#include "Player.h"
#include "Exceptions.h"
#include "Util.h"

Player::Player(std::string const & name, size_t maxTiles):
_name(name),
_maxTiles(maxTiles),
_points(0),
_hand()
{

}

// Player::Player(size_t maxTiles):
// _maxTiles(maxTiles)
// {

// }

Player::~Player()
{
	// delete any remaining tiles
	for(std::multimap<char, Tile *>::const_iterator tileIter = _hand.cbegin(); tileIter != _hand.cend(); ++tileIter)
	{
		delete tileIter->second;
	}
}
void Player::addPoints(unsigned int points)
{
	_points += points;
}

void Player::subtractPoints(unsigned int pointsToSubtract)
{
	if(pointsToSubtract > _points)
	{
		_points = 0;
	}
	else
	{
		_points -= pointsToSubtract;
	}
}

std::string Player::getName() const
{
	return _name;
}

unsigned int Player::getPoints() const
{
	return _points;
}

size_t Player::getNumTiles() const
{
	return _hand.size();
}

size_t Player::getMaxTiles() const
{
	return _maxTiles;
}

unsigned int Player::remainingPoints() const
{
	unsigned int remainingPoints = 0;

	for(std::multimap<char, Tile *>::const_iterator tileIter = _hand.cbegin(); tileIter != _hand.cend(); ++tileIter)
	{
		remainingPoints += tileIter->second->getPoints();
	}

	return remainingPoints;
}

std::set<Tile*> Player::getHandTiles() const
{
	std::set<Tile *> tileSet;

	// add elements from multimap to set
	for(std::multimap<char, Tile *>::const_iterator tileIter = _hand.cbegin(); tileIter != _hand.cend(); ++tileIter)
	{
		tileSet.insert(tileIter->second);
	}

	return tileSet;
}

bool Player::hasTiles(std::string const & move, bool resolveBlanks) const
{

	std::string uppercaseMove = move;
	makeUppercase(uppercaseMove);

	for(size_t charIndex = 0; charIndex < uppercaseMove.length(); ++charIndex)
	{
		if(uppercaseMove[charIndex] == '?' && resolveBlanks)
		{
			ConstHandSearchResult blankSearchResult = _hand.equal_range('?');

			if(blankSearchResult.first == blankSearchResult.second)
			{
				return false;
			}

			// skip next character
			++charIndex;
		}
		else
		{
			ConstHandSearchResult charSearchResult = _hand.equal_range(uppercaseMove[charIndex]);

			if(charSearchResult.first == charSearchResult.second)
			{
				return false;
			}

		}

	}

	return true;

}

std::vector<Tile *> Player::takeTiles(std::string const & move, bool resolveBlanks)
{
	// before we make any modifications, check that it is actually possible
	// to make the given move.

	std::string uppercaseMove = move;
	makeUppercase(uppercaseMove);

	if(!hasTiles(uppercaseMove, resolveBlanks))
	{
		throw MoveException("WRONGTILES");
	}

	std::vector<Tile *> tilesRequested;

	for(size_t charIndex = 0; charIndex < uppercaseMove.length(); ++charIndex)
	{
		if(uppercaseMove[charIndex] == '?' && resolveBlanks)
		{
			HandSearchResult blankSearchResult = _hand.equal_range('?');

			++charIndex;

			// remove the blank tile and assign a letter to it
			Tile * blankTile = blankSearchResult.first->second;
			_hand.erase(blankSearchResult.first);
			blankTile->useAs(uppercaseMove[charIndex]);

			tilesRequested.push_back(blankTile);

		}
		else
		{
			HandSearchResult charSearchResult = _hand.equal_range(uppercaseMove[charIndex]);

			Tile * charTile = charSearchResult.first->second;
			_hand.erase(charSearchResult.first);
			tilesRequested.push_back(charTile);
		}

	}

	return tilesRequested;

}

void Player::addTiles(std::vector<Tile *> const & tilesToAdd)
{
	if(_hand.size() + tilesToAdd.size() > _maxTiles)
	{
		throw MoveException("TILELIMIT");
	}

	for(std::vector<Tile *>::const_iterator newTileIter = tilesToAdd.begin(); newTileIter != tilesToAdd.end(); ++newTileIter)
	{
		Tile * newTile = *newTileIter;
		_hand.emplace(std::toupper(newTile->getLetter()), newTile);
	}
}
