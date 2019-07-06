// Copyright 2019 Edward Hoyle
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SCREENSPACE_REMOVECOMMAND_HH
#define SCREENSPACE_REMOVECOMMAND_HH

#include <maya/MDagModifier.h>
#include <maya/MObjectArray.h>
#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

namespace screenspace {

class RemoveCommand : public MPxCommand {
public:
  static MString typeName;
  static void* creator();
  static MSyntax syntaxCreator();

public:
  RemoveCommand();
  bool isUndoable() const override {return true;}
  MStatus doIt(const MArgList& args) override;
  MStatus redoIt() override;
  MStatus undoIt() override;

private:
  MDagModifier m_dgm;
  MObjectArray m_pickables;
};

}

#endif // SCREENSPACE_REMOVECOMMAND_HH