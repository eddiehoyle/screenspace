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

MObject PickerShape::m_bl;
MObject PickerShape::m_br;
MObject PickerShape::m_tl;
MObject PickerShape::m_tr;

void* PickerShape::creator() {
  TNC_DEBUG << "Creating picker...";
  return new PickerShape();
}

MStatus PickerShape::initialize() {
  TNC_DEBUG << "Initializing picker...";
  MStatus status;

  MFnNumericAttribute nAttr;
  m_bl = nAttr.createPoint("bottomLeft", "bl", &status);
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(status);
  m_br = nAttr.createPoint("bottomRight", "br", &status);
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(status);
  m_tl = nAttr.createPoint("topLeft", "tl", &status);
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(status);
  m_tr = nAttr.createPoint("topRight", "tr", &status);
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(status);

  CHECK_MSTATUS(addAttribute(m_bl));
  CHECK_MSTATUS(addAttribute(m_br));
  CHECK_MSTATUS(addAttribute(m_tl));
  CHECK_MSTATUS(addAttribute(m_tr));

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