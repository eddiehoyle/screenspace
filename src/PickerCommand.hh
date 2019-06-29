#ifndef SCREENSPACE_PICKERCOMMAND_HH
#define SCREENSPACE_PICKERCOMMAND_HH

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

namespace screenspace {

class CreatePickerCommand : public MPxCommand {
public:
  static MString typeName;
  static void* creator();
  static MSyntax createSyntax();
public:
  virtual MStatus doIt(const MArgList& args);
};

}
#endif // SCREENSPACE_PICKERCOMMAND_HH
