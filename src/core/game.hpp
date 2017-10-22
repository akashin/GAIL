//
// Created by acid on 10/22/17.
//
#ifndef PROJECT_GAME_HPP
#define PROJECT_GAME_HPP

template <typename GameState, typename Action>
class Game {
public:
  virtual ~Game() = default;

  virtual const GameState& getState() = 0;

  virtual void makeAction(const Action& action) = 0;
};

#endif //PROJECT_GAME_HPP
