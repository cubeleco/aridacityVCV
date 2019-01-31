#include "aridacity.hpp"
#include "dsp/digital.hpp"


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

	SchmittTrigger clockTrigger;
	SchmittTrigger resetTrigger;
	
	const uint_t numTicks = 16;
	uint_t index = 1;

	ClockDiv() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
	void step() override;
};


void ClockDiv::step() {
	float clockVal = inputs[CLOCK_INPUT].value;
	//increment index on clock high
	if(inputs[CLOCK_INPUT].active) {
		if(clockTrigger.process(clockVal)) {
			++index;
			if(index > numTicks)
				index = 1;
		}
	}
	if(resetTrigger.process(inputs[RESET_INPUT].value))
		index = 1;

	const bool gateIn = clockTrigger.isHigh();
	//if seq input override output value
	if(inputs[SEQ_INPUT].active)
		clockVal = inputs[SEQ_INPUT].value;

	//sequence output mode
	if((params[SEQ_PARAM].value) >= 1.f) {
		//RESET_OUTPUT is first output
		for(uint_t d = RESET_OUTPUT; d < NUM_OUTPUTS; ++d)
			outputs[d].value = gateIn ? (d == index-1 ? clockVal : 0.f) : 0.f;
	}
	//division ouput mode
	else {
		if(index > 1) { //avoid divide by 1 and set division outputs when gate is high
			for(uint_t d = DIV2_OUTPUT; d < NUM_OUTPUTS; ++d)
				outputs[d].value = ( gateIn && ((index % (d+1)) == 0) ) ? clockVal : 0.f;
		}
		/*else if(gateIn) { //keep old outputs while gate is high
			outputs[RESET_OUTPUT].value = clockVal;
		}*/
		else { //drop reset and old outputs
			for(uint_t d = DIV2_OUTPUT; d < NUM_OUTPUTS; ++d)
				outputs[d].value = 0.f;
			outputs[RESET_OUTPUT].value = gateIn ? clockVal : 0.f;
		}
	}
}


struct ClockDivWidget : ModuleWidget {
	ClockDivWidget(ClockDiv *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/ClockDiv.svg")));

		addChild(Widget::create<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(Widget::create<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(Port::create<SmallWhitePort>(Vec(0.75, 30), Port::INPUT, module, ClockDiv::CLOCK_INPUT));
		addInput(Port::create<SmallWhitePort>(Vec(23.25, 30), Port::INPUT, module, ClockDiv::RESET_INPUT));
		
		addParam(ParamWidget::create<SmallWhiteSwitch>(Vec(6.25, 70), module, ClockDiv::SEQ_PARAM, 0.f, 1.f, 0.f));
		// addChild(ModuleLightWidget::create<LEDSwitchLight<GreenLight>>(Vec(5, 84), module, ClockDiv::SEQ_LIGHT));
		addInput(Port::create<SmallWhitePort>(Vec(23.25, 70), Port::INPUT, module, ClockDiv::SEQ_INPUT));

		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 120), Port::OUTPUT, module, ClockDiv::RESET_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 150), Port::OUTPUT, module, ClockDiv::DIV3_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 180), Port::OUTPUT, module, ClockDiv::DIV5_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 210), Port::OUTPUT, module, ClockDiv::DIV7_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 240), Port::OUTPUT, module, ClockDiv::DIV9_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 270), Port::OUTPUT, module, ClockDiv::DIV11_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 300), Port::OUTPUT, module, ClockDiv::DIV13_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(0.75, 330), Port::OUTPUT, module, ClockDiv::DIV15_OUTPUT));

		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 120), Port::OUTPUT, module, ClockDiv::DIV2_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 150), Port::OUTPUT, module, ClockDiv::DIV4_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 180), Port::OUTPUT, module, ClockDiv::DIV6_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 210), Port::OUTPUT, module, ClockDiv::DIV8_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 240), Port::OUTPUT, module, ClockDiv::DIV10_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 270), Port::OUTPUT, module, ClockDiv::DIV12_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 300), Port::OUTPUT, module, ClockDiv::DIV14_OUTPUT));
		addOutput(Port::create<SmallBlackPort>(Vec(23.25, 330), Port::OUTPUT, module, ClockDiv::DIV16_OUTPUT));
	}
};

Model *modelClockDiv = Model::create<ClockDiv, ClockDivWidget>("aridacity", "ClockDiv", "Clock divider", CLOCK_MODULATOR_TAG, SWITCH_TAG, SEQUENCER_TAG);
