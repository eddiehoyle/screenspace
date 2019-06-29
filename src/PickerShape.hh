#ifndef SAMPLEPLUGIN_PICKERSHAPE_HH
#define SAMPLEPLUGIN_PICKERSHAPE_HH

#include <maya/MPxSurfaceShape.h>

namespace screenspace {

class PickerShape : public MPxSurfaceShape {
public:
  static MTypeId id;
  static MString typeName;
  static void* creator();
  static MStatus initialize();

public:
  MSelectionMask getShapeSelectionMask() const override
  {
    return MSelectionMask::kSelectHandles;
  }

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
  static MObject m_offsetX;
  static MObject m_offsetY;
  static MObject m_offset;
};

}

#endif // SAMPLEPLUGIN_PICKERSHAPE_HH