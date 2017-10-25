//
// Created by acid on 10/25/17.
//

#ifndef GAIL_PLAYERS_HPP
#define GAIL_PLAYERS_HPP

#include "clients.hpp"
#include "../../core/player.hpp"

namespace gail {
namespace code_vs_zombies {

class PickLastHumanPlayer : public Player<State, Action> {
public:
  Action takeAction(const State& state) override {
    return Action(state.humans.back().pos);
  }
};

}; // namespace code_vs_zombies
}; // namespace gail

#endif //GAIL_PLAYERS_HPP
