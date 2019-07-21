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

#ifndef SAMPLEPLUGIN_PICKABLEDRAWOVERRIDE_HH
#define SAMPLEPLUGIN_PICKABLEDRAWOVERRIDE_HH

#include <maya/MPxDrawOverride.h>

namespace screenspace {

class PickableDrawOverride : public MHWRender::MPxDrawOverride
{
public:
  static MString classification;
  static MString id;
  static MPxDrawOverride* creator(const MObject& obj);

public:
  ~PickableDrawOverride() override = default;

public:
  MHWRender::DrawAPI supportedDrawAPIs() const override;
  MUserData* prepareForDraw(const MDagPath& pickableDag,
                            const MDagPath& cameraDag,
                            const MHWRender::MFrameContext& frameContext,
                            MUserData* oldData) override;
  bool hasUIDrawables() const override { return true; }
  void addUIDrawables(const MDagPath& pickableDag,
                      MHWRender::MUIDrawManager& drawManager,
                      const MHWRender::MFrameContext& frameContext,
                      const MUserData* data) override;

private:

  /// Check if the pickable is attached to this camera.
  /// \param pickable The pickable
  /// \param camera The camera
  /// \return True if attached, else false.
  bool isAttachedCamera(const MDagPath& pickable, const MDagPath& camera) const;

private:
  PickableDrawOverride(const MObject& obj) : MPxDrawOverride(obj, nullptr) {}
};

}
#endif // SAMPLEPLUGIN_PICKABLEDRAWOVERRIDE_HH