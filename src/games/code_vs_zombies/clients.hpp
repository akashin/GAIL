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

namespace gail {
namespace code_vs_zombies {

class StreamClient : public Client<State, Action> {
public:
  StreamClient(std::istream& state_input_stream, std::ostream& action_output_stream)
      : state_input_stream(state_input_stream), action_output_stream(action_output_stream) {}

  ~StreamClient() override = default;

  int getScore() override {
    return 0;
  }

  bool isGameFinished() override {
    return false;
  }

  State getState() override {
    if (!state_refreshed) {
      readState();
      state_refreshed = true;
    }
    return state;
  }

  void makeAction(const Action& action) override {
    action_output_stream << action.target.x << " " << action.target.y << std::endl;
    state_refreshed = false;
  }

private:
  void readState() {
    state_input_stream >> state;
  }

  std::istream& state_input_stream;
  std::ostream& action_output_stream;
  bool state_refreshed = false;
  State state;
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
