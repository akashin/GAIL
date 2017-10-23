//
// Created by acid on 10/23/17.
//

#ifndef GAIL_FANTASTIC_FOUR_HPP
#define GAIL_FANTASTIC_FOUR_HPP

#include <cstdint>
#include <array>
#include <istream>
#include <ostream>

namespace gail {
namespace fantastic_four {

const int H = 7;
const int W = 8;
using Field = std::array<std::array<int8_t, W>, H>;

const std::array<int, 4> dRow = {1, 0, 1, -1};
const std::array<int, 4> dCol = {0, 1, 1, 1};

const int NO_PLAYER = 0;
const int FIRST_PLAYER = 1;
const int SECOND_PLAYER = 2;
const int DRAW = 3;

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_FANTASTIC_FOUR_HPP
