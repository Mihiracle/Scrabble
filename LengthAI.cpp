#include "LengthAI.h"
#include "Util.h"
#include "Exceptions.h"
#include "Player.h"


#include <set>
#include <string>
#include <map> 
#include <vector> 

using namespace std; 

LengthAI::LengthAI(std::string const & name,size_t maxTiles):
Player(name,maxTiles),
_name("CPUS"),
_maxTiles(maxTiles),
_points(0),
_hand()
{

}



LengthAI::~LengthAI() { 

}



void LengthAI::init(Dictionary* dict) { 
	std::set<std::string>::iterator it; 
	for (it = dict->getDict().begin(); it != dict->getDict().begin(); it++) { 
		_trie->insert(*it); 
	}
	d = dict; 

}

Move* LengthAI::getMove(const Board & board, Player& player){
		moves.clear();
		scores.clear();

		std::string currHand = ""; 

		std::set<Tile*> currTiles = player.getHandTiles();
		for (set<Tile*>::iterator it = currTiles.begin(); it != currTiles.end(); it++) { 
			currHand += (*it)->getLetter(); 
		}

		for (unsigned y = 0; y < board.getColumns(); y++) { 
			for (unsigned x = 0; x < board.getRows(); x++) { 
				horizontalHelper(y,x,y,x,"","",currHand,player,board); 
				verticalHelper(y,x,y,x,"","",currHand,player,board);
			}
		}

		if (moves.empty()) { 
			Move * move = Move::parseMove("PASS",player); 
			return move; 
		} else { 
			unsigned int max = -1; 
			int maxLengthIndex; 
			for (unsigned i = 0; i < moves.size(); i++) { 
				if (moves[i][0].length() > max) { 
					max = moves[i][0].length(); 
					maxLengthIndex = i; 
				}
			}

			std::vector<std::string> bestMove = moves[maxLengthIndex]; 

			std::string moveCommand; 

			if (bestMove[3] == "horizontal") { 
				moveCommand = "PLACE - " + bestMove[2] + " " + bestMove[1] + " " + bestMove[0]; 
			} else { 
				moveCommand = "PLACE | " + bestMove[2] + " " + bestMove[1] + " " + bestMove[0];
			}

			Move* move = Move::parseMove(moveCommand,player); 
			return move; 
		}
		
}



void LengthAI::horizontalHelper(int row, int col, int init_row, int init_col, 
							   std::string used, std::string tmp, std::string toUse, 
							   Player& player, const Board& board) 
{ 


	if (board.getSquare(row,col)->isOccupied() && toUse.length() == 0) {
		return;  
	}

	std::string _used, _tmp, _toUse; 

	if (board.getSquare(row,col)->isOccupied()) { 
		char tileChar = board.getSquare(row,col)->getLetter(); 

		_tmp = tmp + tileChar; 
		_used = used; 
		_toUse = toUse; 

		if(_trie->search(_tmp)) { 
			PlaceMove* currMove = new PlaceMove(init_row+1,init_col+1,true,_used,&player); 
			int score = 0; 
			std::vector<std::pair<std::string, unsigned int>> moveWords = board.getPlaceMoveResults(*currMove); 

			bool validMove = true; 
			for (unsigned i = 0; i < moveWords.size(); i++) { 
				if (!d->isLegalWord(moveWords[i].first)) {
					validMove = false;  
				}
			}

			if (validMove) { 
				for (unsigned i = 0; i < moveWords.size(); i++) { 
					score += moveWords[i].second; 
				}

				std::vector<std::string> move; 

				move.push_back(_used); 
				std::string init_string_row, init_string_col; 
				init_string_row = (init_row+1) + ""; 
				init_string_col = (init_col+1) + ""; 
				move.push_back(init_string_row); 
				move.push_back(init_string_col); 
				move.push_back("horizontal"); 
				moves.push_back(move); 
				scores.push_back(score); 
			}

			if ((unsigned)col+1 == board.getColumns() || (unsigned)row+1 == board.getRows()) { 
				return; 
			} else { 
				horizontalHelper(row,col+1,init_row,init_col,_used,_tmp,_toUse,player,board); 
			}
		}

	} else { 
		for (unsigned i = 0; i < toUse.length(); i++) { 
			if (isalpha(toUse[i])) { 
				_used = used + toUse[i]; 
				_tmp = tmp + toUse[i]; 
				_toUse = toUse.substr(i,0) + toUse.substr(i+1); 
				if (_trie->search(_tmp)) { 
					PlaceMove* currMove = new PlaceMove(init_row+1,init_col+1,true,_used,&player); 
					int score = 0; 
					std::vector<std::pair<std::string, unsigned int>> moveWords = board.getPlaceMoveResults(*currMove); 

					bool validMove = true; 
					for (unsigned i = 0; i < moveWords.size(); i++) { 
						if (!d->isLegalWord(moveWords[i].first)) {
							validMove = false;  
						}
					}

					if (validMove) { 
						for (unsigned i = 0; i < moveWords.size(); i++) { 
							score += moveWords[i].second; 
						}

						std::vector<std::string> move; 

						move.push_back(_used); 
						std::string init_string_row, init_string_col; 
						init_string_row = (init_row+1) + ""; 
						init_string_col = (init_col+1) + ""; 
						move.push_back(init_string_row); 
						move.push_back(init_string_col); 
						move.push_back("horizontal"); 
						moves.push_back(move); 
						scores.push_back(score); 
					}

					if ((unsigned)col+1 == board.getColumns() || (unsigned)row+1 == board.getRows()) { 
						return; 
					} else { 
						horizontalHelper(row,col+1,init_row,init_col,_used,_tmp,_toUse,player,board); 
					}
				}
			} else if (toUse[i] == '?') { 
				for (char c = 'A'; c <= 'Z'; c++) { 
					_used += used + '?' + c; 
					_tmp = tmp + c; 
					_toUse = toUse.substr(i,0) + toUse.substr(i+1);
					if (_trie->search(_tmp)) { 
						PlaceMove* currMove = new PlaceMove(init_row+1,init_col+1,true,_used,&player); 
						int score = 0; 
						std::vector<std::pair<std::string, unsigned int>> moveWords = board.getPlaceMoveResults(*currMove); 

						bool validMove = true; 
						for (unsigned i = 0; i < moveWords.size(); i++) { 
							if (!d->isLegalWord(moveWords[i].first)) {
								validMove = false;  
							}
						}

						if (validMove) { 
							for (unsigned i = 0; i < moveWords.size(); i++) { 
								score += moveWords[i].second; 
							}

							std::vector<std::string> move; 

							move.push_back(_used); 
							std::string init_string_row, init_string_col; 
							init_string_row = (init_row+1) + ""; 
							init_string_col = (init_col+1) + ""; 
							move.push_back(init_string_row); 
							move.push_back(init_string_col); 
							move.push_back("horizontal"); 
							moves.push_back(move); 
							scores.push_back(score); 
						}

						if ((unsigned)col+1 == board.getColumns() || (unsigned)row+1 == board.getRows()) { 
							return; 
						} else { 
							horizontalHelper(row,col+1,init_row,init_col,_used,_tmp,_toUse,player,board); 
						}
				    }
				}

			}


		}
		return;
	}
}


void LengthAI::verticalHelper(int row, int col, int init_row, int init_col, 
							   std::string used, std::string tmp, std::string toUse, 
							   Player& player, const Board& board) 
{ 


	if (board.getSquare(row,col)->isOccupied() && toUse.length() == 0) {
		return;  
	}

	std::string _used, _tmp, _toUse; 

	if (board.getSquare(row,col)->isOccupied()) { 
		char tileChar = board.getSquare(row,col)->getLetter(); 

		_tmp = tmp + tileChar; 
		_used = used; 
		_toUse = toUse; 

		if(_trie->search(_tmp)) { 
			PlaceMove* currMove = new PlaceMove(init_row+1,init_col+1,true,_used,&player); 
			int score = 0; 
			std::vector<std::pair<std::string, unsigned int>> moveWords = board.getPlaceMoveResults(*currMove); 

			bool validMove = true; 
			for (unsigned i = 0; i < moveWords.size(); i++) { 
				if (!d->isLegalWord(moveWords[i].first)) {
					validMove = false;  
				}
			}

			if (validMove) { 
				for (unsigned i = 0; i < moveWords.size(); i++) { 
					score += moveWords[i].second; 
				}

				std::vector<std::string> move; 

				move.push_back(_used); 
				std::string init_string_row, init_string_col; 
				init_string_row = (init_row+1) + ""; 
				init_string_col = (init_col+1) + ""; 
				move.push_back(init_string_row); 
				move.push_back(init_string_col); 
				move.push_back("vertical"); 
				moves.push_back(move); 
				scores.push_back(score); 
			}

			if ((unsigned)col+1 == board.getColumns() || (unsigned)row+1 == board.getRows()) { 
				return; 
			} else { 
				verticalHelper(row+1,col,init_row,init_col,_used,_tmp,_toUse,player,board); 
			}
		}

	} else { 
		for (unsigned i = 0; i < toUse.length(); i++) { 
			if (isalpha(toUse[i])) { 
				_used = used + toUse[i]; 
				_tmp = tmp + toUse[i]; 
				_toUse = toUse.substr(i,0) + toUse.substr(i+1); 
				if (_trie->search(_tmp)) { 
					PlaceMove* currMove = new PlaceMove(init_row+1,init_col+1,true,_used,&player); 
					int score = 0; 
					std::vector<std::pair<std::string, unsigned int>> moveWords = board.getPlaceMoveResults(*currMove); 

					bool validMove = true; 
					for (unsigned i = 0; i < moveWords.size(); i++) { 
						if (!d->isLegalWord(moveWords[i].first)) {
							validMove = false;  
						}
					}

					if (validMove) { 
						for (unsigned i = 0; i < moveWords.size(); i++) { 
							score += moveWords[i].second; 
						}

						std::vector<std::string> move; 

						move.push_back(_used); 
						std::string init_string_row, init_string_col; 
						init_string_row = (init_row+1) + ""; 
						init_string_col = (init_col+1) + ""; 
						move.push_back(init_string_row); 
						move.push_back(init_string_col); 
						move.push_back("vertical"); 
						moves.push_back(move); 
						scores.push_back(score); 
					}

					if ((unsigned)col+1 == board.getColumns() || (unsigned)row+1 == board.getRows()) { 
						return; 
					} else { 
						verticalHelper(row+1,col,init_row,init_col,_used,_tmp,_toUse,player,board); 
					}
				}
			} else if (toUse[i] == '?') { 
				for (char c = 'A'; c <= 'Z'; c++) { 
					_used += used + '?' + c; 
					_tmp = tmp + c; 
					_toUse = toUse.substr(i,0) + toUse.substr(i+1);
					if (_trie->search(_tmp)) { 
						PlaceMove* currMove = new PlaceMove(init_row+1,init_col+1,true,_used,&player); 
						int score = 0; 
						std::vector<std::pair<std::string, unsigned int>> moveWords = board.getPlaceMoveResults(*currMove); 

						bool validMove = true; 
						for (unsigned i = 0; i < moveWords.size(); i++) { 
							if (!d->isLegalWord(moveWords[i].first)) {
								validMove = false;  
							}
						}

						if (validMove) { 
							for (unsigned i = 0; i < moveWords.size(); i++) { 
								score += moveWords[i].second; 
							}

							std::vector<std::string> move; 

							move.push_back(_used); 
							std::string init_string_row, init_string_col; 
							init_string_row = (init_row+1) + ""; 
							init_string_col = (init_col+1) + ""; 
							move.push_back(init_string_row); 
							move.push_back(init_string_col); 
							move.push_back("vertical"); 
							moves.push_back(move); 
							scores.push_back(score); 
						}

						if ((unsigned)col+1 == board.getColumns() || (unsigned)row+1 == board.getRows()) { 
							return; 
						} else { 
							verticalHelper(row+1,col,init_row,init_col,_used,_tmp,_toUse,player,board); 
						}
				    }
				}

			}


		}
		return;
	}
}