
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>

#include "Board.h"
#include "Bag.h"
#include "Dictionary.h"
#include "ConsolePrinter.h"
#include "Player.h"
#include "ConfigFile.h"
#include "Exceptions.h"
#include "rang.h"
#include "Trie.h"
#include "ScoreAI.h"

// Get a friendly string to print to the user in response to a move exception
std::string getFriendlyError(MoveException const & exception)
{
	std::string exMessage(exception.what());

	// first half of invalid word exception message
	std::string invalidWordExPrefix("INVALIDWORD:");

	if(exMessage == "EMPTY")
	{
		return "Move would not manipulate any tiles!";
	}
	else if(exMessage == "MALFORMED")
	{
		return "Format error in command arguments!";
	}
	else if(exMessage == "UNKNOWN")
	{
		return "Unknown command word";
	}
	else if(exMessage == "WRONGTILES")
	{
		return "You don't have all the tiles you'd need for this move!";
	}
	else if(exMessage == "OUTOFBOUNDS")
	{
		return "Would go out of bounds!";
	}
	else if(exMessage == "OCCUPIED")
	{
		return "Coordinates of first tile are occupied!";
	}
	else if(exMessage == "NOSTART")
	{
		return "This is the first move, you have to use the start square!";
	}
	else if(exMessage == "NOWORDS")
	{
		return "Would not create a sequence of at least 2 letters!";
	}
	else if(exMessage == "NONEIGHBOR")
	{
		return "No tile is adjacent to a previously placed tile!";
	}
	else if(exMessage.substr(0, invalidWordExPrefix.size()) == invalidWordExPrefix)
	{
		// get invalid word out of exception message
		std::string invalidWord = exMessage.substr(invalidWordExPrefix.size(), std::string::npos);

		return "Would form the letter sequence \'" + invalidWord + "\', which is not a word!";
	}
	else
	{
		return "Unknown MoveException: " + exMessage;
	}

}

// Perform the final subtraction, whereby all players lose score equivalent to the
// values of the tiles they hold.  If emptyHandPlayerIndex is not -1, this denotes the player
// who ran out of tiles, and they will receive the total of all deducted points.

unsigned int doFinalSubtraction(std::vector<Player *> & players, ssize_t emptyHandPlayerIndex)
{
	unsigned int totalScore = 0;

	for(std::vector<Player *>::iterator playersIter = players.begin(); playersIter != players.end(); ++playersIter)
	{
		unsigned int remainingPoints = (*playersIter)->remainingPoints();
		totalScore += remainingPoints;
		(*playersIter)->subtractPoints(remainingPoints);
	}

	if(emptyHandPlayerIndex != -1)
	{
		players[emptyHandPlayerIndex]->addPoints(totalScore);
	}

	return totalScore;
}

#define MOVE_PROMPT_COLOR rang::fgB::blue << rang::style::bold
#define PLAYER_NAME_COLOR rang::fgB::red << rang::style::bold
#define TILE_NAME_COLOR rang::fg::green << rang::style::bold
#define SCORE_COLOR rang::fgB::magenta << rang::style::bold

int main(int argc, char** argv)
{
	//-------------------------TESTING--AREA------------------------------------------

	// TrieSet* test = new TrieSet(); 
	// test->insert("apple"); 
	// test->insert("app"); 
	// test->insert("applejacks"); 
	// //test->search("apple"); 
	// std::cout << test->search("apple") << std::endl; 
	// test->remove("apple"); 
	// std::cout << test->search("app") << std::endl; 
	// std::cout << test->search("applejacks") << std::endl; 
	// std::cout << test->search("apple") << std::endl; 


	

	//--------------------------------------------------------------------------------
	if(argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <configuration file>" << std::endl;
		return 3;
	}

	// data for running game
	
	ConfigFile * configFile;
	Board * board;
	Bag * bag;
	Dictionary * dictionary;
	std::vector<Player *> players;


	// attempt to read files and initialize game.
	// -------------------------------------------------------------------------------

	
	try
	{
		configFile = new ConfigFile(argv[1]);

		bag = new Bag(configFile->tilesetFile, configFile->randomSeed);
		dictionary = new Dictionary(configFile->dictionaryFile);
		board = new Board(configFile->boardFile);
	}
	catch(FileException & fileException)
	{
		if(fileException.what() == std::string("MISSINGCONFIG"))
		{
			std::cerr << "Error: config file " << argv[1] << " was not found!" << std::endl;
			return 3;
		}
		else if(fileException.what() == std::string("BAG"))
		{
			std::cerr << "Error: tileset file " << configFile->tilesetFile << " could not be read!" << std::endl;
			return 4;
		}
		else if(fileException.what() == std::string("BOARD"))
		{
			std::cerr << "Error: board file " << configFile->boardFile << " could not be read!" << std::endl;
			return 5;
		}
		else if(fileException.what() == std::string("BOARDCONTENT"))
		{
			std::cerr << "Error: board file " << configFile->boardFile << " contained invalid content!" << std::endl;
			return 5;
		}
		else if(fileException.what() == std::string("DICTIONARY"))
		{
			std::cerr << "Error: dictionary file " << configFile->dictionaryFile << " could not be read!" << std::endl;
			return 6;
		}
		else
		{
			std::cerr << "Unexpected FileException: " << fileException.what() << std::endl;
			return 7;
		}
	}

	// question user about players to add
	// -------------------------------------------------------------------------------
	size_t numPlayers;
	std::cout << "Please enter number of players: ";
	std::cin >> numPlayers;

	// this check is not strictly required but it is in the spec...
	if(numPlayers < 1 || numPlayers > 8)
	{
		std::cerr << "Invalid number of players, must be between 1 and 8." << std::endl;
		return 9;
	}
	std::cout << numPlayers << " players confirmed." << std::endl;

	// read in player names
	// -------------------------------------------------------------------------------

	// clear remaining data on cin from the previous prompt
	std::cin.ignore();

	for(size_t playerNum = 1; playerNum <= numPlayers; ++playerNum)
	{
		std::cout << "Please enter name for player " << playerNum << ": ";
		std::string playerName;
		std::getline(std::cin, playerName);
		std::cout << "Player " << playerNum << ", named \"" << playerName << "\", has been added." << std::endl;

		// create player with a full initial hand
		/*if (playerName == "CPUS") { 
			ScoreAI* tmp = new ScoreAI(playerName, configFile->handSize); 
			players.push_back(tmp); 
		}*/
		players.push_back(new Player(playerName, configFile->handSize));
		players[playerNum - 1]->addTiles(bag->drawTiles(configFile->handSize));
	}

	// run game loop
	// -------------------------------------------------------------------------------

	bool gameWon = false;
	ssize_t emptyHandPlayerIndex;

	size_t sequentialPasses = 0;

	while(!gameWon)
	{
		for(size_t playerNum = 0; (!gameWon && playerNum < numPlayers); ++playerNum)
		{
			ConsolePrinter::printBoard(*board);
			ConsolePrinter::printHand(*players[playerNum]);

			Move * playerMove = nullptr;
			bool correctMove = false;

			//if (players[playerNum]->getName() != "CPUS" || players[playerNum]->getName() != "CPUL" ) { 
				
				

				while(!correctMove)
				{


					std::cout << std::endl;
					std::cout << MOVE_PROMPT_COLOR << "Your move, "	<< PLAYER_NAME_COLOR << players[playerNum]->getName() << MOVE_PROMPT_COLOR << ": " << rang::style::reset;
					std::string moveString;
					std::getline(std::cin, moveString);

					try
					{   

						// first construct the move, which could throw an exception
						playerMove = Move::parseMove(moveString, *players[playerNum]);


						// now execute it, which could also throw exceptions
						playerMove->execute(*board, *bag, *dictionary);

						if(playerMove->isPass())
						{
							++sequentialPasses;
						}
						else
						{
							sequentialPasses = 0;
						}

						correctMove = true;
					}
					catch(MoveException & exception)
					{
						// print error message and reprompt the player
						std::cout << "Error in move: " << getFriendlyError(exception) << std::endl;
					}

				}
			/*} else if (players[playerNum]->getName() == "CPUS") { 
				playerMove = players[playerNum]->getMove(*board,*players[playerNum]); 
				
				playerMove->execute(*board, *bag, *dictionary); 

				if(playerMove->isPass())
				{
					++sequentialPasses;
				}
				else
				{
					sequentialPasses = 0;
				}
			} */

			// draw more tiles from the bag to bring the player up to a full hand
			std::vector<Tile *> newTiles = bag->drawTiles(players[playerNum]->getMaxTiles() - players[playerNum]->getNumTiles());
			players[playerNum]->addTiles(newTiles);

			// tell the user about what new tiles they got
			if(!newTiles.empty())
			{
				std::cout << "Picked up new tiles:" << TILE_NAME_COLOR;
				for(std::vector<Tile *>::iterator newTilesIter = newTiles.begin(); newTilesIter != newTiles.end(); ++newTilesIter)
				{
					std::cout << ' ' << static_cast<char>(std::toupper((*newTilesIter)->getLetter()));
				}
				std::cout << rang::style::reset << std::endl;
			}

			std::cout << "Your current score: " << SCORE_COLOR << players[playerNum]->getPoints() << rang::style::reset << std::endl;

			// wait for player confirmation
			std::cout << std::endl << "Press [enter] to continue.";
			std::cin.ignore();

			// if all players pass for one whole loop, the game is over
			if(sequentialPasses >= players.size())
			{
				gameWon = true;

				// no one gets the bonus
				emptyHandPlayerIndex = -1;
			}

			// if this player has run out of tiles, and no more could be drawn from the bag,
			// the game is also over and they get a bonus
			if(players[playerNum]->getNumTiles() == 0)
			{
				gameWon = true;

				// this player gets the bonus
				emptyHandPlayerIndex = playerNum;
			}

			delete playerMove;
		}
	}

	// total up scores
	// -------------------------------------------------------------------------------

	doFinalSubtraction(players, emptyHandPlayerIndex);

	// put all players into a multimap by score
	std::multimap<unsigned int, Player *> playerScores;
	for(std::vector<Player *>::iterator playersIter = players.begin(); playersIter != players.end(); ++playersIter)
	{
		playerScores.insert(std::make_pair((*playersIter)->getPoints(), *playersIter));
		//                                                           ^ dis is weird
	}

	// get high score by looking at last (=largest) key in multimap
	unsigned int highScore = (--playerScores.end())->first;

	// print all players with the high score
	std::pair<std::multimap<unsigned int, Player *>::iterator, std::multimap<unsigned int, Player *>::iterator> winningPlayersRange = playerScores.equal_range(highScore);
	size_t numWinners = static_cast<size_t>(std::distance(winningPlayersRange.first, winningPlayersRange.second));

	if(numWinners == 1)
	{
		std::cout << "Winner:";
	}
	else
	{
		std::cout << "Winners:";
	}
	for(; winningPlayersRange.first != winningPlayersRange.second; ++winningPlayersRange.first)
	{
		std::cout << ' ' << PLAYER_NAME_COLOR << winningPlayersRange.first->second->getName();
	}
	std::cout << rang::style::reset << std::endl;

	// now print score table
	std::cout << "Scores: " << std::endl;
	std::cout << "---------------------------------" << std::endl;

	// justify all integers printed to have the same amount of character as the high score, left-padding with spaces
	std::cout << std::setw(static_cast<uint32_t>(floor(log10(highScore) + 1)));

	for(std::multimap<unsigned int, Player *>::const_reverse_iterator playerScoresIter = playerScores.rbegin(); playerScoresIter != playerScores.rend(); ++playerScoresIter)
	{
		// note: we print the scores first since the names could be any width but the scores will be all the same width
		std::cout << SCORE_COLOR << playerScoresIter->first << rang::style::reset << " | " << PLAYER_NAME_COLOR << playerScoresIter->second->getName() << rang::style::reset << std::endl;
	}
	// cleanup
	// -------------------------------------------------------------------------------

	for(size_t playerIndex = 0; playerIndex < players.size(); ++playerIndex)
	{
		delete players[playerIndex];
	}

	delete configFile;
	delete board;
	delete bag;
	delete dictionary;
}

ConfigFile::ConfigFile(std::string const &configPath)
{
	

	enum class ParserState
	{
		LOOKING_FOR_KEY,    // not inside any structures at the moment, waiting for a key or a comment character
		IN_KEY,             // inside key, waiting for equals sign or whitespace
		LOOKING_FOR_VALUE,  // just saw colon, now looking for start of value
		IN_VALUE            // inside value, waiting for newline to end it.
	};

	std::ifstream configFileStream(configPath);
	if(!configFileStream)
	{
		throw FileException("MISSINGCONFIG");
	}

	std::string keyBuffer, valueBuffer;

	char currChar;
	ParserState state = ParserState::LOOKING_FOR_KEY;
	ParserState nextState;
	while(!configFileStream.eof() && configFileStream)
	{
		// read a single character
		configFileStream.get(currChar);

		if(configFileStream.eof())
		{
			break;
		}

		switch(state)
		{
			case ParserState::LOOKING_FOR_KEY:
				if(currChar == ' ' || currChar == '\n')
				{
					// keep looking
					nextState = ParserState::LOOKING_FOR_KEY;
				}
				else
				{
					// found something!
					nextState = ParserState::IN_KEY;
					keyBuffer = currChar;
				}
				break;

			case ParserState::IN_KEY:
				if(currChar == ':')
				{
					// last char of key
					nextState = ParserState::LOOKING_FOR_VALUE;
				}
				else
				{
					// keep scanning key
					nextState = ParserState::IN_KEY;
					keyBuffer += currChar;
				}
				break;

			case ParserState::LOOKING_FOR_VALUE:
				if(currChar == ' ')
				{
					// keep looking
					nextState = ParserState::LOOKING_FOR_VALUE;
				}
				else
				{
					//found start of value
					valueBuffer = currChar;
					nextState = ParserState::IN_VALUE;
				}
				break;

			case ParserState::IN_VALUE:
				if(currChar == '\n' || currChar == ' ')
				{
					//std::cout << "Parsed config value, key: " << keyBuffer << ", value: " << valueBuffer << std::endl;

					// done with value, now set things appropriately
					if(keyBuffer == "HANDSIZE")
					{
						handSize = static_cast<size_t>(std::stoi(valueBuffer));
					}
					else if(keyBuffer == "TILES")
					{
						tilesetFile = valueBuffer;
					}
					else if(keyBuffer == "DICTIONARY")
					{
						dictionaryFile = valueBuffer;
					}
					else if(keyBuffer == "SEED")
					{
						randomSeed = static_cast<uint32_t>(std::stoi(valueBuffer));
					}
					else if(keyBuffer == "BOARD")
					{
						boardFile = valueBuffer;
					}
					else
					{
						// unrecognized key, ignore
					}

					nextState = ParserState::LOOKING_FOR_KEY;
				}
				else
				{
					// keep scanning value
					valueBuffer += currChar;
					nextState = ParserState::IN_VALUE;
				}
				break;

		}

		state = nextState;
	}
}