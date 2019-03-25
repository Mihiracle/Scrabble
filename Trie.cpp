//
//Trie class 
//

#include <string>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include "Trie.h"

// a constructor for an empty trie
TrieSet::TrieSet () { 
	root = new TrieNode(); 
} 

// destructor
TrieSet::~TrieSet () { 

} 

  /* Add this string to the set.
      Do nothing if the string is already in the set. */
void TrieSet::insert (std::string input) { 

	if (search(input)) { 
		return; 
	}

	TrieNode* curr = root; 

	for (unsigned i = 0; i < input.length(); i++) { 
		int letter = (int)input[i] - (int)'a'; 
		if (curr->children[letter] == NULL) { 
			curr->children[letter] = new TrieNode(); 
			curr->val = input[i];
		}
		curr = curr->children[letter]; 
	}

	curr->isEnd = true; 
} 

  /*returns true if string is in the set and 
	false if it isnt */
bool TrieSet::search (std::string input) { 
	
	if (root == NULL) { 
		return false; 
	} 

	TrieNode* curr = root; 

	for (unsigned i = 0; i < input.length(); i++) { 
		int letter = (int)input[i] - (int)'a';
		curr = curr->children[letter]; 

		if (curr == NULL) { 
			return false; 

		}
	}

	//display(root); 
	return curr->isEnd; 
}

  /* Removes this string from the set.
      Do nothing if the string is not already in the set. */
void TrieSet::remove (std::string input) { 

	if (!search(input)) { 
		return; 
	}

	removeHelper(root, input); 
}

bool TrieSet::removeHelper(TrieNode* curr, std::string input) { 
	if (input.length() != 0) { 
		int firstLetter = (int)input[0] - (int)'a';
		if (curr != NULL && curr->children[firstLetter] != NULL && 
		removeHelper(curr->children[firstLetter],input.substr(1)) &&
		curr->isEnd == false) { 
			if (!hasChildren(curr)) { 
				delete curr; 
				curr = NULL; 
				return true; 
			} else { 
				return false; 
			}
		}
	}

	if (input.length() == 0 && curr->isEnd) { 
		if (!hasChildren(curr)) { 
			delete curr; 
			curr = NULL; 
			return true; 
		} else { 
			curr->isEnd = false; 
			return false; 
		}
	}

	return false; 
}

//checks if node has any children 
bool TrieSet::hasChildren(TrieNode* node) { 
	for (int i = 0; i < 26; i++) { 
		if (node->children[i]) { 
			return true; 
		}
	}
	return false; 
} 

  /* returns the TrieNode matching the provided string, or returns NULL 
      if it is not the prefix of a word in the Trie. */
TrieNode* TrieSet::prefix(std::string px) { 
	if (root == NULL) { 
		return NULL; 
	}

	TrieNode* curr = root; 

	for (unsigned i = 0; i < px.length(); i++) { 
		int letter = (int)px[i] - (int)'a';
		if (curr->children[letter] != NULL) { 
			curr = curr->children[letter]; 
		} else { 
			return NULL; 
		}
	}
	return curr; 

}

void TrieSet::display(TrieNode* n) { 
	if (!hasChildren(n)) { 
		std::cout << n->val << std::endl; 
	}

	for (int i = 0; i < 26; i++) { 
		display(n->children[i]); 
	}

}
