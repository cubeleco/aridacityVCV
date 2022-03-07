#include "aridacity.hpp"

struct Clip : Module {
	enum ParamIds {
		PULL_PARAM,
		ENABLE_LIMIT_PARAM,
		GAIN_PARAM,
		PUSH_PARAM,
		LIMIT_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		AUDIO_INPUT,
		GAIN_INPUT,
		PUSH_SIZ_INPUT,
		PUSH_POS_INPUT,
		LIMIT_SIZ_INPUT,
		LIMIT_POS_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};

	Clip();
	void process(const ProcessArgs &args) override;
};

Clip::Clip() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	configBypass(Clip::AUDIO_INPUT, Clip::AUDIO_OUTPUT);
	configSwitch(Clip::PULL_PARAM, 0, 1, 0, "Inner limit", {"Push", "Pull"});
	configSwitch(Clip::ENABLE_LIMIT_PARAM, 0, 1, 1, "Outer limit", {"off", "on"});
	configParam(Clip::GAIN_PARAM, 0.f, 2.f, 1.f, "Gain");
	configParam(Clip::PUSH_PARAM, 0.f, 2.f, 0.f, "Push");
	configParam(Clip::LIMIT_PARAM, 0.f, 2.f, 1.f, "Limit");
	configInput(Clip::AUDIO_INPUT, "Audio");
	configInput(Clip::GAIN_INPUT, "Gain");
	configInput(Clip::PUSH_SIZ_INPUT, "Push");
	configInput(Clip::PUSH_POS_INPUT, "Push Position");
	configInput(Clip::LIMIT_SIZ_INPUT, "Limit");
	configInput(Clip::LIMIT_POS_INPUT, "Limit Position");
	configOutput(Clip::AUDIO_OUTPUT, "Audio");
}

void Clip::process(const ProcessArgs &args) {
	const int channels = inputs[AUDIO_INPUT].getChannels();
	outputs[AUDIO_OUTPUT].setChannels(channels);


	for(int c = 0; c < channels; ++c) {
		const float pushSiz = params[PUSH_PARAM].getValue() + (inputs[PUSH_SIZ_INPUT].getVoltage(c) / 10.f);
		const float pushCent = inputs[PUSH_POS_INPUT].getVoltage(c) / 5.f;
		//add center offset
		const float pushHi = pushCent + pushSiz;
		const float pushLo = pushCent - pushSiz;

		const float limit = params[LIMIT_PARAM].getValue() + (inputs[LIMIT_SIZ_INPUT].getVoltage(c) / 10.f);
		const float limCenter = inputs[LIMIT_POS_INPUT].getVoltage(c) / 5.f;

		float audi = inputs[AUDIO_INPUT].getVoltage(c) / 5.f;

		//add gain
		audi *= params[GAIN_PARAM].getValue() + (inputs[GAIN_INPUT].getVoltage(c) / 10.f);

		//push inside if in range
		if(audi < pushHi && audi > pushLo) {
			audi = (params[PULL_PARAM].getValue() >= 1.f) ? 0.f :
				(audi > 0.f? pushHi : pushLo);
		}

		//clip limit outside 
		if(params[ENABLE_LIMIT_PARAM].getValue() >= 1.f) {
			audi = clamp(audi, limCenter-limit, limCenter+limit);
		}
		outputs[AUDIO_OUTPUT].setVoltage(audi * 5.f, c);
	}
}

struct ClipWidget : ModuleWidget {
	ClipWidget(Clip *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Clip.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<SmallWhiteSwitch>(Vec(10, 120), module, Clip::PULL_PARAM));
		addParam(createParam<SmallWhiteSwitch>(Vec(40, 120), module, Clip::ENABLE_LIMIT_PARAM));
		addParam(createParam<WhiteKnob>(Vec(10,  40), module, Clip::GAIN_PARAM));
		addParam(createParam<WhiteKnob>(Vec(10, 160), module, Clip::PUSH_PARAM));
		addParam(createParam<WhiteKnob>(Vec(10, 240), module, Clip::LIMIT_PARAM));

		addInput(createInput<SmallWhitePort>(Vec( 4,  80), module, Clip::GAIN_INPUT));
		addInput(createInput<SmallWhitePort>(Vec( 4, 200), module, Clip::PUSH_SIZ_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(35, 200), module, Clip::PUSH_POS_INPUT));
		addInput(createInput<SmallWhitePort>(Vec( 4, 280), module, Clip::LIMIT_SIZ_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(35, 280), module, Clip::LIMIT_POS_INPUT));

		addInput(createInput<SmallWhitePort>(Vec( 4, 330), module, Clip::AUDIO_INPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(35, 330), module, Clip::AUDIO_OUTPUT));
	}
};

Model *modelClip = createModel<Clip, ClipWidget>("Clip");
