//-
// ==========================================================================
// Copyright 2015 Autodesk, Inc.  All rights reserved.
//
// Use of this software is subject to the terms of the Autodesk
// license agreement provided at the time of installation or download,
// or which otherwise accompanies this software in either electronic
// or hard copy form.
// ==========================================================================
//+
// Plugin: uiDrawManager.cpp
// This plugin demonstrates the use of MUIDrawManager.
//
// It provides the 'uiDrawManager' node which uses MUIDrawManager to display 3D (world coordinates)
// or 2D (screen coordinates) primitives such as text, lines, spheres, etc, based on values set
// on its attributes.
//
// For example, to draw a 2D, 15x30 pixel rectangle in the viewport, centered on pixel position
// (50, 20) and tilted at a 45 degree angle, you would do the following:
// createNode -n testo uiDrawManager;
// setAttr testo.draw2D true;
// setAttr testo.rectScale -type double2 15 30;
// setAttr testo.position -type  double3 50 20 0;
// setAttr testo.rectUp -type double3 1 1 0;
#include <maya/MPxLocatorNode.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnStringData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MPointArray.h>
#include <maya/MFloatPointArray.h>
#include <maya/MUintArray.h>
// Viewport 2.0 includes
#include <maya/MDrawRegistry.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <algorithm>
using namespace MHWRender;
class uiDrawManager : public MPxLocatorNode
{
public:
  enum EuiType {
    kText,
    kLine,
    kPoint,
    kRect,
    kQuad,
    kSphere,
    kCircle,
    kArc,
    kLineList,
    kPointList,
    kLineStrip,
    kIcon,
    kCone,
    kBox
  };
public:
  uiDrawManager();
  ~uiDrawManager() override;
  static void*            creator();
  static MStatus            initialize();
  // ui type attribute
  static MObject            aUIType;
  static MObject            aSelectability;
  // color
  static MObject            aPrimitiveColor;
  static MObject            aPrimitiveTransparency;
  // line width and line style attributes
  static MObject            aLineWidth;
  static MObject            aLineStyle;
  // fill attribute
  static MObject            aIsFilled;
  // shaded attribute
  static MObject            aShaded;
  // x-ray attribute
  static MObject            aXRay;
  // radius attribute
  static MObject            aRadius;
  // 2D attributes
  static MObject            aDraw2D;
  static MObject            aPosition;
  // WorldViewportConverter attribute
  static MObject            aWorldViewportConverter;
  // Text attributes
  static MObject            aTextAlignment;
  static MObject            eTextIncline;
  static MObject            aTextWeight;
  static MObject            aTextStretch;
  static MObject            aTextLine;
  static MObject            aTextBoxSize;
  static MObject            aText;
  static MObject            aTextBoxColor;
  static MObject            aTextBoxTransparency;
  static MObject            aTextFontSize;
  static MObject            aFontFaceName;
  // point attributes
  static MObject            aPointSize;
  // line attributes
  static MObject            aLineStartPoint;
  static MObject            aLineEndPoint;
  // rect attributes
  static MObject            aRectUp;
  static MObject            aRectNormal;
  static MObject            aRectScale;
  // quad attributes
  static MObject            aQuadVertex[4];
  // circle attributes
  static MObject            aCircleNormal;
  // arc attributes
  static MObject            aArcStart;
  static MObject            aArcEnd;
  static MObject            aArcNormal;
  // cone attributes
  static MObject          aConeDirection;
  static MObject          aConeHeight;
  // box attributes
  static MObject          aBoxUp;
  static MObject          aBoxRight;
  static MObject          aBoxScale;
  // icon attributes
  static MObject           aIconName;
  static MObject           aIconScale;
public:
  static    MTypeId        id;
  static    MString        drawDbClassification;
  static    MString        drawRegistrantId;
};
MObject uiDrawManager::aUIType;
MObject uiDrawManager::aSelectability;
MObject uiDrawManager::aPrimitiveColor;
MObject uiDrawManager::aPrimitiveTransparency;
MObject uiDrawManager::aLineWidth;
MObject uiDrawManager::aLineStyle;
MObject uiDrawManager::aIsFilled;
MObject uiDrawManager::aShaded;
MObject uiDrawManager::aXRay;
MObject uiDrawManager::aRadius;
MObject uiDrawManager::aDraw2D;
MObject uiDrawManager::aPosition;
MObject uiDrawManager::aWorldViewportConverter;
MObject uiDrawManager::aTextAlignment;
MObject uiDrawManager::eTextIncline;
MObject uiDrawManager::aTextWeight;
MObject uiDrawManager::aTextStretch;
MObject uiDrawManager::aTextLine;
MObject uiDrawManager::aTextBoxSize;
MObject uiDrawManager::aText;
MObject uiDrawManager::aTextBoxColor;
MObject uiDrawManager::aTextBoxTransparency;
MObject uiDrawManager::aTextFontSize;
MObject uiDrawManager::aFontFaceName;
MObject uiDrawManager::aPointSize;
MObject uiDrawManager::aLineStartPoint;
MObject uiDrawManager::aLineEndPoint;
MObject    uiDrawManager::aRectUp;
MObject    uiDrawManager::aRectNormal;
MObject    uiDrawManager::aRectScale;
MObject uiDrawManager::aQuadVertex[4];
MObject    uiDrawManager::aCircleNormal;
MObject    uiDrawManager::aArcStart;
MObject    uiDrawManager::aArcEnd;
MObject    uiDrawManager::aArcNormal;
MObject    uiDrawManager::aIconName;
MObject    uiDrawManager::aIconScale;
MObject    uiDrawManager::aConeDirection;
MObject    uiDrawManager::aConeHeight;
MObject    uiDrawManager::aBoxUp;
MObject    uiDrawManager::aBoxRight;
MObject    uiDrawManager::aBoxScale;
MTypeId uiDrawManager::id(0x0008002A);
MString    uiDrawManager::drawDbClassification("drawdb/geometry/uiDrawManager");
MString    uiDrawManager::drawRegistrantId("uiDrawManagerPlugin");
uiDrawManager::uiDrawManager() {}
uiDrawManager::~uiDrawManager() {}
void* uiDrawManager::creator()
{
  return new uiDrawManager();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Viewport 2.0 override implementation
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class uiDrawManagerData : public MUserData
{
public:
  uiDrawManagerData();
  ~uiDrawManagerData() override {};
  uiDrawManager::EuiType fUIType;
  MUIDrawManager::Selectability fSelectability;
  MColor fColor;
  float fLineWidth;
  MUIDrawManager::LineStyle fLineStyle;
  bool fIsFilled;
  bool fShaded;
  bool fXRay;
  double fRadius;
  bool fDraw2D;
  bool fWorldViewportConverter;
  MPoint fPosition;
  // text
  MString fText;
  unsigned int fTextFontSize;
  unsigned int fFontFaceIndex;
  static MStringArray fFontList;
  MUIDrawManager::TextAlignment fTextAlignment;
  int fTextIncline;
  int fTextWeight;
  int fTextStretch;
  int fTextLine;
  int fTextBoxWidth;
  int fTextBoxHeight;
  MColor fTextBoxColor;
  // point
  float fPointSize;
  MPointArray fPoints;
  // line
  MPoint fLineStartPoint;
  MPoint fLineEndPoint;
  MPointArray fLines;
  // rect
  MVector fRectUp;
  MVector fRectNormal;
  double fRectScaleX;
  double fRectScaleY;
  // quad
  MFloatPoint fQuadVertex[4];
  // circle
  MVector fCircleNormal;
  // arc
  MVector fArcStart;
  MVector fArcEnd;
  MVector fArcNormal;
  // icon
  float fIconScale;
  unsigned int fIconIndex;
  static MStringArray fIconList;
  // cone
  MVector fConeDirection;
  double  fConeHeight;
  // box
  MVector fBoxUp;
  MVector fBoxRight;
  MPoint  fBoxScale;
};
MStringArray uiDrawManagerData::fFontList;
MStringArray uiDrawManagerData::fIconList;
uiDrawManagerData::uiDrawManagerData() : MUserData(false)
    , fUIType(uiDrawManager::kText)
    , fColor(1.0f, 0.0f, 0.0f, 1.0f)
    , fLineWidth(2.0f)
    , fLineStyle(MUIDrawManager::kSolid)
    , fIsFilled(false)
    , fShaded(false)
    , fXRay(false)
    , fRadius(1.0)
    , fDraw2D(false)
    , fPosition(0.0, 0.0, 0.001)
    , fWorldViewportConverter(false)
    // text
    , fText("uiDrawManager-Text")
    , fTextFontSize(MUIDrawManager::kDefaultFontSize)
    , fFontFaceIndex(0)
    , fTextAlignment(MUIDrawManager::kLeft)
    , fTextIncline(MUIDrawManager::kInclineNormal)
    , fTextWeight(MUIDrawManager::kWeightBold)
    , fTextStretch(MUIDrawManager::kStretchUnstretched)
    , fTextLine(0)
    , fTextBoxWidth(0)
    , fTextBoxHeight(0)
    , fTextBoxColor(0.0f, 1.0f, 1.0f, 1.0f)
    // point
    , fPointSize(2.0f)
    // line
    , fLineStartPoint(0.0, 0.0, 0.0)
    , fLineEndPoint(1.0, 1.0, 1.0)
    // rect
    , fRectUp(0.0, 1.0, 0.0)
    , fRectNormal(0.0, 0.0, 1.0)
    , fRectScaleX(1.0)
    , fRectScaleY(1.0)
    // circle
    , fCircleNormal(0.0, 0.0, 1.0)
    // arc
    , fArcStart(1.0, 0.0, 0.0)
    , fArcEnd(0.0, 1.0, 0.0)
    , fArcNormal(0.0, 0.0, 1.0)
    // icon
    , fIconScale(1.0f)
    // Cone
    , fConeDirection(0.0, 0.0, 1.0)
    , fConeHeight(1.0)
    // Box
    , fBoxUp(0.0, 0.0, 1.0)
    , fBoxRight(0.0, 1.0, 0.0)
    , fBoxScale(1.0, 1.0, 1.0)
{
  // quad
  fQuadVertex[0] = MFloatPoint(0.0, 0.0, 0.0);
  fQuadVertex[1] = MFloatPoint(1.0, 0.0, 0.0);
  fQuadVertex[2] = MFloatPoint(1.0, 1.0, 0.0);
  fQuadVertex[3] = MFloatPoint(0.0, 1.0, 0.0);
}
class uiDrawManagerDrawOverride : public MPxDrawOverride
{
public:
  static MPxDrawOverride* Creator(const MObject& obj)
  {
    return new uiDrawManagerDrawOverride(obj);
  }
  ~uiDrawManagerDrawOverride() override;
  DrawAPI supportedDrawAPIs() const override;
  bool isBounded(
      const MDagPath& objPath,
      const MDagPath& cameraPath) const override;
  MBoundingBox boundingBox(
      const MDagPath& objPath,
      const MDagPath& cameraPath) const override;
  MUserData* prepareForDraw(
      const MDagPath& objPath,
      const MDagPath& cameraPath,
      const MFrameContext& frameContext,
      MUserData* oldData) override;
  bool hasUIDrawables() const override { return true; }
  void addUIDrawables(
      const MDagPath& objPath,
      MHWRender::MUIDrawManager& drawManager,
      const MHWRender::MFrameContext& frameContext,
      const MUserData* data) override;
protected:
private:
  uiDrawManagerDrawOverride(const MObject& obj);
};
uiDrawManagerDrawOverride::uiDrawManagerDrawOverride(const MObject& obj)
    : MPxDrawOverride(obj, NULL)
{
}
uiDrawManagerDrawOverride::~uiDrawManagerDrawOverride()
{
}
DrawAPI uiDrawManagerDrawOverride::supportedDrawAPIs() const
{
  // this plugin supports both GL and DX
  return (kOpenGL | kDirectX11 | kOpenGLCoreProfile);
}
bool uiDrawManagerDrawOverride::isBounded(const MDagPath& /*objPath*/,
                                          const MDagPath& /*cameraPath*/) const
{
  return false;
}
MBoundingBox uiDrawManagerDrawOverride::boundingBox(
    const MDagPath& objPath,
    const MDagPath& cameraPath) const
{
  return MBoundingBox();
}
MUserData* uiDrawManagerDrawOverride::prepareForDraw(
    const MDagPath& objPath,
    const MDagPath& cameraPath,
    const MHWRender::MFrameContext& frameContext,
    MUserData* oldData)
{
  uiDrawManagerData* data = dynamic_cast<uiDrawManagerData*>(oldData);
  if (!data) {
    data = new uiDrawManagerData();
  }
  MStatus status;
  MObject uiDrawManagerNode = objPath.node(&status);
  if (status) {
    // retrieve uiType
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aUIType);
      data->fUIType = (uiDrawManager::EuiType)plug.asInt();
    }
    // common attributes
    // retrieve selectability
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aSelectability);
      data->fSelectability = (MUIDrawManager::Selectability)plug.asInt();
    }
    // retrieve color
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aPrimitiveColor);
      MObject o = plug.asMObject();
      MFnNumericData nData(o);
      nData.getData(data->fColor.r, data->fColor.g, data->fColor.b);
    }
    // retrieve transparency
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aPrimitiveTransparency);
      data->fColor.a = 1.0f - plug.asFloat();
    }
    // retrieve line width
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aLineWidth);
      data->fLineWidth = plug.asFloat();
    }
    // retrieve line style
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aLineStyle);
      data->fLineStyle = (MUIDrawManager::LineStyle)plug.asShort();
    }
    // retrieve filled flag
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aIsFilled);
      data->fIsFilled = plug.asBool();
    }
    // retrieve shaded flag
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aShaded);
      data->fShaded = plug.asBool();
    }
    // retrieve x-ray flag
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aXRay);
      data->fXRay = plug.asBool();
    }
    // retrieve radius
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aRadius);
      data->fRadius = plug.asDouble();
    }
    // retrieve 2D flag
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aDraw2D);
      data->fDraw2D = plug.asBool();
    }
    // retrieve WorldViewportConverter flag
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aWorldViewportConverter);
      data->fWorldViewportConverter = plug.asBool();
    }
    // retrieve screen position
    {
      MPlug plug(uiDrawManagerNode, uiDrawManager::aPosition);
      MObject o = plug.asMObject();
      MFnNumericData nData(o);
      nData.getData(data->fPosition.x, data->fPosition.y, data->fPosition.z);
      data->fPosition.w = 1.0f;
    }
    switch (data->fUIType)
    {
      case uiDrawManager::kText:
        // retrieve text
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aText);
        data->fText = plug.asString();
      }
        // retrieve text font size
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextFontSize);
          data->fTextFontSize = std::max(plug.asInt(), 0);
        }
        // retrieve font face index
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aFontFaceName);
          data->fFontFaceIndex = (unsigned int)plug.asInt();
        }
        // retrieve text alignment
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextAlignment);
          data->fTextAlignment = (MUIDrawManager::TextAlignment)plug.asShort();
        }
        // retrieve text incline
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::eTextIncline);
          data->fTextIncline = plug.asInt();
        }
        // retrieve text weight
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextWeight);
          data->fTextWeight = plug.asInt();
        }
        // retrieve text stretch
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextStretch);
          data->fTextStretch = plug.asInt();
        }
        // retrieve text line
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextLine);
          data->fTextLine = plug.asInt();
        }
        // retrieve text box size
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextBoxSize);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fTextBoxWidth, data->fTextBoxHeight);
        }
        // retrieve text box color
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextBoxColor);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fTextBoxColor.r, data->fTextBoxColor.g, data->fTextBoxColor.b);
        }
        // retrieve text box transparency
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aTextBoxTransparency);
          data->fTextBoxColor.a = 1.0f - plug.asFloat();
        }
        break;
      case uiDrawManager::kPoint:
      case uiDrawManager::kPointList:
        // retrieve point size
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aPointSize);
        data->fPointSize = plug.asFloat();
      }
        if (data->fUIType == uiDrawManager::kPoint)
        {
          data->fPoints.clear();
          data->fPoints.append( data->fPosition );
        }
        else
        {
          unsigned int divisionCount = 50;
          double ddivisionCount = 100.0 / (double)divisionCount;
          data->fPoints.clear();
          MPoint newPoint;
          for (unsigned int i=0; i<=divisionCount; i++)
          {
            for (unsigned int j=0; j<=divisionCount; j++)
            {
              for (unsigned int k=0; k<=divisionCount; k++)
              {
                MPoint delta(double(i)*ddivisionCount, double(j)*ddivisionCount, double(k)*ddivisionCount);
                newPoint = data->fPosition + delta;
                data->fPoints.append( newPoint );
              }
            }
          }
        }
        break;
      case uiDrawManager::kLine:
      case uiDrawManager::kLineList:
      case uiDrawManager::kLineStrip:
        // retrieve line start point
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aLineStartPoint);
        MObject o = plug.asMObject();
        MFnNumericData nData(o);
        nData.getData(data->fLineStartPoint.x, data->fLineStartPoint.y, data->fLineStartPoint.z);
        data->fLineStartPoint.w = 1.0f;
      }
        // retrieve line end point
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aLineEndPoint);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fLineEndPoint.x, data->fLineEndPoint.y, data->fLineEndPoint.z);
          data->fLineEndPoint.w = 1.0f;
        }
        if (data->fUIType == uiDrawManager::kLine)
        {
          data->fLines.clear();
          data->fLines.append( data->fLineStartPoint );
          data->fLines.append( data->fLineEndPoint );
        }
        else
        {
          unsigned int divisionCount = 50;
          double ddivisionCount = 100.0 / (double)divisionCount;
          data->fLines.clear();
          MPoint newPoint;
          for (unsigned int i=0; i<=divisionCount; i++)
          {
            for (unsigned int j=0; j<=divisionCount; j++)
            {
              for (unsigned int k=0; k<=divisionCount; k++)
              {
                MPoint delta(double(i)*ddivisionCount, double(j)*ddivisionCount, double(k)*ddivisionCount);
                newPoint = data->fLineStartPoint + delta;
                data->fLines.append( newPoint );
                newPoint = data->fLineEndPoint + delta;
                data->fLines.append( newPoint );
              }
            }
          }
        }
        break;
      case uiDrawManager::kRect:
        // retrieve rect up vector
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aRectUp);
        MObject o = plug.asMObject();
        MFnNumericData nData(o);
        nData.getData(data->fRectUp.x, data->fRectUp.y, data->fRectUp.z);
      }
        // retrieve rect normal vector
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aRectNormal);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fRectNormal.x, data->fRectNormal.y, data->fRectNormal.z);
        }
        // retrieve rect scale
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aRectScale);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fRectScaleX, data->fRectScaleY);
        }
        break;
      case uiDrawManager::kQuad:
        // retrieve quad vertices
      {
        for (int i = 0; i < 4; ++i)
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aQuadVertex[i]);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fQuadVertex[i].x, data->fQuadVertex[i].y, data->fQuadVertex[i].z);
          //printf("retrieve quad data %f, %f, %f \n", data->fQuadVertex[i].x, data->fQuadVertex[i].y, data->fQuadVertex[i].z);
        }
      }
        break;
      case uiDrawManager::kCircle:
        // retrieve circle normal
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aCircleNormal);
        MObject o = plug.asMObject();
        MFnNumericData nData(o);
        nData.getData(data->fCircleNormal.x, data->fCircleNormal.y, data->fCircleNormal.z);
      }
        break;
      case uiDrawManager::kArc:
        // retrieve arc start vector
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aArcStart);
        MObject o = plug.asMObject();
        MFnNumericData nData(o);
        nData.getData(data->fArcStart.x, data->fArcStart.y, data->fArcStart.z);
      }
        // retrieve arc end vector
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aArcEnd);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fArcEnd.x, data->fArcEnd.y, data->fArcEnd.z);
        }
        // retrieve arc normal
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aArcNormal);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fArcNormal.x, data->fArcNormal.y, data->fArcNormal.z);
        }
        break;
      case uiDrawManager::kSphere:
        break;
      case uiDrawManager::kIcon:
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aIconName);
        data->fIconIndex = (unsigned int)plug.asInt();
      }
        // Get the name and scale
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aIconScale);
          data->fIconScale = std::max(plug.asFloat(), 1.0f);
        }
        break;
      case uiDrawManager::kCone:
        // retrieve cone direction vector
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aConeDirection);
        MObject o = plug.asMObject();
        MFnNumericData nData(o);
        nData.getData(data->fConeDirection.x, data->fConeDirection.y, data->fConeDirection.z);
      }
        // retrieve cone height
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aConeHeight);
          data->fConeHeight = std::max(plug.asFloat(), 0.0f);
        }
        break;
      case uiDrawManager::kBox:
        // retrieve box up vector
      {
        MPlug plug(uiDrawManagerNode, uiDrawManager::aBoxUp);
        MObject o = plug.asMObject();
        MFnNumericData nData(o);
        nData.getData(data->fBoxUp.x, data->fBoxUp.y, data->fBoxUp.z);
      }
        // retrieve box right vector
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aBoxRight);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fBoxRight.x, data->fBoxRight.y, data->fBoxRight.z);
        }
        // retrieve box scale
        {
          MPlug plug(uiDrawManagerNode, uiDrawManager::aBoxScale);
          MObject o = plug.asMObject();
          MFnNumericData nData(o);
          nData.getData(data->fBoxScale.x, data->fBoxScale.y, data->fBoxScale.z);
        }
        break;
      default:
        perror("unhandled ui type.");
        break;
    }
  }
  return data;
}
void uiDrawManagerDrawOverride::addUIDrawables(
    const MDagPath& objPath,
    MHWRender::MUIDrawManager& drawManager,
    const MHWRender::MFrameContext& frameContext,
    const MUserData* data)
{
  const uiDrawManagerData* thisdata = dynamic_cast<const uiDrawManagerData*>(data);
  if (!thisdata) {
    return;
  }
  switch (thisdata->fUIType)
  {
    case uiDrawManager::kText:
    {
      // Draw a text "uiDrawManager"
      // All drawing operations must take place between calls to beginDrawable()
      // and endDrawable().
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setFontSize(thisdata->fTextFontSize);
      drawManager.setFontIncline(thisdata->fTextIncline);
      drawManager.setFontWeight(thisdata->fTextWeight);
      drawManager.setFontStretch(thisdata->fTextStretch);
      drawManager.setFontLine(thisdata->fTextLine);
      MString faceName = uiDrawManagerData::fFontList[thisdata->fFontFaceIndex];
      drawManager.setFontName(faceName);
      int boxSize[] = { thisdata->fTextBoxWidth, thisdata->fTextBoxHeight };
      // if fWorldViewportConverter flag is set
      // draw 2D text using text() position is converted from world space.
      // draw 3D text using text2d(), position is converted from view port.
      if (thisdata->fWorldViewportConverter)
      {
        if (thisdata->fDraw2D)
        {
          double x = thisdata->fPosition.x, y = thisdata->fPosition.y;
          double viewX, viewY;
          MPoint worldPos, worldFarPt;
          char buff[128];
          // viewportToWorld() convert a point in viewport to points in near and far cliping plane.
          frameContext.viewportToWorld(x, y, worldPos, worldFarPt);
          // use near point to draw the text, to ensure the point in frustum
          // move it a little towards far point direction.
          worldPos = (worldFarPt - worldPos) * 0.01 + worldPos;
          sprintf(buff, "(%.2lf %.2lf)->(%.2lf, %.2lf, %.2lf)", x, y,
                  worldPos.x, worldPos.y, worldPos.z);
          drawManager.text(worldPos, buff, thisdata->fTextAlignment,
                           boxSize[0]+boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
          frameContext.worldToViewport(worldPos, viewX, viewY);
          printf("2D: (%.2lf %.2lf)->(%.2lf, %.2lf, %.2lf)->(%.2lf, %.2lf)\n", x, y,
                 worldPos.x, worldPos.y, worldPos.z, viewX, viewY);
        }
        else
        {
          double x, y;
          char buff[128];
          MPoint worldPt, worldFarPt;
          frameContext.worldToViewport(thisdata->fPosition, x, y);
          sprintf(buff, "(%.2lf, %.2lf, %.2lf)->(%.2lf %.2lf)", thisdata->fPosition.x,
                  thisdata->fPosition.y, thisdata->fPosition.z, x, y);
          drawManager.text2d(MPoint(x, y), buff, thisdata->fTextAlignment,
                             boxSize[0]+boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
          frameContext.viewportToWorld(x, y, worldPt, worldFarPt);
          printf("3D: (%.2lf, %.2lf, %.2lf)->(%.2lf %.2lf)->(%.2lf, %.2lf, %.2lf)\n", thisdata->fPosition.x,
                 thisdata->fPosition.y, thisdata->fPosition.z, x, y, worldPt.x, worldPt.y, worldPt.z);
        }
      }
      else
      {
        if (thisdata->fDraw2D)
        {
          // uiDrawManagerData::fPosition gives a screen space position
          // where 2D UI item is located.
          drawManager.text2d(thisdata->fPosition, thisdata->fText, thisdata->fTextAlignment,
                             boxSize[0]+boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
        }
        else
        {
          // for 3D items, place it at the origin of the world space.
          drawManager.text(thisdata->fPosition, thisdata->fText, thisdata->fTextAlignment,
                           boxSize[0]+boxSize[1] == 0 ? NULL : boxSize, &thisdata->fTextBoxColor, false);
        }
      }
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kLine:
    case uiDrawManager::kLineList:
    case uiDrawManager::kLineStrip:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      if (thisdata->fUIType == uiDrawManager::kLineStrip)
        drawManager.lineStrip(thisdata->fLines, thisdata->fDraw2D);
      else
        drawManager.lineList(thisdata->fLines, thisdata->fDraw2D);
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kPoint:
    case uiDrawManager::kPointList:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setPointSize(thisdata->fPointSize);
      drawManager.points(thisdata->fPoints, thisdata->fDraw2D);
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kRect:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      drawManager.setPaintStyle(thisdata->fShaded ?
                                MUIDrawManager::kShaded : MUIDrawManager::kFlat);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      if (thisdata->fDraw2D) {
        // For 2d rectangle, an up vector in screen space is used to determine its X
        // and Y directions. In addition, "fRectScaleX" and "fRectScaleY"(in pixels)
        // specify the half-lengths of the 2d rectangle.
        drawManager.rect2d(thisdata->fPosition, thisdata->fRectUp,
                           thisdata->fRectScaleX, thisdata->fRectScaleY, thisdata->fIsFilled);
      }
      else {
        // For 3d rectangle, the up vector should not be parallel with the normal vector.
        drawManager.rect(thisdata->fPosition, thisdata->fRectUp, thisdata->fRectNormal,
                         thisdata->fRectScaleX, thisdata->fRectScaleY, thisdata->fIsFilled);
      }
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kQuad:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      // prepare primitive type
      MUIDrawManager::Primitive mode =
          thisdata->fIsFilled ? MUIDrawManager::kTriStrip : MUIDrawManager::kClosedLine;
      // prepare index
      MUintArray index;
      index.append(0);
      index.append(1);
      index.append(3);
      index.append(2);
      // draw mesh
      drawManager.setPaintStyle(thisdata->fShaded ?
                                MUIDrawManager::kShaded : MUIDrawManager::kFlat);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      if (thisdata->fDraw2D) {
        // prepare position list
        MPointArray position;
        for (int i = 0; i < 4; ++i) {
          position.append(thisdata->fQuadVertex[i]);
        }
        drawManager.mesh2d(mode, position, NULL, thisdata->fIsFilled ? &index : NULL);
      }
      else {
        // prepare position list
        MFloatPointArray position;
        for (int i = 0; i < 4; ++i) {
          position.append(thisdata->fQuadVertex[i]);
          //printf("%f %f %f \n", position[i].x, position[i].y, position[i].z);
        }
        drawManager.mesh(mode, position, NULL, NULL, thisdata->fIsFilled ? &index : NULL);
        //printf("plugin calling drawManager's float version mesh(). \n");
      }
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kSphere:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      drawManager.setPaintStyle(thisdata->fShaded ?
                                MUIDrawManager::kShaded : MUIDrawManager::kFlat);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      drawManager.sphere(thisdata->fPosition, thisdata->fRadius, thisdata->fIsFilled);
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kCircle:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      drawManager.setPaintStyle(thisdata->fShaded ?
                                MUIDrawManager::kShaded : MUIDrawManager::kFlat);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      if (thisdata->fDraw2D) {
        // The radius in specified as pixel unit for 2d items.
        drawManager.circle2d(thisdata->fPosition, thisdata->fRadius,
                             thisdata->fIsFilled);
      }
      else {
        drawManager.circle(thisdata->fPosition, thisdata->fCircleNormal, thisdata->fRadius,
                           thisdata->fIsFilled);
      }
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kArc:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      drawManager.setPaintStyle(thisdata->fShaded ?
                                MUIDrawManager::kShaded : MUIDrawManager::kFlat);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      if (thisdata->fDraw2D) {
        // If 2d, the range of the arc is defined by the start and end vectors
        // specified in screen space.
        drawManager.arc2d(thisdata->fPosition, thisdata->fArcStart, thisdata->fArcEnd,
                          thisdata->fRadius, thisdata->fIsFilled);
      }
      else {
        // For 3d arc, the projections of the start and end vectors onto the arc plane(
        // determined by the normal vector) determine the range of the arc.
        drawManager.arc(thisdata->fPosition, thisdata->fArcStart, thisdata->fArcEnd,
                        thisdata->fArcNormal, thisdata->fRadius, thisdata->fIsFilled);
      }
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kIcon:
    {
      MString iconName = uiDrawManagerData::fIconList[thisdata->fIconIndex];
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      drawManager.icon(thisdata->fPosition, iconName, thisdata->fIconScale);
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kCone:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      drawManager.setPaintStyle(thisdata->fShaded ?
                                MUIDrawManager::kShaded : MUIDrawManager::kFlat);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      drawManager.cone(thisdata->fPosition,
                       thisdata->fConeDirection,
                       thisdata->fRadius,
                       thisdata->fConeHeight,
                       thisdata->fIsFilled);
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    case uiDrawManager::kBox:
    {
      drawManager.beginDrawable(thisdata->fSelectability);
      drawManager.setColor(thisdata->fColor);
      drawManager.setLineWidth(thisdata->fLineWidth);
      drawManager.setLineStyle(thisdata->fLineStyle);
      drawManager.setPaintStyle(thisdata->fShaded ?
                                MUIDrawManager::kShaded : MUIDrawManager::kFlat);
      if (thisdata->fXRay)
        drawManager.beginDrawInXray();
      drawManager.box(thisdata->fPosition,
                      thisdata->fBoxUp,
                      thisdata->fBoxRight,
                      thisdata->fBoxScale[0],
                      thisdata->fBoxScale[1],
                      thisdata->fBoxScale[2],
                      thisdata->fIsFilled);
      if (thisdata->fXRay)
        drawManager.endDrawInXray();
      drawManager.endDrawable();
    }
      break;
    default:
      perror("unhandled ui types.");
      break;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Plugin Registration
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
MStatus uiDrawManager::initialize()
{
  MStatus status;
  MFnNumericAttribute nAttr;
  MFnEnumAttribute eAttr;
  MFnTypedAttribute typedAttr;
  // Add ui type attribute
  aUIType = eAttr.create("uiType", "ut", uiDrawManager::kText);
  eAttr.addField("text", uiDrawManager::kText);
  eAttr.addField("line", uiDrawManager::kLine);
  eAttr.addField("point", uiDrawManager::kPoint);
  eAttr.addField("rect", uiDrawManager::kRect);
  eAttr.addField("quad", uiDrawManager::kQuad);
  eAttr.addField("sphere", uiDrawManager::kSphere);
  eAttr.addField("circle", uiDrawManager::kCircle);
  eAttr.addField("arc", uiDrawManager::kArc);
  eAttr.addField("line list", uiDrawManager::kLineList);
  eAttr.addField("line strip", uiDrawManager::kLineStrip);
  eAttr.addField("point list", uiDrawManager::kPointList);
  eAttr.addField("icon", uiDrawManager::kIcon);
  eAttr.addField("cone", uiDrawManager::kCone);
  eAttr.addField("box", uiDrawManager::kBox);
  MPxNode::addAttribute(aUIType);
  // Add selectability attribute
  aSelectability = eAttr.create("selectability", "st", MUIDrawManager::kAutomatic);
  eAttr.addField("NonSelectable", MUIDrawManager::kNonSelectable);
  eAttr.addField("Selectable", MUIDrawManager::kSelectable);
  eAttr.addField("Automatic", MUIDrawManager::kAutomatic);
  MPxNode::addAttribute(aSelectability);
  // Add color attribute
  aPrimitiveColor = nAttr.create("primitiveColor", "pc", MFnNumericData::k3Float);
  nAttr.setDefault(1.0f, 0.0f, 0.0f);
  nAttr.setUsedAsColor(true);
  MPxNode::addAttribute(aPrimitiveColor);
  // Add transparency attribute
  aPrimitiveTransparency = nAttr.create("primitiveTransparency", "pt", MFnNumericData::kFloat, 0.0);
  nAttr.setSoftMin(0.0);
  nAttr.setSoftMax(1.0);
  MPxNode::addAttribute(aPrimitiveTransparency);
  // add line width and line style attributes
  aLineWidth = nAttr.create("lineWidth", "lw", MFnNumericData::kFloat, 2.0);
  MPxNode::addAttribute(aLineWidth);
  aLineStyle = eAttr.create("lineStyle", "ls", MUIDrawManager::kSolid);
  eAttr.addField("solid", MUIDrawManager::kSolid);
  eAttr.addField("shortdotted", MUIDrawManager::kShortDotted);
  eAttr.addField("shortdashed", MUIDrawManager::kShortDashed);
  eAttr.addField("dashed", MUIDrawManager::kDashed);
  eAttr.addField("dotted", MUIDrawManager::kDotted);
  MPxNode::addAttribute(aLineStyle);
  // Add filled attribute
  aIsFilled = nAttr.create("isFilled", "if", MFnNumericData::kBoolean, 0);
  MPxNode::addAttribute(aIsFilled);
  // Add shaded attribute
  aShaded = nAttr.create("shaded", "sd", MFnNumericData::kBoolean, 0);
  MPxNode::addAttribute(aShaded);
  // Add xray attribute
  aXRay = nAttr.create("xray", "xr", MFnNumericData::kBoolean, 0);
  MPxNode::addAttribute(aXRay);
  // Add radius attribute
  aRadius = nAttr.create("radius", "ra", MFnNumericData::kDouble, 1.0);
  MPxNode::addAttribute(aRadius);
  // add 2D attributes
  aDraw2D = nAttr.create("draw2D", "d2", MFnNumericData::kBoolean, 0);
  MPxNode::addAttribute(aDraw2D);
  // add  WorldViewportConverter attribute
  aWorldViewportConverter = nAttr.create("useWorldViewportConverter", "wvc", MFnNumericData::kBoolean, 0);
  MPxNode::addAttribute(aWorldViewportConverter);
  aPosition = nAttr.create("position", "pos", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 0.0, 0.001);
  MPxNode::addAttribute(aPosition);
  // Add text attributes.
  MFnStringData stringFn;
  MObject defaultText = stringFn.create("uiDrawManager-Text");
  aText = typedAttr.create("text", "t", MFnData::kString, defaultText);
  MPxNode::addAttribute(aText);
  aTextFontSize = nAttr.create("textFontSize", "tfs", MFnNumericData::kInt, MUIDrawManager::kDefaultFontSize);
  nAttr.setMin(-1);
  nAttr.setMax(99);
  MPxNode::addAttribute(aTextFontSize);
  unsigned int nFont = MUIDrawManager::getFontList(uiDrawManagerData::fFontList);
  if (nFont == 0)
  {
    perror("No font available!");
  }
  aFontFaceName = eAttr.create("fontFaceName", "ffn", 0);
  for (unsigned int i = 0; i < nFont; i++)
  {
    MString str = uiDrawManagerData::fFontList[i];
    eAttr.addField(str, (short)i);
  }
  MPxNode::addAttribute(aFontFaceName);
  aTextAlignment = eAttr.create("textAlignment", "ta", MUIDrawManager::kLeft);
  eAttr.addField("left", MUIDrawManager::kLeft);
  eAttr.addField("center", MUIDrawManager::kCenter);
  eAttr.addField("right", MUIDrawManager::kRight);
  MPxNode::addAttribute(aTextAlignment);
  eTextIncline = eAttr.create("textIncline", "tic", MUIDrawManager::kInclineNormal);
  eAttr.addField("normal", MUIDrawManager::kInclineNormal);
  eAttr.addField("italic", MUIDrawManager::kInclineItalic);
  MPxNode::addAttribute(eTextIncline);
  aTextWeight = eAttr.create("textWeight", "tw", MUIDrawManager::kWeightBold);
  eAttr.addField("light", MUIDrawManager::kWeightLight);
  eAttr.addField("normal", MUIDrawManager::kWeightNormal);
  eAttr.addField("demiBold", MUIDrawManager::kWeightDemiBold);
  eAttr.addField("bold", MUIDrawManager::kWeightBold);
  eAttr.addField("black", MUIDrawManager::kWeightBlack);
  MPxNode::addAttribute(aTextWeight);
  aTextStretch = nAttr.create("textStretch", "ts", MFnNumericData::kInt, MUIDrawManager::kStretchUnstretched);
  nAttr.setMin(50);
  nAttr.setMax(200);
  MPxNode::addAttribute(aTextStretch);
  aTextLine = eAttr.create("textLine", "tl", 0);
  eAttr.addField("none", 0);
  eAttr.addField("overline", MUIDrawManager::kLineOverline);
  eAttr.addField("underline", MUIDrawManager::kLineUnderline);
  eAttr.addField("strikeout", MUIDrawManager::kLineStrikeoutLine);
  MPxNode::addAttribute(aTextLine);
  aTextBoxSize = nAttr.create("textBoxSize", "tbs", MFnNumericData::k2Int);
  nAttr.setDefault(0, 0);
  MPxNode::addAttribute(aTextBoxSize);
  aTextBoxColor = nAttr.create("textBoxColor", "tbc", MFnNumericData::k3Float);
  nAttr.setDefault(0.0f, 1.0f, 1.0f);
  nAttr.setUsedAsColor(true);
  MPxNode::addAttribute(aTextBoxColor);
  aTextBoxTransparency = nAttr.create("textBoxTransparency", "tbt", MFnNumericData::kFloat, 0.0);
  nAttr.setSoftMin(0.0);
  nAttr.setSoftMax(1.0);
  MPxNode::addAttribute(aTextBoxTransparency);
  // add point attributes
  aPointSize = nAttr.create("pointSize", "ps", MFnNumericData::kFloat, 2.0);
  MPxNode::addAttribute(aPointSize);
  // add line attributes
  aLineStartPoint = nAttr.create("lineStartPoint", "lsp", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 0.0, 0.0);
  MPxNode::addAttribute(aLineStartPoint);
  aLineEndPoint = nAttr.create("lineEndPoint", "lep", MFnNumericData::k3Double);
  nAttr.setDefault(1.0, 1.0, 1.0);
  MPxNode::addAttribute(aLineEndPoint);
  // add rect attributes
  aRectUp = nAttr.create("rectUp", "ru", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 1.0, 0.0);
  MPxNode::addAttribute(aRectUp);
  aRectNormal = nAttr.create("rectNormal", "rn", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 0.0, 1.0);
  MPxNode::addAttribute(aRectNormal);
  aRectScale = nAttr.create("rectScale", "rs", MFnNumericData::k2Double);
  nAttr.setDefault(1.0, 1.0);
  MPxNode::addAttribute(aRectScale);
  // add quad attributes
  float defaultPosition[4][3] =
      {
          {0.0, 0.0, 0.0},
          {1.0, 0.0, 0.0},
          {1.0, 1.0, 0.0},
          {0.0, 1.0, 0.0}
      };
  for (int i = 0; i < 4;  ++i)
  {
    MString fullName = "quadVertex";
    MString shortName = "qv";
    aQuadVertex[i] = nAttr.create(fullName + i, shortName + i, MFnNumericData::k3Float);
    nAttr.setDefault(defaultPosition[i][0], defaultPosition[i][1], defaultPosition[i][2]);
    MPxNode::addAttribute(aQuadVertex[i]);
  }
  // add circle attributes
  aCircleNormal = nAttr.create("circleNormal", "cn", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 0.0, 1.0);
  MPxNode::addAttribute(aCircleNormal);
  // add arc attributes
  aArcStart = nAttr.create("arcStartVector", "asv", MFnNumericData::k3Double);
  nAttr.setDefault(1.0, 0.0, 0.0);
  MPxNode::addAttribute(aArcStart);
  aArcEnd = nAttr.create("arcEndVector", "aev", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 1.0, 0.0);
  MPxNode::addAttribute(aArcEnd);
  aArcNormal = nAttr.create("arcNormal", "an", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 0.0, 1.0);
  MPxNode::addAttribute(aArcNormal);
  // icon attributes
  aIconName = eAttr.create("icon", "i", 0 );
  unsigned int iconCount = MUIDrawManager::getIconNames(uiDrawManagerData::fIconList);
  for (unsigned int i=0; i<iconCount; i++)
  {
    MString str = uiDrawManagerData::fIconList[i];
    eAttr.addField(str, (short)i);
  }
  MPxNode::addAttribute(aIconName);
  aIconScale = nAttr.create("iconScale", "cs", MFnNumericData::kFloat, 1.0);
  MPxNode::addAttribute(aIconScale);
  // cone attributes
  aConeDirection = nAttr.create("coneDirection", "cd", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 0.0, 1.0);
  MPxNode::addAttribute(aConeDirection);
  aConeHeight = nAttr.create("coneHeight", "ch", MFnNumericData::kDouble);
  nAttr.setDefault(1.0);
  MPxNode::addAttribute(aConeHeight);
  // box attributes
  aBoxUp = nAttr.create("boxUp", "bu", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 0.0, 1.0);
  MPxNode::addAttribute(aBoxUp);
  aBoxRight = nAttr.create("boxRight", "br", MFnNumericData::k3Double);
  nAttr.setDefault(0.0, 1.0, 0.0);
  MPxNode::addAttribute(aBoxRight);
  aBoxScale = nAttr.create("boxScale", "bs", MFnNumericData::k3Double);
  nAttr.setDefault(1.0, 1.0, 1.0);
  MPxNode::addAttribute(aBoxScale);
  return MS::kSuccess;
}
MStatus initializePlugin( MObject obj )
{
  MStatus   status;
  MFnPlugin plugin( obj, PLUGIN_COMPANY, "3.0", "Any");
  status = plugin.registerNode(
      "uiDrawManager",
      uiDrawManager::id,
      &uiDrawManager::creator,
      &uiDrawManager::initialize,
      MPxNode::kLocatorNode,
      &uiDrawManager::drawDbClassification);
  if (!status) {
    status.perror("registerNode");
    return status;
  }
  status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
      uiDrawManager::drawDbClassification,
      uiDrawManager::drawRegistrantId,
      uiDrawManagerDrawOverride::Creator);
  if (!status) {
    status.perror("registerDrawOverrideCreator");
    return status;
  }
  return status;
}
MStatus uninitializePlugin( MObject obj)
{
  MStatus   status;
  MFnPlugin plugin( obj );
  status = MDrawRegistry::deregisterGeometryOverrideCreator(
      uiDrawManager::drawDbClassification,
      uiDrawManager::drawRegistrantId);
  if (!status) {
    status.perror("deregisterGeometryOverrideCreator");
    return status;
  }
  status = plugin.deregisterNode( uiDrawManager::id );
  if (!status) {
    status.perror("deregisterNode");
    return status;
  }
  return status;
}