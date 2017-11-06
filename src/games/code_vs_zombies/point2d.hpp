//
// Created by acid on 11/6/17.
//

#ifndef GAIL_POINT2D_HPP
#define GAIL_POINT2D_HPP

#include <istream>
#include <cmath>

namespace gail {
namespace code_vs_zombies {

struct Point2D {
  Point2D(int x, int y)
      : x(x), y(y) {}

  Point2D(): Point2D(0, 0) {}

  int x, y;

  bool operator == (const Point2D& other) const {
    return x == other.x && y == other.y;
  }
};

int dist2(const Point2D& p) {
  return p.x * p.x + p.y * p.y;
}

int dist2(const Point2D& p, const Point2D& q) {
  return dist2(Point2D(p.x - q.x, p.y - q.y));
}

std::istream& operator >> (std::istream& is, Point2D& point) {
  is >> point.x >> point.y;
  return is;
}

}; // namespace gail
}; // namespace code_vs_zombies

#endif //GAIL_POINT2D_HPP
