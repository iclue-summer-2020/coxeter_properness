// Copyright 2020 ICLUE @ UIUC. All rights reserved.
#include <coxeter_properness/search.h>

#include <algorithm>
#include <climits>
#include <deque>
#include <numeric>
#include <ostream>
#include <cctype>
#include <stack>
#include <utility>
#include <vector>

#include <coxeter_properness/search.h>

#include <coxeter/constants.h>
#include <coxeter/coxtypes.h>
#include <coxeter/interactive.h>
#include <coxeter/type.h>

#include <boost/math/special_functions/binomial.hpp>

namespace coxeter_properness {

using interactive::CoxGroup;
using coxeter::CoxWord;

auto numLDescents(const CoxGroup& W, const CoxWord& w) -> int32_t {
  auto numDescents = 0;
  for (auto f1 = W.ldescent(w); f1 != 0; f1 &= (f1-1)) {
    // const auto s = bits::firstBit(f1);
    ++numDescents;
  }
  return numDescents;
}

auto choose(const int n, const int k) -> int32_t {
  if (n < k) return 0;
  return static_cast<int32_t>(boost::math::binomial_coefficient<double>(n, k));
}

auto maxw0(const CoxGroup& W, const CoxWord& w) -> int32_t {
  const auto n = W.rank();
  const auto dw = numLDescents(W, w);
  const auto& t = W.type();
  const auto& typeName = t.name();

  if (typeName == io::String("A"))  return choose(dw+1, 2);
  if (typeName == io::String("B")) return dw * dw;
  if (typeName == io::String("D")) return dw > 3 ? dw*(dw-1) : choose(dw+1, 2);
  if (typeName == io::String("E")) {
    const static std::vector<int32_t> m_e = {0, 1, 3, 6, 12, 20, 36, 63, 120};
    return m_e.at(dw);
  }
  if (typeName == io::String("F")) {
    const static std::vector<int32_t> m_f = {0, 1, 4, 9, 24};
    return m_f[dw];
  }
  if (typeName == io::String("G")) {
    const static std::vector<int32_t> m_g = {0, 1, 6};
    return m_g[dw];
  }
  if (typeName == io::String("H")) {
    const static std::vector<int32_t> m_h = {0, 1, 5, 15, 60};
    return m_h[dw];
  }
  if (typeName == io::String("I")) return dw == 2 ? n : dw;

  throw std::invalid_argument("no type matching");
}

bool proper(const CoxGroup& W, const CoxWord& w) {
  const auto n = W.rank();
  return w.length() <= n + maxw0(W, w);
}

Transformer::Transformer(const std::shared_ptr<CoxGroup>& W) : W(W) { }

bool Transformer::parse(const std::string& line, CoxWord* w) {
  elements.clear();
  for (const auto& x : line) {
    // This assumes that the rank of the group is < 10.
    if (std::isdigit(x)) {
      elements.push_back(x-'0');
    }
  }

  if (elements.empty()) return false;

  // Don't trust the coxeter code to handle dynamic resource allocation.
  rawWord.setLength(elements.size());
  for (int i = 0; i < elements.size(); ++i) {
    rawWord[i] = elements[i];
  }

  // W->reduced(*w, rawWord);
  // We can assume that the word is already reduced.
  *w = rawWord;
  return true;
}

}  // namespace coxeter_properness
