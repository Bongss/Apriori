#include <iostream>
#include <cstdlib>
#include <vector>
#include "trie.h"

using namespace std;
int main(int argc, char* argv[]) {
  
  Trie trie;
  vector<int> intlist;
  vector<int> intlist2;
  vector<int> intlist3;
  vector<int> intlist4;
  vector<int> intlist5;

  intlist.push_back(1);
  intlist.push_back(2);

  intlist2.push_back(3);
  intlist2.push_back(4);

  intlist3.push_back(1);
  intlist3.push_back(3);


  intlist5.push_back(2);
  intlist5.push_back(4);

  intlist4.push_back(1);
  intlist4.push_back(2);
  intlist4.push_back(3);
  intlist4.push_back(4);

  trie.insert(intlist);
  trie.insert(intlist2);
  trie.insert(intlist3);
  trie.insert(intlist5);

  vector< vector<int> > items = trie.GetAllLengthK_itemset(intlist4, 2);
  cout << items.size() << endl;

  return 0;
}
