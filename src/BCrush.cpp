#include "aridacity.hpp"
#include "dsp/digital.hpp"


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

	SchmittTrigger holdTrigger;
	
	const float maxRes = 12.8f;
	float	ampRes = maxRes,
			sampleRate = engineGetSampleRate(),
			curSampleTime = 0.f;

	BCrush() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	
	void step() override;
	void setOutput();
	void onSampleRateChange() override;
};


void BCrush::step() {
	// if(!(inputs[AUDIO_INPUT].active && outputs[AUDIO_OUTPUT].active))
	// 	return;

	//limit resolution to avoid divide by zero and at least binary output
	ampRes = max( (params[AMP_RES_PARAM].value + inputs[AMP_RES_INPUT].value) * maxRes, 1.f);

	if(inputs[CLOCK_HOLD_INPUT].active) {
		//update output on clock input
		if(holdTrigger.process(inputs[CLOCK_HOLD_INPUT].value)) {
			setOutput();
		}
	}
	else {
		//add time according to sample rate
		curSampleTime += clamp( (params[SAMPLE_RATE_PARAM].value + (inputs[SAMPLE_RATE_INPUT].value / 10.f)) * sampleRate, 100.f, sampleRate );

		//update output if enough time has passed
		if(curSampleTime >= sampleRate) {
			curSampleTime -= sampleRate;
			setOutput();
		}
	}
}
void BCrush::setOutput() {
	float audi = (inputs[AUDIO_INPUT].value / 5.f);

	if(inputs[GAIN_INPUT].active)
		audi *= (inputs[GAIN_INPUT].value / 5.f);

	//quantize output according to resolution input
	int quant = audi * ampRes;
	//apply bit operations and quantize all inputs
	if(inputs[SHIFTL_INPUT].active)
		quant <<= static_cast<int>( (inputs[SHIFTL_INPUT].value / 100.f) * ampRes );
	if(inputs[SHIFTR_INPUT].active)
		quant >>= static_cast<int>( (inputs[SHIFTR_INPUT].value / 100.f) * ampRes );
	if(inputs[AND_INPUT].active)
		quant &= static_cast<int>( (inputs[AND_INPUT].value / 10.f) * ampRes );
	if(inputs[OR_INPUT].active)
		quant |= static_cast<int>( (inputs[OR_INPUT].value / 10.f) * ampRes );
	if(inputs[XOR_INPUT].active)
		quant ^= static_cast<int>( (inputs[XOR_INPUT].value / 10.f) * ampRes );
	if(inputs[NOT_INPUT].active && fabs(inputs[NOT_INPUT].value) > 1.f)
		quant = ~quant;

	//descale output
	outputs[AUDIO_OUTPUT].value = (quant / ampRes) * 5.f;
}

void BCrush::onSampleRateChange() {
	sampleRate = engineGetSampleRate();
}

struct BCrushWidget : ModuleWidget {
	BCrushWidget(BCrush *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/BCrush.svg")));

		addChild(Widget::create<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(ParamWidget::create<WhiteKnob>(Vec(10, 40), module, BCrush::SAMPLE_RATE_PARAM, 0.f, 1.f, 1.f));
		addParam(ParamWidget::create<WhiteKnob>(Vec(10, 120), module, BCrush::AMP_RES_PARAM, 0.f, 10.f, 10.f));

		addInput(Port::create<SmallWhitePort>(Vec(4, 80), Port::INPUT, module, BCrush::SAMPLE_RATE_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(35, 80), Port::INPUT, module, BCrush::CLOCK_HOLD_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(4, 160), Port::INPUT, module, BCrush::AMP_RES_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(35, 160), Port::INPUT, module, BCrush::GAIN_INPUT));

		addInput(Port::create<SmallWhitePort>(Vec(4, 200), Port::INPUT, module, BCrush::SHIFTL_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(35, 200), Port::INPUT, module, BCrush::SHIFTR_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(4, 240), Port::INPUT, module, BCrush::AND_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(35, 240), Port::INPUT, module, BCrush::OR_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(4, 280), Port::INPUT, module, BCrush::XOR_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(35, 280), Port::INPUT, module, BCrush::NOT_INPUT));

		addInput(Port::create<SmallWhitePort>(Vec(4, 330), Port::INPUT, module, BCrush::AUDIO_INPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(35, 330), Port::OUTPUT, module, BCrush::AUDIO_OUTPUT));
	}
};

Model *modelBCrush = Model::create<BCrush, BCrushWidget>("aridacity", "BCrush", "Bit crush", DIGITAL_TAG, QUANTIZER_TAG, DISTORTION_TAG, SAMPLE_AND_HOLD_TAG);
