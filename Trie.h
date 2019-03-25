
/* Trie is a way to terminate a search when you've assembled a prefix that cannot be completed into a word. 
*/

#ifndef TRIE_H_
#define TRIE_H_

#include <string>
#include <set>
#include <map>
#include <vector>


struct TrieNode {
  bool isEnd;  
  TrieNode *parent; 
  TrieNode *children[26]; // one child for each letter of the alphabet.
  char val; 

  TrieNode() { 
    isEnd = false; 
    for (int i = 0; i < 26; i++) { 
      children[i] = NULL; 
    }
  } 



};

class TrieSet {
public:

  TrieSet (); // a constructor for an empty trie
  ~TrieSet (); // destructor

  void insert (std::string input);
   /* Add this string to the set.
      Do nothing if the string is already in the set. */

  void remove (std::string input);
   /* Removes this string from the set.
      Do nothing if the string is not already in the set. */

  TrieNode* prefix(std::string px); 
   /* returns the TrieNode matching the provided string, or returns NULL 
      if it is not the prefix of a word in the Trie. */

  //returns whether string is in tree
  bool search (std::string input); 

  //checks if node has any children 
  bool hasChildren(TrieNode* node);

  //recursive removal helper 
  bool removeHelper(TrieNode* curr, std::string input);

  void display(TrieNode* n); 

private: 
  TrieNode* root; 
  


};

#endif /* TRIE_H_ */