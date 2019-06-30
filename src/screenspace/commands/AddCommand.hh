#ifndef SCREENSPACE_ADDCOMMAND_HH
#define SCREENSPACE_ADDCOMMAND_HH

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

namespace screenspace {

class AddCommand : public MPxCommand {
public:
  static MString typeName;
  static void* creator();
  static MSyntax createSyntax();
public:
  virtual MStatus doIt(const MArgList& args);
};

}
#endif // SCREENSPACE_ADDCOMMAND_HH
