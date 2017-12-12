#ifndef GAIL_STRATEGY_MCTS_PLAYER_HPP
#define GAIL_STRATEGY_MCTS_PLAYER_HPP

#include <memory>
#include <ctime>
#include <map>

#include "../simulator.hpp"
#include "../../../core/player.hpp"
#include "scorers.hpp"
#include "score_utils.hpp"
#include "hasher.hpp"
#include "../clients.hpp"
#include "../../../utils/config.hpp"

namespace gail {
namespace fantastic_four {

struct MCTSNode {
  PlayerState st;
  std::array<MCTSNode*, W> child;
  int score, trials;
  MCTSNode() :st(), score(0), trials(0) {
    child.fill(nullptr);
  }
};

template<typename Rnd>
class MCTSPlayer : public Player<PlayerState, PlayerAction> {
public:
  MCTSPlayer(int player_id, Config config)
      : player_id(player_id) {
    rnd.reset(config.get<Rnd *>("rnd"));
    scorer.reset(config.get<Scorer *>("scorer"));
    max_turn_time = config.get<int>("max_turn_time");
    max_turn_sims = config.get<int>("max_turn_sims");
    print_debug_info = config.get("print_debug_info", false);
    C = config.get("C", 1.0);
  }

  PlayerAction takeAction(const PlayerState &state) override {
    startTime = clock();
    HashField hf(state.field);
    auto* root = getNode(hf); root->st = state;
    int sims = 0;
    while (true) {
      if (max_turn_sims && ++sims > max_turn_sims) break;
      if (max_turn_time && getTimeMs() >= max_turn_time) break;
      go(root);
    }
    auto best = mostTrials(root);
    if (print_debug_info) {
      auto score = root->trials ? double(root->score)/root->trials : 0;
      std::cerr << "[mcts] root" <<
        " score: " << score <<
        " trials: " << root->trials << std::endl;
      if (auto* c = best.action.isCorrect() ?
        root->child[best.action.column] : nullptr) {
        auto cscore = c->trials ? double(c->score)/c->trials : 0;
        std::cerr << "[mcts] child" <<
          " score: " << cscore <<
          " trials: " << c->trials << std::endl;
      }
      std::cerr << "[mcts] best" <<
        " action: " << best.action.column <<
        " time: " << getTimeMs() << std::endl;
    }
    return best.action;
  }

  std::string name() const {
      return "MCTSPlayer("
             " max_turn_time = " + std::to_string(max_turn_time)
             + ", max_turn_sims = " + std::to_string(max_turn_sims)
             + ")";
  }
private:
  ActionWithScore mostTrials(MCTSNode* t) {
    ActionWithScore best{0};
    for (int i = 0; i < W; ++i) {
      if (t->child[i] == nullptr) continue;
      ActionWithScore counter{PlayerAction(i), t->child[i]->trials};
      if (best < counter) best = counter;
    }
    return best;
  }

  const HashField& getHF(MCTSNode* t) {
    using pair = std::pair<HashField, MCTSNode>;
    ssize_t off = offsetof(pair, second);
    return *reinterpret_cast<HashField*>(reinterpret_cast<char*>(t) - off);
  }

  void go(MCTSNode* t) {
    std::array<MCTSNode*, W*H> stack; size_t ss = 0;
    while (t->st.winner == NO_PLAYER) {
      stack[ss++] = t;
      std::array<int, W> random_moves; size_t nrand = 0;
      std::array<int, W> tree_moves; size_t ntree = 0;
      for (int j = 0; j < W; ++j) {
        if (!canMove(t->st.field, j)) continue;
        if (t->child[j] == nullptr) random_moves[nrand++] = j;
        else tree_moves[ntree++] = j;
      }
      if (nrand) {
        int id = std::uniform_int_distribution<int>(0, nrand - 1)(*rnd);
        int move = random_moves[id];
        auto hf = getHF(t);
        int row = upperRow(t->st.field, move);
        auto nhf = hf.make(t->st.player_id, row, move);
        auto* p = t->child[move] = getNode(nhf);
        Simulator sim(t->st.field);
        sim.makeAction(Action(t->st.player_id, move));
        auto next_state = sim.getState();
        p->st = PlayerState(next_state.winner, oppositePlayer(t->st.player_id), next_state.field);
        t = p;
        break;
      } else {
        int move = getBestMove(t);
        t = t->child[move];
      }
    }
    int r = 0;
    if (t->st.winner == NO_PLAYER) {
      int sc = scorer->score(t->st.field, t->st.player_id);
      r = (sc > 0 ? 1: (sc < 0 ? -1: 0));
    } else if (t->st.winner == t->st.player_id) {
      r = 1;
    } else if (t->st.winner == oppositePlayer(t->st.player_id)) {
      r = -1;
    }
    t->score += r; t->trials += 1;
    while (ss > 0) {
      r = -r; t = stack[--ss];
      t->score += r; t->trials += 1;
    }
  }

  int getBestMove(MCTSNode* t) {
    int best = -1; double best_w;
    for (int i = 0; i < W; ++i) {
      if (t->child[i] == nullptr) continue;
      auto* p = t->child[i];
      double w = -double(p->score) / p->trials;
      w += C * sqrt(logf(t->trials) / p->trials);
      if (best == -1 || best_w < w) {
        best = i; best_w = w;
      }
    }
    return best;
  }

  int getTimeMs() const {
    clock_t currentTime = clock();
    return (currentTime - startTime) * 1000 / CLOCKS_PER_SEC;
  }

  MCTSNode* getNode(const HashField& hf) {
    auto it = tree.lower_bound(hf);
    if (it->first != hf) {
      it = tree.emplace_hint(it, hf, MCTSNode());
    }
    return &(it->second);
  }

  int player_id;
  int max_turn_time;
  int max_turn_sims;
  double C;
  bool print_debug_info;
  std::unique_ptr<Rnd> rnd;
  std::unique_ptr<Scorer> scorer;
  std::map<HashField, MCTSNode> tree;
  clock_t startTime = 0;
};

} // namespace fantastic_four
} // namespace gail

#endif //GAIL_STRATEGY_MCTS_PLAYER_HPP
