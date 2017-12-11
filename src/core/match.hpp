//
// Created by acid on 10/22/17.
//

#ifndef GAIL_MATCH_HPP
#define GAIL_MATCH_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include <ctime>
#include <algorithm>

#include "client.hpp"
#include "player.hpp"

namespace gail {

struct MatchResult {
  MatchResult(int turn_count, std::vector<int> scores, std::vector<clock_t> total_time)
      : turn_count(turn_count)
      , scores(std::move(scores))
      , total_time(std::move(total_time)) {
  }

  int turn_count;
  std::vector<int> scores;
  std::vector<clock_t> total_time;
};

// Plays a game between multiple players until none of them have any possible moves.
// Returns the final scores of each player.
template <typename State, typename Action>
MatchResult playMatch(std::vector<gail::Client<State, Action> *> clients,
                      std::vector<gail::Player<State, Action> *> players,
                      std::vector<Action> *actions = nullptr,
                      int skip_first_clients = 0
) {
  if (clients.size() != players.size()) {
    throw std::logic_error("Number of clients should be the same as number of players.");
  }
  if (clients.empty()) {
    throw std::logic_error("There should be at least one client.");
  }

  std::vector<clock_t> total_time(clients.size(), 0);
  int turn_count = 0;
  while (true) {
    bool had_moves = false;
    ++turn_count;
    for (int i = 0; i < clients.size(); ++i) {
      if (skip_first_clients-- > 0)
        continue;
      if (!clients[i]->isGameFinished()) {
        clock_t t0 = clock();
        Action action = players[i]->takeAction(clients[i]->getState());
        total_time[i] += clock() - t0;
        clients[i]->makeAction(action);
        had_moves = true;
        if (actions) {
          actions->emplace_back(action);
        }
      }
    }
    if (!had_moves) {
      break;
    }
  }

  std::vector<int> scores(clients.size());
  std::transform(clients.begin(), clients.end(), scores.begin(), [] (Client<State, Action>* client) {
    return client->getScore();
  });
  return MatchResult(turn_count, std::move(scores), std::move(total_time));
}

}; // namespace gail

#endif //GAIL_MATCH_HPP
