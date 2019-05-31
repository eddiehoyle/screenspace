#ifndef SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH
#define SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH

#include <maya/MPxDrawOverride.h>

namespace screenspace {

class PickerDrawOverride : public MPxDrawOverride
{
public:
  static MString classifcation;
  static MString id;
  static MPxDrawOverride* creator(const MObject& obj);

public:

  PickerDrawOverride() = default;
  ~PickerDrawOverride() override = default;
//  DrawAPI supportedDrawAPIs() const override;

  bool isBounded(const MDagPath& objPath,
                 const MDagPath& cameraPath) const override;

  MBoundingBox boundingBox(const MDagPath& objPath,
                           const MDagPath& cameraPath) const override;

  MUserData* prepareForDraw(const MDagPath& objPath,
                            const MDagPath& cameraPath,
                            const MFrameContext& frameContext,
                            MUserData* oldData) override;

  bool hasUIDrawables() const override { return true; }

  void addUIDrawables(const MDagPath& objPath,
                      MHWRender::MUIDrawManager& drawManager,
                      const MHWRender::MFrameContext& frameContext,
                      const MUserData* data) override;
private:
  PickerDrawOverride(const MObject& obj);
};

}
#endif // SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH