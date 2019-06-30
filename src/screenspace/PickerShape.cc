#include "PickerShape.hh"
#include "screenspace/Log.hh"
#include "screenspace/Types.hh"

#include <maya/MFnTypedAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnMessageAttribute.h>

namespace screenspace {

MString PickerShape::typeName = "picker";
MTypeId PickerShape::id(0x8701F);

MObject PickerShape::m_camera;
MObject PickerShape::m_shape;
MObject PickerShape::m_color;
MObject PickerShape::m_opacity;
MObject PickerShape::m_size;
MObject PickerShape::m_width;
MObject PickerShape::m_height;
MObject PickerShape::m_depth;
MObject PickerShape::m_position;
MObject PickerShape::m_horizontalAlign;
MObject PickerShape::m_verticalAlign;
MObject PickerShape::m_offsetX;
MObject PickerShape::m_offsetY;
MObject PickerShape::m_offset;

void* PickerShape::creator() {
  return new PickerShape();
}

MStatus PickerShape::initialize() {

  MStatus status;

  MFnTypedAttribute tAttr;
  MFnNumericAttribute nAttr;
  MFnEnumAttribute eAttr;
  MFnMessageAttribute mAttr;

  m_camera = mAttr.create("camera", "cam", &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(mAttr.setWritable(true));
  CHECK_MSTATUS(mAttr.setReadable(false));

  m_shape = eAttr.create("shape", "shp", static_cast<short>(Shape::Rectangle), &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Circle", static_cast<short>(Shape::Circle)));
  CHECK_MSTATUS(eAttr.addField("Rectangle", static_cast<short>(Shape::Rectangle)));
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

  m_opacity = nAttr.create("opacity", "opc", MFnNumericData::kFloat, 1.0f, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.0f));
  CHECK_MSTATUS(nAttr.setMax(1.0f));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_size = nAttr.create("size", "sz", MFnNumericData::kFloat, 1.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_width = nAttr.create("width", "w", MFnNumericData::kFloat, 10.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_height = nAttr.create("height", "h", MFnNumericData::kFloat, 10.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0.01));
  CHECK_MSTATUS(nAttr.setSoftMin(1.0));
  CHECK_MSTATUS(nAttr.setSoftMax(100.0));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_depth = nAttr.create("depth", "d", MFnNumericData::kInt, 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(0));
  CHECK_MSTATUS(nAttr.setSoftMax(32));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  m_position = eAttr.create("position", "pos", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Relative", static_cast<short>(Position::Relative)));
  CHECK_MSTATUS(eAttr.addField("Absolute", static_cast<short>(Position::Absolute)));
  CHECK_MSTATUS(eAttr.setKeyable(true));
  CHECK_MSTATUS(eAttr.setStorable(true));
  CHECK_MSTATUS(eAttr.setWritable(true));
  CHECK_MSTATUS(eAttr.setCached(true));

  m_horizontalAlign = eAttr.create("horizontalAlign", "hal", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Left", static_cast<short>(HorizontalAlign::Left)));
  CHECK_MSTATUS(eAttr.addField("Middle", static_cast<short>(HorizontalAlign::Middle)));
  CHECK_MSTATUS(eAttr.addField("Right", static_cast<short>(HorizontalAlign::Right)));

  m_verticalAlign = eAttr.create("verticalAlign", "val", 0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Bottom", static_cast<short>(VerticalAlign::Bottom)));
  CHECK_MSTATUS(eAttr.addField("Middle", static_cast<short>(VerticalAlign::Middle)));
  CHECK_MSTATUS(eAttr.addField("Top", static_cast<short>(VerticalAlign::Top)));

  m_offsetX = nAttr.create("offsetX", "ofsx", MFnNumericData::kFloat, 0.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setMin(-1000.0f));
  CHECK_MSTATUS(nAttr.setMax(1000.0f));

  m_offsetY = nAttr.create("offsetY", "ofsy", MFnNumericData::kFloat, 0.0, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setSoftMin(-1000.0f));
  CHECK_MSTATUS(nAttr.setSoftMax(1000.0f));

  m_offset = nAttr.create("offset", "ofs", m_offsetX, m_offsetY, MObject::kNullObj, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(nAttr.setDefault(0.0f, 0.0f));
  CHECK_MSTATUS(nAttr.setKeyable(true));
  CHECK_MSTATUS(nAttr.setStorable(true));
  CHECK_MSTATUS(nAttr.setWritable(true));
  CHECK_MSTATUS(nAttr.setCached(true));

  CHECK_MSTATUS(addAttribute(m_camera));
  CHECK_MSTATUS(addAttribute(m_shape));
  CHECK_MSTATUS(addAttribute(m_color));
  CHECK_MSTATUS(addAttribute(m_opacity));
  CHECK_MSTATUS(addAttribute(m_size));
  CHECK_MSTATUS(addAttribute(m_width));
  CHECK_MSTATUS(addAttribute(m_height));
  CHECK_MSTATUS(addAttribute(m_depth));
  CHECK_MSTATUS(addAttribute(m_position));
  CHECK_MSTATUS(addAttribute(m_horizontalAlign));
  CHECK_MSTATUS(addAttribute(m_verticalAlign));
  CHECK_MSTATUS(addAttribute(m_offset));

  return MStatus::kSuccess;
}

}