//
// Created by acid on 10/22/17.
//

#ifndef GAIL_MATCH_HPP
#define GAIL_MATCH_HPP

#include <vector>
#include <stdexcept>
#include <iostream>

#include "client.hpp"
#include "player.hpp"

namespace gail {

struct MatchResult {
  MatchResult(int turn_count, std::vector<int> scores)
      : turn_count(turn_count), scores(std::move(scores)) {}

  int turn_count;
  std::vector<int> scores;
};

// Plays a game between multiple players until none of them have any possible moves.
// Returns the final scores of each player.
template <typename State, typename Action>
MatchResult playMatch(std::vector<gail::Client<State, Action> *> clients,
                      std::vector<gail::Player<State, Action> *> players) {
  if (clients.size() != players.size()) {
    throw std::logic_error("Number of clients should be the same as number of players.");
  }
  if (clients.empty()) {
    throw std::logic_error("There should be at least one client.");
  }

  auto game_finished = [&]() {
    for (auto client : clients) {
      if (!client->isGameFinished()) {
        return false;
      }
    }
    return true;
  };

  int turn_count = 0;
  while (!game_finished()) {
    ++turn_count;
    for (int i = 0; i < clients.size(); ++i) {
      if (!clients[i]->isGameFinished()) {
        clients[i]->makeAction(players[i]->takeAction(clients[i]->getState()));
      }
    }
  }

  std::vector<int> scores;
  for (auto client : clients) {
    scores.push_back(client->getScore());
  }
  return MatchResult(turn_count, std::move(scores));
}

}; // namespace gail

#endif //GAIL_MATCH_HPP
