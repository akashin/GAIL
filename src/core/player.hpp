//
// Created by acid on 10/22/17.
//

#ifndef GAIL_PLAYER_HPP
#define GAIL_PLAYER_HPP

namespace gail {

// An interface for the player/strategy that plays the game.
template <typename State, typename Action>
class Player {
public:
  // Returns an action to take in the given state.
  virtual Action takeAction(const State& state) = 0;
};

} // namespace gail

#endif //GAIL_PLAYER_HPP
