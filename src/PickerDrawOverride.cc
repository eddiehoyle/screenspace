#include "PickerDrawOverride.hh"
#include "PickerShape.hh"
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
#include <cmath>
//#include <GL/gl.h>


namespace screenspace {

MString PickerDrawOverride::classifcation = "drawdb/geometry/screenspace/picker";
MString PickerDrawOverride::id = "picker";

class PickerUserData : public MUserData {
public:
  PickerUserData() : MUserData(false), m_viewportWidth(-1), m_viewportHeight(-1), m_viewportWidthScalar(0), m_viewportHeightScalar(0) {}
  ~PickerUserData() override = default;
  std::pair<int, int> m_viewport;
  std::pair<double, double> m_scalar;

  int m_viewportWidth;
  int m_viewportHeight;
  double m_viewportWidthScalar;
  double m_viewportHeightScalar;
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

  int originX, originY;
  frameContext.getViewportDimensions(originX, originY, data->m_viewportWidth, data->m_viewportHeight);

  MMatrix viewMatrix = cameraPath.inclusiveMatrix();
  MMatrix viewMatrix2 = frameContext.getMatrix(MFrameContext::MatrixType::kViewMtx);

  MPoint nearBL, nearBR, nearTL, nearTR;
  MPoint farBL, farBR, farTL, farTR;
  frameContext.viewportToWorld(0, 0, nearBL, farBL);
//  frameContext.viewportToWorld(0, data->m_viewportHeight, nearTL, farTL);
//  frameContext.viewportToWorld(data->m_viewportWidth, 0, nearBR, farBR);
  frameContext.viewportToWorld(data->m_viewportWidth, data->m_viewportHeight, nearTR, farTR);

  // Screenspace
//  double diagonal = std::sqrt(std::pow(data->m_viewportWidth, 2) + std::pow(data->m_viewportHeight, 2));
//  double theta = std::atan(double(data->m_viewportHeight) / double(data->m_viewportWidth));
//  double height = std::sin(theta) * diagonal;
//  double width = std::cos(theta) * diagonal;
//  TNC_DEBUG << "theta=" << (theta * 180.0/M_PI) << ", viewport=(" << width << ", " << height << "), diagonal=" << diagonal;

  // Worldspace
  double diagonal = (nearTR - nearBL).length();
  double theta = std::atan(double(data->m_viewportHeight) / double(data->m_viewportWidth));
  double height = std::sin(theta) * diagonal;
  double width = std::cos(theta) * diagonal;

  MPoint bl = ((farBL - nearBL) * 0.01 + nearBL);
//  MPoint br = ((farBR - nearBR) * 0.01 + nearBR);
//  MPoint tl = ((farTL - nearTL) * 0.01 + nearTL);
  MPoint tr = ((farTR - nearTR) * 0.01 + nearTR);

  MVector right(viewMatrix(0, 0), viewMatrix(0, 1), viewMatrix(0, 2));
  MVector up(viewMatrix(1, 0), viewMatrix(1, 1), viewMatrix(1, 2));
  MPoint br = bl + (right * width);
  MPoint tl = bl + (up * height);

  TNC_DEBUG << "bl=" << bl << ", br=" << br << ", tl=" << tl << ", tr=" << tr;
//  TNC_DEBUG << "theta=" << (theta * 180.0/M_PI) << ", worldpos=(" << width << ", " << height << "), diagonal=" << diagonal;


  MPlug blPlug(objPath.node(), PickerShape::m_bl);
  MPlug brPlug(objPath.node(), PickerShape::m_br);
  MPlug tlPlug(objPath.node(), PickerShape::m_tl);
  MPlug trPlug(objPath.node(), PickerShape::m_tr);

  blPlug.child(0).setValue(bl.x);
  blPlug.child(1).setValue(bl.y);
  blPlug.child(2).setValue(bl.z);

  brPlug.child(0).setValue(br.x);
  brPlug.child(1).setValue(br.y);
  brPlug.child(2).setValue(br.z);

  tlPlug.child(0).setValue(tl.x);
  tlPlug.child(1).setValue(tl.y);
  tlPlug.child(2).setValue(tl.z);

  trPlug.child(0).setValue(tr.x);
  trPlug.child(1).setValue(tr.y);
  trPlug.child(2).setValue(tr.z);

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

  MMatrix viewMatrix = view.inclusiveMatrix();
  MMatrix modelMatrix = model.inclusiveMatrix();

//  MMatrix viewMatrix = MFnTransform(view).transformationMatrix();
//  MMatrix modelMatrix = MFnTransform(model).transformationMatrix();

//  MTransformationMatrix modelXform = MTransformationMatrix(modelMatrix);
//  MTransformationMatrix viewXform = MTransformationMatrix(viewMatrix);

  MMatrix inverseViewMatrix = view.inclusiveMatrixInverse();
  MMatrix inverseModelMatrix = model.inclusiveMatrixInverse();

  // Removes parent transforms
//  MPoint from = MTransformationMatrix(inverseModelMatrix).getTranslation(MSpace::kPostTransform);
//  MPoint to = MTransformationMatrix(viewMatrix).getTranslation(MSpace::kPostTransform);

  MPoint worldNearPt, worldFarPt;
  frameContext.viewportToWorld(data->m_viewportWidth/2, data->m_viewportHeight/2, worldNearPt, worldFarPt);
  worldNearPt = (worldFarPt - worldNearPt) * 0.01 + worldNearPt;

  MPoint blA, blB, brA, brB, tlA, tlB, trA, trB;
  frameContext.viewportToWorld(0, 0, blA, blB);
  frameContext.viewportToWorld(data->m_viewportWidth, 0, brA, brB);
  frameContext.viewportToWorld(0, data->m_viewportHeight, tlA, tlB);
  frameContext.viewportToWorld(data->m_viewportWidth, data->m_viewportHeight, trA, trB);

  MPoint bl = ((blB - blA) * 0.01 + blA);
  MPoint br = ((brB - brA) * 0.01 + brA);
  MPoint tl = ((tlB - tlA) * 0.01 + tlA);
  MPoint tr = ((trB - trA) * 0.01 + trA);

//  MMatrix aim;
//  lookAt(MPoint(0,0,0), to, aim);
//  MMatrix foo = aim * inverseModelMatrix;
//  MPoint pos = MPoint(foo(3, 0), foo(3, 1), foo(3, 2)) + worldNearPt;
//  MVector cameraY(foo(1, 0), foo(1, 1), foo(1, 2));
//  MVector cameraZ(foo(2, 0), foo(2, 1), foo(2, 2));

//  MMatrix blAim, brAim, tlAim, trAim;
//  lookAt(bl, blA, blAim);
//  lookAt(br, to, brAim);
//  lookAt(tl, to, tlAim);
//  lookAt(tr, to, trAim);

  MMatrix blSpace = viewMatrix * inverseModelMatrix;
//  MMatrix brSpace = aim * inverseModelMatrix;
//  MMatrix tlSpace = aim * inverseModelMatrix;
//  MMatrix trSpace = aim * inverseModelMatrix;

  MMatrix posMatrix;
  MTransformationMatrix foo(posMatrix);
  foo.setTranslation(bl, MSpace::kWorld);

  MMatrix ass = foo.asMatrix() * inverseModelMatrix;

//  MPoint blPos = MTransformationMatrix(inverseModelMatrix).getTranslation(MSpace::kPostTransform) + bl;
  MPoint blPos = MTransformationMatrix(ass).getTranslation(MSpace::kWorld);


//  MPoint blPos = MTransformationMatrix(blSpace).getTranslation(MSpace::kPostTransform) - bl;
//  MPoint brPos = MTransformationMatrix(brSpace).getTranslation(MSpace::kWorld);
//  MPoint tlPos = MTransformationMatrix(tlSpace).getTranslation(MSpace::kWorld);
//  MPoint trPos = MTransformationMatrix(trSpace).getTranslation(MSpace::kWorld);

//  MPoint blPos = bl;
//  MPoint brPos = inverseModelMatrix * br;
//  MPoint tlPos = inverseModelMatrix * tl;
//  MPoint trPos = inverseModelMatrix * tr;

  MVector blUp = MVector(blSpace(1, 0), blSpace(1, 1), blSpace(1, 2));
//  MVector blUp = MVector(blSpace(1, 0), blSpace(1, 1), blSpace(1, 2));
//  MVector brUp = MVector(brSpace(1, 0), brSpace(1, 1), brSpace(1, 2));
//  MVector tlUp = MVector(tlSpace(1, 0), tlSpace(1, 1), tlSpace(1, 2));
//  MVector trUp = MVector(trSpace(1, 0), trSpace(1, 1), trSpace(1, 2));

  MPoint blNormal = MVector(blSpace(2, 0), blSpace(2, 1), blSpace(2, 2));
//  MPoint brNormal = MVector(brSpace(2, 0), brSpace(2, 1), brSpace(2, 2));
//  MPoint tlNormal = MVector(tlSpace(2, 0), tlSpace(2, 1), tlSpace(2, 2));
//  MPoint trNormal = MVector(trSpace(2, 0), trSpace(2, 1), trSpace(2, 2));

//  MMatrix foo = aim * inverseModelMatrix;
//  MPoint pos = MPoint(foo(3, 0), foo(3, 1), foo(3, 2)) + worldNearPt;
//  MVector cameraY(foo(1, 0), foo(1, 1), foo(1, 2));
//  MVector cameraZ(foo(2, 0), foo(2, 1), foo(2, 2));

  double sizeX = 50 * (brA-blA).length();
  double sizeY = 50 * (tlA-blA).length();
  double halfX = 10;
  double halfY = 10;

//  TNC_DEBUG << "------------------------------------------------------------------------------------------------------";
//  TNC_DEBUG << "inverseModelMatrix=" << inverseModelMatrix;
//  TNC_DEBUG << "modelMatrix=" << modelMatrix;
//  TNC_DEBUG << "inverseModelMatrix pos=" << MTransformationMatrix(inverseModelMatrix).getTranslation(MSpace::kWorld);
//  TNC_DEBUG << "bl near pt=" << bl;

  drawManager.beginDrawable(MUIDrawManager::Selectability::kSelectable);
  drawManager.setPaintStyle(MUIDrawManager::kFlat);
  drawManager.setColor(MColor(0.0, 0.0, 1.0, 0.2));
  drawManager.rect(blPos, blUp, blNormal, sizeX + halfX, sizeY + halfY, true);
//  drawManager.rect(blPos, blUp, blNormal, sizeX + halfX, sizeY + halfY, true);
//  drawManager.rect(brPos, brUp, brNormal, sizeX - halfX, sizeY + halfY, true);
//  drawManager.rect(tlPos, tlUp, tlNormal, sizeX + halfX, sizeY - halfY, true);
//  drawManager.rect(trPos, trUp, trNormal, sizeX - halfX, sizeY - halfY, true);
//  TNC_DEBUG << "blPos=" << blPos << ", blUp=" << blUp << ", blNormal=" << blNormal;

  drawManager.setColor(MColor(0.0, 1.0, 0.0));
  drawManager.circle2d(MPoint(data->m_viewportWidth/2/2, data->m_viewportHeight/2/2), 20, true);
  drawManager.endDrawable();
}
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