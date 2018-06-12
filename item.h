#ifndef ITEM_H_
#define ITEM_H_

#include <vector>

using namespace std;

typedef vector<int> ItemSet;
typedef vector<ItemSet> ItemSetList;

bool equal(ItemSet itemset1, ItemSet itemset2, size_t end) {
  for (size_t i = 0; i < end; i++) {
    if (itemset1.at(i) != itemset2.at(i)) {
      return false;
    }
  }
  return true;
}

#endif  // ITEM_H_
