#include "PickerCommand.hh"
#include "PickerShape.hh"
#include "Types.hh"
#include "Log.hh"

#include <maya/MGlobal.h>
#include <maya/MArgParser.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>
#include <maya/MDagModifier.h>
#include <maya/MNodeClass.h>

namespace screenspace {

using Flags = std::pair<const char*, const char*>;

static Flags kCameraFlags = {"-c", "-camera"};
static Flags kParentFlags = {"-p", "-parent"};

static Flags kDepthFlags = {"-d", "-depth"};
static Flags kPositionFlags = {"-l", "-layout"};
static Flags kVerticalAlignFlags = {"-va", "-verticalAlign"};
static Flags kHorizontalAlignFlags = {"-ha", "-horizontalAlign"};

static Flags kShapeFlags = {"-s", "-shape"};
static Flags kColorFlags = {"-clr", "-color"};
static Flags kOpacityFlags = {"-op", "-opacity"};
static Flags kSizeFlags = {"-sz", "-size"};
static Flags kWidthFlags = {"-w", "-width"};
static Flags kHeightFlags = {"-w", "-height"};
static Flags kOffsetFlags = {"-o", "-offset"};

MString CreatePickerCommand::typeName = "attachPicker";

void* CreatePickerCommand::creator() {
  return new CreatePickerCommand();
}

MSyntax CreatePickerCommand::createSyntax() {

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
  syntax.addFlag(kOffsetFlags.first, kOffsetFlags.second, MSyntax::kDouble, MSyntax::kDouble);

  return syntax;
}

MStatus CreatePickerCommand::doIt(const MArgList& args)
{

  MStatus status;
  MArgParser parser(syntax(), args);

  if (!parser.isFlagSet(kCameraFlags.second))
  {
    MGlobal::displayError("Error attaching picker: No camera set.");
    return MS::kFailure;
  }

  if (!parser.isFlagSet(kParentFlags.second))
  {
    MGlobal::displayError("Error attaching picker: No parent set.");
    return MS::kFailure;
  }

  MString cameraName;
  MString parentName;
  parser.getFlagArgument(kCameraFlags.second, 0, cameraName);
  parser.getFlagArgument(kParentFlags.second, 0, parentName);

  MDagPath parentPath;
  MDagPath cameraPath;

  {
    MSelectionList list;
    status = list.add(parentName);
    if (status != MStatus::kSuccess)
    {
      MGlobal::displayError("Error attaching picker! Parent does not exist: " + parentName);
      return MS::kFailure;
    }
    CHECK_MSTATUS(list.getDagPath(0, parentPath));
  }

  {
    MSelectionList list;
    status = list.add(cameraName);
    if (status != MStatus::kSuccess)
    {
      MGlobal::displayError("Error attaching picker! Camera does not exist: " + cameraName);
      return MS::kFailure;
    }
    CHECK_MSTATUS(list.getDagPath(0, cameraPath));
  }

  if (cameraPath.apiType() == MFn::Type::kTransform)
  {
    unsigned int cameraChildCount = cameraPath.childCount(&status);
    CHECK_MSTATUS(status);
    for (unsigned int i = 0; i < cameraChildCount; ++i)
    {
      MObject cameraChildObj = cameraPath.child(i, &status);
      CHECK_MSTATUS(status);
      if (cameraChildObj.hasFn(MFn::Type::kCamera))
      {
        MDagPath cameraChildPath;
        status = MDagPath::getAPathTo(cameraChildObj, cameraChildPath);
        if (status == MS::kSuccess)
        {
          cameraPath = cameraChildPath;
          break;
        }
      }
    }
  }

  // Process
  MDagModifier dgm;
  MObject pickerObj = dgm.createNode(PickerShape::id, parentPath.node(), &status);
  CHECK_MSTATUS(status);
  CHECK_MSTATUS(dgm.connect(MPlug(cameraPath.node(), MNodeClass("camera").attribute("message")),
                            MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("camera"))));

  // Layout
  if (parser.isFlagSet(kDepthFlags.second))
  {
    int depth;
    CHECK_MSTATUS(parser.getFlagArgument(kDepthFlags.second, 0, depth));
    CHECK_MSTATUS(dgm.newPlugValueInt(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("depth")), float(depth)));
  }

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
      MGlobal::displayError("Error attaching picker! '" + _position + "' is not a valid position.");
      return MS::kFailure;
    }

    CHECK_MSTATUS(dgm.newPlugValueInt(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("position")), position));
  }

  if (parser.isFlagSet(kVerticalAlignFlags.second))
  {
    MString _alignment;
    CHECK_MSTATUS(parser.getFlagArgument(kVerticalAlignFlags.second, 0, _alignment));

    // TODO: Use Enum here, remove hard-coded values
    int alignment = -1;
    if (_alignment == "bottom")
      alignment = static_cast<int>(VerticalAlign::Bottom);
    else if (_alignment == "middle")
      alignment = static_cast<int>(VerticalAlign::Middle);
    else if (_alignment == "top")
      alignment = static_cast<int>(VerticalAlign::Top);

    if (alignment == -1) {
      MGlobal::displayError("Error attaching picker! '" + _alignment + "' is not a valid vertical alignment.");
      return MS::kFailure;
    }
    CHECK_MSTATUS(dgm.newPlugValueInt(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("verticalAlign")), alignment));
  }

  if (parser.isFlagSet(kHorizontalAlignFlags.second))
  {
    MString _alignment;
    CHECK_MSTATUS(parser.getFlagArgument(kHorizontalAlignFlags.second, 0, _alignment));

    // TODO: Use Enum here, remove hard-coded values
    int alignment = -1;
    if (_alignment == "left")
      alignment = static_cast<int>(HorizontalAlign::Left);
    else if (_alignment == "middle")
      alignment = static_cast<int>(HorizontalAlign::Middle);
    else if (_alignment == "right")
      alignment = static_cast<int>(HorizontalAlign::Right);

    if (alignment == -1)
    {
      MGlobal::displayError("Error attaching picker! '" + _alignment + "' is not a valid horizontal alignment.");
      return MS::kFailure;
    }
    CHECK_MSTATUS(dgm.newPlugValueInt(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("horizontalAlign")), alignment));
  }

  // Geometry
  if (parser.isFlagSet(kShapeFlags.second))
  {
    MString _shape;
    CHECK_MSTATUS(parser.getFlagArgument(kShapeFlags.second, 0, _shape));

    // TODO: Use Enum here, remove hard-coded values
    int shape = -1;
    if (_shape == "circle")
      shape = static_cast<int>(Shape::Circle);
    else if (_shape == "rectangle")
      shape = static_cast<int>(Shape::Rectangle);

    if (shape == -1)
    {
      MGlobal::displayError("Error attaching picker! '" + _shape + "' is not a valid shape.");
      return MS::kFailure;
    }
    CHECK_MSTATUS(dgm.newPlugValueInt(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("shape")), shape));
  }

  if (parser.isFlagSet(kColorFlags.second))
  {
    double r, g, b;
    CHECK_MSTATUS(parser.getFlagArgument(kColorFlags.second, 0, r));
    CHECK_MSTATUS(parser.getFlagArgument(kColorFlags.second, 1, g));
    CHECK_MSTATUS(parser.getFlagArgument(kColorFlags.second, 2, b));

    MFnNumericData numData;
    MObject numObj = numData.create(MFnNumericData::k3Float, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numData.setData(float(r), float(g), float(b)));
    CHECK_MSTATUS(dgm.newPlugValue(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("color")), numObj));
  }

  if (parser.isFlagSet(kOpacityFlags.second))
  {
    double opacity;
    CHECK_MSTATUS(parser.getFlagArgument(kOpacityFlags.second, 0, opacity));
    CHECK_MSTATUS(dgm.newPlugValueFloat(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("opacity")), float(opacity)));
  }

  if (parser.isFlagSet(kSizeFlags.second))
  {
    double size;
    CHECK_MSTATUS(parser.getFlagArgument(kSizeFlags.second, 0, size));
    CHECK_MSTATUS(dgm.newPlugValueFloat(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("size")), float(size)));
  }

  if (parser.isFlagSet(kWidthFlags.second))
  {
    double width;
    CHECK_MSTATUS(parser.getFlagArgument(kWidthFlags.second, 0, width));
    CHECK_MSTATUS(dgm.newPlugValueFloat(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("width")), float(width)));
  }

  if (parser.isFlagSet(kHeightFlags.second))
  {
    double height;
    CHECK_MSTATUS(parser.getFlagArgument(kWidthFlags.second, 0, height));
    CHECK_MSTATUS(dgm.newPlugValueFloat(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("height")), float(height)));
  }

  if (parser.isFlagSet(kOffsetFlags.second))
  {
    double offsetX, offsetY;
    CHECK_MSTATUS(parser.getFlagArgument(kOffsetFlags.second, 0, offsetX));
    CHECK_MSTATUS(parser.getFlagArgument(kOffsetFlags.second, 1, offsetY));

    MFnNumericData numData;
    MObject numObj = numData.create(MFnNumericData::k2Float, &status);
    CHECK_MSTATUS(status);
    CHECK_MSTATUS(numData.setData(float(offsetX), float(offsetY)));
    CHECK_MSTATUS(dgm.newPlugValue(MPlug(pickerObj, MNodeClass(PickerShape::id).attribute("offset")), numObj));
  }

  CHECK_MSTATUS(dgm.doIt());
  return MS::kSuccess;
}

}