//
// Created by acid on 10/25/17.
//

#ifndef GAIL_CLIENTS_HPP
#define GAIL_CLIENTS_HPP

#include <vector>
#include <iostream>
#include <cmath>

#include "../../core/client.hpp"
#include "code_vs_zombies.hpp"
#include "simulator.hpp"
#include "../../core/stream_client.hpp"

namespace gail {
namespace code_vs_zombies {

class StreamClient : public StreamClientBase<State, Action> {
public:
  StreamClient(std::istream& state_input_stream, std::ostream& action_output_stream)
      : StreamClientBase(state_input_stream, action_output_stream) {}

  int getScore() override {
    return 0;
  }

  bool isGameFinished() override {
    return false;
  }

};

class SimulatorClient : public Client<State, Action> {
public:
  explicit SimulatorClient(Simulator& simulator)
      : simulator(simulator) {}

  int getScore() override {
    return simulator.getScore();
  }

  bool isGameFinished() override {
    return simulator.isFinished();
  }

  State getState() override {
    return simulator.getState();
  }

  void makeAction(const Action& action) override {
    simulator.makeAction(action);
  }

private:
  Simulator& simulator;
};

}; // namespace code_vs_zombies
}; // namespace gail

#endif //GAIL_CLIENTS_HPP
