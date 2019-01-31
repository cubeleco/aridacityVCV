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

	Clip() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;
};


void Clip::step() {
	// if(!(inputs[AUDIO_INPUT].active && outputs[AUDIO_OUTPUT].active))
	// 	return;

	float audi = (inputs[AUDIO_INPUT].value / 5.f);

	//add gain
	audi *= params[GAIN_PARAM].value + (inputs[GAIN_INPUT].value / 10.f);

	const float pushSiz = params[PUSH_PARAM].value + (inputs[PUSH_SIZ_INPUT].value / 10.f);
	const float pushCent = (inputs[PUSH_POS_INPUT].value / 5.f);
	//add center offset
	const float pushHi = pushCent + pushSiz;
	const float pushLo = pushCent - pushSiz;

	//push inside if in range
	if( (audi <= pushHi) && (audi >= pushLo) ) {
		if(params[PULL_PARAM].value >= 1.f)
			audi = 0.f;
		else
			audi = (audi > 0.f? pushHi : pushLo);
	}

	//clip limit outside 
	if(params[ENABLE_LIMIT_PARAM].value >= 1.f) {
		const float limit = params[LIMIT_PARAM].value + (inputs[LIMIT_SIZ_INPUT].value / 10.f);
		const float limCenter = inputs[LIMIT_POS_INPUT].value / 5.f;
		
		audi = clamp(audi, (-limit)+limCenter, limit+limCenter);
	}
	outputs[AUDIO_OUTPUT].value = audi * 5.f;
}

struct ClipWidget : ModuleWidget {
	ClipWidget(Clip *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/Clip.svg")));

		addChild(Widget::create<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<SmallWhiteSwitch>(Vec(10, 120), module, Clip::PULL_PARAM, 0.f, 1.f, 0.f));
		addParam(ParamWidget::create<SmallWhiteSwitch>(Vec(40, 120), module, Clip::ENABLE_LIMIT_PARAM, 0.f, 1.f, 1.f));
		addParam(ParamWidget::create<WhiteKnob>(Vec(10,  40), module, Clip::GAIN_PARAM, 0.f, 2.f, 1.f));
		addParam(ParamWidget::create<WhiteKnob>(Vec(10, 160), module, Clip::PUSH_PARAM, 0.f, 2.f, 0.f));
		addParam(ParamWidget::create<WhiteKnob>(Vec(10, 240), module, Clip::LIMIT_PARAM, 0.f, 2.f, 1.f));

		addInput(Port::create<SmallWhitePort>(Vec( 4,  80), Port::INPUT, module, Clip::GAIN_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec( 4, 200), Port::INPUT, module, Clip::PUSH_SIZ_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(35, 200), Port::INPUT, module, Clip::PUSH_POS_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec( 4, 280), Port::INPUT, module, Clip::LIMIT_SIZ_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(35, 280), Port::INPUT, module, Clip::LIMIT_POS_INPUT));

		addInput(Port::create<SmallWhitePort>(Vec( 4, 330), Port::INPUT, module, Clip::AUDIO_INPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(35, 330), Port::OUTPUT, module, Clip::AUDIO_OUTPUT));
	}
};

Model *modelClip = Model::create<Clip, ClipWidget>("aridacity", "Clip", "Clip limiter", AMPLIFIER_TAG, LIMITER_TAG);
