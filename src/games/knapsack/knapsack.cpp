//
// Created by Andrei Kashin on 24/11/2017.
//

#include <unordered_set>
#include <set>
#include "clients.hpp"
#include "players.hpp"
#include "../../core/match.hpp"

using namespace gail::knapsack;

const double EPS = 1e-7;

State generateState(int itemCount, int capacity) {
  State state;

  state.capacity = capacity;
  for (int i = 0; i < itemCount; ++i) {
    state.items.emplace_back(i, i * i, i);
  }

  return state;
}

void solve_greedily(const State& state) {
  OneStateClient client(state);
  GreedyPlayer player;

  auto results = gail::playMatch<State, Action>({&client}, {&player});
  std::cout << "Greedy score: " << results.scores[0] << std::endl;
}

float clamp(float x, float lo, float hi) {
  return std::min(std::max(x, lo), hi);
}

void solve_with_evolution(const State& state) {
  int populationSize = 500;
  int iterationCount = 10000;
  float stepSize = 0.000005;
  int maxScore = -1000000000;

  EvolutionParameters parameters = generateEvolutionParameters(state);
  for (int iteration = 0; iteration < iterationCount; ++iteration) {
    std::vector<std::pair<Action, int>> populationWithScore;
    float meanScore = 0;
    for (int i = 0; i < populationSize; ++i) {
      OneStateClient client(state);
      Action action(parameters.sample());
      OneActionClient player(action);

      auto results = gail::playMatch<State, Action>({&client}, {&player});
      int score = results.scores.back();
      maxScore = std::max(maxScore, score);
      meanScore += score;
      populationWithScore.emplace_back(action, score);
    }
    meanScore /= populationSize;

    std::vector<float> gradient(parameters.probabilities.size(), 0.0);
    auto& probabilities = parameters.probabilities;
    for (int i = 0; i < populationSize; ++i) {
      std::vector<int> weights(parameters.probabilities.size(), 0);
      for (int item : populationWithScore[i].first.takenItems) {
        weights[item] = 1;
      }
      for (int j = 0; j < parameters.probabilities.size(); ++j) {
        float logProb;
        if (weights[j] == 1) {
          logProb = weights[j] / probabilities[j];
        } else {
          logProb = -(1 - weights[j]) / (1 - probabilities[j]);
        }
        float baseline = maxScore;
        gradient[j] += (populationWithScore[i].second - baseline) * logProb / populationSize;
      }
    }

    double gradNorm = 0;
    double entropy = 0;
    for (int j = 0; j < parameters.probabilities.size(); ++j) {
      gradNorm += gradient[j] * gradient[j];
      probabilities[j] += gradient[j] * stepSize;
      probabilities[j] = clamp(probabilities[j], 0 + EPS, 1 - EPS);
      entropy += -(probabilities[j] * log(probabilities[j]))
                 -(1 - probabilities[j]) * log(1 - probabilities[j]);
    }
    entropy /= parameters.probabilities.size();
    gradNorm = sqrt(gradNorm);
    if (iteration % (iterationCount / 100) == 0) {
      std::cerr << "Evolution score at iteration " << iteration << ": " << maxScore
                << ", gradient norm: " << gradNorm
                << ", entropy: " << entropy
//                << ", mean score: " << meanScore
                << std::endl;
    }
  }

//  auto results = gail::playMatch<State, Action>({&client}, {&player});
}

int main() {
  State state = generateState(100, 5000);
  solve_greedily(state);
  solve_with_evolution(state);
}
