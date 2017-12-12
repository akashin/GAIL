//
// Created by acid on 11/14/17.
//

#ifndef GAIL_STREAM_CLIENT_HPP
#define GAIL_STREAM_CLIENT_HPP

#include <istream>
#include "client.hpp"

namespace gail {

template <typename State, typename Action>
class StreamClientBase : public Client<State, Action> {
public:
  StreamClientBase(std::istream& state_input_stream, std::ostream& action_output_stream)
      : Client<State, Action>()
      , state_input_stream(state_input_stream)
      , action_output_stream(action_output_stream) {
  }

  ~StreamClientBase() override = default;

  State getState() override {
    if (!state_refreshed) {
      state_input_stream >> state;
      state_refreshed = true;
    }
    return state;
  }

  void makeAction(const Action& action) override {
    action_output_stream << action << std::endl;
    state_refreshed = false;
  }

protected:
  std::istream& state_input_stream;
  std::ostream& action_output_stream;
  bool state_refreshed = false;
  State state;

};

}; // namespace gail

#endif //GAIL_STREAM_CLIENT_HPP
