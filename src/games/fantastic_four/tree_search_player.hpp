//
// Created by acid on 10/23/17.
//

#ifndef GAIL_TREE_SEARCH_PLAYER_HPP
#define GAIL_TREE_SEARCH_PLAYER_HPP

#include "../../core/player.hpp"
#include "clients.hpp"
#include "simulator.hpp"

namespace gail {
namespace fantastic_four {

const int INF = 1e9;

// TODO(akashin): Can we reuse this?
bool isValid(int row, int column) {
  return row >= 0 && column >= 0 && row < H && column < W;
}

int oppositePlayer(int player_id) {
  if (player_id == FIRST_PLAYER) {
    return SECOND_PLAYER;
  } else if (player_id == SECOND_PLAYER) {
    return FIRST_PLAYER;
  }
  throw std::logic_error("Invalid player: " + std::to_string(player_id));
}

int scoreState(PlayerState state) {
  int totalScore = 0;
  for (int row = 0; row < H; ++row) {
    for (int col = 0; col < W; ++col) {
      for (int d = 0; d < dRow.size(); ++d) {
        if (state.field[row][col] == NO_PLAYER) {
          continue;
        }

        int count = 0;
        for (int k = 0; k < WIN_LENGTH; ++k) {
          int nRow = row + k * dRow[d];
          int nCol = col + k * dCol[d];

          if (!isValid(nRow, nCol)) {
            break;
          }

          if (state.field[nRow][nCol] != state.field[row][col]) {
            break;
          }
          ++count;
        }

        if (count >= WIN_LENGTH) {
          // TODO(akashin): Maybe more principled approach?
          totalScore = INF / 16;
        } else {
          totalScore += count * count * count;
        }
      }
    }
  }
  return totalScore;
}

class TreeSearchPlayer : public Player<PlayerState, PlayerAction> {
public:
  TreeSearchPlayer(int player_id)
      : player_id(player_id) {}

  PlayerAction takeAction(const PlayerState& state) override {
    return findBestAction(player_id, state, 1).first;
  }

private:
  std::pair<PlayerAction, int> findBestAction(int current_player_id, const PlayerState& state, int depth) {
    if (depth == 0) {
      // TODO(akashin): -1 is invalid move.
      return std::make_pair(PlayerAction(-1), scoreState(state));
    }
    int next_player_id = oppositePlayer(current_player_id);
    std::pair<PlayerAction, int> bestActionWithScore(-1, -INF);
    for (int column = 0; column < W; ++column) {
      Simulator simulator(state.field);
      Action action(current_player_id, column);
      if (simulator.isValidAction(action)) {
        auto next_state = simulator.getState();
        auto actionWithScore = findBestAction(next_player_id,
                                              PlayerState(next_state.winner, player_id, next_state.field),
                                              depth - 1);
        if (player_id != current_player_id) {
          actionWithScore.second *= -1;
        }

        if (actionWithScore.second > bestActionWithScore.second) {
          bestActionWithScore = actionWithScore;
          bestActionWithScore.first = PlayerAction(column);
        }
      }
    }
    return bestActionWithScore;
  }

  int player_id;

};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_TREE_SEARCH_PLAYER_HPP
