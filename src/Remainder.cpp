#include "aridacity.hpp"

struct Remainder : Module {
	enum ParamId {
		GAIN_PARAM,
		FEEDBACK_PARAM,
		SHAPE_PARAM,
		MIX_PARAM,
		FOLD_PARAM,
		GAIN_CV_PARAM,
		FEEDBACK_CV_PARAM,
		SHAPE_CV_PARAM,
		MIX_CV_PARAM,
		NUM_PARAMS
	};
	enum InputId {
		AUDIO_INPUT,
		GAIN_INPUT,
		FEEDBACK_INPUT,
		SHAPE_INPUT,
		MIX_INPUT,
		FOLD_INPUT,
		NUM_INPUTS
	};
	enum OutputId {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};

	float lastWet = 0.f;

	Remainder() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configParam(GAIN_PARAM, 0.f, 10.f, 1.f, "Gain", "x");
		configParam(SHAPE_PARAM, 0.f, 1.f, 0.f, "Shape");

		configParam(FEEDBACK_PARAM, 0.f, 2.f, 0.f, "Feedback", "%", 0, 100);
		configParam(MIX_PARAM, 0.f, 1.f, 1.f, "Mix", "%", 0, 100);
		configParam(FOLD_PARAM, 0.f, 10.f, 5.f, "Fold Position", " V");
		getParamQuantity(FOLD_PARAM)->randomizeEnabled = false;

		configParam(GAIN_CV_PARAM, -1.f, 1.f, 0.f, "Gain CV", "%", 0, 100);
		configParam(SHAPE_CV_PARAM, -1.f, 1.f, 0.f, "Shape CV", "%", 0, 100);
		configParam(FEEDBACK_CV_PARAM, -1.f, 1.f, 0.f, "Feedback CV", "%", 0, 100);
		configParam(MIX_CV_PARAM, -1.f, 1.f, 0.f, "Mix CV", "%", 0, 100);

		configInput(GAIN_INPUT, "Gain");
		configInput(SHAPE_INPUT, "Shape");
		configInput(FEEDBACK_INPUT, "Feedback");
		configInput(MIX_INPUT, "Mix");

		configInput(AUDIO_INPUT, "Audio");
		configInput(FOLD_INPUT, "Fold");
		configOutput(AUDIO_OUTPUT, "Audio");
		configBypass(AUDIO_INPUT, AUDIO_OUTPUT);
	}

	void process(const ProcessArgs& args) override {
		float dry = inputs[AUDIO_INPUT].getVoltageSum();
		float gain = params[GAIN_PARAM].getValue() + params[GAIN_CV_PARAM].getValue() * inputs[GAIN_INPUT].getVoltage();
		float divisor = params[FOLD_PARAM].getValue() + inputs[FOLD_INPUT].getVoltage();

		//attenuate audio
		float in = (dry * gain);
		float feedback = params[FEEDBACK_PARAM].getValue() + params[FEEDBACK_CV_PARAM].getValue() * inputs[FEEDBACK_INPUT].getVoltage() / 10.f;
		in += lastWet * feedback;
		//initialize output
		float wet = 0.f;

		//avoid divide by zero
		if(fabs(divisor) > 0.01f) {
			float shape = params[SHAPE_PARAM].getValue() + params[SHAPE_CV_PARAM].getValue() * inputs[SHAPE_INPUT].getVoltage() / 10.f;
			shape = clamp(shape, 0.f, 1.f);

			//fold audio using remainder
			float remainder = std::trunc(in / divisor) * divisor;
			divisor *= 2.f;
			float remSigned = std::round(in / divisor) * divisor;

			wet = in - crossfade(remainder, remSigned, shape);
		}
		lastWet = wet;

		float mix = params[MIX_PARAM].getValue() + params[MIX_CV_PARAM].getValue() * inputs[MIX_INPUT].getVoltage() / 10.f;
		mix = clamp(mix, 0.f, 1.f);
		outputs[AUDIO_OUTPUT].setVoltage(crossfade(dry, wet, mix));
	}
};


struct RemainderWidget : ModuleWidget {
	RemainderWidget(Remainder* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Remainder.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<WhiteKnob>(Vec(30, 60), module, Remainder::GAIN_PARAM));
		addParam(createParamCentered<WhiteKnob>(Vec(90, 60), module, Remainder::FEEDBACK_PARAM));
		addParam(createParamCentered<WhiteKnob>(Vec(60, 120), module, Remainder::FOLD_PARAM));
		addParam(createParamCentered<WhiteKnob>(Vec(30, 180), module, Remainder::SHAPE_PARAM));
		addParam(createParamCentered<WhiteKnob>(Vec(90, 180), module, Remainder::MIX_PARAM));

		addParam(createParamCentered<SmallWhiteKnob>(Vec(15, 250), module, Remainder::GAIN_CV_PARAM));
		addParam(createParamCentered<SmallWhiteKnob>(Vec(45, 250), module, Remainder::FEEDBACK_CV_PARAM));
		addParam(createParamCentered<SmallWhiteKnob>(Vec(75, 250), module, Remainder::SHAPE_CV_PARAM));
		addParam(createParamCentered<SmallWhiteKnob>(Vec(105, 250), module, Remainder::MIX_CV_PARAM));

		addInput(createInputCentered<SmallWhitePort>(Vec(15, 290), module, Remainder::GAIN_INPUT));
		addInput(createInputCentered<SmallWhitePort>(Vec(45, 290), module, Remainder::FEEDBACK_INPUT));
		addInput(createInputCentered<SmallWhitePort>(Vec(75, 290), module, Remainder::SHAPE_INPUT));
		addInput(createInputCentered<SmallWhitePort>(Vec(105, 290), module, Remainder::MIX_INPUT));

		addInput(createInputCentered<SmallWhitePort>(Vec(30, 340), module, Remainder::AUDIO_INPUT));
		addInput(createInputCentered<SmallWhitePort>(Vec(60, 340), module, Remainder::FOLD_INPUT));
		addOutput(createOutputCentered<SmallBlackPort>(Vec(90, 340), module, Remainder::AUDIO_OUTPUT));
	}
};


Model* modelRemainder = createModel<Remainder, RemainderWidget>("Remainder");
