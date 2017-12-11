#ifndef GAIL_ALPHA_BETTA_PLAYER_HPP
#define GAIL_ALPHA_BETTA_PLAYER_HPP

#include <iostream>
#include <memory>
#include <ctime>
#include <cassert>

#include "../../../core/player.hpp"
#include "../clients.hpp"
#include "../simulator.hpp"
#include "../../../utils/config.hpp"
#include "scorers.hpp"

namespace gail {
namespace fantastic_four {

namespace alpha_betta {
const int NO_ACTION = -1;
const int TIMEOUT_ACTION = -2;
}

const std::array<uint32_t, 7*8> hash_array0 = {
    0x1d556a56,0x28ae7bdf,0x9e7ce2a6,0x4de8b6c2,0xdf03edf6,0xe1339d4f,0xdd1f7df1,
    0xcbe71e4,0xee1fb308,0x5b9b00d2,0x46a62286,0x3d81151b,0xd4f34cc6,0xeea8d0df,
    0x879b387c,0x1aa6034b,0x3b315de1,0xfe50c8b3,0x9ea9aa2c,0x3d216dcf,0x2fc8713b,
    0xd21bda68,0xebb697e6,0x1f7d8082,0x64cbfa73,0x6356452a,0x51b3d503,0x27716e4a,
    0x23ec9eca,0xd5e4bb7b,0xdcd27f20,0x2314f1a6,0x507e7631,0x1f39a429,0xaaae7bd0,
    0x1378dc32,0x8907af00,0xa474fcb9,0x61a4cf36,0x5598b231,0xcbf7a575,0x6bed1ba9,
    0xfe78053b,0x170cd680,0x3f652259,0x3f520403,0x7229648a,0xef272001,0x9486bc88,
    0xccafd2be,0xf7bf9e8c,0x60c3701f,0x738c444a,0x4f4f6cc2,0xb6a25871,0x1c663bf5 };

const std::array<uint32_t, 7*8> hash_array1 = {
    0xf2cbde9c,0xb8e82fc6,0x62037487,0x9949e0f1,0xb2d47a72,0x5310582c,0xf87d1380,
    0xac841b53,0x9451531a,0x426d57de,0xaed69a70,0xaa1ac4a7,0xf1cd010b,0xdd0e7cd2,
    0x96c94ce9,0xe7d1d8b2,0x2c90e50a,0x64c1eb3d,0xb58209c5,0xe3e2391,0xad93b765,
    0x48e0c306,0xe61b3317,0x6bcbb74e,0x1835ced2,0xf6ede088,0x177edf,0xcb995008,
    0x977061e2,0xed3246ea,0xc50cf35b,0xdd4eff7e,0x1c845cef,0x192c351c,0x4939c4e6,
    0xb9fbcae4,0xe33ff40d,0xf659a8fe,0x319c7b08,0x79404da3,0x84540a62,0x9bd4afb4,
    0x6f444757,0xa19ffe20,0xde102edf,0x50037966,0xdff08958,0xc79e00f8,0x950dd5f2,
    0xcaf55e16,0xe96e11fe,0x38cc992,0xc2eb086b,0x32c744aa,0x59d5fb1d,0xec3f0241 };

struct HashField {
private:
  uint32_t h;
public:
  HashField(uint32_t h = 0) : h(h) {}
  bool operator == (const HashField& o) const { return h == o.h; }
  bool operator != (const HashField& o) const { return h != o.h; }
  HashField make(int player, int i, int j) const {
    if (player == FIRST_PLAYER) return HashField(h ^ hash_array0[i * W + j]);
    if (player == SECOND_PLAYER) return HashField(h ^ hash_array1[i * W + j]);
  }
  HashField(const Field& f) : h(0) {
    for (int i = 0; i < H; ++i) {
      for (int j = 0; j < W; ++j) {
        if (f[i][j] == FIRST_PLAYER) h ^= hash_array0[i * W + j];
        if (f[i][j] == SECOND_PLAYER) h ^= hash_array1[i * W + j];
      }
    }
  }
  friend class Hash;
};

struct Hash {
  size_t operator () (const HashField& hf) const {
    return hf.h;
  }
};

class AlphaBettaPlayer : public Player<PlayerState, PlayerAction> {
public:
  explicit AlphaBettaPlayer(int player_id, Config config)
      : player_id(player_id) {
    scorer.reset(config.get<Scorer *>("scorer"));
    start_depth = config.get("start_depth", 1);
    end_depth = config.get("end_depth", 8 * 7);
    max_turn_time = config.get<int>("max_turn_time", 0);
    print_debug_info = config.get("print_debug_info", false);
    cache_max_size = config.get("cache_max_size", 0);
    cache_min_depth = config.get("cache_min_depth", 0);
  }

  PlayerAction takeAction(const PlayerState& state) override {
    PlayerAction best_action(-1);
    for (int j = 0; j < W; ++j) if (state.field[0][j] == 0) {
      best_action.column = j;
      break;
    }
    startTime = clock();
    std::tuple<int, int, int, int, size_t> dbg;
    for (int depth = start_depth; depth <= end_depth; ++depth) {
      cache.clear();
      auto actionWithScore = findBestAction(player_id, state, HashField(state.field), depth, -INF, INF);
      if (actionWithScore.first.column < 0) break;
      if (print_debug_info) {
        dbg = std::make_tuple(depth, getTimeMs(), actionWithScore.first.column, actionWithScore
                                  .second,
                              cache.size());
      }
      if (actionWithScore.second < -INF / 2) break;
      best_action = actionWithScore.first;
      if (actionWithScore.second > INF / 2) break;
    }
    if (print_debug_info) {
      std::cerr << "[abp]" <<
                " depth: " << std::get<0>(dbg) <<
                " time: " << std::get<1>(dbg) <<
                " action: " << std::get<2>(dbg) <<
                " best: " << best_action.column <<
                " score: " << std::get<3>(dbg) <<
                " cache: " << std::get<4>(dbg) << std::endl;
    }
    return best_action;
  }

  int getTimeMs() const {
    clock_t currentTime = clock();
    return (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;
  }

private:
  std::pair<PlayerAction, int>
  findBestAction(int current_player_id, const PlayerState& state,
                 const HashField& hf, int depth, int alpha, int betta) {
    if (state.winner == current_player_id) {
      return std::make_pair(PlayerAction(alpha_betta::NO_ACTION), INF);
    } else if (state.winner == oppositePlayer(current_player_id)) {
      return std::make_pair(PlayerAction(alpha_betta::NO_ACTION), -INF);
    } else if (state.winner == DRAW) {
      return std::make_pair(PlayerAction(alpha_betta::NO_ACTION), 0);
    }
    size_t min_invalidate_size = cache.max_load_factor() * cache.bucket_count(); // (23.2.5/14)
    if (depth == 0) {
      return std::make_pair(PlayerAction(alpha_betta::NO_ACTION),
                            scorer->score(state.field, current_player_id));
    }
    if (max_turn_time > 0 && getTimeMs() > max_turn_time * 0.9) {
      return std::make_pair(PlayerAction(alpha_betta::TIMEOUT_ACTION), 0);
    }
    // TODO(gchebanov) process depth=0, cache_min_depth=0 correctly
    auto it = cache.end();
    if (depth >= cache_min_depth) {
      if (cache.size() < cache_max_size) {
        it = cache.emplace(hf, -INF).first;
      } else {
        it = cache.find(hf);
      }
    }
    int next_player_id = oppositePlayer(current_player_id);
    std::pair<PlayerAction, int> bestActionWithScore(
        PlayerAction(alpha_betta::NO_ACTION), -INF);
    if (it != cache.end()) {
      bestActionWithScore.second = it->second;
      if (bestActionWithScore.second > betta) {
        return bestActionWithScore;
      }
    }

    for (int column = 0; column < W; ++column) {
      Simulator simulator(state.field);
      Action action(current_player_id, column);
      if (simulator.isValidAction(action)) {
        simulator.makeAction(action);
        auto next_state = simulator.getState();
        int row = 0;
        while (row < H && next_state.field[row][column] == 0) ++row;
        assert(row < H);
        auto actionWithScore = findBestAction(next_player_id,
                                              PlayerState(next_state.winner, player_id,
                                                          next_state.field),
                                              hf.make(current_player_id, column, row),
                                              depth - 1, -betta, -alpha);
        if (actionWithScore.first.column == alpha_betta::TIMEOUT_ACTION) {
          return actionWithScore;
        }
        actionWithScore.second *= -1;

        if (actionWithScore.second > bestActionWithScore.second) {
          bestActionWithScore.first = PlayerAction(column);
          bestActionWithScore.second = actionWithScore.second;
          if (alpha <= actionWithScore.second) {
            alpha = actionWithScore.second;
            if (alpha > betta)
              break;
          }
        }
      }
    }
    if (it != cache.end()) {
      if (cache.size() >= min_invalidate_size) {
        it = cache.find(hf);
      }
      if (it->second < bestActionWithScore.second) {
        it->second = bestActionWithScore.second;
      }
    }
    return bestActionWithScore;
  }

  int player_id;
  int max_turn_time;
  int start_depth;
  int end_depth;
  bool print_debug_info;
  std::unique_ptr<Scorer> scorer;
  clock_t startTime = 0;
  std::unordered_map<HashField, int, Hash> cache;
  size_t cache_max_size;
  int cache_min_depth;
};

}; // namespace fantastic_four
}; // namespace gail

#endif //GAIL_ALPHA_BETTA_PLAYER_HPP
