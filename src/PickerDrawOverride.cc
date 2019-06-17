#include "PickerDrawOverride.hh"
#include "PickerShape.hh"
#include "Log.hh"

#include <maya/MBoundingBox.h>
#include <maya/MUserData.h>
#include <maya/MPxNode.h>
#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MNodeClass.h>
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
#include <maya/MSelectionContext.h>
#include <maya/MUIDrawManager.h>
#include <maya/MPlug.h>
#include <maya/MFnTransform.h>
#include <maya/MVectorArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MColorArray.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MQuaternion.h>

#include <vector>
#include <cmath>
#include <maya/MPlugArray.h>
//#include <GL/gl.h>


namespace screenspace {

MString PickerDrawOverride::classifcation = "drawdb/geometry/screenspace/picker";
MString PickerDrawOverride::id = "picker";

enum class Shape {
  Circle,
  Rectangle,
};

enum class Layout {
  Relative,
  Absolute,
};

enum class VerticalAlign {
  Bottom,
  Center,
  Top,
};

enum class HorizontalAlign {
  Left,
  Center,
  Right,
};

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
  bool fill;
  float lineWidth;
  MUIDrawManager::LineStyle lineStyle;
};

/// Compute world position for viewport and offset slightly in front of near clipping plane
MPoint computeViewportToWorld(const MFrameContext& context,
                              int x, int y,
                              int depth)
{
  MPoint near, far;
  context.viewportToWorld(x, y, near, far);

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

// ---------------------------------------------------------------------------------------------------------------------

void prepareMatrix(const MDagPath& pickerDag,
                   const MDagPath& cameraDag,
                   const MFrameContext& frameContext,
                   PickerUserData* data)
{
  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(pickerDag.node());

  int _, screenspaceWidth, screenspaceHeight;
  frameContext.getViewportDimensions(_, _, screenspaceWidth, screenspaceHeight);

  // Draw depth
  int depth;

  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("depth")).getValue(depth));

  const MPoint nearBL = computeViewportToWorld(frameContext, 0, 0, depth);
  const MPoint nearTR = computeViewportToWorld(frameContext, screenspaceWidth, screenspaceHeight, depth);

  float hyp = float((nearTR - nearBL).length());
  float theta = atanf(float(screenspaceHeight) / float(screenspaceWidth));
  float distanceX = cosf(theta) * hyp;
  float distanceY = sinf(theta) * hyp;

  Viewport viewport;
  viewport.width = screenspaceWidth;
  viewport.height = screenspaceHeight;
  viewport.worldspaceWidth = distanceX;
  viewport.worldspaceHeight = distanceY;
  data->m_viewport = viewport;

  // Fetch data
  float size, width, height;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("size")).getValue(size));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("width")).getValue(width));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("height")).getValue(height));

  // Draw origin
  float viewportOffsetX, viewportOffsetY;
  MFnNumericData numData(MPlug(pickerObj, pickerCls.attribute("offset")).asMObject());
  CHECK_MSTATUS(numData.getData(viewportOffsetX, viewportOffsetY));

  short _layout;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("layout")).getValue(_layout));
  Layout layout = static_cast<Layout>(_layout);

  // Viewport scale factor
  float worldspaceUnitX, worldspaceUnitY;
  switch (layout)
  {
    case Layout::Relative:
      worldspaceUnitX = viewport.worldspaceWidth / 100.0f;
      worldspaceUnitY = viewport.worldspaceHeight / 100.0f;
      break;
    case Layout::Absolute:
      worldspaceUnitX = viewport.worldspaceWidth / viewport.width;
      worldspaceUnitY = viewport.worldspaceHeight / viewport.height;
      break;
  }

  // Prepare matrices
  MMatrix offsetMatrix;
  MMatrix screenspaceTranslateMatrix;
  MMatrix screenspaceRotateMatrix;
  MMatrix screenspaceScaleMatrix;

  // Translate
  {
    const MPoint origin = nearBL;
    MTransformationMatrix xform(MMatrix::identity);
    xform.setTranslation(origin, MSpace::kWorld);
    screenspaceTranslateMatrix = xform.asMatrix();
  }

  // Rotate
  screenspaceRotateMatrix = MTransformationMatrix(cameraDag.inclusiveMatrix()).asRotateMatrix();

  // Scale
  {
    MTransformationMatrix xform(MMatrix::identity);
    const double scale[3] = {size * width * worldspaceUnitX, size * height * worldspaceUnitY, 1.0};
    xform.setScale(scale, MSpace::kTransform);
    screenspaceScaleMatrix = xform.asMatrix();
  }

  // Offset
  {
//    TNC_DEBUG << "origin=" << origin;
//    MPoint fooPointA(viewportOffsetX * worldspaceUnitX, viewportOffsetY * worldspaceUnitY, 0.0);
//    MTransformationMatrix xform(MMatrix::identity);
//    xform.setTranslation(fooPointA, MSpace::kTransform);
//    MMatrix fooMatrix = xform.asMatrix();
//    xform = MTransformationMatrix(fooMatrix * shapeMatrix);
//    MPoint fooPointB = xform.getTranslation(MSpace::kWorld);
//    TNC_DEBUG << "fooPointB=" << fooPointB;
//    shapeMatrix = xform.asMatrix();
  }

  data->m_matrix = screenspaceScaleMatrix * screenspaceRotateMatrix * screenspaceTranslateMatrix * pickerDag.inclusiveMatrixInverse();

  // Debug
  {
//    float viewportspaceOffsetX = tanf(theta) * viewportOffsetX * worldspaceUnitY;
//    float viewportspaceOffsetY = sinf(theta) * viewportOffsetY * worldspaceUnitY;

    // This is view space
//    MTransformationMatrix xform(MMatrix::identity);
//    xform.setTranslation(MPoint(viewportOffsetX, viewportOffsetY, 0), MSpace::kWorld);
//    MMatrix viewspaceOffsetMatrix = xform.asMatrix();
//    data->m_matrix  = viewspaceOffsetMatrix * data->m_matrix;

//    MTransformationMatrix bt(viewspaceOffsetMatrix);
//    MPoint blahPos(bt.getTranslation(MSpace::kWorld));

//    MStatus status;
//    MFnNumericData pointData;
//    MObject pointObj = pointData.create(MFnNumericData::Type::k3Float, &status);
//    CHECK_MSTATUS(status);
//    status = pointData.setData(float(blahPos.x), float(blahPos.y), float(blahPos.z));
//    CHECK_MSTATUS(status);
//    CHECK_MSTATUS(MFnDependencyNode(pickerObj).findPlug("outPosition").setValue(pointObj));
  }
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


  float _vertices[4][4] = {{0.0, 0.0, 0.0, 1.0},
                           {1.0, 0.0, 0.0, 1.0},
                           {1.0, 1.0, 0.0, 1.0},
                           {0.0, 1.0, 0.0, 1.0}};

  float _normals[4][3] = {{0.0, 0.0, 1.0},
                          {0.0, 0.0, 1.0},
                          {0.0, 0.0, 1.0},
                          {0.0, 0.0, 1.0}};

  float _colors[4][4] = {{color.r, color.g, color.b, color.a},
                         {color.r, color.g, color.b, color.a},
                         {color.r, color.g, color.b, color.a},
                         {color.r, color.g, color.b, color.a}};

  unsigned int _indices[6] = {0, 1, 2, 0, 2, 3};

  Geometry geometry;
  geometry.primitive = MUIDrawManager::Primitive::kTriangles;
  geometry.vertices = MPointArray(_vertices, 4);
  geometry.normals = MVectorArray(_normals, 4);
  geometry.colors = MColorArray(_colors, 4);
  geometry.indices = MUintArray(_indices, 6);

  // Apply transformation
  for (std::size_t i = 0; i < geometry.vertices.length(); ++i)
    geometry.vertices[i] = data->m_matrix.transpose() * geometry.vertices[i];

  // Store
  data->m_geometry = geometry;
}

void prepareStyle(const MDagPath& pickerDag,
                  const MDagPath& cameraDag,
                  const MFrameContext& context,
                  PickerUserData* data)
{
  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(pickerDag.node());

  short _shape;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("shape")).getValue(_shape));
  Shape drawShape = static_cast<Shape>(_shape);

  MColor color;
  MFnNumericData colorData(MPlug(pickerObj, pickerCls.attribute("color")).asMObject());
  CHECK_MSTATUS(colorData.getData(color.r, color.g, color.b));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("opacity")).getValue(color.a));

  float fill, lineWidth;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("fill")).getValue(fill));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("lineWidth")).getValue(lineWidth));

  short _lineStyle;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("lineStyle")).getValue(_lineStyle));
  MUIDrawManager::LineStyle lineStyle = static_cast<MUIDrawManager::LineStyle>(_lineStyle);

  Style style;
  style.shape = drawShape;
  style.color = color;
  style.fill = fill;
  style.lineStyle = lineStyle;
  style.lineWidth = lineWidth;
  data->m_style = style;
}

// ---------------------------------------------------------------------------------------------------------------------

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

  // Debug
  MFnMatrixData matrixData;
  MObject matrixObj = matrixData.create(data->m_matrix);
  MFnDependencyNode(pickerDag.node()).findPlug("outMatrix").setValue(matrixObj);

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
  drawManager.setLineWidth(style.lineWidth);
  drawManager.setLineStyle(style.lineStyle);
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