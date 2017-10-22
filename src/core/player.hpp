//
// Created by acid on 10/22/17.
//

#ifndef GAIL_PLAYER_HPP
#define GAIL_PLAYER_HPP

namespace gail {

template <typename State, typename Action>
class Player {
public:
  virtual Action takeAction(const State& state) = 0;
};

} // namespace gail

#endif //GAIL_PLAYER_HPP
