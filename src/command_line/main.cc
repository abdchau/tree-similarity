// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <iostream>
#include "node.h"
#include "string_label.h"
#include "json_label.h"
#include "unit_cost_model.h"
#include "bracket_notation_parser.h"
#include "apted_tree_index.h"
#include "quickjedi_index.h"

using StringLabel = label::StringLabel;
using StringCostModelLD = cost_model::UnitCostModelLD<StringLabel>;
using StringLabelDictionary = label::LabelDictionary<StringLabel>;

using JSONLabel = label::JSONLabel;
using JSONCostModelLD = cost_model::UnitCostModelJSON<JSONLabel>;
using JSONLabelDictionary = label::LabelDictionary<JSONLabel>;

int evaluate_apted(std::string, std::string);
int evaluate_quickjedi(std::string, std::string);

/// Simple command-line tool for executing Tree Edit Distance.
int main(int argc, char** argv) {

  // Verify parameters.
  if (argc != 5) {
    std::cerr << "Incorrect number of parameters. Sample usage: ./ted apted string {x{a}} {x{b}}" << std::endl;
    std::cerr << "Usage: ./ted <algorithm> <input-format> <input1> <input2>" << std::endl;
    return -1;
  }

  std::string source_tree_string;
  std::string dest_tree_string;

  
  // Verify the input format before parsing.
  if (std::strcmp(argv[2], "string") == 0) {
    source_tree_string = argv[3];
    dest_tree_string = argv[4];
  } else if (std::strcmp(argv[2], "file") == 0) {
    std::ifstream tree_file(argv[3]);
    source_tree_string.assign((std::istreambuf_iterator<char>(tree_file)),
                              std::istreambuf_iterator<char>());
    tree_file.close();

    tree_file = std::ifstream(argv[4]);
    dest_tree_string.assign((std::istreambuf_iterator<char>(tree_file)),
                            std::istreambuf_iterator<char>());
    tree_file.close();
  }
  else {
      std::cerr << "Incorrect input format. Use either string or file." << std::endl;
      return -1;
  }

  if (std::strcmp(argv[1], "apted") == 0) {
      return evaluate_apted(source_tree_string, dest_tree_string);
  } else if (std::strcmp(argv[1], "quickjedi") == 0) {
      return evaluate_quickjedi(source_tree_string, dest_tree_string);
  } else {
      std::cerr << "Incorrect algorithm name. Use apted." << std::endl;
      return -1;
  }

  return 0;
}

int evaluate_quickjedi(std::string source_tree_string, std::string dest_tree_string) {
  parser::BracketNotationParser<JSONLabel> bnp;
  if (!bnp.validate_input(source_tree_string)) {
    std::cerr << "Incorrect format of source tree. Is the number of opening and closing brackets equal?" << std::endl;
    return -1;
  }
  const node::Node<JSONLabel> source_tree = bnp.parse_single(source_tree_string);

  if (!bnp.validate_input(dest_tree_string)) {
    std::cerr << "Incorrect format of destination tree. Is the number of opening and closing brackets equal?" << std::endl;
    return -1;
  }
  const node::Node<JSONLabel> destination_tree = bnp.parse_single(dest_tree_string);

  std::cout << "Size of source tree:" << source_tree.get_tree_size() << std::endl;
  std::cout << "Size of destination tree:" << destination_tree.get_tree_size() << std::endl;

  JSONLabelDictionary ld;
  JSONCostModelLD ucm(ld);
  json::QuickJEDITreeIndex<JSONCostModelLD, node::TreeIndexJSON> jedi_algorithm(ucm);
  node::TreeIndexJSON ti1;
  node::TreeIndexJSON ti2;
  node::index_tree(ti1, source_tree, ld, ucm);
  node::index_tree(ti2, destination_tree, ld, ucm);
  std::cout << "Distance TED:" << jedi_algorithm.jedi(ti1, ti2) << std::endl;

  return 0;
}

int evaluate_apted(std::string source_tree_string, std::string dest_tree_string) {
    parser::BracketNotationParser<StringLabel> bnp;
    if (!bnp.validate_input(source_tree_string)) {
        std::cerr << "Incorrect format of source tree. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
    }
    const node::Node<StringLabel> source_tree = bnp.parse_single(source_tree_string);

    if (!bnp.validate_input(dest_tree_string)) {
        std::cerr << "Incorrect format of destination tree. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
    }
    const node::Node<StringLabel> destination_tree = bnp.parse_single(dest_tree_string);

  std::cout << "Size of source tree:" << source_tree.get_tree_size() << std::endl;
  std::cout << "Size of destination tree:" << destination_tree.get_tree_size() << std::endl;

  StringLabelDictionary ld;
  StringCostModelLD ucm(ld);
  ted::APTEDTreeIndex<StringCostModelLD, node::TreeIndexAPTED> apted_algorithm(ucm);
  node::TreeIndexAPTED ti1;
  node::TreeIndexAPTED ti2;
  node::index_tree(ti1, source_tree, ld, ucm);
  node::index_tree(ti2, destination_tree, ld, ucm);
  std::cout << "Distance TED:" << apted_algorithm.ted(ti1, ti2) << std::endl;

}