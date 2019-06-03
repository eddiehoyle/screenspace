#include "PickerDrawOverride.hh"
#include "Log.hh"

#include <maya/MBoundingBox.h>
#include <maya/MUserData.h>
#include <maya/MPxNode.h>
#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MStringArray.h>
#include <maya/MFrameContext.h>
#include <maya/MFnPluginData.h>
#include <maya/MGeometryRequirements.h>
#include <maya/MHWGeometry.h>
#include <maya/MHWGeometryUtilities.h>
#include <maya/MPxGeometryOverride.h>
#include <maya/MShaderManager.h>
#include <maya/MSelectionContext.h>
#include <maya/MUIDrawManager.h>
#include <maya/MPlug.h>
#include <maya/MFnTransform.h>
#include <maya/MTransformationMatrix.h>

#include <vector>
//#include <GL/gl.h>


namespace screenspace {

MString PickerDrawOverride::classifcation = "drawdb/geometry/screenspace/picker";
MString PickerDrawOverride::id = "picker";

class PickerUserData : public MUserData {
public:
  PickerUserData() : MUserData(false), m_width(0), m_height(0) {}
  ~PickerUserData() override = default;
  int m_width;
  int m_height;
};


MPxDrawOverride* PickerDrawOverride::creator(const MObject& obj)
{
  TNC_DEBUG << "Creating draw override...";
  return new PickerDrawOverride(obj);
}
//
//MBoundingBox PickerDrawOverride::boundingBox(const MDagPath& objPath, const MDagPath& cameraPath) const
//{
////  return MBoundingBox();
//  MBoundingBox bbox;
//  double min = std::numeric_limits<double>::min();
//  double max = std::numeric_limits<double>::max();
//  bbox.expand(MPoint(min, min, min));
//  bbox.expand(MPoint(max, max, max));
//  return bbox;
//}

MUserData* PickerDrawOverride::prepareForDraw(const MDagPath& objPath,
                                              const MDagPath& cameraPath,
                                              const MFrameContext& frameContext,
                                              MUserData* userData) {

  PickerUserData* data = dynamic_cast<PickerUserData*>(userData);
  if (!data)
    data = new PickerUserData();

  int originX, originY, width, height;
  frameContext.getViewportDimensions(originX, originY, width, height);
//  TNC_DEBUG << "drawing: (" << originX << ", " << originY << ", " << width << ", " << height << ")";

  data->m_width = width;
  data->m_height = height;
  return data;
}

void PickerDrawOverride::addUIDrawables(const MDagPath& objPath,
                                        MHWRender::MUIDrawManager& drawManager,
                                        const MHWRender::MFrameContext& frameContext,
                                        const MUserData* userData) {
  const PickerUserData* data = dynamic_cast<const PickerUserData*>(userData);
  if (!data)
    return;

  drawManager.beginDrawable(MUIDrawManager::Selectability::kSelectable);
  drawManager.setColor(MColor(1.0, 0.0, 0.0));
  drawManager.setPaintStyle(MUIDrawManager::kFlat);
//  drawManager.circle(MPoint(0,0,0), MVector(0, 1, 0), 12222, true);
  drawManager.setColor(MColor(0.0, 1.0, 0.0));
  drawManager.circle2d(MPoint(data->m_width/2/2, data->m_height/2/2), 20, true);

  MMatrix matrix = frameContext.getMatrix(MFrameContext::MatrixType::kWorldViewMtx);
  MPoint worldNearPt, worldFarPt;
  frameContext.viewportToWorld(0, 0, worldNearPt, worldFarPt);
  worldNearPt = (worldFarPt - worldNearPt) * 0.01 + worldNearPt;

  MTransformationMatrix xform(matrix);
  MVector viewPt = xform.getTranslation(MSpace::Space::kWorld);
//  double* viewPos = matrix[3];
//  MPoint viewPt(viewPos[0], viewPos[1], viewPos[2]);
//  drawManager.setColor(MColor(0.0, 0.0, 1.0));
//  drawManager.rect(viewPt - worldNearPt, MVector(0, 1, 0), MVector(0, 1, 0), 20, 20, true);

  TNC_DEBUG << "viewPt=" << viewPt;

  drawManager.endDrawable();

}

//bool PickerDrawOverride::userSelect(MSelectionInfo& selectInfo,
//                                    const MHWRender::MDrawContext& context,
//                                    MPoint& hitPoint,
//                                    const MUserData* data)
//{
//  unsigned int x, y, width, height;
//  selectInfo.selectRect(x, y, width, height);
//  TNC_DEBUG << "userSelect: (" << x << ", " << y << ", " << width << ", " << height << ")";
//  return false;
//}

bool PickerDrawOverride::wantUserSelection() const {
  TNC_DEBUG << "wantUserSelection()";
  return true;
}

bool PickerDrawOverride::userSelect(MSelectionInfo& selectInfo,
                                    const MHWRender::MDrawContext& context,
                                    MPoint& hitPoint, const MUserData* data) {
  TNC_DEBUG << "userSelect()";
  return true;
}

bool PickerDrawOverride::refineSelectionPath(const MSelectionInfo& selectInfo,
                                             const MRenderItem& hitItem,
                                             MDagPath& path,
                                             MObject& components,
                                             MSelectionMask& objectMask)
{

  unsigned int x, y, width, height;
  selectInfo.selectRect(x, y, width, height);
  TNC_DEBUG << "refineSelectionPath: (" << x << ", " << y << ", " << width << ", " << height << ")";
  return true;
}

PickerDrawOverride::PickerDrawOverride(const MObject& obj) : MPxDrawOverride(obj, nullptr) {

}


}