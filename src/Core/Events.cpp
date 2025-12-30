#include "Events.h"

namespace Events {
bool edgeIndicesDirty = false;
bool faceIndicesDirty = false;
bool volumeIndicesDirty = false;
bool verticesDirty = false;
bool frameContextDirty = false;

bool ShouldRender() {
  return edgeIndicesDirty || faceIndicesDirty || volumeIndicesDirty || verticesDirty ||
         frameContextDirty;
}

void ResetDirtyFlags() {
  edgeIndicesDirty = false;
  faceIndicesDirty = false;
  volumeIndicesDirty = false;
  verticesDirty = false;
  frameContextDirty = false;
}
}  // namespace Events