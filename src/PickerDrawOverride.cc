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

void lookAt(const MVector& from, const MVector& to, MMatrix &V)
{
  static const MVector tmp(0, 1, 0);
  MVector forward = (from - to);
  forward.normalize();
  MVector right = tmp ^ forward;
  MVector up = forward ^ right;
//  Vec3f forward = normalize(from - to);
//  Vec3f right = crossProduct(normalize(tmp), forward);
//  Vec3f up = crossProduct(forward, right);

  V(0, 0) = right.x;
  V(0, 1) = right.y;
  V(0, 2) = right.z;
  V(1, 0) = up.x;
  V(1, 1) = up.y;
  V(1, 2) = up.z;
  V(2, 0) = forward.x;
  V(2, 1) = forward.y;
  V(2, 2) = forward.z;

  V(3, 0) = from.x;
  V(3, 1) = from.y;
  V(3, 2) = from.z;
}

void PickerDrawOverride::addUIDrawables(const MDagPath& objPath,
                                        MHWRender::MUIDrawManager& drawManager,
                                        const MHWRender::MFrameContext& frameContext,
                                        const MUserData* userData) {
  const PickerUserData* data = dynamic_cast<const PickerUserData*>(userData);
  if (!data)
    return;

  MDagPath shape = frameContext.getCurrentCameraPath();
  if (shape.partialPathName() != "perspShape")
    return;

  MDagPath view;
  MDagPath model;

  MDagPath::getAPathTo(MFnDagNode(objPath).parent(0), model);
  MDagPath::getAPathTo(MFnDagNode(frameContext.getCurrentCameraPath()).parent(0), view);

  MMatrix viewMatrix = MFnTransform(view).transformationMatrix();
  MMatrix modelMatrix = MFnTransform(model).transformationMatrix();

  MTransformationMatrix modelXform = MTransformationMatrix(modelMatrix);
  MTransformationMatrix viewXform = MTransformationMatrix(viewMatrix);

  MMatrix modelRotateMatrix = modelXform.asRotateMatrix();
  MMatrix viewRotateMatrix = viewXform.asRotateMatrix();

//  MMatrix foo = modelRotateMatrix.inverse() * modelRotateMatrix;
//  MMatrix foo = MMatrix::identity;
//  MMatrix foo = modelRotateMatrix.inverse() * modelRotateMatrix;
//  MVector cameraZ(foo(2, 0), foo(2, 1), foo(2, 2));
  MMatrix inverseModelMatrix = modelXform.asMatrixInverse();
  MMatrix inverseViewMatrix = viewXform.asMatrixInverse();

  MPoint origin = MTransformationMatrix(inverseModelMatrix).getTranslation(MSpace::kPostTransform);
//  MMatrix foo = inverseModelMatrix * modelMatrix;
  MMatrix foo = inverseViewMatrix * inverseModelMatrix;
  MVector cameraY(foo(1, 0), foo(1, 1), foo(1, 2));

  TNC_DEBUG << "origin=" << origin;
//  TNC_DEBUG << "drawPos=" << drawPos << ", cameraPos=" << cameraPos << ", cameraZ=" << cameraZ;


//  MPoint worldNearPt, worldFarPt;
//  frameContext.viewportToWorld(0, 0, worldNearPt, worldFarPt);
//  worldNearPt = (worldFarPt - worldNearPt) * 0.01 + worldNearPt;


  drawManager.beginDrawable(MUIDrawManager::Selectability::kSelectable);
  drawManager.setColor(MColor(1.0, 0.0, 0.0, 0.2));
  drawManager.rect(origin, MVector(0, 0, 1), cameraY, 10, 10, true);
  drawManager.setPaintStyle(MUIDrawManager::kFlat);
  drawManager.setColor(MColor(0.0, 1.0, 0.0));
  drawManager.circle2d(MPoint(data->m_width/2/2, data->m_height/2/2), 20, true);
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