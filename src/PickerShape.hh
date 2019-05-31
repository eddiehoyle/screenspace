#include <maya/MPxSurfaceShape.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionMask.h>

namespace screenspace {

class PickerShape : public MPxSurfaceShape {

public:
  static MTypeId id;
  static MString typeName;
  static void* creator();
  static MStatus initialize();

public:
  PickerShape() = default;
  virtual ~PickerShape() = default;

  void postConstructor() override;
  bool isBounded() const override;
  MBoundingBox boundingBox() const override;
  MSelectionMask getShapeSelectionMask() const override;
  MSelectionMask getComponentSelectionMask() const override;
};

}