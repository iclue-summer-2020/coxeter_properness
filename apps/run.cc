// Copyright (c) 2020 ICLUE @ UIUC. All rights reserved.

#include <gflags/gflags.h>
#include <coxeter_properness/search.h>
#include <prettyprint.hpp>
#include <coxeter/constants.h>
#include <coxeter/coxtypes.h>
#include <coxeter/interactive.h>
#include <coxeter/type.h>

#include <boost/math/special_functions/binomial.hpp>
#include <indicators/indicators.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>


DEFINE_string(type, "", "type");
DEFINE_uint32(rank, 0, "rank");
DEFINE_string(in_file, "", "in-file");
DEFINE_string(out_file, "", "out-file");

using coxeter_properness::Transformer;
using coxeter_properness::proper;


int main(int argc, char** argv) {
  gflags::SetUsageMessage("Iterate over elements of a finite Coxeter group.");
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  const auto type = FLAGS_type;
  const auto rank = FLAGS_rank;
  const auto in_file = FLAGS_in_file;
  const auto out_file = FLAGS_out_file;

  if (type.empty() || rank == 0 || in_file.empty() || out_file.empty()) {
    std::cout << "Need to set each of --type, --rank, --out-file, and --in-file flags." << std::endl;
    return EXIT_FAILURE;
  }

  std::ifstream ifs{in_file};
  if (!ifs.is_open()) {
    std::cout << "Could not open input file '" << in_file << "'" << std::endl;
    return EXIT_FAILURE;
  }

  std::ofstream ofs{out_file};
  if (!ofs.is_open()) {
    std::cout << "Could not open output file '" << out_file << "'" << std::endl;
    return EXIT_FAILURE;
  }

  // Always need to run this line first.
  constants::initConstants();
  const std::shared_ptr<CoxGroup> group{
    interactive::coxeterGroup(type.c_str(), rank)
  };
  Transformer transformer(group);
  CoxWord word;

  // indicators::ProgressSpinner spinner;
  std::string line;
  for (int numLinesProcessed = 0; std::getline(ifs, line); ++numLinesProcessed) {
    if (numLinesProcessed % 100000 == 0) {
      std::cout << "processed: " + std::to_string(numLinesProcessed) << std::endl;
    }

    if (!transformer.parse(line, &word)) {
      std::cout << "could not parse line '" << line << "'" << std::endl;
      continue;
    }
    if (!proper(*group, word)) continue;

    for (int i = 0; i < word.length(); ++i) {
      ofs << std::to_string(word[i]);
    }
    ofs << '\n';
    // ofs << std::endl;

    // spinner.set_option(indicators::option::PrefixText{
    //   "processed: " + std::to_string(numLinesProcessed)
    // });
  }

  return EXIT_SUCCESS;
}
