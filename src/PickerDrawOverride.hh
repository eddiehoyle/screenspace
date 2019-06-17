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

  ~PickerDrawOverride() override = default;
  MHWRender::DrawAPI supportedDrawAPIs() const override
  {
    return (MHWRender::kAllDevices);
  }

  MUserData* prepareForDraw(const MDagPath& pickerDag,
                            const MDagPath& cameraDag,
                            const MFrameContext& frameContext,
                            MUserData* oldData) override;

  bool hasUIDrawables() const override { return true; }

  void addUIDrawables(const MDagPath& pickerDag,
                      MHWRender::MUIDrawManager& drawManager,
                      const MHWRender::MFrameContext& frameContext,
                      const MUserData* data) override;

private:
  bool isTargetCamera(const MDagPath& picker, const MDagPath& camera) const;

private:
  PickerDrawOverride(const MObject& obj);
};

}
#endif // SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH