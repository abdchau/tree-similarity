#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cassert>
#include "unit_cost_model.h"
#include "json_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "tree_indexer.h"
#include "quickjedi_index.h"

// argc argument name omitted because not used.
int main(int, char** argv) {

  // Input file name.
  std::string input_file_name = std::string(argv[1]);

  // Type aliases.
  using Label = label::JSONLabel;
  using CostModel = cost_model::UnitCostModelJSON<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  using TreeIndexer = node::TreeIndexJSON;
  using QUICKJEDI = json::QuickJEDITreeIndex<CostModel, TreeIndexer>;
  
  // Initialize label dictionary - separate dictionary for each test tree
  // because it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialize cost model.
  CostModel ucm(ld);
  QUICKJEDI quickjedi_algorithm(ucm);

  // Initialize two tree indexes.
  TreeIndexer ti1;
  TreeIndexer ti2;

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;
  
  // Parse the dataset.
  parser::BracketNotationParser<Label> bnp;
  bnp.parse_collection(trees_collection, input_file_name);

  // Iterate over all trees in the given collection
  unsigned int test_count = 0;
  unsigned int passed_count = 0;
  unsigned int failed_count = 0;

  for (unsigned int i = 1; i < trees_collection.size(); i++) {
    test_count++;
    
    // Index input trees.
    node::index_tree(ti1, trees_collection[i], ld, ucm);
    node::index_tree(ti2, trees_collection[i-1], ld, ucm);

    // Compute the actual JEDI distance.
    double actual_jedi = quickjedi_algorithm.jedi(ti1, ti2);

    // Compute the bounds.
    auto bounds = quickjedi_algorithm.compute_bounds(ti1, ti2);

    // Assert that the actual JEDI is within the bounds.
    // The actual distance should be >= lower_bound and <= upper_bound
    bool within_bounds = (actual_jedi >= bounds.lower_bound) && 
                        (actual_jedi <= bounds.upper_bound);

    if (!within_bounds) {
      std::cout << "ERROR: Test " << test_count << " (trees " << i << " and " << i+1 << ") failed:" << std::endl;
      std::cout << "  Actual JEDI: " << actual_jedi << std::endl;
      std::cout << "  Lower Bound: " << bounds.lower_bound << std::endl;
      std::cout << "  Upper Bound: " << bounds.upper_bound << std::endl;
      std::cout << "  Hungarian Row LB: " << bounds.hungarian_row_lb << std::endl;
      std::cout << "  Hungarian Col LB: " << bounds.hungarian_col_lb << std::endl;
      failed_count++;
    } else {
      passed_count++;
    }
  }

  std::cout << "\nBounds Test Summary:" << std::endl;
  std::cout << "  Total tests: " << test_count << std::endl;
  std::cout << "  Passed: " << passed_count << std::endl;
  std::cout << "  Failed: " << failed_count << std::endl;

  if (failed_count > 0) {
    std::cout << "\nERROR: Some tests failed. Actual JEDI distance was not within computed bounds." << std::endl;
    return -1;
  }

  return 0;
}

