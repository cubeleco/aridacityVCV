#include <rack.hpp>

typedef unsigned int uint_t;
using namespace rack;

extern Plugin *pluginInstance;

extern Model *modelClockDiv;
extern Model *modelBCrush;
extern Model *modelClip;
extern Model *modelRemainder;

struct SmallWhiteSwitch : app::SvgSwitch {
	SmallWhiteSwitch() {
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/smallWhiteSwitch0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/smallWhiteSwitch1.svg")));
	}
};
struct WhiteSwitch : app::SvgSwitch {
	WhiteSwitch() {
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/whiteSwitch0.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/whiteSwitch1.svg")));
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/whiteSwitch2.svg")));
	}
};
struct LEDSwitchToggle : app::SvgSwitch {
	LEDSwitchToggle() {
		addFrame(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/LEDSwitchToggle.svg")));
	}
};
template <typename BASE>
struct LEDSwitchLight : BASE {
	LEDSwitchLight() {
		LEDSwitchLight::bgColor = SCHEME_BLACK_TRANSPARENT;
		LEDSwitchLight::box.size = Vec(13.0, 13.0);
	}
};

struct SmallWhiteKnob : RoundKnob {
	SmallWhiteKnob() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/smallWhiteKnob.svg")));
	}
};
struct WhiteKnob : RoundKnob {
	WhiteKnob() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/whiteKnob.svg")));
	}
};

struct SmallWhitePort : SVGPort {
	SmallWhitePort() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/smallWhitePort.svg")));
	}
};
struct SmallBlackPort : SVGPort {
	SmallBlackPort() {
		setSvg(Svg::load(asset::plugin(pluginInstance, "res/ComponentLibrary/smallBlackPort.svg")));
	}
};
