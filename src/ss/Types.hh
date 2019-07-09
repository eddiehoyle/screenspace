// Copyright 2019 Edward Hoyle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SCREENSPACE_TYPES_HH
#define SCREENSPACE_TYPES_HH

namespace screenspace {

enum class Shape {
  Circle,
  Rectangle,
  Triangle,
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
