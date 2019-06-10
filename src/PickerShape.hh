#include <maya/MPxLocatorNode.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionMask.h>

namespace screenspace {

class PickerShape : public MPxLocatorNode {

public:
  static MTypeId id;
  static MString typeName;
  static void* creator();
  static MStatus initialize();

public:
  PickerShape() = default;
  virtual ~PickerShape() = default;

private:
  static MObject m_shape;
  static MObject m_color;
  static MObject m_opacity;
  static MObject m_fill;
  static MObject m_lineWidth;
  static MObject m_lineStyle;

  static MObject m_size;
  static MObject m_width;
  static MObject m_height;

  static MObject m_depth;
  static MObject m_layout;
  static MObject m_verticalAlign;
  static MObject m_horizontalAlign;
  static MObject m_offset;

  static MObject m_camera;

  // Debug
  static MObject m_outMatrix;
};

}