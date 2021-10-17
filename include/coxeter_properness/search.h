// Copyright (c) 2021 ICLUE @ UIUC. All rights reserved.

#ifndef COXETER_PROPERNESS_SEARCH_H_
#define COXETER_PROPERNESS_SEARCH_H_

#include <cstdint>
#include <memory>
#include <ostream>
#include <set>
#include <vector>

#include <coxeter/coxtypes.h>
#include <coxeter/coxgroup.h>

namespace coxeter_properness {

bool proper(const coxtypes::CoxGroup& W, const coxtypes::CoxWord& w);

class Transformer {
 public:
  Transformer(const std::shared_ptr<coxtypes::CoxGroup>& W);
  ~Transformer() = default;

  bool parse(const std::string& line, coxtypes::CoxWord* w);

 private:
  const std::shared_ptr<coxtypes::CoxGroup> W;
  coxtypes::CoxWord rawWord;
  std::vector<int> elements;
};

}  // namespace coxeter_properness

#endif  // COXETER_PROPERNESS_SEARCH_H_
