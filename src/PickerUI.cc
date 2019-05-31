#include "PickerUI.hh"

#include <maya/MSelectInfo.h>
#include <maya/MSelectionList.h>
#include <maya/MPointArray.h>

namespace screenspace {

void* PickerUI::creator() {
    return new PickerUI();
}

bool PickerUI::select(MSelectInfo& selectInfo,
                                MSelectionList& selectionList,
                                MPointArray& worldSpaceSelectPts) const {
    bool selected = false;
    return false;
}

}