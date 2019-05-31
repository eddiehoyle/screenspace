#include "PickerDrawOverride.hh"
#include "PickerShape.hh"
#include "PickerUI.hh"
#include "Log.hh"

#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

using namespace screenspace;

MStatus initializePlugin(MObject obj) {
  MFnPlugin plugin(obj, "Eddie Hoyle", "1.0", "Any");
  MGlobal::displayInfo("initializePlugin!");

  MStatus status;

  status = plugin.registerNode(PickerShape::typeName,
                               PickerShape::id,
                               &PickerShape::creator,
                               & PickerShape::initialize,
                               MPxNode::kDependNode,
                               &PickerDrawOverride::classifcation);
  CHECK_MSTATUS(status);

  status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
      PickerDrawOverride::classifcation,
      PickerDrawOverride::id,
      PickerDrawOverride::creator);
  CHECK_MSTATUS(status);
  return status;
}

MStatus uninitializePlugin(MObject obj) {
  MFnPlugin plugin(obj);
  MGlobal::displayInfo("uninitializePlugin!");
  MStatus status;
  return status;
}
