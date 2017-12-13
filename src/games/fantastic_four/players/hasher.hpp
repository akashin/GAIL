#ifndef GAIL_HASHER_HPP
#define GAIL_HASHER_HPP

#include <cstdint>
#include <array>
#include <cassert>

namespace gail {
namespace fantastic_four {

const std::array<uint64_t, 7*8> hash_array0 = {
    0x4d2b6a7a973af34f,0x8fbbc77eb564d3a5,0xf6bc60e8d1439200,0x16233d2ea6e2b76a,
    0x303419aa2e32a626,0x178eb16979b972a4,0x84c8513e53ba7a59,0x47c50b9540af7cc7,
    0x27af488883245047,0xf1aa5d788eb676a5,0xf7a8296916a3956e,0xf9d3b162734b83fe,
    0xf612af32ee470e47,0xec30075e2ac0508e,0x3ebbaa21e17ec33a,0xe1ff9f2d14bfef8e,
    0x7d19c827e2c5bb45,0x05eb47a00bf51aa5,0x4e00f1eb517095a0,0x3d15776ee40fc7c4,
    0x6841fe144f8d443b,0xe4e063db1a722f36,0x9eeeb78ffb511268,0xf0760b65b82bdccb,
    0x86cb2bd663acbfa1,0xe311a7c2e17df728,0x4112d89fdec0d0df,0x3483ae8e82d42952,
    0x9470df62a3cb711a,0x4cf7ed7ad732caad,0x922819f492b5bad8,0xcc3b9d3dac499b56,
    0x369dd73d41161075,0x19917a906297c99f,0xec3695ec9604b4fe,0x46fc1b4d5a058ee5,
    0xb753f7519ef557f8,0xea62dfcaabd87ce7,0x1dc238ad31aa6f2c,0xac386f8b6780506b,
    0xd6becc038107744b,0x8ed5d40f2f5797db,0xabc200693397d39e,0x0b94341fd5e2bced,
    0xbd17974bf129b089,0x0d10ee04b0edf508,0x7bee8f8c749e5ca8,0x6164ac2a64ccc058,
    0x0869b46c7f35bcf5,0x8d406a7b4c5465a8,0xfabc752729627fc5,0x5980ec87a23efd35,
    0xf9679f461711de4a,0xdf73a0dda014e2d3,0x27d4367130f2ceb5,0xbf868dadb60330a9,
};

const std::array<uint64_t, 7*8> hash_array1 = {
    0x71b64c39a5c25be2,0x8a1105ae8e1a98c5,0x4cc889aa7798c07e,0xf9035bea4437c5e0,
    0xae4b4d14261face1,0x2711c0e077046ee9,0x453d1021967edd11,0x95aee752e519160b,
    0x9f529ea459d9eb1f,0x0d8a00486524c1b0,0xfded1182a9c07671,0x69b36c43c513e7d9,
    0x8d7a443f4dea280f,0x328131e287c120cb,0x083dcc09e6ef254c,0x0f826963b4a6f383,
    0xd0157b402ab4a07b,0xecd1f3e85d0e7a68,0x67e5d4b64b318ad9,0xfc3a9780b7386f4b,
    0xed12c1673ba8d06e,0xe38c97c3de259a9f,0x740afa4c33c23786,0x4de4d928f9ed3f34,
    0x553bd1807d625593,0xffeaff20d464b35b,0x2bddf4bbb695dddb,0xde58280cbfd191e8,
    0xa1267ddd30a3e60b,0xc5c68b9f2463b9c4,0xf4fbf4bb649ff5a6,0x3b4c8839aa0b0f2a,
    0x6610ca66ec71f9d4,0xbea7f78bd409ec13,0x0ad99f2df7c2a106,0x569be60e044cc540,
    0x09dd47e33bfa60aa,0x31533bd16a2f95bd,0xfc1dc76cdadb0c91,0x649ffc32870207f3,
    0x3ce6a94f1a440a80,0xe2c9eb0ac7a25baa,0x11c877c8f2453167,0x2bb493ae6384742c,
    0x50691763a549fcc3,0xd0ad6cd97d79d6e0,0xb88679fcf43a8a7a,0x0169b66634566cd3,
    0x011fba333d03e9e3,0xc9e35a841883d791,0x0fa77ee0f9202ca0,0x2377cfe839cb8371,
    0x5869f8c4c0a1e240,0x39aabe43a46ba18b,0x20ace17b65469444,0x4d91e26dc7a0ef8d,
};

struct HashField {
private:
  uint64_t h;
public:
  HashField(uint64_t h = 0) : h(h) {}
  bool operator == (const HashField& o) const { return h == o.h; }
  bool operator != (const HashField& o) const { return h != o.h; }
  bool operator < (const HashField& o) const { return h < o.h; }
  HashField make(int player, int i, int j) const {
    if (player == FIRST_PLAYER) return HashField(h ^ hash_array0[i * W + j]);
    assert(player == SECOND_PLAYER);
    return HashField(h ^ hash_array1[i * W + j]);
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

int upperRow(const Field& f, int column) {
  int row = 0;
  while (row < H && f[row][column] == 0) ++row;
  assert(row > 0);
  return row - 1;
}

int lastRow(const Field& f, int column) {
  int row = 0;
  while (row < H && f[row][column] == 0) ++row;
  assert(row < H);
  return row;
}

} // namespace fantastic_four
} // namespace gail

#endif //GAIL_HASHER_HPP
