// Copyright (c) 2021 ICLUE @ UIUC. All rights reserved.

#define CATCH_CONFIG_MAIN

#include <exception>
#include <iostream>
#include <numeric>

#include <catch2/catch.hpp>

TEST_CASE("easy", "") {
  REQUIRE(1 + 1 == 2);
}