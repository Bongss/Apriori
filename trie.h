//
// This is for Apriori Algorithm.
// So, This Trie structure are diffrent slightly from the original trie implementation
//
#include <iterator>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include "item.h"

using namespace std;

typedef size_t Item;

/* 
 * Assume that all of items in Trie is sorted 
 */
class TrieNode;
typedef map<Item, TrieNode*> NodeMap;

class TrieNode {
  NodeMap nodemap;
  Item item;
  bool is_leaf;
  size_t count;

public:
  TrieNode() {
    item = 0;
    count = 0;
    is_leaf = false;
  }

  bool FindChildNode(Item _item) {
    if (nodemap.find(_item) == nodemap.end()) {
      return false;
    }
    return true;
  }
  
  void AddChildNode(pair<int, TrieNode*> child) {
    nodemap.insert(child);
  }
  
  TrieNode* GetChildNode(Item _item) {
    return nodemap.at(_item);
  }

  NodeMap GetChilds() {
    return nodemap;
  }

  void set(int _item) {
    item = _item;
  }

  size_t get() {
    return item;
  }

  void SetCount(size_t _count) {
    count = _count;
  }


  size_t GetCount() {
    return count;
  }

  void SetIsLeaf(bool _is_leaf) {
    is_leaf = _is_leaf;
  }

  bool IsLeaf() {
    return is_leaf;
  }
};

class Trie {
  TrieNode* root;
  int nums;

public:
  Trie() {
    root = new TrieNode();
    nums = 0;
  };

  ~Trie() {
    delete root;
  }

  void clear() {
    delete root;
    root = new TrieNode();
    nums = 0;
  }

  void IncNums() {
    nums++;
  } 

  int GetNums() {
    return nums;
  }

  TrieNode* GetRoot() {
    return root;
  }


  void insert(ItemSet itemset) {
    TrieNode *cur = root;
    TrieNode *next = nullptr;
    Item item;
    
    while (!itemset.empty()) {
      cur->SetIsLeaf(false);
      item = itemset.front();
      itemset.erase(itemset.begin());
      
      if (!(cur->FindChildNode(item))) {
        next = new TrieNode();
        next->set(item);
        cur->AddChildNode(make_pair(item, next));
        this->IncNums();
      }
      cur = cur->GetChildNode(item);
    }
    cur->SetIsLeaf(true);
    cur = nullptr;
  }

  void insert(ItemSet itemset, size_t count) {
    TrieNode *cur = root;
    TrieNode *next = nullptr;
    Item item;
    
    while (!itemset.empty()) {
      cur->SetIsLeaf(false);
      item = itemset.front();
      itemset.erase(itemset.begin());
      
      if (!(cur->FindChildNode(item))) {
        next = new TrieNode();
        next->set(item);
        cur->AddChildNode(make_pair(item, next));
        this->IncNums();
      }
      cur = cur->GetChildNode(item);
    }
    cur->SetCount(count);
    cur->SetIsLeaf(true);
    cur = nullptr;
  }


  void insertAll(ItemSetList itemset_list) {
    for (auto itemset : itemset_list) {
      insert(itemset);
    }
  }

  bool search(ItemSet itemset) {
    TrieNode *cur = root;
    Item item;
    
    while (!itemset.empty()) {
      item = itemset.front();
      itemset.erase(itemset.begin());

      if (!(cur->FindChildNode(item)) || (cur->IsLeaf() && !itemset.empty())) {
        return false;
      }
      cur = cur->GetChildNode(item);
    }
    cur = nullptr;
    return true;
  }

  ItemSetList GetAll_itemset(Item item) {
    ItemSetList itemset_list;
    
    if (root->FindChildNode(item)) {
      _GetAll_itemset(root->GetChildNode(item), itemset_list, 0);
    }
    return itemset_list;
  }

  ItemSetList GetAll_itemset(ItemSet itemset) {
    TrieNode *cur = root;
    NodeMap childes;
    ItemSetList itemset_list;
    int index = 0;

    for (auto item : itemset) {
      if (cur->FindChildNode(item)) {
        cur = cur->GetChildNode(item);
        index++;
      } else {
        break;
      }
    }
    
    if (cur->IsLeaf()) {
      itemset_list.push_back(itemset);
    } else {
      childes = cur->GetChilds();
      itemset_list.push_back(itemset);
      for (auto child : childes) {
        _GetAll_itemset(child.second, itemset_list, itemset.size());
      }
    }

    return itemset_list;
  }

  
  void _GetAll_itemset(TrieNode* node, ItemSetList& itemset_list, size_t length) {
    bool is_leaf = node->IsLeaf();
    TrieNode *cur = node;
    NodeMap childes = node->GetChilds();
    ItemSet temp_itemset;
    Item item = node->get();

    if (itemset_list.size() != 0) {
      for (auto it = itemset_list.rbegin(); it != itemset_list.rend(); ++it) {
        if ((it->size()) == length) {
          temp_itemset = *(it);
          temp_itemset.push_back(item);
          itemset_list.push_back(temp_itemset);
          break;
        } else {
          continue;
        }
      }
    } else {
      temp_itemset.push_back(item);
      itemset_list.push_back(temp_itemset);
    }

    if (!is_leaf) {
      for (auto const& child : childes) {
        _GetAll_itemset(child.second, itemset_list, length + 1);
      }
    }
  }

  ItemSetList GetAllLengthK_itemset(ItemSet itemset, size_t length) {
    ItemSetList itemset_list;
    ItemSetList temp_itemset_list;
    NodeMap childes = root->GetChilds();

    if (itemset.size() < length) {
      return itemset_list;
    }

    int count = 0;

    for (auto const& item : itemset) {
      if (root->FindChildNode(item)) {
        temp_itemset_list = _GetAllLengthK_itemset(root->GetChildNode(item), itemset, 0, length - 1);
        for (auto temp_itemset : temp_itemset_list) {
          sort(temp_itemset.begin(), temp_itemset.end());
          itemset_list.push_back(temp_itemset);
        }
      }
    }
    return itemset_list;
  }

  ItemSetList _GetAllLengthK_itemset(TrieNode* node, ItemSet itemset, int begin, int length) {
    bool is_leaf = node->IsLeaf();
    TrieNode *cur = node;
    ItemSetList itemset_list;
    ItemSetList temp_itemset_list;
    Item item;

    if (length == -1 || (length != 0 && is_leaf)) {
      return temp_itemset_list;
    }

    item = node->get();

    if (is_leaf) {
      ItemSet temp_itemset;
      temp_itemset.push_back(item);
      itemset_list.push_back(temp_itemset);
      return itemset_list;
    }

    for (size_t i = begin; i < itemset.size(); i++) {
      if (cur->FindChildNode(itemset[i])) {
        temp_itemset_list = _GetAllLengthK_itemset(cur->GetChildNode(itemset[i]), itemset, begin + 1, length - 1); 
        for (auto temp_itemset : temp_itemset_list) {
          if (temp_itemset.size() == length) {
            temp_itemset.push_back(item);
            itemset_list.push_back(temp_itemset);
          }
        }
      }
    }
    return itemset_list;
  }
};
