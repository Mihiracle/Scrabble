//
// Board class implementation 
//

#include "Board.h"

#include "Exceptions.h"
#include "Util.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::vector<Board::SquareCoords> Board::getWordSquares(Board::SquareCoords startSquare, size_t wordLength, bool horizontal) const
{

	std::vector<SquareCoords> letterCoords;

	

	SquareCoords currCoords(startSquare);

	for(int letterIndex = 0; letterIndex < static_cast<int>(wordLength); ++letterIndex)
	{
		if(getSquare(currCoords)->isOccupied())
		{
			// skip this square and move to the next one
			// note: should make test to see if squares get overwritten from this
			--letterIndex;
		}
		else
		{
			letterCoords.push_back(currCoords);
		}

		if(horizontal)
		{
			++currCoords.col;

			if(currCoords.col >= _numColumns && letterIndex < static_cast<int>(wordLength) - 1)
			{
				throw MoveException("OUTOFBOUNDS");
			}
		}
		else
		{
			++currCoords.row;

			if(currCoords.row >= _numRows && letterIndex < static_cast<int>(wordLength) - 1)
			{
				throw MoveException("OUTOFBOUNDS");
			}
		}
	}

	return letterCoords;
}

std::pair<std::string, unsigned int> Board::getNormalWord(Board::SquareCoords placedTileCoords, Tile *placedTile, bool normalIsHorizontal) const
{
	std::stringstream word;
	unsigned int wordPoints = 0;
	unsigned int wordMultiplier = 1;

	SquareCoords currCoords = findStartOfSequence(placedTileCoords, normalIsHorizontal);

	while(currCoords.row < _numRows && currCoords.col < _numColumns)
	{
		if(getSquare(currCoords)->isOccupied())
		{
			//note: make test that squares with used ? tiles can be used as part of subsequent words
			word << getSquare(currCoords)->getLetter();

			// take only the points from the letter itself
			wordPoints += getSquare(currCoords)->getScore();
		}
		else if(currCoords == placedTileCoords)
		{
			word << placedTile->getUse();

			// take the points from the letter and its multipliers
			wordPoints += placedTile->getPoints() * getSquare(currCoords)->getLMult();
			wordMultiplier *= getSquare(currCoords)->getWMult();
		}
		else
		{
			// end of word
			break;
		}

		if(normalIsHorizontal)
		{
			++currCoords.col;
		}
		else
		{
			++currCoords.row;
		}
	}

	std::string wordUppercase = word.str();
	makeUppercase(wordUppercase);


	return std::make_pair(wordUppercase, wordPoints * wordMultiplier);
}

Board::SquareCoords Board::findStartOfSequence(Board::SquareCoords firstLetterCoords, bool horizontal) const
{
	SquareCoords wordStartCoords = firstLetterCoords;
	while(wordStartCoords.row > 0 && wordStartCoords.col > 0)
	{
		SquareCoords prevSquareCoords(wordStartCoords);
		if(horizontal)
		{
			prevSquareCoords.col -= 1;
		}
		else
		{
			prevSquareCoords.row -= 1;
		}

		// prev square is occupied, look there for the start of the word
		if(getSquare(prevSquareCoords)->isOccupied())
		{
			wordStartCoords = prevSquareCoords;
		}
		else
		{
			break;
		}
	}

	return wordStartCoords;
}

Square* Board::getSquare(const Board::SquareCoords &coords) const
{
	return _squares[coords.row][coords.col];
}

Board::Board(std::string board_file_name):
_squares(),
_numRows(),
_numColumns(),
_isFirstMove(false)
{
	std::ifstream boardFileStream(board_file_name);

	if(!boardFileStream)
	{
		throw FileException("BOARD");
	}

	size_t lineIndex = 0;

	size_t startRow = 1;
	size_t startCol = 1;

	while(!boardFileStream.eof())
	{

		if(lineIndex == 0)
		{
			boardFileStream >> _numColumns;
			boardFileStream >> _numRows;
		}
		else if(lineIndex == 1)
		{
			boardFileStream >> startCol;
			boardFileStream >> startRow;

			// skip until the start of the next line
			std::string dummy;
			std::getline(boardFileStream, dummy);
		}
		else
		{
			std::string row;
			std::getline(boardFileStream, row);

			// if the file ends with a newline, we will get a final "phantom" 0-length string
			// so here we expressly ix-nay that.
			if(boardFileStream.eof() && row.empty())
			{
				break;
			}

			
			if(row.size() != _numColumns)
			{
				throw FileException("BOARDCONTENT");
			}

			// add another row vector
			_squares.emplace_back();

			for(size_t colIndex = 0; colIndex < _numColumns; ++colIndex)
			{
				char squareChar = row[colIndex];

				unsigned int letterMultiplier = 1;
				unsigned int wordMultiplier = 1;

				switch(squareChar)
				{
				case '.':
					break;

				case '2':
					letterMultiplier = 2;
					break;

				case '3':
					letterMultiplier = 3;
					break;

				case 'd':
					wordMultiplier = 2;
					break;

				case 't':
					wordMultiplier = 3;
					break;

				default: // unknown character
					throw FileException("BOARDCONTENT");
				}

				// line index 2 is row 1, col index 0 is col 1
				bool isStart = ((lineIndex - 1) == startRow) && ((colIndex + 1) == startCol);

				Square * newSquare = new Square(letterMultiplier, wordMultiplier, isStart);
				_squares[lineIndex - 2].push_back(newSquare);
			}
		}

		++lineIndex;
	}
}

Board::~Board()
{
	// delete all squares
	for(std::vector<std::vector<Square *>>::iterator rowIter = _squares.begin(); rowIter != _squares.end(); ++rowIter)
	{
		for(std::vector<Square *>::iterator colIter = rowIter->begin(); colIter != rowIter->end(); ++colIter)
		{
			delete *colIter;
		}
	}
}

std::vector<std::pair<std::string, unsigned int>> Board::getPlaceMoveResults(const PlaceMove &m) const
{
	// check start square
	// --------------------------------------------------------
	if((m.x() <= 0 || m.x() > _numColumns) || (m.y() <= 0 || m.y() > _numRows))
	{
		throw MoveException("OUTOFBOUNDS");
	}

	size_t startRow = m.y() - 1;
	size_t startCol = m.x() - 1;
	Square * firstSquare = _squares[startRow][startCol];

	if(firstSquare->isOccupied())
	{
		throw MoveException("OCCUPIED");
	}

	if(_isFirstMove && !firstSquare->isStart())
	{
		throw MoveException("NOSTART");
	}

	// iterate through letters and look at tangential words
	// --------------------------------------------------------

	std::vector<std::pair<std::string, unsigned int>> wordsFormed;
	bool hasAdjacentTile = false;

	// get the coordinates on the board and check each letter (also checks if word fits in bounds)
	SquareCoords firstSquareCoords(startRow, startCol);
	std::vector<SquareCoords> letterCoords = getWordSquares(firstSquareCoords, m.tileVector().size(), m.isHorizontal());

	for(size_t letterIndex = 0; letterIndex < m.tileVector().size(); ++letterIndex)
	{
		SquareCoords letterCoord = letterCoords[letterIndex];
		std::pair<std::string, unsigned int> tangentialWord = getNormalWord(letterCoord, m.tileVector()[letterIndex], !m.isHorizontal());
		if(tangentialWord.first.size() >= minWordLength)
		{
			// it has more than 1 letters, it's a word
			wordsFormed.push_back(tangentialWord);
		}

		// check if at least one adjacent square is occupied
		if(letterCoord.col > 0)
		{
			hasAdjacentTile |= _squares[letterCoord.row][letterCoord.col - 1]->isOccupied();
		}
		if(letterCoord.col < _numColumns - 1)
		{
			hasAdjacentTile |= _squares[letterCoord.row][letterCoord.col + 1]->isOccupied();
		}
		if(letterCoord.row > 0)
		{
			hasAdjacentTile |= _squares[letterCoord.row - 1][letterCoord.col]->isOccupied();
		}
		if(letterCoord.row < _numRows - 1)
		{
			hasAdjacentTile |= _squares[letterCoord.row + 1][letterCoord.col]->isOccupied();
		}

		hasAdjacentTile |= getSquare(letterCoord)->isStart();
	}

	// throw if there was not one adjacent tile along the whole word
	if(!hasAdjacentTile)
	{
		throw MoveException("NONEIGHBOR");
	}

	// now look at main word
	// --------------------------------------------------------

	// look backwards to figure out first square of word
	SquareCoords mainWordStartCoords = findStartOfSequence(firstSquareCoords, m.isHorizontal());

	std::stringstream mainWord;
	unsigned int mainWordPoints = 0;
	unsigned int mainWordMultiplier = 1;
	SquareCoords wordCurrCoords(mainWordStartCoords);
	size_t indexInLetters = 0;

	// add up word score
	while(wordCurrCoords.row < _numRows && wordCurrCoords.col < _numColumns)
	{
		if(getSquare(wordCurrCoords)->isOccupied())
		{
			mainWord << getSquare(wordCurrCoords)->getLetter();

			// take only the points from the letter itself
			mainWordPoints += getSquare(wordCurrCoords)->getScore();
		}
		else if(indexInLetters < m.tileVector().size())
		{
			Tile * tileToBePlaced = m.tileVector()[indexInLetters];
			mainWord << tileToBePlaced->getUse();

			// take the points from the letter and its multipliers
			mainWordPoints += tileToBePlaced->getPoints() * getSquare(wordCurrCoords)->getLMult();
			mainWordMultiplier *= getSquare(wordCurrCoords)->getWMult();

			++indexInLetters;
		}
		else
		{
			// end of word
			break;
		}

		if(m.isHorizontal())
		{
			++wordCurrCoords.col;
		}
		else
		{
			++wordCurrCoords.row;
		}
	}

	if(mainWord.str().length() >= minWordLength)
	{
		std::string mainWordUppercase = mainWord.str();
		makeUppercase(mainWordUppercase);
		wordsFormed.emplace_back(mainWordUppercase, mainWordPoints * mainWordMultiplier);
	}

	if(wordsFormed.empty())
	{
		throw MoveException("NOWORDS");
	}

	return wordsFormed;

}

void Board::executePlaceMove(const PlaceMove &m)
{

	std::vector<SquareCoords> letterCoords = getWordSquares(SquareCoords(m.y() - 1, m.x() - 1), m.tileVector().size(), m.isHorizontal());

	for(size_t letterIndex = 0; letterIndex < m.tileVector().size(); ++letterIndex)
	{
		getSquare(letterCoords[letterIndex])->placeTile(m.tileVector()[letterIndex]);
	}
}

std::map<char,int> Board::getTileCount() const
{
	std::map<char, int> tileCounts;

	for(size_t row = 0; row < _numRows; ++row)
	{
		for(size_t col = 0; col < _numColumns; ++col)
		{
			Square * currSquare = _squares[row][col];
			if(currSquare->isOccupied())
			{
				++tileCounts[currSquare->getLetter()];
			}
		}
	}

	return tileCounts;
}

Square* Board::getSquare(size_t x, size_t y) const
{
	return _squares.at(y - 1).at(x - 1);
}

size_t Board::getRows() const
{
	return _numRows;
}

size_t Board::getColumns() const
{
	return _numColumns;
}

