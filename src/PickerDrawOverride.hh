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

//  bool isBounded(const MDagPath& objPath, const MDagPath& cameraPath) const override { return true; }
//  MBoundingBox boundingBox(const MDagPath& objPath,
//                           const MDagPath& cameraPath) const override;

  MUserData* prepareForDraw(const MDagPath& objPath,
                            const MDagPath& cameraPath,
                            const MFrameContext& frameContext,
                            MUserData* oldData) override;

  bool hasUIDrawables() const override { return true; }

  void addUIDrawables(const MDagPath& objPath,
                      MHWRender::MUIDrawManager& drawManager,
                      const MHWRender::MFrameContext& frameContext,
                      const MUserData* data) override;

//  bool wantUserSelection() const override { return true; }
//
//  bool userSelect(MSelectionInfo& selectInfo,
//                  const MHWRender::MDrawContext& context,
//                  MPoint& hitPoint,
//                  const MUserData* data) override;

  bool wantUserSelection() const override;

  bool userSelect(MSelectionInfo& selectInfo,
                  const MHWRender::MDrawContext& context,
                  MPoint& hitPoint,
                  const MUserData* data) override;


  /// Only triggers when selection is successful
  bool refineSelectionPath(const MSelectionInfo& selectInfo,
                                   const MRenderItem& hitItem,
                                   MDagPath& path,
                                   MObject& geomComponents,
                                   MSelectionMask& objectMask) override;

private:
  PickerDrawOverride(const MObject& obj);
};

}
#endif // SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH