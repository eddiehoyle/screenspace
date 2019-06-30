#ifndef SCREENSPACE_TYPES_HH
#define SCREENSPACE_TYPES_HH

namespace screenspace {

enum class Shape {
  Circle,
  Rectangle,
};

enum class Position {
  Relative,
  Absolute,
};

enum class VerticalAlign {
  Bottom,
  Middle,
  Top,
};

enum class HorizontalAlign {
  Left,
  Middle,
  Right,
};

}

#endif // SCREENSPACE_TYPES_HH
