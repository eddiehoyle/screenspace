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

private:
  static MObject m_bl;
  static MObject m_br;
  static MObject m_tl;
  static MObject m_tr;
};

}