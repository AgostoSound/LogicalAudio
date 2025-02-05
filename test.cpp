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

// --------------------   Set initial values  ------------------------------------

	float phase = 0.f;
	random::Xoroshiro128Plus rng;  // Pseudorandom number generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime;  // Clock generator instance.
    std::chrono::steady_clock::time_point lastUpdateTimeL;  // Clock generator instance.
    std::chrono::steady_clock::time_point lastUpdateTimeR;  // Clock generator instance.

	// Avaliable values.
	std::vector<std::__cxx11::basic_string<char>> fractions_labels = {"÷16", "÷8", "÷4", "÷2", "0", "x2", "x4", "x8", "x16"};
	std::vector<std::__cxx11::basic_string<char>> selector_labels = {"Clock", "???", "BPM"};
	std::vector<float> original_fraction_values = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	std::vector<float> normalized_fraction_values = {-16, -8, -4, -2, 0, 2, 4, 8, 16};

	// Initial variables.
	int ms;
	int ms_l = 500;
	int ms_r = 250;
	bool lastClockTrigger = false; // Clock status in the previous cycle.
    float lastVoltage1 = 0.0f; // Last generated voltage.
    float lastVoltage2 = 0.0f; // Last generated voltage.
    float minVoltage = -1.0f;
    float maxVoltage = 1.0f;
	float l_volt;
	float r_volt;

// --------------------   Config module  -----------------------------------------
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

// --------------------   Functions  ---------------------------------------------

	// Beat fraction normalizer.
	float normalizeBeatFraction(float value) {
		// Map index to index between original and normalize values.
		auto it = std::find(original_fraction_values.begin(), original_fraction_values.end(), value);
		size_t index = std::distance(original_fraction_values.begin(), it);
    	return normalized_fraction_values[index];
	}
	// To check if X milliseconds have passed (For BPM Signal).
	bool shouldUpdate(int ms_arg) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime);
        return duration.count() >= ms_arg; // 1000 ms = 1 segundo
    }
	// To check if X milliseconds have passed (For Vampi L).
	bool shouldUpdate_L(int ms_l) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTimeL);
        return duration.count() >= ms_l; // 1000 ms = 1 segundo
    }
	// To check if X milliseconds have passed (For Vampi R).
	bool shouldUpdate_R(int ms_r) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTimeR);
        return duration.count() >= ms_r; // 1000 ms = 1 segundo
    }
	// -2 +2 Standar volt signal to 30-480 bpm scale.
	float bpmSignalLimiterAndNormalizer(float value) {
        float result = 120 * (pow(2, value));  // bpm = 120 x 2^volt;
		result = 60000/result;
        return result;
    }
	// Random spreaded voltage generator.
	float generateRandomVoltage(float spread) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(spread - 1.0f, spread + 1.0f);  // Applying spreads.
		return dis(gen);
	}
	// Spread input converter. ||  (-5V, +5V) -->
	float spread_convert_in(float voltage) {
		float voltage_out = voltage;
		
		if (voltage > 5) {voltage_out = 5;} 
		else {
			if (voltage < -5) {voltage_out = -5;} 
			else {voltage_out = voltage;}
		}

        return voltage_out + 6;
    }


// --------------------   Main cycle logic  --------------------------------------

	void process(const ProcessArgs& args) override {

		// Get and format initial cycle params.
		float beat_fraction = params[BEAT_FRAC_PARAM].getValue();  // Beat fraction param (0-8).
		beat_fraction = normalizeBeatFraction(beat_fraction);  // Normalized beat fraction. (-16 +16).
		
		float spread_1 = params[L_SPREAD_PARAM].getValue();  // L Spread (1-9).
		float spread_2 = params[R_SPREAD_PARAM].getValue();  // R Spread (1-9).
		
		float spread_cv1 = spread_convert_in(inputs[L_CV_INPUT].getVoltage());
		float spread_cv2 = spread_convert_in(inputs[R_CV_INPUT].getVoltage());

		// Select knob or CV for spread.
		if (inputs[L_CV_INPUT].isConnected()) {spread_1 = spread_cv1;}
		else {spread_1 = spread_1;}
		if (inputs[R_CV_INPUT].isConnected()) {spread_2 = spread_cv2;}
		else {spread_2 = spread_2;}

		ms = bpmSignalLimiterAndNormalizer(inputs[BPM_INPUT].getVoltage());  // BPM Input (-2 +2).
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
			if (shouldUpdate_L(ms_l)) {
				l_volt = generateRandomVoltage(spread_1);
				lastUpdateTimeL = std::chrono::steady_clock::now();
			}
			if (shouldUpdate_R(ms_r)) {
				r_volt = generateRandomVoltage(spread_2);
				lastUpdateTimeR = std::chrono::steady_clock::now();
			}
		} 
		else {  // BPM Logic.
			if (shouldUpdate(ms)) {
				l_volt = generateRandomVoltage(spread_1);
				r_volt = generateRandomVoltage(spread_2);
				lastUpdateTime = std::chrono::steady_clock::now();
			} 
		}}

		// OUTS
		outputs[OUT1_OUTPUT].setVoltage(l_volt);  // Set L voltage.
		outputs[OUT2_OUTPUT].setVoltage(r_volt);  // Set R voltage.

};  // End main cycle logic.
};  // End general structure.


// --------------------   Visual components  -------------------------------------
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