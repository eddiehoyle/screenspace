#ifndef SCREENSPACE_REMOVECOMMAND_HH
#define SCREENSPACE_REMOVECOMMAND_HH

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

namespace screenspace {

class RemoveCommand : public MPxCommand {
public:
  static MString typeName;
  static void* creator();
  static MSyntax createSyntax();
public:
  virtual MStatus doIt(const MArgList& args);
};

}
#endif // SCREENSPACE_REMOVECOMMAND_HH
