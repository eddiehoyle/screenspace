#include "ss/Log.hh"
#include "ss/PickableShape.hh"
#include "ss/PickableDrawOverride.hh"
#include "ss/commands/AddCommand.hh"
#include "ss/commands/RemoveCommand.hh"

#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>

using namespace screenspace;

static const MString AEpickableTemplate = R"(global proc AEpickableTemplate(string $nodeName)
{
  editorTemplate -beginScrollLayout;
  editorTemplate -beginLayout "Geometry" -collapse 0;
  editorTemplate -addControl -label "Shape" "shape" "swapShape";
  editorTemplate -addControl "size";
  editorTemplate -addControl "width";
  editorTemplate -addControl "height";
  editorTemplate -addControl "color";
  editorTemplate -addControl "opacity";
  editorTemplate -addSeparator;
  editorTemplate -addControl "position";
  editorTemplate -addControl "offset";
  editorTemplate -addControl "verticalAlign";
  editorTemplate -addControl "horizontalAlign";
  editorTemplate -addControl "depth";
  editorTemplate -endLayout;
  editorTemplate -addExtraControls;
  editorTemplate -endScrollLayout;
})";

MStatus initializePlugin(MObject obj) {
  MFnPlugin plugin(obj, "Eddie Hoyle", "1.0", "Any");

  MStatus status;
  status = plugin.registerNode(PickableShape::typeName,
                               PickableShape::id,
                               &PickableShape::creator,
                               &PickableShape::initialize,
                               MPxNode::kSurfaceShape,
                               &PickableDrawOverride::classifcation);
  CHECK_MSTATUS(status);

  status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
      PickableDrawOverride::classifcation,
      PickableDrawOverride::id,
      PickableDrawOverride::creator);
  CHECK_MSTATUS(status);

  status = plugin.registerCommand(AddCommand::typeName,
                                  AddCommand::creator,
                                  AddCommand::createSyntax);
  CHECK_MSTATUS(status);

  status = plugin.registerCommand(RemoveCommand::typeName,
                                  RemoveCommand::creator,
                                  RemoveCommand::createSyntax);
  CHECK_MSTATUS(status);

//  MGlobal::executeCommandOnIdle(AEpickableTemplate);
  return status;
}

MStatus uninitializePlugin(MObject obj) {
  MFnPlugin plugin(obj);
  MStatus status;

  status = plugin.deregisterNode(PickableShape::id);
  CHECK_MSTATUS(status);

  status = plugin.deregisterCommand(AddCommand::typeName);
  CHECK_MSTATUS(status);

  status = plugin.deregisterCommand(RemoveCommand::typeName);
  CHECK_MSTATUS(status);

  return status;
}