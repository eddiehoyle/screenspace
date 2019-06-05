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

  MMatrix inverseModelMatrix = modelXform.asMatrixInverse();
  MMatrix inverseViewMatrix = viewXform.asMatrixInverse();

  // Removes parent transforms
  MPoint from = MTransformationMatrix(inverseModelMatrix).getTranslation(MSpace::kPostTransform);
  MPoint to = MTransformationMatrix(viewMatrix).getTranslation(MSpace::kPostTransform);

  MPoint worldNearPt, worldFarPt;
  frameContext.viewportToWorld(data->m_width/2, data->m_height/2, worldNearPt, worldFarPt);
  worldNearPt = (worldFarPt - worldNearPt) * 0.01 + worldNearPt;

//  MPoint origin, x, y, _;
//  frameContext.viewportToWorld(0, 0, bl, _);
//  frameContext.viewportToWorld(data->m_width, data->m_height, tr, _);
//  frameContext.viewportToWorld(data->m_width, data->m_height, x, _);
//  frameContext.viewportToWorld(0, data->m_height, y, _);

//  TNC_DEBUG << "viewport(" << data->m_width << ", " << data->m_height << "), origin=" << origin << ", values=(" << x << ", " << y << "), scaled(" << (x-origin).length() << ", " << (y-origin).length() << ")";

  MPoint blA, blB, brA, brB, tlA, tlB, trA, trB;
  frameContext.viewportToWorld(0, 0, blA, blB);
  frameContext.viewportToWorld(data->m_width, 0, brA, brB);
  frameContext.viewportToWorld(0, data->m_height, tlA, tlB);
  frameContext.viewportToWorld(data->m_width, data->m_height, trA, trB);

  MPoint bl = ((blB - blA) * 0.01 + blA) * inverseModelMatrix;
  MPoint br = ((brB - brA) * 0.01 + brA) * inverseModelMatrix;
  MPoint tl = ((tlB - tlA) * 0.01 + tlA) * inverseModelMatrix;
  MPoint tr = ((trB - trA) * 0.01 + trA) * inverseModelMatrix;

  MMatrix aim;
  lookAt(MPoint(0,0,0), to, aim);
//  MMatrix foo = aim * inverseModelMatrix;
//  MPoint pos = MPoint(foo(3, 0), foo(3, 1), foo(3, 2)) + worldNearPt;
//  MVector cameraY(foo(1, 0), foo(1, 1), foo(1, 2));
//  MVector cameraZ(foo(2, 0), foo(2, 1), foo(2, 2));

  MMatrix blAim, brAim, tlAim, trAim;
  lookAt(bl, to, blAim);
  lookAt(br, to, brAim);
  lookAt(tl, to, tlAim);
  lookAt(tr, to, trAim);

  MMatrix blSpace = aim * inverseModelMatrix;
  MMatrix brSpace = aim * inverseModelMatrix;
  MMatrix tlSpace = aim * inverseModelMatrix;
  MMatrix trSpace = aim * inverseModelMatrix;

//  MPoint blPos = MTransformationMatrix(blSpace).getTranslation(MSpace::kWorld);
//  MPoint brPos = MTransformationMatrix(brSpace).getTranslation(MSpace::kWorld);
//  MPoint tlPos = MTransformationMatrix(tlSpace).getTranslation(MSpace::kWorld);
//  MPoint trPos = MTransformationMatrix(trSpace).getTranslation(MSpace::kWorld);

  MPoint blPos = inverseModelMatrix * bl;
  MPoint brPos = inverseModelMatrix * br;
  MPoint tlPos = inverseModelMatrix * tl;
  MPoint trPos = inverseModelMatrix * tr;

  MVector blUp = MVector(blSpace(1, 0), blSpace(1, 1), blSpace(1, 2));
  MVector brUp = MVector(brSpace(1, 0), brSpace(1, 1), brSpace(1, 2));
  MVector tlUp = MVector(tlSpace(1, 0), tlSpace(1, 1), tlSpace(1, 2));
  MVector trUp = MVector(trSpace(1, 0), trSpace(1, 1), trSpace(1, 2));

  MPoint blNormal = MVector(blSpace(2, 0), blSpace(2, 1), blSpace(2, 2));
  MPoint brNormal = MVector(brSpace(2, 0), brSpace(2, 1), brSpace(2, 2));
  MPoint tlNormal = MVector(tlSpace(2, 0), tlSpace(2, 1), tlSpace(2, 2));
  MPoint trNormal = MVector(trSpace(2, 0), trSpace(2, 1), trSpace(2, 2));

//  MMatrix foo = aim * inverseModelMatrix;
//  MPoint pos = MPoint(foo(3, 0), foo(3, 1), foo(3, 2)) + worldNearPt;
//  MVector cameraY(foo(1, 0), foo(1, 1), foo(1, 2));
//  MVector cameraZ(foo(2, 0), foo(2, 1), foo(2, 2));

  double sizeX = 50 * (brA-blA).length();
  double sizeY = 50 * (tlA-blA).length();
  double halfX = 10;
  double halfY = 10;

  drawManager.beginDrawable(MUIDrawManager::Selectability::kSelectable);
  drawManager.setPaintStyle(MUIDrawManager::kFlat);
  drawManager.setColor(MColor(0.0, 0.0, 1.0, 0.2));
  drawManager.rect(blPos, blUp, blNormal, sizeX + halfX, sizeY + halfY, true);
  drawManager.rect(brPos, brUp, brNormal, sizeX - halfX, sizeY + halfY, true);
  drawManager.rect(tlPos, tlUp, tlNormal, sizeX + halfX, sizeY - halfY, true);
  drawManager.rect(trPos, trUp, trNormal, sizeX - halfX, sizeY - halfY, true);
  TNC_DEBUG << "sizeX=" << sizeX << ", sizeY=" << sizeY;

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