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

#ifndef SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH
#define SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH

#include <maya/MPxDrawOverride.h>

namespace screenspace {

class PickerDrawOverride : public MPxDrawOverride
{
public:
  static MString classifcation;
  static MString id;
  static MPxDrawOverride* creator(const MObject& obj);

public:

  ~PickerDrawOverride() override = default;
  MHWRender::DrawAPI supportedDrawAPIs() const override
  {
    return (MHWRender::kAllDevices);
  }

  MUserData* prepareForDraw(const MDagPath& pickerDag,
                            const MDagPath& cameraDag,
                            const MFrameContext& frameContext,
                            MUserData* oldData) override;

  bool hasUIDrawables() const override { return true; }

  void addUIDrawables(const MDagPath& pickerDag,
                      MHWRender::MUIDrawManager& drawManager,
                      const MHWRender::MFrameContext& frameContext,
                      const MUserData* data) override;

private:
  bool isTargetCamera(const MDagPath& picker, const MDagPath& camera) const;

private:
  PickerDrawOverride(const MObject& obj);
};

}
#endif // SAMPLEPLUGIN_PICKERDRAWOVERRIDE_HH