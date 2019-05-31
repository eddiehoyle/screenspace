#include "PickerDrawOverride.hh"
#include "PartMesh.hh"
#include "PickerShape.hh"
#include "PartGeometryData.hh"

#include <ldraw/Log.hh>

#include <maya/MBoundingBox.h>
#include <maya/MUserData.h>
#include <maya/MPxNode.h>
#include <maya/MColor.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
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
#include <maya/MUIDrawManager.h>
#include <maya/MPlug.h>
#include <vector>

namespace screenspace {

MString PickerDrawOverride::classifcation = "drawdb/picker";
MString PickerDrawOverride::id = "picker";

}