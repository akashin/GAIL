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

const int WIN_LENGTH = 4;

const std::array<int, 4> dRow = {1, 0, 1, -1};
const std::array<int, 4> dCol = {0, 1, 1, 1};

const int NO_PLAYER = 0;
const int FIRST_PLAYER = 1;
const int SECOND_PLAYER = 2;
const int DRAW = 3;

std::ostream &operator<<(std::ostream &os, const Field &field) {
  for (int row = 0; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      os << int(field[row][col]) << " ";
    }
    os << std::endl;
  }
  return os;
}

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_FANTASTIC_FOUR_HPP
