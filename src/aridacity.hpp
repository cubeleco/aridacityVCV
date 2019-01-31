#include "rack.hpp"

typedef unsigned int uint_t;
using namespace rack;

extern Plugin *plugin;

extern Model *modelClockDiv;
extern Model *modelBCrush;
extern Model *modelClip;

struct SmallWhiteSwitch : SVGSwitch, ToggleSwitch {
	SmallWhiteSwitch() {
		addFrame(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/smallWhiteSwitch0.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/smallWhiteSwitch1.svg")));
	}
};
struct WhiteSwitch : SVGSwitch, ToggleSwitch {
	WhiteSwitch() {
		addFrame(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/whiteSwitch0.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/whiteSwitch1.svg")));
		addFrame(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/whiteSwitch2.svg")));
	}
};
struct LEDSwitchToggle : SVGSwitch, ToggleSwitch {
	LEDSwitchToggle() {
		addFrame(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/LEDSwitchToggle.svg")));
	}
};
template <typename BASE>
struct LEDSwitchLight : BASE {
	LEDSwitchLight() {
		this->bgColor = COLOR_BLACK_TRANSPARENT;
		this->box.size = Vec(13.0, 13.0);
	}
};

struct SmallWhiteKnob : RoundKnob {
	SmallWhiteKnob() {
		setSVG(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/smallWhiteKnob.svg")));
	}
};
struct WhiteKnob : RoundKnob {
	WhiteKnob() {
		setSVG(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/whiteKnob.svg")));
	}
};

struct SmallWhitePort : SVGPort {
	SmallWhitePort() {
		setSVG(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/smallWhitePort.svg")));
	}
};
struct SmallBlackPort : SVGPort {
	SmallBlackPort() {
		setSVG(SVG::load(assetPlugin(plugin, "res/ComponentLibrary/smallBlackPort.svg")));
	}
};
