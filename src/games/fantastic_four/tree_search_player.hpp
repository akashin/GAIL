//
// Created by acid on 10/23/17.
//

#ifndef GAIL_TREE_SEARCH_PLAYER_HPP
#define GAIL_TREE_SEARCH_PLAYER_HPP

#include "../../core/player.hpp"
#include "clients.hpp"

namespace gail {
namespace fantastic_four {

class TreeSearchPlayer : public Player<PlayerState, PlayerAction> {
public:
  PlayerAction takeAction(const PlayerState& state) override {
  }

private:
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_TREE_SEARCH_PLAYER_HPP
