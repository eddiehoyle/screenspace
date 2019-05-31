#include "PickerShape.hh"

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

namespace screenspace {

MString PickerShape::typeName = "picker";
MTypeId PickerShape::id(0x8701F);

void* PickerShape::creator() {
  return new PickerShape();
}

MStatus PickerShape::initialize() {
  MStatus status;
  return MStatus::kSuccess;
}

void PickerShape::postConstructor() {
  setRenderable(false);
}

bool PickerShape::isBounded() const {
  return true;
}

MBoundingBox PickerShape::boundingBox() const {
  MBoundingBox bbox;
  bbox.expand(MPoint(-1.0f, -1.0f, -1.0f))
  bbox.expand(MPoint( 1.0f,  1.0f,  1.0f))
  return bbox;
}

MSelectionMask PickerShape::getShapeSelectionMask() const {
  return MSelectionMask(MSelectionMask::kSelectHandles);
}

MSelectionMask PickerShape::getComponentSelectionMask() const {
  return MSelectionMask();
}

}