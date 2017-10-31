//
// Created by acid on 10/24/17.
//

#ifndef GAIL_CLIENT_HPP
#define GAIL_CLIENT_HPP

namespace gail {

// An interface for the frontend to the game engine that will be used by the players to
// interact with the game.
template <typename State, typename Action>
class Client {
public:
  virtual ~Client() = default;

  // Returns current game score of this client.
  virtual int getScore() = 0;

  // Returns |true| if game is finished and |false| otherwise.
  virtual bool isGameFinished() = 0;

  // Returns current observation of this client.
  virtual State getState() = 0;

  // Submits an action for this client.
  virtual void makeAction(const Action& action) = 0;
};

}; // namespace gail

#endif //GAIL_CLIENT_HPP
