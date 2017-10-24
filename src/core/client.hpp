//
// Created by acid on 10/24/17.
//

#ifndef GAIL_CLIENT_HPP
#define GAIL_CLIENT_HPP

namespace gail {

template <typename State, typename Action>
class Client {
public:
  virtual ~Client() = default;

  virtual int getScore() = 0;

  virtual bool isGameFinished() = 0;

  virtual State getState() = 0;

  virtual void makeAction(const Action& action) = 0;
};

}; // namespace gail

#endif //GAIL_CLIENT_HPP
