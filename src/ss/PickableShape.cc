#include "PickableShape.hh"

#include "ss/Log.hh"
#include "ss/Types.hh"

#include <maya/MFnEnumAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>

namespace screenspace {

MString PickableShape::typeName = "pickable";
MTypeId PickableShape::id(0x8701F);

MObject PickableShape::m_camera;
MObject PickableShape::m_shape;
MObject PickableShape::m_color;
MObject PickableShape::m_opacity;
MObject PickableShape::m_size;
MObject PickableShape::m_width;
MObject PickableShape::m_height;
MObject PickableShape::m_depth;
MObject PickableShape::m_position;
MObject PickableShape::m_horizontalAlign;
MObject PickableShape::m_verticalAlign;
MObject PickableShape::m_rotate;
MObject PickableShape::m_offsetX;
MObject PickableShape::m_offsetY;
MObject PickableShape::m_offset;

void* PickableShape::creator() {
  return new PickableShape();
}

MStatus PickableShape::initialize() {

  MStatus status;

  MFnTypedAttribute tAttr;
  MFnNumericAttribute nAttr;
  MFnEnumAttribute eAttr;
  MFnMessageAttribute mAttr;
  MFnUnitAttribute uAttr;

  m_camera = mAttr.create("camera", "cam", &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(mAttr.setWritable(true));
  CHECK_MSTATUS(mAttr.setReadable(false));

  m_shape = eAttr.create("shape", "shp", static_cast<short>(Shape::Rectangle), &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(eAttr.addField("Circle", static_cast<short>(Shape::Circle)));
  CHECK_MSTATUS(eAttr.addField("Rectangle", static_cast<short>(Shape::Rectangle)));
  CHECK_MSTATUS(eAttr.addField("Triangle", static_cast<short>(Shape::Triangle)));
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

  m_rotate = uAttr.create("rotate", "rot", MFnUnitAttribute::kAngle, 0.0, &status);
  CHECK_MSTATUS(status);

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
  CHECK_MSTATUS(addAttribute(m_rotate));
  CHECK_MSTATUS(addAttribute(m_offset));

  return MStatus::kSuccess;
}

MSelectionMask PickableShape::getShapeSelectionMask() const {
  return MSelectionMask::kSelectHandles;
}

}