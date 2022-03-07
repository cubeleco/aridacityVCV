#include "aridacity.hpp"

struct BCrush : Module {
	enum ParamIds {
		SAMPLE_RATE_PARAM,
		AMP_RES_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		AUDIO_INPUT,
		SAMPLE_RATE_INPUT,
		CLOCK_HOLD_INPUT,
		AMP_RES_INPUT,
		GAIN_INPUT,
		SHIFTL_INPUT,
		SHIFTR_INPUT,
		AND_INPUT,
		OR_INPUT,
		XOR_INPUT,
		NOT_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};

	dsp::SchmittTrigger holdTrigger;

	const float maxRes = 12.8f;
	float curSampleTime = 0.f;

	BCrush();
	void process(const ProcessArgs &args) override;
	void onSampleRateChange(const SampleRateChangeEvent& e) override;
};

BCrush::BCrush() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	configParam(BCrush::SAMPLE_RATE_PARAM, 0.f, 1.f, 1.f, "Sample rate", "Hz", 0.f, APP->engine->getSampleRate());
	configParam(BCrush::AMP_RES_PARAM, 0.f, 10.f, 10.f, "Resolution", "", 0.f, maxRes);
	configBypass(BCrush::AUDIO_INPUT, BCrush::AUDIO_OUTPUT);

	configInput(BCrush::AUDIO_INPUT, "Audio");
	configInput(BCrush::SAMPLE_RATE_INPUT, "Sample rate");
	configInput(BCrush::CLOCK_HOLD_INPUT, "Sample and hold");
	configInput(BCrush::AMP_RES_INPUT, "Resolution");
	configInput(BCrush::GAIN_INPUT, "Gain");
	configInput(BCrush::SHIFTL_INPUT, "Bit-shift left");
	configInput(BCrush::SHIFTR_INPUT, "Bit-shift right");
	configInput(BCrush::AND_INPUT, "AND logic");
	configInput(BCrush::OR_INPUT, "OR logic");
	configInput(BCrush::XOR_INPUT, "XOR logic");
	configInput(BCrush::NOT_INPUT, "NOT logic");
	configOutput(BCrush::AUDIO_OUTPUT, "Audio");
}

void BCrush::process(const ProcessArgs &args) {
	if(inputs[CLOCK_HOLD_INPUT].isConnected()) {
		//update output on clock input
		if(!holdTrigger.process(inputs[CLOCK_HOLD_INPUT].getVoltage()))
			return;
	}
	else {
		//add time according to sample rate
		curSampleTime += clamp( (params[SAMPLE_RATE_PARAM].getValue() +
					(inputs[SAMPLE_RATE_INPUT].getVoltage() / 10.f)) * args.sampleRate,
					100.f, args.sampleRate );

		//update output if enough time has passed
		if(curSampleTime >= args.sampleRate) {
			curSampleTime -= args.sampleRate;
		}
		//return to keep output sample
		else	return;
	}
	//process input signal channels
	const float resParam = params[AMP_RES_PARAM].getValue();
	const int channels = inputs[AUDIO_INPUT].getChannels();
	outputs[AUDIO_OUTPUT].setChannels(channels);

	for(int c = 0; c < channels; ++c) {
		//limit resolution to avoid divide by zero
		float ampRes = std::max( (resParam + inputs[AMP_RES_INPUT].getVoltage(c)) * maxRes, 1.f);

		float audi = inputs[AUDIO_INPUT].getVoltage(c) / 5.f;

		if(inputs[GAIN_INPUT].isConnected())
			audi *= (inputs[GAIN_INPUT].getVoltage(c) / 5.f);

		//quantize output according to resolution input
		int quant = audi * ampRes;
		//apply bit operations and quantize all inputs
		if(inputs[SHIFTL_INPUT].isConnected())
			quant <<= static_cast<int>( fabs(inputs[SHIFTL_INPUT].getVoltage(c) / 100.f) * ampRes );
		if(inputs[SHIFTR_INPUT].isConnected())
			quant >>= static_cast<int>( (inputs[SHIFTR_INPUT].getVoltage(c) / 100.f) * ampRes );
		if(inputs[AND_INPUT].isConnected())
			quant &= static_cast<int>( (inputs[AND_INPUT].getVoltage(c) / 10.f) * ampRes );
		if(inputs[OR_INPUT].isConnected())
			quant |= static_cast<int>( (inputs[OR_INPUT].getVoltage(c) / 10.f) * ampRes );
		if(inputs[XOR_INPUT].isConnected())
			quant ^= static_cast<int>( (inputs[XOR_INPUT].getVoltage(c) / 10.f) * ampRes );
		if(inputs[NOT_INPUT].isConnected() && fabs(inputs[NOT_INPUT].getVoltage(c)) > 1.f)
			quant = ~quant;
			//quant %= static_cast<int>( (inputs[NOT_INPUT].getVoltage(c) / 10.f) * ampRes );

		//descale output
		outputs[AUDIO_OUTPUT].setVoltage((quant / ampRes) * 5.f, c);
	}
}

void BCrush::onSampleRateChange(const SampleRateChangeEvent& e) {
	paramQuantities[SAMPLE_RATE_PARAM]->displayMultiplier = e.sampleRate;
}

struct BCrushWidget : ModuleWidget {
	BCrushWidget(BCrush *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/BCrush.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParam<WhiteKnob>(Vec(10, 40), module, BCrush::SAMPLE_RATE_PARAM));
		addParam(createParam<WhiteKnob>(Vec(10, 120), module, BCrush::AMP_RES_PARAM));

		addInput(createInput<SmallWhitePort>(Vec(4, 80), module, BCrush::SAMPLE_RATE_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(35, 80), module, BCrush::CLOCK_HOLD_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(4, 160), module, BCrush::AMP_RES_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(35, 160), module, BCrush::GAIN_INPUT));

		addInput(createInput<SmallWhitePort>(Vec(4, 200), module, BCrush::SHIFTL_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(35, 200), module, BCrush::SHIFTR_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(4, 240), module, BCrush::AND_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(35, 240), module, BCrush::OR_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(4, 280), module, BCrush::XOR_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(35, 280), module, BCrush::NOT_INPUT));

		addInput(createInput<SmallWhitePort>(Vec(4, 330), module, BCrush::AUDIO_INPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(35, 330), module, BCrush::AUDIO_OUTPUT));
	}
};

Model *modelBCrush = createModel<BCrush, BCrushWidget>("BCrush");
