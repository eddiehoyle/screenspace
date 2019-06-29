#include "PickerDrawOverride.hh"
#include "PickerShape.hh"
#include "Types.hh"
#include "Log.hh"

#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MNodeClass.h>
#include <maya/MColorArray.h>
#include <maya/MPointArray.h>
#include <maya/MUintArray.h>
#include <maya/MVectorArray.h>

#include <cmath>


namespace screenspace {

MString PickerDrawOverride::classifcation = "drawdb/geometry/screenspace/picker";
MString PickerDrawOverride::id = "picker";

struct Viewport {
  int width;
  int height;
  float worldspaceWidth;
  float worldspaceHeight;
};

struct Geometry {
  MUIDrawManager::Primitive primitive;
  MPointArray vertices;
  MVectorArray normals;
  MColorArray colors;
  MUintArray indices;
};

struct Style {
  Shape shape;
  MColor color;
};

bool linePlaneIntersection(const MVector& ray, const MVector& origin,
                           const MVector& normal, const MPoint& coord,
                           MPoint& contact) {
  if (normal * ray == 0)
    return false;
  const float dot = normal * coord;
  const float scalar = (dot - (normal * origin)) / (normal * ray);
  contact = origin + ray * scalar;
  return true;
}

/// Compute world position for viewport and offset slightly in front of near clipping plane
MPoint computeViewportToWorld(const MFrameContext& context,
                              int x, int y, int depth)
{
  MPoint near, far;
  context.viewportToWorld(x, y, near, far);

  // TODO:
  // Make this to be close to the nearClipPlane
  // than arbitrary 0.1f vale.
  float scalar = 0.1f * (depth + 1);

  MVector direction = (far - near);
  direction.normalize();

  return near + (direction * scalar);
}

class PickerUserData : public MUserData {
public:
  PickerUserData() : MUserData(false) {}
  ~PickerUserData() override = default;

  inline const MMatrix& matrix() const {return m_matrix;}
  inline const Viewport& viewport() const {return m_viewport;}
  inline const Geometry& geometry() const {return m_geometry;}
  inline const Style& style() const {return m_style;}

public:

  MMatrix m_matrix;
  Viewport m_viewport;
  Geometry m_geometry;
  Style m_style;
};

void prepareMatrix(const MDagPath& pickerDag,
                   const MDagPath& cameraDag,
                   const MFrameContext& frameContext,
                   PickerUserData* data)
{
  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(pickerDag.node());

  int _, viewportWidth, viewportHeight;
  frameContext.getViewportDimensions(_, _, viewportWidth, viewportHeight);

  // Draw depth
  int depth;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("depth")).getValue(depth));

  const MPoint nearBL = computeViewportToWorld(frameContext, 0, 0, depth);
  const MPoint nearTR = computeViewportToWorld(frameContext, viewportWidth, viewportHeight, depth);

  float hyp = float((nearTR - nearBL).length());
  float theta = atanf(float(viewportHeight) / float(viewportWidth));
  float worldspaceWidth = cosf(theta) * hyp;
  float worldspaceHeight = sinf(theta) * hyp;

  Viewport viewport;
  viewport.width = viewportWidth;
  viewport.height = viewportHeight;
  viewport.worldspaceWidth = worldspaceWidth;
  viewport.worldspaceHeight = worldspaceHeight;
  data->m_viewport = viewport;

  short _position;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("position")).getValue(_position));
  Position position = static_cast<Position>(_position);

  // Viewport scale factor
  float worldspaceUnitX, worldspaceUnitY;
  float viewportUnitX, viewportUnitY;
  switch (position)
  {
    case Position::Relative:
      worldspaceUnitX = viewport.worldspaceWidth / 100.0f;
      worldspaceUnitY = viewport.worldspaceHeight / 100.0f;
      viewportUnitX = viewport.width / 100.0f;
      viewportUnitY = viewport.height / 100.0f;
      break;
    case Position::Absolute:
      worldspaceUnitX = viewport.worldspaceWidth / viewport.width;
      worldspaceUnitY = viewport.worldspaceHeight / viewport.height;
      viewportUnitX = 1.0f;
      viewportUnitY = 1.0f;
      break;
  }

  short _horizontalAlign;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("horizontalAlign")).getValue(_horizontalAlign));
  HorizontalAlign horizontalAlign = static_cast<HorizontalAlign>(_horizontalAlign);

  float _verticalAlign;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("verticalAlign")).getValue(_verticalAlign));
  VerticalAlign verticalAlign = static_cast<VerticalAlign>(_verticalAlign);

  float alignOffsetX;
  switch (horizontalAlign)
  {
    case HorizontalAlign::Left:
      alignOffsetX = 0.0f;
      break;
    case HorizontalAlign::Middle:
      alignOffsetX = viewport.width / 2.0f;
      break;
    case HorizontalAlign::Right:
      alignOffsetX = viewport.width;
      break;
  }

  float alignOffsetY;
  switch (verticalAlign)
  {
    case VerticalAlign::Bottom:
      alignOffsetY = 0.0f;
      break;
    case VerticalAlign::Middle:
      alignOffsetY = viewport.height / 2.0f;
      break;
    case VerticalAlign::Top:
      alignOffsetY = viewport.height;
      break;
  }

  // Fetch offset
  float viewportOffsetX, viewportOffsetY;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("offsetX")).getValue(viewportOffsetX));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("offsetY")).getValue(viewportOffsetY));
  MPoint viewportOffset = computeViewportToWorld(frameContext,
                                                 alignOffsetX + viewportOffsetX * viewportUnitX,
                                                 alignOffsetY + viewportOffsetY * viewportUnitY,
                                                 depth);

  // Compute offset
  MMatrix viewMatrix = cameraDag.inclusiveMatrix();
  MPoint rayOrigin = MTransformationMatrix(viewMatrix).getTranslation(MSpace::kWorld);
  MVector ray = viewportOffset - rayOrigin;
  ray.normalize();
  MVector normal = MVector(viewMatrix(2, 0), viewMatrix(2, 1), viewMatrix(2, 2));
  MPoint origin;
  linePlaneIntersection(ray, rayOrigin, normal, nearBL, origin);

  // Fetch geometry
  float size, width, height;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("size")).getValue(size));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("width")).getValue(width));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("height")).getValue(height));

  // Prepare matrices
  MMatrix screenspaceTranslateMatrix;
  MMatrix screenspaceRotateMatrix;
  MMatrix screenspaceScaleMatrix;

  // Translate
  {
    MTransformationMatrix xform(MMatrix::identity);
    xform.setTranslation(origin, MSpace::kWorld);
    screenspaceTranslateMatrix = xform.asMatrix();
  }

  // Rotate
  screenspaceRotateMatrix = MTransformationMatrix(cameraDag.inclusiveMatrix()).asRotateMatrix();

  // Scale
  {
    MTransformationMatrix xform(MMatrix::identity);
    const double scale[3] = {size * width * worldspaceUnitX,
                             size * height * worldspaceUnitY, 1.0};
    xform.setScale(scale, MSpace::kTransform);
    screenspaceScaleMatrix = xform.asMatrix();
  }

  MMatrix screenWorldMatrix = screenspaceScaleMatrix * screenspaceRotateMatrix * screenspaceTranslateMatrix;
  data->m_matrix = screenWorldMatrix * pickerDag.inclusiveMatrixInverse();
}

void prepareGeometry(const MDagPath& pickerDag,
                     const MDagPath& cameraDag,
                     const MFrameContext& frameContext,
                     PickerUserData* data)
{
  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(pickerDag.node());

  MColor color;
  MFnNumericData colorData(MPlug(pickerObj, pickerCls.attribute("color")).asMObject());
  CHECK_MSTATUS(colorData.getData(color.r, color.g, color.b));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("opacity")).getValue(color.a));

  short _shape;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("shape")).getValue(_shape));
  Shape shape = static_cast<Shape>(_shape);

  Geometry geometry;

  switch(shape)
  {
    case Shape::Circle:
    {
      geometry.primitive = MUIDrawManager::Primitive::kTriangles;

      const std::size_t num = 16;
      float increment = 2.0 * M_PI / float(num);

      // Center
      geometry.vertices.append(MPoint(0.0f, 0.0f, 0.0f, 1.0f));
      geometry.normals.append(MVector(0.0f, 0.0f, 1.0f));
      geometry.colors.append(color);

      // Outside
      for (std::size_t i = 0; i <= num; ++i) {
        float angle = increment * i;
        geometry.vertices.append(MPoint(cosf(angle), sinf(angle), 0.0f, 1.0f));
        geometry.normals.append(MVector(0.0f, 0.0f, 1.0f));
        geometry.colors.append(color);
      }

      // Indices
      for (std::size_t i = 0; i < num; ++i) {
        geometry.indices.append(0);
        geometry.indices.append(i);
        geometry.indices.append(i + 1);
      }
      geometry.indices.append(0);
      geometry.indices.append(num);
      geometry.indices.append(1);
      break;
    }
    case Shape::Rectangle:
    {
      geometry.primitive = MUIDrawManager::Primitive::kTriangles;

      geometry.vertices.append(MPoint(0.0, 0.0, 0.0, 1.0));
      geometry.vertices.append(MPoint(1.0, 0.0, 0.0, 1.0));
      geometry.vertices.append(MPoint(1.0, 1.0, 0.0, 1.0));
      geometry.vertices.append(MPoint(0.0, 1.0, 0.0, 1.0));

      for (std::size_t i = 0; i < 4; ++i) {
        geometry.normals.append(MVector(0.0f, 0.0f, 1.0f));
        geometry.colors.append(color);
      }

      for (unsigned int index: {0, 1, 2, 0, 2, 3})
        geometry.indices.append(index);

      break;
    }
  }

  // Apply transformation
  for (std::size_t i = 0; i < geometry.vertices.length(); ++i)
    geometry.vertices[i] = data->m_matrix.transpose() * geometry.vertices[i];

  data->m_geometry = geometry;
}

void prepareStyle(const MDagPath& pickerDag,
                  const MDagPath& cameraDag,
                  const MFrameContext& context,
                  PickerUserData* data)
{
  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(pickerDag.node());

  MColor color;
  MFnNumericData colorData(MPlug(pickerObj, pickerCls.attribute("color")).asMObject());
  CHECK_MSTATUS(colorData.getData(color.r, color.g, color.b));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("opacity")).getValue(color.a));

  Style style;
  style.color = color;
  data->m_style = style;
}

MPxDrawOverride* PickerDrawOverride::creator(const MObject& obj)
{
  return new PickerDrawOverride(obj);
}

bool PickerDrawOverride::isTargetCamera(const MDagPath& pickerDag, const MDagPath& cameraDag) const
{
  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(pickerDag.node());
  const MPlug cameraPlug(pickerObj, pickerCls.attribute("camera"));

  MPlugArray srcPlugArray;
  cameraPlug.connectedTo(srcPlugArray, true, false);
  if (srcPlugArray.length() == 1)
  {
    const MPlug srcPlug(srcPlugArray[0]);
    const MObject srcNode(srcPlug.node());
    if (srcNode.hasFn(MFn::kCamera) && (srcNode == cameraDag.node()))
      return true;
  }
  return false;
}

MUserData* PickerDrawOverride::prepareForDraw(const MDagPath& pickerDag,
                                              const MDagPath& cameraDag,
                                              const MFrameContext& frameContext,
                                              MUserData* userData) {

  if (!isTargetCamera(pickerDag, cameraDag))
    return nullptr;

  PickerUserData* data = dynamic_cast<PickerUserData*>(userData);
  if (!data)
    data = new PickerUserData();

  prepareMatrix(pickerDag, cameraDag, frameContext, data);
  prepareGeometry(pickerDag, cameraDag, frameContext, data);
  prepareStyle(pickerDag, cameraDag, frameContext, data);

  return data;
}

void PickerDrawOverride::addUIDrawables(const MDagPath& objPath,
                                        MHWRender::MUIDrawManager& drawManager,
                                        const MHWRender::MFrameContext& frameContext,
                                        const MUserData* userData) {

  const PickerUserData* data = dynamic_cast<const PickerUserData*>(userData);
  if (!data)
    return;

  const MMatrix& matrix = data->matrix();
  const Geometry& geometry = data->geometry();
  const Style& style = data->style();

  drawManager.beginDrawable(MUIDrawManager::Selectability::kSelectable);
  drawManager.setPaintStyle(MUIDrawManager::kFlat);
  drawManager.setColor(style.color);
  drawManager.mesh(MUIDrawManager::Primitive::kTriangles,
                   geometry.vertices,
                   &geometry.normals,
                   &geometry.colors,
                   &geometry.indices,
                   nullptr);
  drawManager.endDrawable();
}

PickerDrawOverride::PickerDrawOverride(const MObject& obj) : MPxDrawOverride(obj, nullptr) {

}

}