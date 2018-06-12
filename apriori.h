#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include "item.h"
#include "trie.h"

using namespace std;

class Apriori {
  map<ItemSet, int> itemset_table;
  map<ItemSet, int> all_itemset_table;
  ItemSetList L_k;          // (k)-FrequentItemSets
  ItemSetList C_k_1;        // (K+1)-CandidateItemSets
  Trie frequent_trie;       // k-frequentItemSet Trie for prouning
  Trie candidate_trie;      // (k+1)-candidateItemSet Trie for extracting candidateItemset matching transaction
  Trie all_frequent_trie;   // All-frequentItemSEt Trie for analysis

  map<pair<ItemSet,ItemSet>, pair<double, double>> associations;
  size_t k;
  size_t min_sup_percent;
  size_t tx_count;
  size_t min_sup;
  string input_path;
  string output_path;
  
public:
  Apriori(size_t _min_sup_percent, string _input_path, string _output_path) {
    min_sup_percent = _min_sup_percent;
    input_path = _input_path;
    output_path = _output_path;
    tx_count = 0;
    k = 0;
    L_k.clear();
    C_k_1.clear();
    frequent_trie.clear();
    candidate_trie.clear();
    itemset_table.clear();
    all_itemset_table.clear();
  };
  
  void run() {
    if (k == 0) {
      DBFullScan(); //DB scan and count number_of_tx and create itemset(k+1) couning table
      min_sup = tx_count * 0.01 * min_sup_percent;
      FilteringAndCreateFrequentItemSet();
      C_k_1 = L_k;
      ++k;
    } else {
      CandidateGenerate();  // Generate C_K+1 from L_K
      candidate_trie.insertAll(C_k_1);
      CandidateTestAndFrequentKSet();
      ++k;
    }
    if (!C_k_1.empty()) {
      run();
    }
  }

  void DBFullScan() {
    ItemSet itemset;
    ifstream file(input_path, ifstream::in);
    string line;

    while (getline(file, line)) {
      itemset = split(line, "\t");
      sort(itemset.begin(), itemset.end());

      if (k == 0) {
        ++tx_count;
        for (auto const &item : itemset) {
          ItemSet candidate_itemset;
          candidate_itemset.push_back(item);
          if (itemset_table.find(candidate_itemset) == itemset_table.end()) {
            itemset_table.insert(make_pair(candidate_itemset, 1));
          } else {
            itemset_table.at(candidate_itemset) = itemset_table.at(candidate_itemset) + 1;
          }
        }
      } else {  // k > 0
        ItemSetList candidate_itemset_list = candidate_trie.GetAllLengthK_itemset(itemset, k + 1);
        for (auto const &candidate_itemset : candidate_itemset_list) {
          if (itemset_table.find(candidate_itemset) == itemset_table.end()) {
            itemset_table.insert(make_pair(candidate_itemset, 1));
          } else {
            itemset_table.at(candidate_itemset) = itemset_table.at(candidate_itemset) + 1;
          }
        }
      }
    }
    file.close();
  }

  void FilteringAndCreateFrequentItemSet() {
    map<ItemSet, int>::iterator it;
    for (it = itemset_table.begin(); it != itemset_table.end(); ++it) {
      if ((it->second) < min_sup) {
        continue;
      }
      L_k.push_back(it->first);
      all_itemset_table.insert(make_pair(it->first, it->second));
      all_frequent_trie.insert(it->first, it->second);
    }
    itemset_table.clear();
  }

  void CandidateGenerate() {
    C_k_1.clear();
    frequent_trie.insertAll(L_k);
    SelfJoin();
    L_k.clear();
  }
  
  void SelfJoin() {
    size_t L_k_size = L_k.size();
    bool is_candidate = false;

    for (size_t i = 0; i < L_k_size - 1; i++) {
      for (size_t j = (i + 1); j < L_k_size; j++) {
        ItemSet itemset1 = L_k.at(i);
        ItemSet itemset2 = L_k.at(j);

        if (k == 1) {
          itemset1.push_back(itemset2.at(0));
          C_k_1.push_back(itemset1);
        } else {
          if (equal(itemset1, itemset2, k - 1)) {
            itemset1.push_back(itemset2.at(k - 1));

            is_candidate = !prune(itemset1);
            if (is_candidate) {
              C_k_1.push_back(itemset1);
            }
          }
        }
      }
    }
    frequent_trie.clear();
  }

  void CandidateTestAndFrequentKSet() {
    DBFullScan();
    FilteringAndCreateFrequentItemSet();
  }

  bool prune(ItemSet candidate_itemset) {
    ItemSet sub_itemset;
    size_t size = candidate_itemset.size();
    for (size_t i = 0; i < size; i++) {
      for (size_t j = 0; j < size; j++) {
        if (i != j) {
          sub_itemset.push_back(candidate_itemset.at(j));
        }
      }
      if (!frequent_trie.search(sub_itemset)) {
        return true;
      }
      sub_itemset.clear();
    }
    return false;
  }

  void Analyze() {
    vector<ItemSet> frequent_itemsets;
    size_t num_frequent = all_itemset_table.size();
    size_t count = 0;

    for (auto const &itemsetX : all_itemset_table) {
      for (auto const &itemsetY : all_itemset_table) {
        set<int> check;
        ItemSet temp_X = itemsetX.first;
        ItemSet temp_Y = itemsetY.first;

        size_t len_X = temp_X.size();
        size_t len_Y = temp_Y.size();

        if (len_X >= len_Y) {
          continue;
        }

        size_t len_max = max(len_X, len_Y);

        for (auto const &item : temp_X) {
          check.insert(item);
        }
        for (auto const &item : temp_Y) {
          check.insert(item);
        }

        if (check.size() != len_max) {
          continue;
        }

        double supportX = all_itemset_table.at(temp_X);
        double supportY = all_itemset_table.at(temp_Y);
        double support = 0;
        double confidence = 0;
        double rconfidence = 0;

        if (len_X < len_Y) {
          support = (supportY / tx_count) * 100;
          confidence = (supportY / supportX) * 100;
          for (size_t i = 0; i < len_X; i++) {
            for (size_t j = 0; j < len_Y; j++) {
              if (temp_X.at(i) == temp_Y.at(j)) {
                temp_Y.erase(temp_Y.begin() + j);
                --len_Y;
              }
            }
          }
          rconfidence = (supportY / all_itemset_table.at(temp_Y)) * 100;
        } else {
          support = (supportX / tx_count) * 100;
          confidence = (supportX / supportY) * 100;
          for (size_t i = 0; i < len_Y; i++) {
            for (size_t j = 0; j < len_X; j++) {
              if (temp_Y.at(i) == temp_X.at(j)) {
                temp_X.erase(temp_X.begin() + j);
                --len_X;
              }
            }
          }
          rconfidence = (supportX / all_itemset_table.at(temp_X)) * 100;
        }
        if (support == 0) {
          continue;
        }
        pair<pair<ItemSet, ItemSet>, pair<double, double>> association = make_pair(make_pair(temp_X, temp_Y), make_pair(support, confidence));
        pair<pair<ItemSet, ItemSet>, pair<double, double>> r_association = make_pair(make_pair(temp_Y, temp_X), make_pair(support, rconfidence));
        if (associations.find(association.first) == associations.end()) {
          associations.insert(association);
        }

        if (associations.find(r_association.first) == associations.end()) {
          associations.insert(r_association);
        }
      }
    }
  }

  void FileWrite() {
    ofstream outfile;
    outfile.open(output_path);
    for (auto const& association: associations) {
      ItemSet itemsetX = association.first.first;
      ItemSet itemsetY = association.first.second;
      double support = association.second.first;
      double confidence = association.second.second;

      outfile << "{";
      auto iter = itemsetX.begin();
      while (true) {
        outfile << *iter;
        ++iter;
        if (iter == itemsetX.end()) {
          break;
        } else {
          outfile << ",";
        }
      }
      outfile << "}\t";

      outfile << "{";
      iter = itemsetY.begin();
      while (true) {
        outfile << *iter;
        ++iter;
        if (iter == itemsetY.end()) {
          break;
        } else {
          outfile << ",";
        }
      }
      outfile << "}\t";

      outfile <<fixed;
      outfile.precision(2);
      outfile << support << "\t";
      outfile << confidence;
      outfile << endl;
    }
    outfile.close();
  }

  vector<int> split(const string &s, const string& delimiter) {
    vector<int> tokens;
    int token;
    size_t delimiter_length = delimiter.length();
    size_t prev_pos = 0;
    size_t pos = s.find_first_of(delimiter);
    
    while (pos != string::npos) {
      token = stoi(s.substr(prev_pos, (pos - prev_pos)));
      tokens.push_back(token);
      prev_pos = pos + delimiter_length;
      pos = s.find_first_of(delimiter, pos + 1);
    }

    if (prev_pos != s.length()) {
      token = stoi(s.substr(prev_pos));
      tokens.push_back(token);
    }
    return tokens;
  }
};
