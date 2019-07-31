#include "aridacity.hpp"

struct ClockDiv : Module {
	enum ParamIds {
		SEQ_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		CLOCK_INPUT,
		RESET_INPUT,
		SEQ_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		RESET_OUTPUT,
		DIV2_OUTPUT,
		DIV3_OUTPUT,
		DIV4_OUTPUT,
		DIV5_OUTPUT,
		DIV6_OUTPUT,
		DIV7_OUTPUT,
		DIV8_OUTPUT,
		DIV9_OUTPUT,
		DIV10_OUTPUT,
		DIV11_OUTPUT,
		DIV12_OUTPUT,
		DIV13_OUTPUT,
		DIV14_OUTPUT,
		DIV15_OUTPUT,
		DIV16_OUTPUT,
		NUM_OUTPUTS
	};

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	
	const uint_t numTicks = 16;
	uint_t index = 1;

	ClockDiv();
	void process(const ProcessArgs &args) override;
};

ClockDiv::ClockDiv() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	configParam(ClockDiv::SEQ_PARAM, 0.f, 1.f, 0.f, "sequencer mode");
}


void ClockDiv::process(const ProcessArgs &args) {
	float clockVal = inputs[CLOCK_INPUT].getVoltage();
	//increment index on clock high
	if(inputs[CLOCK_INPUT].isConnected()) {
		if(clockTrigger.process(clockVal)) {
			++index;
			if(index > numTicks)
				index = 1;
		}
	}
	if(resetTrigger.process(inputs[RESET_INPUT].getVoltage()))
		index = 1;

	//clear all outputs when clock is low; nothing left to update
	if(!clockTrigger.isHigh()) {
		for(uint_t d = RESET_OUTPUT; d < NUM_OUTPUTS; ++d)
			outputs[d].setVoltage(0.f);
		return;
	}

	//if seq input override output value
	if(inputs[SEQ_INPUT].isConnected())
		clockVal = inputs[SEQ_INPUT].getVoltage();

	//process outputs
	//sequence output mode
	if((params[SEQ_PARAM].getValue()) >= 1.f) {
		//RESET_OUTPUT is first output
		for(uint_t d = RESET_OUTPUT; d < NUM_OUTPUTS; ++d)
			outputs[d].setVoltage( (d == index-1)? clockVal : 0.f );
	}
	//division ouput mode
	else {
		if(index > 1) { //avoid divide by 1 and set division outputs when gate is high
			for(uint_t d = DIV2_OUTPUT; d < NUM_OUTPUTS; ++d)
				outputs[d].setVoltage( ((index % (d+1)) == 0)? clockVal : 0.f);
		}
		/*else if(clockTrigger.isHigh()) { //keep old outputs while gate is high
			outputs[RESET_OUTPUT].setVoltage(clockVal);
		}*/
		else { //drop reset and old outputs
			outputs[RESET_OUTPUT].setVoltage(clockVal);
			for(uint_t d = DIV2_OUTPUT; d < NUM_OUTPUTS; ++d)
				outputs[d].setVoltage(0.f);
		}
	}
}


struct ClockDivWidget : ModuleWidget {
	ClockDivWidget(ClockDiv *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ClockDiv.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInput<SmallWhitePort>(Vec(0.75, 30), module, ClockDiv::CLOCK_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(23.25, 30), module, ClockDiv::RESET_INPUT));
		
		addParam(createParam<SmallWhiteSwitch>(Vec(6.25, 70), module, ClockDiv::SEQ_PARAM));
		// addChild(createLight<LEDSwitchLight<GreenLight>>(Vec(5, 84), module, ClockDiv::SEQ_LIGHT));
		addInput(createInput<SmallWhitePort>(Vec(23.25, 70), module, ClockDiv::SEQ_INPUT));

		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 120), module, ClockDiv::RESET_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 150), module, ClockDiv::DIV3_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 180), module, ClockDiv::DIV5_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 210), module, ClockDiv::DIV7_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 240), module, ClockDiv::DIV9_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 270), module, ClockDiv::DIV11_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 300), module, ClockDiv::DIV13_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 330), module, ClockDiv::DIV15_OUTPUT));

		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 120), module, ClockDiv::DIV2_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 150), module, ClockDiv::DIV4_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 180), module, ClockDiv::DIV6_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 210), module, ClockDiv::DIV8_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 240), module, ClockDiv::DIV10_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 270), module, ClockDiv::DIV12_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 300), module, ClockDiv::DIV14_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 330), module, ClockDiv::DIV16_OUTPUT));
	}
};

Model *modelClockDiv = createModel<ClockDiv, ClockDivWidget>("ClockDiv");
