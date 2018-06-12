#include <iostream>
#include <cstdlib>
#include "apriori.h"

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cerr << "USAGE : " << argv[0] << "[minimum_support_percentage(omit %)] [input_path] [output_path]" << endl;
    exit(1);
  }
  
  size_t min_sup_percentage = atol(argv[1]);
  string input_path = argv[2];
  string output_path = argv[3];
  
  Apriori apriori(min_sup_percentage, input_path, output_path);
  apriori.run();
  apriori.Analyze();
  apriori.FileWrite();

  return 0;
}
