#include "PickerShape.hh"
#include "Log.hh"

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnMatrixAttribute.h>
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

MObject PickerShape::m_shape;
MObject PickerShape::m_color;
MObject PickerShape::m_opacity;
MObject PickerShape::m_fill;
MObject PickerShape::m_lineWidth;
MObject PickerShape::m_lineStyle;

MObject PickerShape::m_size;
MObject PickerShape::m_width;
MObject PickerShape::m_height;

MObject PickerShape::m_depth;
MObject PickerShape::m_layout;
MObject PickerShape::m_verticalAlign;
MObject PickerShape::m_horizontalAlign;
MObject PickerShape::m_offsetX;
MObject PickerShape::m_offsetY;
MObject PickerShape::m_offset;

MObject PickerShape::m_camera;

void* PickerShape::creator() {
  return new PickerShape();
}

MStatus PickerShape::initialize() {
  MObject obj;
  MStatus status;

  MFnTypedAttribute tAttr;
  MFnNumericAttribute nAttr;
  MFnEnumAttribute eAttr;
  MFnMessageAttribute mAttr;

  m_shape = eAttr.create("shape", "dsh", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Circle", 0));
  CHECK_MSTATUS(eAttr.addField("Rectangle", 1));
  CHECK_MSTATUS(eAttr.setKeyable(true));
  CHECK_MSTATUS(eAttr.setCached(true));
  CHECK_MSTATUS(eAttr.setStorable(true));
  CHECK_MSTATUS(eAttr.setWritable(true));

  m_color = nAttr.createColor("color", "clr", &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setDefault(1.0f, 1.0f, 1.0f));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setUsedAsColor(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_opacity = nAttr.create("opacity", "op", MFnNumericData::kFloat, 1.0f, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.0f));
  CHECK_MSTATUS(nAttr.setMax(1.0f));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_fill = nAttr.create("fill", "fl", MFnNumericData::kBoolean, true, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_lineWidth = nAttr.create("lineWidth", "lw", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(10.0));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_lineStyle = eAttr.create("lineStyle", "ls", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Solid", 0));
  CHECK_MSTATUS(eAttr.addField("Short Dotted", 1));
  CHECK_MSTATUS(eAttr.addField("Short Dashed", 2));
  CHECK_MSTATUS(eAttr.addField("Dashed", 3));
  CHECK_MSTATUS(eAttr.addField("Dotted", 4));
  CHECK_MSTATUS(eAttr.setKeyable(true));
  CHECK_MSTATUS(eAttr.setStorable(true));
  CHECK_MSTATUS(eAttr.setWritable(true));
  CHECK_MSTATUS(eAttr.setCached(true));

  m_size = nAttr.create("size", "sz", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_width = nAttr.create("width", "w", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_height = nAttr.create("height", "h", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_depth = nAttr.create("depth", "d", MFnNumericData::kInt, 1, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(1));
  CHECK_MSTATUS(nAttr.setSoftMax(32));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_layout = eAttr.create("layout", "lay", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Relative", 0));
  CHECK_MSTATUS(eAttr.addField("Absolute", 1));
  CHECK_MSTATUS(eAttr.setKeyable(true));
  CHECK_MSTATUS(eAttr.setStorable(true));
  CHECK_MSTATUS(eAttr.setWritable(true));
  CHECK_MSTATUS(eAttr.setCached(true));

  m_verticalAlign = eAttr.create("verticalAlign", "val", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Bottom", 0));
  CHECK_MSTATUS(eAttr.addField("Center", 1));
  CHECK_MSTATUS(eAttr.addField("Top", 2));

  m_horizontalAlign = eAttr.create("horizontalAlign", "hal", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Left", 0));
  CHECK_MSTATUS(eAttr.addField("Center", 1));
  CHECK_MSTATUS(eAttr.addField("Right", 2));

  m_offsetX = nAttr.create("offsetX", "ofsx", MFnNumericData::kFloat, 0.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.0f));
  CHECK_MSTATUS(nAttr.setMax(100.0f));

  m_offsetY = nAttr.create("offsetY", "ofsy", MFnNumericData::kFloat, 0.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.0f));
  CHECK_MSTATUS(nAttr.setMax(100.0f));

  m_offset = nAttr.create("offset", "ofs", m_offsetX, m_offsetY, MObject::kNullObj, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setDefault(0.0f, 0.0f));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_camera = mAttr.create("camera", "cam", &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setReadable(false));

  CHECK_MSTATUS(addAttribute(m_shape));
  CHECK_MSTATUS(addAttribute(m_color));
  CHECK_MSTATUS(addAttribute(m_opacity));
  CHECK_MSTATUS(addAttribute(m_fill));
  CHECK_MSTATUS(addAttribute(m_lineWidth));
  CHECK_MSTATUS(addAttribute(m_lineStyle));

  CHECK_MSTATUS(addAttribute(m_size));
  CHECK_MSTATUS(addAttribute(m_width));
  CHECK_MSTATUS(addAttribute(m_height));

  CHECK_MSTATUS(addAttribute(m_depth));
  CHECK_MSTATUS(addAttribute(m_layout));
  CHECK_MSTATUS(addAttribute(m_verticalAlign));
  CHECK_MSTATUS(addAttribute(m_horizontalAlign));
  CHECK_MSTATUS(addAttribute(m_offset));

  CHECK_MSTATUS(addAttribute(m_camera));

  return MStatus::kSuccess;
}

}