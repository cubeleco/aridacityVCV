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
	bool reset = false;
	bool divideByOne = false;

	ClockDiv();
	void process(const ProcessArgs &args) override;
	void onReset(const ResetEvent &e) override;

	json_t* dataToJson() override;
	void dataFromJson(json_t *rootJ) override;
};

ClockDiv::ClockDiv() {
	config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	configSwitch(ClockDiv::SEQ_PARAM, 0, 1, 0, "Mode", {"Clock Divider", "Sequencer"});
	configInput(ClockDiv::CLOCK_INPUT, "Clock");
	configInput(ClockDiv::RESET_INPUT, "Reset");
	configInput(ClockDiv::SEQ_INPUT, "Modulation");
}


void ClockDiv::process(const ProcessArgs &args) {
	float clockVal = inputs[CLOCK_INPUT].getVoltage();
	//increment index on clock high
	if(clockTrigger.process(clockVal)) {
		++index;
		if(reset || index > numTicks) {
			index = 1;
			reset = false;
		}
	}
	//trigger reset on next clock high
	if(resetTrigger.process(inputs[RESET_INPUT].getVoltage()))
		reset = true;

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
		if(divideByOne && index == 1) {
			//on divide by 1 set all outputs to input
			for(uint_t d = DIV_OUTPUT; d < NUM_OUTPUTS; ++d)
				outputs[d].setVoltage(clockVal);
		}
		else {
			for(uint_t d = DIV_OUTPUT; d < NUM_OUTPUTS; ++d)
				outputs[d].setVoltage( ((index % (d+1)) == 0)? clockVal : 0.f);
		}
	}
}

//user manually initialized module
void ClockDiv::onReset(const ResetEvent &e) {
	index = 1;
	//avoid reseting parameters
}

json_t* ClockDiv::dataToJson() {
	json_t *rootJ = json_object();

	json_object_set_new(rootJ, "divideByOne", json_boolean(divideByOne));
	return rootJ;
}
void ClockDiv::dataFromJson(json_t *rootJ) {
	json_t *divOneJ = json_object_get(rootJ, "divideByOne");
	if(divOneJ)
		divideByOne = json_boolean_value(divOneJ);
}

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

		menu->addChild(construct<DivideOneItem>(&MenuItem::text, "Divisible by 1", &DivideOneItem::module, module));
	}
};

Model *modelClockDiv = createModel<ClockDiv, ClockDivWidget>("ClockDiv");
