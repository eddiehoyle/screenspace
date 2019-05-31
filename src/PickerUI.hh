#ifndef SAMPLEPLUGIN_PICKERUI_HH
#define SAMPLEPLUGIN_PICKERUI_HH

#include <maya/MPxSurfaceShapeUI.h>

namespace screenspace {

class PickerUI : public MPxSurfaceShapeUI{
public:
  static void* creator();
public:
  bool select(MSelectInfo& selectInfo,
              MSelectionList& selectionList,
              MPointArray& worldSpaceSelectPts) const override;
};

}

#endif // SAMPLEPLUGIN_PICKERUI_HH
