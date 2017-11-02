//
// Created by acid on 11/3/17.
//

#ifndef GAIL_UTILS_HPP
#define GAIL_UTILS_HPP

#include "fantastic_four.hpp"

namespace gail {
namespace fantastic_four {

int oppositePlayer(int player_id) {
  if (player_id == FIRST_PLAYER) {
    return SECOND_PLAYER;
  } else if (player_id == SECOND_PLAYER) {
    return FIRST_PLAYER;
  }
  throw std::logic_error("Invalid player: " + std::to_string(player_id));
}

int getWinner(const Field& field) {
  int filledCount = 0;
  for (int row = 0; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      if (field[row][col] == NO_PLAYER) {
        continue;
      }
      ++filledCount;

      for (int d = 0; d < dRow.size(); ++d) {
        bool covered = true;
        for (int k = 0; k < WIN_LENGTH; ++k) {
          int nRow = row + k * dRow[d];
          int nCol = col + k * dCol[d];

          if (!isOnField(nRow, nCol)) {
            covered = false;
            break;
          }

          if (field[nRow][nCol] != field[row][col]) {
            covered = false;
            break;
          }
        }

        if (covered) {
          return field[row][col];
        }
      }
    }
  }

  if (filledCount == H * W) {
    return DRAW;
  }
  return NO_PLAYER;
}

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_UTILS_HPP
