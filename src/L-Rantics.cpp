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

	random::Xoroshiro128Plus rng;  // Pseudorandom number generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime1;  // Clock generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime2;  // Clock generator instance.	int ms1;
	int ms1;
	int ms2;

	// Definir los valores permitidos
	std::vector<std::__cxx11::basic_string<char>> fractions_labels = {"÷16", "÷8", "÷4", "÷2", "0", "x2", "x4", "x8", "x16"};
	std::vector<std::__cxx11::basic_string<char>> selector_labels = {"Clock", "???", "BPM"};
	std::vector<float> original_fraction_values = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	std::vector<float> normalized_fraction_values = {-16, -8, -4, -2, 0, 2, 4, 8, 16};

	// init.
	L_Rantics() {
		// Random states.
		uint64_t seed0 = std::random_device{}();
        uint64_t seed1 = std::random_device{}();
        rng.seed(seed0, seed1);

		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(BEAT_FRAC_PARAM, 0.f, 8.f, 4.f, "Beat Fraction", fractions_labels);
		configSwitch(SELECT_PARAM, 0.f, 2.f, 0.f, "Beat selector", selector_labels);
		configParam(L_SPREAD_PARAM, 1.f, 9.f, 1.f, "L Spread");
		configParam(R_SPREAD_PARAM, 1.f, 9.f, 1.f, "R Spread");
		configInput(CLOCK_INPUT, "Clock");
		configInput(BPM_INPUT, "BPM Signal");
		configInput(L_CV_INPUT, "L CV Spread");
		configInput(R_CV_INPUT, "R CV Spread");
		configOutput(OUT1_OUTPUT, "L Random");
		configOutput(OUT2_OUTPUT, "R Random");
	}

	bool lastClockTrigger = false; // Estado del reloj en el ciclo anterior
    float lastVoltage = 0.0f; // Último voltaje generado

    float minVoltage = -5.0f;
    float maxVoltage = 5.0f;

	// Beat fraction normalizer.
	float normalizeBeatFraction(float value) {
		// Map index to index between original and normalize values.
		auto it = std::find(original_fraction_values.begin(), original_fraction_values.end(), value);
		size_t index = std::distance(original_fraction_values.begin(), it);
    	return normalized_fraction_values[index];
	}

	// To check if X milliseconds have passed.
	bool shouldUpdate1(int ms) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime1);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }
    bool shouldUpdate2(int ms) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime2);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }

	void generateRandomVoltage(float spread, float& outputVoltage, bool isBipolar) {
		std::uniform_real_distribution<float> voltage_top((spread-1.0f), (spread+1.0f));  // Top range.
		std::uniform_real_distribution<float> voltage_bottom(-(spread+1.0f), -(spread-1.0f));  // Bottom Range.
		std::uniform_real_distribution<float> choice(0.0f, 2.0f);  // Range choicing.
		float selected = choice(rng);  // Random from 0 to 2.

		float r;  // Choice range.
		if (selected >= 1) {
			r = voltage_top(rng);
		} else {
			r = voltage_bottom(rng);
		}

		outputVoltage = isBipolar ? r : std::abs(r) * 2;
	}


	// MAIN LOGIC.
	void process(const ProcessArgs& args) override {
		float beat_fraction = params[BEAT_FRAC_PARAM].getValue();  // Beat fraction param (0-8).
		beat_fraction = normalizeBeatFraction(beat_fraction);  // Normalidez beat fraction. (-16 +16)

		float spread_1 = params[L_SPREAD_PARAM].getValue();
		float spread_2 = params[R_SPREAD_PARAM].getValue();
		
		outputs[OUT1_OUTPUT].setVoltage(spread_1);  // Set voltage.
		outputs[OUT2_OUTPUT].setVoltage(spread_2);  // Set voltage.
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