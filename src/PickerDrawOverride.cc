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
#include <maya/MFnMatrixData.h>
#include <maya/MTransformationMatrix.h>

#include <vector>
#include <cmath>
#include <maya/MPlugArray.h>
//#include <GL/gl.h>


namespace screenspace {

MString PickerDrawOverride::classifcation = "drawdb/geometry/screenspace/picker";
MString PickerDrawOverride::id = "picker";


class PickerUserData : public MUserData {
public:
  struct Viewport {
    int width;
    int height;
    float scaleX;
    float scaleY;
  };

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

public:
  PickerUserData() : MUserData(false) {}
  ~PickerUserData() override = default;

public:
  Viewport m_viewport;
  Shape m_shape;
  MColor m_color;
  bool m_fill;
  float m_lineWidth;
  MUIDrawManager::LineStyle m_lineStyle;

  float m_size;
  float m_width;
  float m_height;

  Layout m_layout;
  float2 m_position;
  VerticalAlign m_verticalAlign;
  HorizontalAlign m_horizontalAlign;

  /// Matrix scaled to viewport worldspace
  MMatrix m_matrix;
};

/// TODO
static bool isTargetCamera(MDagPath picker, MDagPath camera)
{
  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(picker.node());
  const MPlug cameraPlug(pickerObj, pickerCls.attribute("camera"));

  MPlugArray srcPlugArray;
  cameraPlug.connectedTo(srcPlugArray, true, false);
  if (srcPlugArray.length() == 1)
  {
    const MPlug srcPlug(srcPlugArray[0]);
    const MObject srcNode(srcPlug.node());
    if (srcNode.hasFn(MFn::kCamera) && (srcNode == camera.node()))
      return true;
  }
  return false;
}

/// Compute world position for viewport and offset slightly in front of near clipping plane
void computeViewportToWorld(const MFrameContext& context,
                            int x, int y,
                            int depth,
                            MPoint& position)
{
  MPoint near, far;
  context.viewportToWorld(x, y, near, far);

  float increment = 1.0 + (0.01 * depth);

  // Output
  MVector direction = (far - near);
  direction.normalize();
  position = near + (direction * increment);
}

void static computeViewportToWorldDistance(const MFrameContext& context,
//                                           int offsetX,
//                                           int offsetY,
                                           int depth,
//                                           PickerUserData::HorizontalAlign horizontalAlign,
//                                           PickerUserData::VerticalAlign verticalAlign,
                                           MPoint& origin,
                                           float& scalarX,
                                           float& scalarY)
{
  int _, width, height;
  context.getViewportDimensions(_, _, width, height);

  MPoint nearBL, nearTR, offset;
  computeViewportToWorld(context, 0, 0, depth, nearBL);
  computeViewportToWorld(context, width, height, depth, nearTR);
//  computeViewportToWorld(context, offsetX, offsetY, depth, offset);

  float hyp = float((nearTR - nearBL).length());
  float theta = atanf(float(height) / float(width));
  scalarX = cosf(theta) * hyp;
  scalarY = sinf(theta) * hyp;

//  MPoint offsetHorizontalAlign;
//  switch (horizontalAlign)
//  {
//    case PickerUserData::HorizontalAlign::Left:
//      computeViewportToWorld(context, 0, 0, depth, offsetHorizontalAlign);
//      TNC_DEBUG << "HorizontalAlign::Left : offsetHorizontalAlign=" << offsetHorizontalAlign;
//      break;
//    case PickerUserData::HorizontalAlign::Center:
//      computeViewportToWorld(context, int(width/2.0f), 0, depth, offsetHorizontalAlign);
//      TNC_DEBUG << "HorizontalAlign::Center : offsetHorizontalAlign=" << offsetHorizontalAlign;
//      break;
//    case PickerUserData::HorizontalAlign::Right:
//      computeViewportToWorld(context, width, 0, depth, offsetHorizontalAlign);
//      TNC_DEBUG << "HorizontalAlign::Right : offsetHorizontalAlign=" << offsetHorizontalAlign;
//      break;
//  }
//
//  MPoint offsetVerticalAlign;
//  switch (verticalAlign)
//  {
//    case PickerUserData::VerticalAlign::Bottom:
//      computeViewportToWorld(context, 0, 0, depth, offsetVerticalAlign);
//      TNC_DEBUG << "VerticalAlign::Bottom : offsetVerticalAlign=" << offsetVerticalAlign;
//      break;
//    case PickerUserData::VerticalAlign::Center:
//      computeViewportToWorld(context, 0, int(height/2.0f), depth, offsetVerticalAlign);
//      TNC_DEBUG << "VerticalAlign::Center : offsetVerticalAlign=" << offsetVerticalAlign;
//      break;
//    case PickerUserData::VerticalAlign::Top:
//      computeViewportToWorld(context, 0, height, depth, offsetVerticalAlign);
//      TNC_DEBUG << "VerticalAlign::Top : offsetVerticalAlign=" << offsetVerticalAlign;
//      break;
//  }
//
//  TNC_DEBUG << "horizonal=" << (offsetHorizontalAlign - nearBL) << ", vertical=" << (offsetVerticalAlign - nearBL);

  // Output
//  origin = nearBL + (offsetHorizontalAlign - nearBL) + (offsetVerticalAlign - nearBL) + (offset - nearBL);
  origin = nearBL;
}

MPxDrawOverride* PickerDrawOverride::creator(const MObject& obj)
{
  return new PickerDrawOverride(obj);
}

MUserData* PickerDrawOverride::prepareForDraw(const MDagPath& objPath,
                                              const MDagPath& cameraPath,
                                              const MFrameContext& frameContext,
                                              MUserData* userData) {

  if (!isTargetCamera(objPath, cameraPath))
    return nullptr;

  PickerUserData* data = dynamic_cast<PickerUserData*>(userData);
  if (!data)
    data = new PickerUserData();

  const MNodeClass pickerCls(PickerShape::id);
  const MObject pickerObj(objPath.node());

  // Viewport
  int _, width, height;
  frameContext.getViewportDimensions(_, _, width, height);
  data->m_viewport.width = width;
  data->m_viewport.height = height;

  // Draw depth
  int depth;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("depth")).getValue(depth));

  // Distance between points in worldspace
  MPoint origin;
  float scaleX, scaleY;
  computeViewportToWorldDistance(frameContext,
                                 depth,
                                 origin,
                                 scaleX,
                                 scaleY);

  // Viewport scale factor
  switch (data->m_layout)
  {
    case PickerUserData::Layout::Relative:
      data->m_viewport.scaleX = scaleX / 100.0f;
      data->m_viewport.scaleY = scaleY / 100.0f;
      break;
    case PickerUserData::Layout::Absolute:
      data->m_viewport.scaleX = scaleX / data->m_viewport.width;
      data->m_viewport.scaleY = scaleY / data->m_viewport.height;
      break;
  }

  short horizontalAlign;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("horizontalAlign")).getValue(horizontalAlign));
  data->m_horizontalAlign = static_cast<PickerUserData::HorizontalAlign>(horizontalAlign);

  short verticalAlign;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("verticalAlign")).getValue(verticalAlign));
  data->m_verticalAlign = static_cast<PickerUserData::VerticalAlign>(verticalAlign);

  MPoint offset;
  {
    MPlug plug(pickerObj, pickerCls.attribute("offset"));
    MFnNumericData numData(plug.asMObject());

    float offsetX, offsetY;
    CHECK_MSTATUS(numData.getData(offsetX, offsetY));
    computeViewportToWorld(frameContext, offsetX, offsetY, depth, offset);
  }

  origin += (offset - origin) ;

  // Fetch data
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("size")).getValue(data->m_size));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("width")).getValue(data->m_width));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("height")).getValue(data->m_height));

  short layout;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("layout")).getValue(layout));
  data->m_layout = static_cast<PickerUserData::Layout>(layout);

  short shape;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("shape")).getValue(shape));
  data->m_shape = static_cast<PickerUserData::Shape>(shape);

  {
    MPlug plug(pickerObj, pickerCls.attribute("color"));
    MFnNumericData numData(plug.asMObject());
    CHECK_MSTATUS(numData.getData(data->m_color.r,
                                  data->m_color.g,
                                  data->m_color.b));
    CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("opacity")).getValue(data->m_color.a));
  }

  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("fill")).getValue(data->m_fill));
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("lineWidth")).getValue(data->m_lineWidth));

  short lineStyle;
  CHECK_MSTATUS(MPlug(pickerObj, pickerCls.attribute("lineStyle")).getValue(lineStyle));
  data->m_lineStyle = static_cast<MUIDrawManager::LineStyle>(lineStyle);

  float t[4][4] = {{1.0f,0.0f,0.0f,0.0f},
                   {0.0f,1.0f,0.0f,0.0f},
                   {0.0f,0.0f,1.0f,0.0f},
                   {float(origin.x), float(origin.y),float(origin.z),1}};
  float s[4][4] = {{scaleX,0.0f,0.0f,0.0f},
                   {0.0f,scaleY,0.0f,0.0f},
                   {0.0f,0.0f,1.0f,0.0f},
                   {0.0f,0.0f,0.0f,1.0f}};

  MMatrix translate(t);
  MMatrix rotate = MTransformationMatrix(cameraPath.inclusiveMatrix()).asRotateMatrix();
  MMatrix scale(s);
  MMatrix matrix = scale * rotate * translate;
  data->m_matrix = matrix * objPath.inclusiveMatrixInverse();

  MFnMatrixData matrixData;
  MObject matrixObj = matrixData.create(data->m_matrix);
  MFnDependencyNode(objPath.node()).findPlug("outMatrix").setValue(matrixObj);

  return data;
}

void PickerDrawOverride::addUIDrawables(const MDagPath& objPath,
                                        MHWRender::MUIDrawManager& drawManager,
                                        const MHWRender::MFrameContext& frameContext,
                                        const MUserData* userData) {

  const PickerUserData* data = dynamic_cast<const PickerUserData*>(userData);
  if (!data)
    return;

  MMatrix matrix = data->m_matrix;
  MPoint position(matrix(3, 0), matrix(3, 1), matrix(3, 2));
  MVector up(matrix(1, 0), matrix(1, 1), matrix(1, 2));
  MVector normal(matrix(2, 0), matrix(2, 1), matrix(2, 2));

  double scale[3];
  MTransformationMatrix(matrix).getScale(scale, MSpace::kWorld);

  float sizeX = data->m_size *  data->m_viewport.scaleX + data->m_width * data->m_viewport.scaleX ;
  float sizeY = data->m_size *  data->m_viewport.scaleY + data->m_height * data->m_viewport.scaleY;

  drawManager.beginDrawable(MUIDrawManager::Selectability::kSelectable);
  drawManager.setPaintStyle(MUIDrawManager::kFlat);
  drawManager.setColor(data->m_color);
  drawManager.setLineWidth(data->m_lineWidth);
  drawManager.setLineStyle(data->m_lineStyle);
  switch (data->m_shape)
  {
    case PickerUserData::Shape::Circle:
      drawManager.circle(position, normal, data->m_size * data->m_viewport.scaleY, data->m_fill);
      break;
    case PickerUserData::Shape::Rectangle:
      drawManager.rect(position, up, normal, sizeX, sizeY, data->m_fill);
      break;
  }
  drawManager.endDrawable();
}

PickerDrawOverride::PickerDrawOverride(const MObject& obj) : MPxDrawOverride(obj, nullptr) {

}

}