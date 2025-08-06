#include "aridacity.hpp"

Plugin *pluginInstance;

void init(Plugin *p) {
	pluginInstance = p;

	p->addModel(modelClockDiv);
	p->addModel(modelBCrush);
	p->addModel(modelClip);
	p->addModel(modelRemainder);
}
