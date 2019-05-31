#include "PickerDrawOverride.hh"
#include "PickerShape.hh"
#include "PickerUI.hh"
#include "Log.hh"

#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

using namespace screenspace;

MStatus initializePlugin(MObject obj) {
  MFnPlugin plugin(obj, "Eddie Hoyle", "1.0", "Any");
  MGlobal::displayInfo("initializePlugin!");

  MStatus status;

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

MStatus uninitializePlugin(MObject obj) {
  MFnPlugin plugin(obj);
  MGlobal::displayInfo("uninitializePlugin!");
  MStatus status;
  return status;
}
