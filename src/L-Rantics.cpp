#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>

struct L_Rantics : Module {
	enum ParamId {
		BEAT_FRAC_PARAM,
		L_SPREAD_PARAM,
		R_SPREAD_PARAM,
		SELECT_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLOCK_INPUT,
		BPM_INPUT,
		L_CV_INPUT,
		R_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	L_Rantics() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BEAT_FRAC_PARAM, -16.f, 16.f, 1.f, "Beat Fraction");
		configParam(L_SPREAD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(R_SPREAD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SELECT_PARAM, 0.f, 2.f, 1.f, "Beat selector");
		configInput(CLOCK_INPUT, "");
		configInput(BPM_INPUT, "");
		configInput(L_CV_INPUT, "");
		configInput(R_CV_INPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct L_RanticsWidget : ModuleWidget {
	L_RanticsWidget(L_Rantics* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/L-Rantics.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(30.504, 49.33)), module, L_Rantics::BEAT_FRAC_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(13.571, 73.8)), module, L_Rantics::L_SPREAD_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(48.496, 73.8)), module, L_Rantics::R_SPREAD_PARAM));

        addParam(createParam<CKSSThreeHorizontal>(mm2px(Vec(25.40, 33.30)), module, L_Rantics::SELECT_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.471, 35.84)), module, L_Rantics::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.391, 35.84)), module, L_Rantics::BPM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.466, 87.3)), module, L_Rantics::L_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.258, 87.3)), module, L_Rantics::R_CV_INPUT));

		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(13.273, 106.29)), module, L_Rantics::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(48.383, 106.29)), module, L_Rantics::OUT2_OUTPUT));
	}
};


Model* modelL_Rantics = createModel<L_Rantics, L_RanticsWidget>("L-Rantics");