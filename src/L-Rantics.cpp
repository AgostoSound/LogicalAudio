#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>

// General logic.
struct L_Rantics : Module {
	
	// Connection variables to visual components.
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
    std::chrono::steady_clock::time_point lastUpdateTime2;  // Clock generator instance.
	int ms1;
	int ms2;

	// Avaliable values.
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

	bool lastClockTrigger = false; // Clock status in the previous cycle.
    float lastVoltage1 = 0.0f; // Last generated voltage.
    float lastVoltage2 = 0.0f; // Last generated voltage.

    float minVoltage = -1.0f;
    float maxVoltage = 1.0f;
	float l_volt;
	float r_volt;

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

	// -2 +2 Standar volt signal to 30-480 bpm scale.
	float bpmSignalLimiterAndNormalizer(float value) {
        float result;

		if (value > 2) {
			result = 2;
		} else {
			if (value < -2) {
				result = -2;
			} else {
				result = value;
			}
		}

		result = 120 * (pow(2, result));  // bpm = 120 x 2^volt
		result = (500 * result) / 120;
        return result;
    }

	// Random voltage generator.
	// void generateRandomVoltage(float spread) {
	// 	std::uniform_real_distribution<float> value((spread-1.0f), (spread+1.0f));
	// 	float output = abs(value);
	// 	return output;
	// }

	float generateRandomVoltage(float spread) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(spread - 1.0f, spread + 1.0f);
		float output = dis(gen);
		return output;
	}

	// MAIN LOGIC.
	void process(const ProcessArgs& args) override {

		// Get and format initial cycle params.
		float beat_fraction = params[BEAT_FRAC_PARAM].getValue();  // Beat fraction param (0-8).
		beat_fraction = normalizeBeatFraction(beat_fraction);  // Normalized beat fraction. (-16 +16).
		
		float spread_1 = params[L_SPREAD_PARAM].getValue();  // L Spread (1-9).
		float spread_2 = params[R_SPREAD_PARAM].getValue();  // R Spread (1-9).

		//float bpm_voltage_input = bpmSignalLimiterAndNormalizer(inputs[BPM_INPUT].getVoltage());  // BPM Input (-2 +2).
		ms1 = bpmSignalLimiterAndNormalizer(inputs[BPM_INPUT].getVoltage());  // BPM Input (-2 +2).
		
		bool clockTrigger = inputs[CLOCK_INPUT].getVoltage() >= 1.0f;  // Reading clock state.
		
		int selector = params[SELECT_PARAM].getValue();  // Selector  value.

		// Beat source selection.
		if (selector == 0) {  // Clock logic.
			if (clockTrigger != lastClockTrigger) {  // If a change in the clock signal is detected.
				if (clockTrigger) {  // New clock tic.
					// Random voltages between -1V y +1V.
					std::uniform_real_distribution<float> distribution_1(minVoltage, maxVoltage);  
					std::uniform_real_distribution<float> distribution_2(minVoltage, maxVoltage);
					
					// Get random voltages.
					float randomVoltage1 = distribution_1(rng);  
					float randomVoltage2 = distribution_2(rng);

					// Calculate final L-R voltages.
					l_volt = spread_1 + randomVoltage1;  
					r_volt = spread_2 + randomVoltage2;

					// Update last voltages.
					lastVoltage1 = randomVoltage1;
					lastVoltage2 = randomVoltage2;
				} else {
					// Assign last L-R voltages.
					l_volt = spread_1 + lastVoltage1;
					r_volt = spread_2 + lastVoltage2;
				}
			}
			lastClockTrigger = clockTrigger;  // Update clock state.

		} else {

		if (selector == 1) {  //  ??? Logic
			selector = selector + 1;

		} else {  // BPM Logic.

			//l_volt = generateRandomVoltage(spread_1)
			//r_volt = generateRandomVoltage(spread_2)

			if (shouldUpdate1(ms1)) {
				l_volt = generateRandomVoltage(spread_1);
				r_volt = generateRandomVoltage(spread_2);
				lastUpdateTime1 = std::chrono::steady_clock::now();
			}
			
			// r_volt = bpm_voltage_input / 100;
		}
		}

		// OUTS
		outputs[OUT1_OUTPUT].setVoltage(l_volt);  // Set L voltage.
		outputs[OUT2_OUTPUT].setVoltage(r_volt);  // Set R voltage.
	}
};


// Visual components.
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