#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>


// General structure.
struct L_Rantics : Module {
// --------------------   Visual components namespace  ---------------------------

	enum ParamId {
		L_SPREAD_PARAM,
		R_SPREAD_PARAM,
		POLARITY_PARAM,
		TIC_SELECTOR_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		TIC1_INPUT,
		TIC2_INPUT,
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
		UNIPOLAR_LED_LIGHT,
		BIPOLAR_LED_LIGHT,
		LIGHTS_LEN
	};

// --------------------   Set initial values  ------------------------------------
	std::vector<std::__cxx11::basic_string<char>> lvl_labels = {"1", "2", "3", "4", "5", "6", "7", "8", "9"};
	random::Xoroshiro128Plus rng;  // Pseudorandom number generator instance.



// --------------------   Config module  -----------------------------------------
	L_Rantics() {
		// Random states.
		uint64_t seed0 = std::random_device{}();
        uint64_t seed1 = std::random_device{}();
        rng.seed(seed0, seed1);

		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configSwitch(L_SPREAD_PARAM, 1.f, 9.f, 1.f, "LvL L", lvl_labels);
		configSwitch(R_SPREAD_PARAM, 1.f, 9.f, 1.f, "LvL R", lvl_labels);

		configParam(POLARITY_PARAM, 0.f, 1.f, 0.f, "Polarity");
		configParam(TIC_SELECTOR_PARAM, 0.f, 1.f, 0.f, "Tic Selector");

		configInput(TIC1_INPUT, "Gate L");
		configInput(TIC2_INPUT, "Gate R");
		configInput(L_CV_INPUT, "LvL CV L");
		configInput(R_CV_INPUT, "LvL CV R");

		configOutput(OUT1_OUTPUT, "Random L");
		configOutput(OUT2_OUTPUT, "Random R");
	}


// --------------------   Main cycle logic  --------------------------------------

	void process(const ProcessArgs& args) override {
		// Obtiene el valor del parámetro de polaridad y enciende LED correspondiente.
		float polarity = params[POLARITY_PARAM].getValue();
		lights[UNIPOLAR_LED_LIGHT].setBrightness(polarity == 0.f ? 1.f : 0.f);
		lights[BIPOLAR_LED_LIGHT].setBrightness(polarity == 1.f ? 1.f : 0.f);

		// Cuantización de L_SPREAD_PARAM y R_SPREAD_PARAM
		float L_lvl = params[L_SPREAD_PARAM].getValue();
		float R_lvl = params[R_SPREAD_PARAM].getValue();

		// Salidas.
		outputs[OUT1_OUTPUT].setVoltage(L_lvl);
		outputs[OUT2_OUTPUT].setVoltage(R_lvl);
	}
};


// --------------------   Visual components  -------------------------------------

struct L_RanticsWidget : ModuleWidget {
	L_RanticsWidget(L_Rantics* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/L-Rantics.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(13.571, 73.8)), module, L_Rantics::L_SPREAD_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(48.496, 73.8)), module, L_Rantics::R_SPREAD_PARAM));

		addParam(createParamCentered<CKSS>(mm2px(Vec(30.504, 52.33)), module, L_Rantics::POLARITY_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(30.504, 35.30)), module, L_Rantics::TIC_SELECTOR_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.471, 35.84)), module, L_Rantics::TIC1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.391, 35.84)), module, L_Rantics::TIC2_INPUT));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.466, 87)), module, L_Rantics::L_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.258, 87)), module, L_Rantics::R_CV_INPUT));

		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(13.273, 106.29)), module, L_Rantics::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(48.383, 106.29)), module, L_Rantics::OUT2_OUTPUT));

		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(25.742, 52.33)), module, L_Rantics::UNIPOLAR_LED_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(35.267, 52.33)), module, L_Rantics::BIPOLAR_LED_LIGHT));
		
	}
};


Model* modelL_Rantics = createModel<L_Rantics, L_RanticsWidget>("L-Rantics");