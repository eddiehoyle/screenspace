#include "ss/commands/RemoveCommand.hh"
#include "ss/Log.hh"
#include "ss/Types.hh"
#include "ss/PickableShape.hh"
#include "RemoveCommand.hh"


#include <maya/MGlobal.h>
#include <maya/MArgParser.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>
#include <maya/MDagModifier.h>
#include <maya/MDagPathArray.h>
#include <maya/MNodeClass.h>
#include <maya/MObjectArray.h>

namespace screenspace {

using Flags = std::pair< const char*, const char* >;

MString RemoveCommand::typeName = "removePickables";

static Flags kParentFlags = { "-p", "-parent" };
static Flags kSelectedFlags = { "-sl", "-selected" };

void* RemoveCommand::creator() {
  return new RemoveCommand();
}

RemoveCommand::RemoveCommand()
    : m_dgm(),
      m_pickables()
{}

MSyntax RemoveCommand::syntaxCreator() {

  MSyntax syntax;
  syntax.addFlag(kParentFlags.first, kParentFlags.second, MSyntax::kString);
  syntax.addFlag(kSelectedFlags.first, kSelectedFlags.second,
                 MSyntax::kBoolean);
  return syntax;
}

MStatus RemoveCommand::doIt(const MArgList& args) {

  MStatus status;
  MArgParser parser(syntax(), args);

  if (parser.isFlagSet(kParentFlags.second) &&
      parser.isFlagSet(kSelectedFlags.second)) {
    MGlobal::displayError("Error removing pickable(s)! Flags 'parent' and 'selected' must be used separately");
    return MS::kFailure;
  }

  if (!parser.isFlagSet(kParentFlags.second) &&
      !parser.isFlagSet(kSelectedFlags.second)) {
    MGlobal::displayError("Error removing pickable(s)! Flags 'parent' or 'selected' are required");
    return MS::kFailure;
  }

  MDagPathArray paths;

  if (parser.isFlagSet(kParentFlags.second)) {

    MString parentName;
    status = parser.getFlagArgument(kParentFlags.second, 0, parentName);
    CHECK_MSTATUS(status);

    MSelectionList list;
    MDagPath parentPath;
    status = list.add(parentName);
    if (status != MStatus::kSuccess) {
      MGlobal::displayError(
          "Error removing pickable(s)! Parent does not exist: " + parentName);
      return MS::kFailure;
    }

    CHECK_MSTATUS(list.getDagPath(0, parentPath));
    if (parentPath.apiType() == MFn::kTransform)
      paths.append(parentPath);
  } else if (parser.isFlagSet(kSelectedFlags.second)) {

    bool state = false;
    parser.getFlagArgument(kSelectedFlags.second, 0, state);
    if (state) {
      MSelectionList list;
      MGlobal::getActiveSelectionList(list);
      for (std::size_t i = 0; i < list.length(); ++i) {
        MDagPath path;
        CHECK_MSTATUS(list.getDagPath(i, path));
        if (path.apiType() == MFn::kTransform)
          paths.append(path);
      }
    }

    if (paths.length() == 0) {
      MGlobal::displayError("Error removing pickable(s)! No transforms selected");
      return MS::kFailure;
    }
  }

  for (std::size_t i = 0; i < paths.length(); ++i) {

    const MDagPath& path = paths[i];
    unsigned int childCount = path.childCount(&status);
    CHECK_MSTATUS(status);

    for (unsigned int i = 0; i < childCount; ++i) {
      MFnDependencyNode fnDepNode(path.child(i, &status));
      if (fnDepNode.typeId() == PickableShape::id) {
        m_pickables.append(fnDepNode.object());
      }
    }
  }

  if (m_pickables.length() == 0) {
    MGlobal::displayError(
        "Error removing pickable(s)! Couldn't find any to remove");
    return MS::kFailure;
  }

  return redoIt();
}

MStatus RemoveCommand::redoIt()
{
  MStatus status;
  for (std::size_t i = 0; i < m_pickables.length(); ++i)
  CHECK_MSTATUS(m_dgm.deleteNode(m_pickables[i]));
  CHECK_MSTATUS(m_dgm.doIt());
  return MS::kSuccess;
}

MStatus RemoveCommand::undoIt()
{
  return m_dgm.undoIt();
}

}