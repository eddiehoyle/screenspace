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

#ifndef SAMPLEPLUGIN_PICKABLESHAPE_HH
#define SAMPLEPLUGIN_PICKABLESHAPE_HH

#include <maya/MPxSurfaceShape.h>

namespace screenspace {

class PickableShape : public MPxSurfaceShape {
public:
  static MTypeId id;
  static MString typeName;
  static void* creator();
  static MStatus initialize();

public:
  MSelectionMask getShapeSelectionMask() const override;

private:
  static MObject m_camera;
  static MObject m_shape;
  static MObject m_color;
  static MObject m_opacity;
  static MObject m_size;
  static MObject m_width;
  static MObject m_height;
  static MObject m_depth;
  static MObject m_position;
  static MObject m_horizontalAlign;
  static MObject m_verticalAlign;
  static MObject m_rotate;
  static MObject m_offsetX;
  static MObject m_offsetY;
  static MObject m_offset;
};

}

#endif // SAMPLEPLUGIN_PICKABLESHAPE_HH