#include "PickableDrawOverride.hh"

#include "ss/Log.hh"
#include "ss/PickableShape.hh"
#include "ss/Types.hh"

#include <maya/MColorArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MNodeClass.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPointArray.h>
#include <maya/MUintArray.h>
#include <maya/MVectorArray.h>

#include <cmath>

namespace screenspace {

MString PickableDrawOverride::classification = "drawdb/geometry/ss/pickable";
MString PickableDrawOverride::id = "pickable";

/// Viewport helper
struct Viewport {
  int width;               // Width of viewport in pixels
  int height;              // Height of viewport in pixels
  float worldspaceWidth;   // Distance in worldspace for height of viewport
  float worldspaceHeight;  // Distance in worldspace for width of viewport
};

/// Geometry helper
struct Geometry {
  MUIDrawManager::Primitive primitive;  // Render primitive
  MPointArray vertices;                 // Vertices
  MVectorArray normals;                 // Per-vertex normal
  MColorArray colors;                   // Per-vertex color
  MUintArray indices;                   // Poly indices
};

/// Style helper
struct Style {
  Shape shape;   // Draw shape
  MColor color;  // Shape color
};

/// Find intersection point on a plane.
/// \param ray The normalized ray direction.
/// \param origin The origin of the ray.
/// \param normal The normal of the plane.
/// \param coord Some point on the plane
/// \param contact The intersection point in worldspace.
/// \return If an intersection occurred.
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

/// Compute a worldspace point for (x, y) coordinates in pixels
/// from bottom left of viewport.
/// \param frameContext The frame context before drawing.
/// \param nearClipPlane Near clip plane value from camera.
/// \param x Viewport position in pixels.
/// \param y Viewport position in pixels.
/// \param depth Depth of shape.
/// \return The point.
MPoint computeViewportToWorld(const MFrameContext& frameContext,
                              float nearClipPlane,
                              int x, int y, int depth)
{
  MPoint near, far;
  frameContext.viewportToWorld(x, y, near, far);

  MVector direction = (far - near);
  direction.normalize();

  float scalar = nearClipPlane + 0.001f * (depth + 1);
  return near + (direction * scalar);
}

class PickableUserData : public MUserData {
public:
  PickableUserData() : MUserData(false) {}
  ~PickableUserData() override = default;

public:
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

/// Prepare matrix for drawing.
/// \param pickablePath Path to pickable.
/// \param cameraPath Path to camera.
/// \param frameContext Viewport frame context.
/// \param data Will have it's matrix and viewport data populated.
void prepareMatrix(const MDagPath& pickablePath,
                   const MDagPath& cameraPath,
                   const MFrameContext& frameContext,
                   PickableUserData* data)
{
  const MNodeClass pickableCls(PickableShape::id);
  const MObject pickableObj(pickablePath.node());

  int _, viewportWidth, viewportHeight;
  frameContext.getViewportDimensions(_, _, viewportWidth, viewportHeight);

  const MDagPath camera = frameContext.getCurrentCameraPath();
  const MFnDependencyNode cameraDep(camera.node());
  const MNodeClass cameraCls(cameraDep.typeId());

  float nearClipPlane;
  MPlug(camera.node(), cameraCls.attribute("nearClipPlane")).getValue(nearClipPlane);

  // Draw depth
  int depth;
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("depth")).getValue(depth));

  // Compute viewport data
  const MPoint nearBL = computeViewportToWorld(frameContext, nearClipPlane, 0, 0, depth);
  const MPoint nearTR = computeViewportToWorld(frameContext, nearClipPlane, viewportWidth, viewportHeight, depth);
  float hyp = float((nearTR - nearBL).length());
  float theta = atanf(float(viewportHeight) / float(viewportWidth));
  float worldspaceWidth = cosf(theta) * hyp;
  float worldspaceHeight = sinf(theta) * hyp;

  // Populate viewport
  Viewport viewport;
  viewport.width = viewportWidth;
  viewport.height = viewportHeight;
  viewport.worldspaceWidth = worldspaceWidth;
  viewport.worldspaceHeight = worldspaceHeight;
  data->m_viewport = viewport;

  short _position;
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("position")).getValue(_position));
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
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("horizontalAlign")).getValue(_horizontalAlign));
  HorizontalAlign horizontalAlign = static_cast<HorizontalAlign>(_horizontalAlign);

  float _verticalAlign;
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("verticalAlign")).getValue(_verticalAlign));
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
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("offsetX")).getValue(viewportOffsetX));
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("offsetY")).getValue(viewportOffsetY));
  MPoint viewportOffset = computeViewportToWorld(frameContext,
                                                 nearClipPlane,
                                                 alignOffsetX + viewportOffsetX * viewportUnitX,
                                                 alignOffsetY + viewportOffsetY * viewportUnitY,
                                                 depth);

  // Compute offset
  MMatrix viewMatrix = cameraPath.inclusiveMatrix();
  MPoint rayOrigin = MTransformationMatrix(viewMatrix).getTranslation(MSpace::kWorld);
  MVector ray = viewportOffset - rayOrigin;
  ray.normalize();
  MVector normal = MVector(viewMatrix(2, 0), viewMatrix(2, 1), viewMatrix(2, 2));
  MPoint origin;
  linePlaneIntersection(ray, rayOrigin, normal, nearBL, origin);

  // Fetch geometry
  float size, width, height;
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("size")).getValue(size));
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("width")).getValue(width));
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("height")).getValue(height));

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
  screenspaceRotateMatrix = MTransformationMatrix(cameraPath.inclusiveMatrix()).asRotateMatrix();

  // Scale
  {
    MTransformationMatrix xform(MMatrix::identity);
    const double scale[3] = {size * width * worldspaceUnitX,
                             size * height * worldspaceUnitY, 1.0};
    xform.setScale(scale, MSpace::kTransform);
    screenspaceScaleMatrix = xform.asMatrix();
  }

  MMatrix screenWorldMatrix = screenspaceScaleMatrix * screenspaceRotateMatrix * screenspaceTranslateMatrix;
  data->m_matrix = screenWorldMatrix * pickablePath.inclusiveMatrixInverse();
}

/// Prepare geometry to be drawn.
/// \param pickablePath Path to pickable.
/// \param cameraPath Path to camera.
/// \param frameContext Viewport frame context.
/// \param data Will have it's geometry data populated.
void prepareGeometry(const MDagPath& pickablePath,
                     const MDagPath& cameraPath,
                     const MFrameContext& frameContext,
                     PickableUserData* data)
{
  const MNodeClass pickableCls(PickableShape::id);
  const MObject pickableObj(pickablePath.node());

  MColor color;
  MFnNumericData colorData(MPlug(pickableObj, pickableCls.attribute("color")).asMObject());
  CHECK_MSTATUS(colorData.getData(color.r, color.g, color.b));
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("opacity")).getValue(color.a));

  short _shape;
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("shape")).getValue(_shape));
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

/// Prepare geometry style.
/// \param pickablePath Path to pickable.
/// \param cameraPath Path to camera.
/// \param frameContext Viewport frame context.
/// \param data Will have it's style data populated.
void prepareStyle(const MDagPath& pickableDag,
                  const MDagPath& cameraDag,
                  const MFrameContext& context,
                  PickableUserData* data)
{
  const MNodeClass pickableCls(PickableShape::id);
  const MObject pickableObj(pickableDag.node());

  MColor color;
  MFnNumericData colorData(MPlug(pickableObj, pickableCls.attribute("color")).asMObject());
  CHECK_MSTATUS(colorData.getData(color.r, color.g, color.b));
  CHECK_MSTATUS(MPlug(pickableObj, pickableCls.attribute("opacity")).getValue(color.a));

  Style style;
  style.color = color;
  data->m_style = style;
}

MPxDrawOverride* PickableDrawOverride::creator(const MObject& obj)
{
  return new PickableDrawOverride(obj);
}

bool PickableDrawOverride::isAttachedCamera(const MDagPath& pickableDag,
                                            const MDagPath& cameraDag) const
{
  const MNodeClass pickableCls(PickableShape::id);
  const MObject pickableObj(pickableDag.node());
  const MPlug cameraPlug(pickableObj, pickableCls.attribute("camera"));

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

MHWRender::DrawAPI PickableDrawOverride::supportedDrawAPIs() const {
  return MHWRender::kAllDevices;
}

MUserData* PickableDrawOverride::prepareForDraw(const MDagPath& pickableDag,
                                              const MDagPath& cameraDag,
                                              const MFrameContext& frameContext,
                                              MUserData* userData) {

  if (!isAttachedCamera(pickableDag, cameraDag))
    return nullptr;

  PickableUserData* data = dynamic_cast<PickableUserData*>(userData);
  if (!data)
    data = new PickableUserData();

  // Prepare
  prepareMatrix(pickableDag, cameraDag, frameContext, data);
  prepareGeometry(pickableDag, cameraDag, frameContext, data);
  prepareStyle(pickableDag, cameraDag, frameContext, data);

  return data;
}

void PickableDrawOverride::addUIDrawables(const MDagPath& objPath,
                                        MHWRender::MUIDrawManager& drawManager,
                                        const MHWRender::MFrameContext& frameContext,
                                        const MUserData* userData) {

  const PickableUserData* data = dynamic_cast<const PickableUserData*>(userData);
  if (!data)
    return;

  // Fetch
  const MMatrix& matrix = data->matrix();
  const Geometry& geometry = data->geometry();
  const Style& style = data->style();

  // Draw
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

}