#ifndef LENGTHAI_H_
#define LENGTHAI_H_

#include "Move.h"
#include "Dictionary.h"
#include "Player.h"
#include "Board.h"
#include "Exceptions.h"
#include "Trie.h"
#include "Tile.h"

#include <set>
#include <string>
#include <map> 
#include <vector> 


class LengthAI : public Player { 

public: 

	LengthAI(std::string const & name,size_t maxTiles); 

 

	~LengthAI();  

	void init(Dictionary* dict); 

	Move* getMove(const Board& board, Player& player); 

	void horizontalHelper(int row, int col, int init_row, int init_col, 
					      std::string used, std::string tmp, std::string toUse, 
					      Player& player, const Board& board); 

	void verticalHelper(int row, int col, int init_row, int init_col, 
					    std::string used, std::string tmp, std::string toUse, 
					    Player& player, const Board& board);





private: 
	std::vector<std::vector<std::string> > moves; 
	std::vector<int> scores; 
	TrieSet* _trie;
	Dictionary* d;  


	const std::string _name;
	const size_t _maxTiles;
	unsigned int _points;

	// stores the player's tiles indexed by character
	std::multimap<char, Tile *> _hand;


}; 



#endif //LENGTHAI_H_