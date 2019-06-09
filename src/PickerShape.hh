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

//  bool isBounded() const override { return true; }
//  MBoundingBox boundingBox() const override;

  void postConstructor() override;

  static MObject m_bl;
  static MObject m_br;
  static MObject m_tl;
  static MObject m_tr;

private:
  static MObject m_shape;
  static MObject m_color;
  static MObject m_fill;
  static MObject m_lineThickness;
  static MObject m_lineStyle;

  static MObject m_size;
  static MObject m_width;
  static MObject m_height;

  static MObject m_layout;
  static MObject m_position;
};

}