#include "AddCommand.hh"

#include "ss/Log.hh"
#include "ss/PickableShape.hh"
#include "ss/Types.hh"

#include <maya/MArgParser.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MNodeClass.h>
#include <maya/MSelectionList.h>

namespace screenspace {

using Flags = std::pair<const char*, const char*>;

static Flags kCameraFlags = {"-c", "-camera"};
static Flags kParentFlags = {"-p", "-parent"};
static Flags kDepthFlags = {"-d", "-depth"};
static Flags kPositionFlags = {"-p", "-position"};
static Flags kVerticalAlignFlags = {"-va", "-verticalAlign"};
static Flags kHorizontalAlignFlags = {"-ha", "-horizontalAlign"};
static Flags kShapeFlags = {"-s", "-shape"};
static Flags kColorFlags = {"-clr", "-color"};
static Flags kOpacityFlags = {"-op", "-opacity"};
static Flags kSizeFlags = {"-sz", "-size"};
static Flags kWidthFlags = {"-w", "-width"};
static Flags kHeightFlags = {"-w", "-height"};
static Flags kRotateFlags = {"-r", "-rotate"};
static Flags kOffsetFlags = {"-o", "-offset"};

MString AddCommand::typeName = "addPickable";

void* AddCommand::creator() {
  return new AddCommand();
}

AddCommand::AddCommand()
    : m_dgm(),
      m_parent(),
      m_camera(),
      m_depth(0),
      m_position(Position::Relative),
      m_verticalAlign(VerticalAlign::Bottom),
      m_horizontalAlign(HorizontalAlign::Left),
      m_shape(Shape::Rectangle),
      m_color(1.0, 0.0, 0.0, 1.0),
      m_size(1.0),
      m_width(10.0),
      m_height(10.0),
      m_offset(0.0, 0.0)
{}

MSyntax AddCommand::syntaxCreator() {

  MSyntax syntax;
  syntax.addFlag(kCameraFlags.first, kCameraFlags.second, MSyntax::kString);
  syntax.addFlag(kParentFlags.first, kParentFlags.second, MSyntax::kString);
  syntax.addFlag(kDepthFlags.first, kDepthFlags.second, MSyntax::kDouble);
  syntax.addFlag(kPositionFlags.first, kPositionFlags.second, MSyntax::kString);
  syntax.addFlag(kVerticalAlignFlags.first, kVerticalAlignFlags.second, MSyntax::kString);
  syntax.addFlag(kHorizontalAlignFlags.first, kHorizontalAlignFlags.second, MSyntax::kString);
  syntax.addFlag(kShapeFlags.first, kShapeFlags.second, MSyntax::kString);
  syntax.addFlag(kColorFlags.first, kColorFlags.second, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
  syntax.addFlag(kOpacityFlags.first, kOpacityFlags.second, MSyntax::kDouble);
  syntax.addFlag(kSizeFlags.first, kSizeFlags.second, MSyntax::kDouble);
  syntax.addFlag(kWidthFlags.first, kWidthFlags.second, MSyntax::kDouble);
  syntax.addFlag(kHeightFlags.first, kHeightFlags.second, MSyntax::kDouble);
  syntax.addFlag(kRotateFlags.first, kRotateFlags.second, MSyntax::kAngle);
  syntax.addFlag(kOffsetFlags.first, kOffsetFlags.second, MSyntax::kDouble, MSyntax::kDouble);
  return syntax;
}

MStatus AddCommand::doIt(const MArgList& args)
{
  MStatus status;
  MArgParser parser(syntax(), args);

  if (!parser.isFlagSet(kCameraFlags.second))
  {
    MGlobal::displayError("Error attaching pickable! No camera set.");
    return MS::kFailure;
  }

  if (!parser.isFlagSet(kParentFlags.second))
  {
    MGlobal::displayError("Error attaching pickable! No parent set.");
    return MS::kFailure;
  }

  MString cameraName;
  MString parentName;
  parser.getFlagArgument(kCameraFlags.second, 0, cameraName);
  parser.getFlagArgument(kParentFlags.second, 0, parentName);

  {
    MSelectionList list;
    status = list.add(parentName);
    if (status != MStatus::kSuccess)
    {
      MGlobal::displayError("Error attaching pickable! Parent does not exist: " + parentName);
      return MS::kFailure;
    }
    CHECK_MSTATUS(list.getDependNode(0, m_parent));
  }

  {
    MSelectionList list;
    status = list.add(cameraName);
    if (status != MStatus::kSuccess)
    {
      MGlobal::displayError("Error attaching pickable! Camera does not exist: " + cameraName);
      return MS::kFailure;
    }
    CHECK_MSTATUS(list.getDependNode(0, m_camera));
  }

  if (m_camera.apiType() == MFn::Type::kTransform)
  {
    MDagPath cameraPath;
    MDagPath::getAPathTo(m_camera, cameraPath);
    unsigned int cameraChildCount = cameraPath.childCount(&status);
    CHECK_MSTATUS(status);
    for (unsigned int i = 0; i < cameraChildCount; ++i)
    {
      MObject cameraChildObj = cameraPath.child(i, &status);
      CHECK_MSTATUS(status);
      if (cameraChildObj.hasFn(MFn::Type::kCamera))
      {
        m_camera = cameraChildObj;
        break;
      }
    }
  }

  // Layout
  if (parser.isFlagSet(kDepthFlags.second))
  CHECK_MSTATUS(parser.getFlagArgument(kDepthFlags.second, 0, m_depth));

  if (parser.isFlagSet(kPositionFlags.second))
  {
    MString _position;
    CHECK_MSTATUS(parser.getFlagArgument(kPositionFlags.second, 0, _position));

    int position = -1;
    if (_position == "relative")
      position = static_cast<int>(Position::Relative);
    else if (_position == "absolute")
      position = static_cast<int>(Position::Absolute);

    if (position == -1) {
      MGlobal::displayError("Error attaching pickable! '" + _position + "' is not a valid position.");
      return MS::kFailure;
    }
    m_position = static_cast<Position>(position);
  }

  if (parser.isFlagSet(kVerticalAlignFlags.second))
  {
    MString _alignment;
    CHECK_MSTATUS(parser.getFlagArgument(kVerticalAlignFlags.second, 0, _alignment));

    int alignment = -1;
    if (_alignment == "bottom")
      alignment = static_cast<int>(VerticalAlign::Bottom);
    else if (_alignment == "middle")
      alignment = static_cast<int>(VerticalAlign::Middle);
    else if (_alignment == "top")
      alignment = static_cast<int>(VerticalAlign::Top);

    if (alignment == -1) {
      MGlobal::displayError("Error attaching pickable! '" + _alignment + "' is not a valid vertical alignment.");
      return MS::kFailure;
    }
    m_verticalAlign = static_cast<VerticalAlign>(alignment);
  }

  if (parser.isFlagSet(kHorizontalAlignFlags.second))
  {
    MString _alignment;
    CHECK_MSTATUS(parser.getFlagArgument(kHorizontalAlignFlags.second, 0, _alignment));

    int alignment = -1;
    if (_alignment == "left")
      alignment = static_cast<int>(HorizontalAlign::Left);
    else if (_alignment == "middle")
      alignment = static_cast<int>(HorizontalAlign::Middle);
    else if (_alignment == "right")
      alignment = static_cast<int>(HorizontalAlign::Right);

    if (alignment == -1)
    {
      MGlobal::displayError("Error attaching pickable! '" + _alignment + "' is not a valid horizontal alignment.");
      return MS::kFailure;
    }
    m_horizontalAlign = static_cast<HorizontalAlign>(alignment);
  }

  // Geometry
  if (parser.isFlagSet(kShapeFlags.second))
  {
    MString _shape;
    CHECK_MSTATUS(parser.getFlagArgument(kShapeFlags.second, 0, _shape));

    int shape = -1;
    if (_shape == "circle")
      shape = static_cast<int>(Shape::Circle);
    else if (_shape == "rectangle")
      shape = static_cast<int>(Shape::Rectangle);
    else if (_shape == "triangle")
      shape = static_cast<int>(Shape::Triangle);

    if (shape == -1)
    {
      MGlobal::displayError("Error attaching pickable! '" + _shape + "' is not a valid shape.");
      return MS::kFailure;
    }

    m_shape = static_cast<Shape>(shape);
  }

  if (parser.isFlagSet(kColorFlags.second))
  {
    double r, g, b;
    CHECK_MSTATUS(parser.getFlagArgument(kColorFlags.second, 0, r));
    CHECK_MSTATUS(parser.getFlagArgument(kColorFlags.second, 1, g));
    CHECK_MSTATUS(parser.getFlagArgument(kColorFlags.second, 2, b));
    m_color = MColor(r, g, b);
  }

  if (parser.isFlagSet(kOpacityFlags.second))
  {
    double a;
    CHECK_MSTATUS(parser.getFlagArgument(kOpacityFlags.second, 0, a));
    m_color.a = a;
  }

  if (parser.isFlagSet(kRotateFlags.second))
  {
    double rotate;
    CHECK_MSTATUS(parser.getFlagArgument(kRotateFlags.second, 0, rotate));
    m_rotate = MAngle(rotate, MAngle::kDegrees);
  }

  if (parser.isFlagSet(kSizeFlags.second))
    CHECK_MSTATUS(parser.getFlagArgument(kSizeFlags.second, 0, m_size));

  if (parser.isFlagSet(kWidthFlags.second))
    CHECK_MSTATUS(parser.getFlagArgument(kWidthFlags.second, 0, m_width));

  if (parser.isFlagSet(kHeightFlags.second))
    CHECK_MSTATUS(parser.getFlagArgument(kWidthFlags.second, 0, m_height));

  if (parser.isFlagSet(kOffsetFlags.second))
  {
    CHECK_MSTATUS(parser.getFlagArgument(kOffsetFlags.second, 0, m_offset.x));
    CHECK_MSTATUS(parser.getFlagArgument(kOffsetFlags.second, 1, m_offset.y));
  }

  return redoIt();
}

MStatus AddCommand::redoIt() {

  // Process
  MStatus status;
  MObject pickableObj = m_dgm.createNode(PickableShape::id, m_parent, &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(m_dgm.connect(MPlug(m_camera, MNodeClass("camera").attribute("message")),
                              MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("camera"))));

  CHECK_MSTATUS(m_dgm.newPlugValueInt(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("depth")), m_depth));
  CHECK_MSTATUS(m_dgm.newPlugValueShort(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("position")), static_cast<short>(m_position)));
  CHECK_MSTATUS(m_dgm.newPlugValueShort(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("verticalAlign")), static_cast<short>(m_verticalAlign)));
  CHECK_MSTATUS(m_dgm.newPlugValueShort(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("horizontalAlign")), static_cast<short>(m_horizontalAlign)));
  CHECK_MSTATUS(m_dgm.newPlugValueShort(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("shape")), static_cast<short>(m_shape)));

  {
    MFnNumericData numData;
    MObject numObj = numData.create(MFnNumericData::k3Float, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numData.setData(float(m_color.r), float(m_color.g), float(m_color.b)));
    CHECK_MSTATUS(m_dgm.newPlugValue(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("color")), numObj));
  }

  CHECK_MSTATUS(m_dgm.newPlugValueFloat(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("opacity")), m_color.a));
  CHECK_MSTATUS(m_dgm.newPlugValueFloat(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("size")), float(m_size)));
  CHECK_MSTATUS(m_dgm.newPlugValueFloat(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("width")), float(m_width)));
  CHECK_MSTATUS(m_dgm.newPlugValueFloat(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("height")), float(m_height)));
  CHECK_MSTATUS(m_dgm.newPlugValueMAngle(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("rotate")), m_rotate));

  {
    MFnNumericData numData;
    MObject numObj = numData.create(MFnNumericData::k2Float, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numData.setData(float(m_offset.x), float(m_offset.y)));
    CHECK_MSTATUS(m_dgm.newPlugValue(MPlug(pickableObj, MNodeClass(PickableShape::id).attribute("offset")), numObj));
  }

  CHECK_MSTATUS(m_dgm.doIt());
  return MS::kSuccess;
}

MStatus AddCommand::undoIt() {
  return m_dgm.undoIt();
}

}