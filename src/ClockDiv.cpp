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
		ENUMS(DIV_OUTPUT, 16),
		NUM_OUTPUTS
	};

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger resetTrigger;
	
	const uint_t numTicks = 16;
	uint_t index = 1;
	bool first = false, divideByOne = false;

	ClockDiv();
	void process(const ProcessArgs &args) override;

	json_t* dataToJson() override;
	void dataFromJson(json_t *rootJ) override;
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
		for(uint_t d = DIV_OUTPUT; d < NUM_OUTPUTS; ++d)
			outputs[d].setVoltage(0.f);
		return;
	}

	//if input override output value
	if(inputs[SEQ_INPUT].isConnected())
		clockVal = inputs[SEQ_INPUT].getVoltage();

	//sequence output mode
	if((params[SEQ_PARAM].getValue()) >= 1.f) {
		for(uint_t d = DIV_OUTPUT; d < NUM_OUTPUTS; ++d)
			outputs[d].setVoltage( (d == (index-1))? clockVal : 0.f );
	}
	//division output mode
	else {
		if(index > 1) {
			for(uint_t d = DIV_OUTPUT+first; d < NUM_OUTPUTS; ++d)
				outputs[d].setVoltage( ((index % (d+1)) == 0)? clockVal : 0.f);
		}
		else if(divideByOne) { //on divide by 1 set all outputs to input
			for(uint_t d = DIV_OUTPUT; d < NUM_OUTPUTS; ++d)
				outputs[d].setVoltage(clockVal);
		}
		else { //on first tick mode
			outputs[DIV_OUTPUT].setVoltage(clockVal);
			for(uint_t d = DIV_OUTPUT+1; d < NUM_OUTPUTS; ++d)
				outputs[d].setVoltage(0.f);
		}
	}
}

json_t* ClockDiv::dataToJson() {
	json_t *rootJ = json_object();
	
	json_object_set_new(rootJ, "first", json_boolean(first));
	json_object_set_new(rootJ, "divideByOne", json_boolean(divideByOne));
	return rootJ;
}
void ClockDiv::dataFromJson(json_t *rootJ) {
	json_t *firstJ = json_object_get(rootJ, "first");
	if(firstJ)
		first = json_boolean_value(firstJ);
	
	json_t *divOneJ = json_object_get(rootJ, "divideByOne");
	if(divOneJ)
		divideByOne = json_boolean_value(divOneJ);
}

struct FirstTickItem : MenuItem {
	ClockDiv *module;
	void onAction(const event::Action &e) override {
		module->first ^= true;
	}
	void step() override {
		rightText = CHECKMARK(module->first);
	}
};
struct DivideOneItem : MenuItem {
	ClockDiv *module;
	void onAction(const event::Action &e) override {
		module->divideByOne ^= true;
	}
	void step() override {
		rightText = CHECKMARK(module->divideByOne);
	}
};

struct ClockDivWidget : ModuleWidget {
	ClockDivWidget(ClockDiv *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ClockDiv.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInput<SmallWhitePort>(Vec(0.75, 40), module, ClockDiv::CLOCK_INPUT));
		addInput(createInput<SmallWhitePort>(Vec(23.25, 40), module, ClockDiv::RESET_INPUT));
		
		addParam(createParam<SmallWhiteSwitch>(Vec(6.25, 80), module, ClockDiv::SEQ_PARAM));
		// addChild(createLight<LEDSwitchLight<GreenLight>>(Vec(5, 84), module, ClockDiv::SEQ_LIGHT));
		addInput(createInput<SmallWhitePort>(Vec(23.25, 80), module, ClockDiv::SEQ_INPUT));

		//odd div outputs 
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 120), module, ClockDiv::DIV_OUTPUT));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 150), module, ClockDiv::DIV_OUTPUT + 2));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 180), module, ClockDiv::DIV_OUTPUT + 4));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 210), module, ClockDiv::DIV_OUTPUT + 6));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 240), module, ClockDiv::DIV_OUTPUT + 8));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 270), module, ClockDiv::DIV_OUTPUT + 10));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 300), module, ClockDiv::DIV_OUTPUT + 12));
		addOutput(createOutput<SmallBlackPort>(Vec(0.75, 330), module, ClockDiv::DIV_OUTPUT + 14));
		//even div outputs
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 120), module, ClockDiv::DIV_OUTPUT + 1));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 150), module, ClockDiv::DIV_OUTPUT + 3));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 180), module, ClockDiv::DIV_OUTPUT + 5));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 210), module, ClockDiv::DIV_OUTPUT + 7));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 240), module, ClockDiv::DIV_OUTPUT + 9));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 270), module, ClockDiv::DIV_OUTPUT + 11));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 300), module, ClockDiv::DIV_OUTPUT + 13));
		addOutput(createOutput<SmallBlackPort>(Vec(23.25, 330), module, ClockDiv::DIV_OUTPUT + 15));
	}

	void appendContextMenu(Menu *menu) override {
		menu->addChild(new MenuEntry);

		ClockDiv *module = dynamic_cast<ClockDiv*>(this->module);
		assert(module);

		menu->addChild(construct<FirstTickItem>(&MenuItem::text, "First tick", &FirstTickItem::module, module));
		menu->addChild(construct<DivideOneItem>(&MenuItem::text, "Divisible by 1", &DivideOneItem::module, module));
	}
};

Model *modelClockDiv = createModel<ClockDiv, ClockDivWidget>("ClockDiv");
