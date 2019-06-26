#include "PickerDrawOverride.hh"
#include "PickerShape.hh"
#include "PickerUI.hh"
#include "PickerCommand.hh"
#include "Log.hh"

#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

using namespace screenspace;

static const MString AEpickerTemplate = R"(global proc AEpickerTemplate(string $nodeName)
{
  editorTemplate -beginScrollLayout;
  editorTemplate -beginLayout "Geometry" -collapse 0;
    editorTemplate -addControl "size";
    editorTemplate -addControl "width";
    editorTemplate -addControl "height";
  editorTemplate -endLayout;
  editorTemplate -addExtraControls;
  editorTemplate -endScrollLayout;
})";

MStatus initializePlugin(MObject obj) {
  MFnPlugin plugin(obj, "Eddie Hoyle", "1.0", "Any");

  MStatus status;

  status = plugin.registerNode(PickerShape::typeName,
                               PickerShape::id,
                               &PickerShape::creator,
                               &PickerShape::initialize,
                               MPxNode::kLocatorNode,
                               &PickerDrawOverride::classifcation);

  CHECK_MSTATUS(status);

  status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
      PickerDrawOverride::classifcation,
      PickerDrawOverride::id,
      PickerDrawOverride::creator);
  CHECK_MSTATUS(status);

  status = plugin.registerCommand(CreatePickerCommand::typeName,
                                  CreatePickerCommand::creator,
                                  CreatePickerCommand::createSyntax);
  CHECK_MSTATUS(status);

  TNC_DEBUG << "Start plugin";
//  MGlobal::executeCommandOnIdle(AEpickerTemplate);
  TNC_DEBUG << "Done!";
  return status;
}

MStatus uninitializePlugin(MObject obj) {
  MFnPlugin plugin(obj);
  TNC_DEBUG << "Finish plugin";
  MStatus status;

  status = plugin.deregisterCommand("attachPicker");
  CHECK_MSTATUS(status);

  return status;
}