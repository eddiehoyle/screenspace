#include "PickerShape.hh"
#include "Log.hh"

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPluginData.h>
#include <maya/MPlugArray.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnMesh.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnMeshData.h>
#include <maya/MBoundingBox.h>

namespace screenspace {

MString PickerShape::typeName = "picker";
MTypeId PickerShape::id(0x8701F);

void* PickerShape::creator() {
  TNC_DEBUG << "Creating picker...";
  return new PickerShape();
}

MStatus PickerShape::initialize() {
  TNC_DEBUG << "Initializing picker...";
  MStatus status;
  return MStatus::kSuccess;
}

//MBoundingBox PickerShape::boundingBox() const {
//  MBoundingBox bbox;
//  double min = std::numeric_limits<double>::min();
//  double max = std::numeric_limits<double>::max();
//  bbox.expand(MPoint(min, min, min));
//  bbox.expand(MPoint(max, max, max));
//  return bbox;
//}

void PickerShape::postConstructor() {
}

}