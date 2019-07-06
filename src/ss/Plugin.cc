#include "ss/commands/AddCommand.hh"
#include "ss/commands/RemoveCommand.hh"
#include "ss/Log.hh"
#include "ss/PickableDrawOverride.hh"
#include "ss/PickableShape.hh"

#include <maya/MDrawRegistry.h>
#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>

using namespace screenspace;

MStatus initializePlugin(MObject obj) {
  MFnPlugin plugin(obj, "Eddie Hoyle", "1.0", "Any");

  MStatus status;
  status = plugin.registerNode(PickableShape::typeName,
                               PickableShape::id,
                               &PickableShape::creator,
                               &PickableShape::initialize,
                               MPxNode::kSurfaceShape,
                               &PickableDrawOverride::classification);
  CHECK_MSTATUS(status);

  status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
      PickableDrawOverride::classification,
      PickableDrawOverride::id,
      PickableDrawOverride::creator);
  CHECK_MSTATUS(status);

  status = plugin.registerCommand(AddCommand::typeName,
                                  AddCommand::creator,
                                  AddCommand::syntaxCreator);
  CHECK_MSTATUS(status);

  status = plugin.registerCommand(RemoveCommand::typeName,
                                  RemoveCommand::creator,
                                  RemoveCommand::syntaxCreator);
  CHECK_MSTATUS(status);
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