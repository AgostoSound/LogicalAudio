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

	// Definir los valores permitidos
	std::vector<std::__cxx11::basic_string<char>> fractions_labels = {"÷16", "÷8", "÷4", "÷2", "0", "x2", "x4", "x8", "x16"};
	std::vector<std::__cxx11::basic_string<char>> selector_labels = {"Clock", "???", "BPM"};
	std::vector<float> original_fraction_values = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	std::vector<float> normalized_fraction_values = {-16, -8, -4, -2, 0, 2, 4, 8, 16};

	L_Rantics() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(BEAT_FRAC_PARAM, 0.f, 8.f, 4.f, "Beat Fraction", fractions_labels);
		configSwitch(SELECT_PARAM, 0.f, 2.f, 0.f, "Beat selector", selector_labels);
		configParam(L_SPREAD_PARAM, 0.f, 1.f, 0.f, "L Spread");
		configParam(R_SPREAD_PARAM, 0.f, 1.f, 0.f, "R Spread");
		configInput(CLOCK_INPUT, "Clock");
		configInput(BPM_INPUT, "BPM Signal");
		configInput(L_CV_INPUT, "L CV Spread");
		configInput(R_CV_INPUT, "R CV Spread");
		configOutput(OUT1_OUTPUT, "L Random");
		configOutput(OUT2_OUTPUT, "R Random");
	}

	// Beat fraction normalizer.
	float normalizeBeatFraction(float value) {
		auto it = std::find(original_fraction_values.begin(), original_fraction_values.end(), value);
		size_t index = std::distance(original_fraction_values.begin(), it);
    	return normalized_fraction_values[index];
	}


	void process(const ProcessArgs& args) override {
		float beat_fraction = params[BEAT_FRAC_PARAM].getValue();  // Beat fraction param (0-8).
		beat_fraction = normalizeBeatFraction(beat_fraction);  // Normalidez beat fraction. (-16 +16)
		
		outputs[OUT1_OUTPUT].setVoltage(beat_fraction);  // Set voltage.
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