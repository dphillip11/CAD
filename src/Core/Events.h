#pragma once

#include "Utilities/Event.h"

class FrameContext;
class Model;
struct ModelViews;

namespace Events {
extern bool edgeIndicesDirty;
extern bool faceIndicesDirty;
extern bool volumeIndicesDirty;
extern bool verticesDirty;
extern bool frameContextDirty;

bool ShouldRender();

void ResetDirtyFlags();
}  // namespace Events
