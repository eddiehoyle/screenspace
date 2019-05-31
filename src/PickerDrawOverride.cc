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
#include <maya/MUIDrawManager.h>
#include <maya/MPlug.h>
#include <vector>

namespace screenspace {

MString PickerDrawOverride::classifcation = "drawdb/picker";
MString PickerDrawOverride::id = "picker";


MPxDrawOverride* PickerDrawOverride::creator(const MObject& obj)
{
  return new PickerDrawOverride(obj);
}

bool PickerDrawOverride::isBounded(const MDagPath& objPath,
                                   const MDagPath& cameraPath) const {
  return true;
}

MBoundingBox PickerDrawOverride::boundingBox(const MDagPath& objPath,
                                             const MDagPath& cameraPath) const {
  MBoundingBox bbox;
  bbox.expand(MPoint(-1.0f, -1.0f, -1.0f));
  bbox.expand(MPoint( 1.0f,  1.0f,  1.0f));
  return bbox;
}

MUserData* PickerDrawOverride::prepareForDraw(const MDagPath& objPath,
                                              const MDagPath& cameraPath,
                                              const MFrameContext& frameContext,
                                              MUserData* oldData) {
  TNC_DEBUG << "drawing";
  return nullptr;
}

void PickerDrawOverride::addUIDrawables(const MDagPath& objPath,
                                        MHWRender::MUIDrawManager& drawManager,
                                        const MHWRender::MFrameContext& frameContext,
                                        const MUserData* data) {
  TNC_DEBUG << "ui";
}

PickerDrawOverride::PickerDrawOverride(const MObject& obj) : MPxDrawOverride(obj, nullptr) {

}


}