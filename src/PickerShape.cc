#include "PickerShape.hh"
#include "Log.hh"

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
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

MObject PickerShape::m_shape;
MObject PickerShape::m_color;
MObject PickerShape::m_fill;
MObject PickerShape::m_lineThickness;
MObject PickerShape::m_lineStyle;

MObject PickerShape::m_size;
MObject PickerShape::m_width;
MObject PickerShape::m_height;

MObject PickerShape::m_layout;
MObject PickerShape::m_position;

void* PickerShape::creator() {
  TNC_DEBUG << "Creating picker...";
  return new PickerShape();
}

MStatus PickerShape::initialize() {
  TNC_DEBUG << "Initializing picker...";

  MObject obj;
  MStatus status;

  MFnTypedAttribute tAttr;
  MFnNumericAttribute nAttr;
  MFnEnumAttribute eAttr;

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

  // ------------------------------------------------

  m_shape = eAttr.create("shape", "sh", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("circle", 0));
  CHECK_MSTATUS(eAttr.addField("rect", 1));

  m_color = nAttr.createColor("color", "c", &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setDefault(1.0f, 0.0f, 0.0f));

  m_fill = nAttr.create("fill", "fl", MFnNumericData::kBoolean, true, &status);
  CHECK_MSTATUS(status);

  m_lineThickness = nAttr.create("lineThickness", "lt", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(10.0));

  m_lineStyle = eAttr.create("lineStyle", "ls", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("solid", 0));
  CHECK_MSTATUS(eAttr.addField("short dotted", 1));
  CHECK_MSTATUS(eAttr.addField("short dashed", 2));
  CHECK_MSTATUS(eAttr.addField("dashed", 3));
  CHECK_MSTATUS(eAttr.addField("dotted", 4));

  m_size = nAttr.create("size", "sz", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(status);

  m_width = nAttr.create("width", "w", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(status);

  m_height = nAttr.create("height", "h", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(status);

  m_layout = eAttr.create("layout", "ly", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("absolute", 0));
  CHECK_MSTATUS(eAttr.addField("relative", 1));

  m_position = nAttr.create("position", "pos", MFnNumericData::k2Float, 0.0f, &status);
  CHECK_MSTATUS(nAttr.setDefault(0.0f, 0.0f));
  CHECK_MSTATUS(status);

  CHECK_MSTATUS(addAttribute(m_shape));
  CHECK_MSTATUS(addAttribute(m_color));
  CHECK_MSTATUS(addAttribute(m_fill));
  CHECK_MSTATUS(addAttribute(m_lineThickness));
  CHECK_MSTATUS(addAttribute(m_lineStyle));

  CHECK_MSTATUS(addAttribute(m_size));
  CHECK_MSTATUS(addAttribute(m_width));
  CHECK_MSTATUS(addAttribute(m_height));
  CHECK_MSTATUS(addAttribute(m_layout));
  CHECK_MSTATUS(addAttribute(m_position));

  return MStatus::kSuccess;
}

void PickerShape::postConstructor() {
}

}