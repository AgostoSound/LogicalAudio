#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>
#include <array>
#include <vector>


struct L_Random : Module {
	enum ParamId {
		C_FREQ_PARAM,
		CR_FREQ_PARAM,
		P_FREQ_PARAM,
		PR_FREQ_PARAM,
		C_SPREAD_PARAM,
		CR_SPREAD_PARAM,
		P_SPREAD_PARAM,
		PR_SPREAD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		GENERAL_FREQ_INPUT,
		GENERAL_SPREAD_INPUT,
		C_FREQ_CV_INPUT,
		CR_FREQ_CV_INPUT,
		P_FREQ_CV_INPUT,
		PR_FREQ_CV_INPUT,
		C_SPREAD_CV_INPUT,
		CR_SPREAD_CV_INPUT,
		P_SPREAD_CV_INPUT,
		PR_SPREAD_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		CONST_OUTPUT,
		CONST_RECT_OUTPUT,
		PULSE_OUTPUT,
		PULSE_RECT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};
	
    random::Xoroshiro128Plus rng;  // Pseudorandom number generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime1;  // Clock generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime2;  // Clock generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime3;  // Clock generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime4;  // Clock generator instance.

	std::chrono::steady_clock::time_point lastPulseTime3;  // Pulse duration.
	std::chrono::steady_clock::time_point lastPulseTime4;  // Pulse duration.

	// Actualizar la lógica de proceso para generar un pulso
	std::chrono::steady_clock::time_point now3 = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point now4 = std::chrono::steady_clock::now();
	std::chrono::milliseconds duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(now3 - lastPulseTime3);
	std::chrono::milliseconds duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(now4 - lastPulseTime4);

    int ms1;
    int ms2;
    int ms3;
    int ms4;

	// Init config.
	L_Random() {
		// Random states.
		uint64_t seed0 = std::random_device{}();
        uint64_t seed1 = std::random_device{}();
        rng.seed(seed0, seed1);

		// Components.
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);  // All params.

		configParam(C_FREQ_PARAM, 1.f, 9.f, 0.f, "C-Freq");  // Constant frequency.
		configParam(CR_FREQ_PARAM, 1.f, 9.f, 0.f, "CR-Freq");  // Constant rectified frequency.
		configParam(P_FREQ_PARAM, 1.f, 9.f, 0.f, "P-Freq");  // Pulsed frequency.
		configParam(PR_FREQ_PARAM, 1.f, 9.f, 0.f, "PR-Freq");  // Pulsed rectified frequency.
		configParam(C_SPREAD_PARAM, 1.f, 9.f, 1.f, "C-Spread");  // Constant spread.
		configParam(CR_SPREAD_PARAM, 1.f, 9.f, 1.f, "CR-Spread");  // Constant rectified spread.
		configParam(P_SPREAD_PARAM, 1.f, 9.f, 1.f, "P-Spread");  // Pulsed spread.
		configParam(PR_SPREAD_PARAM, 1.f, 9.f, 1.f, "PR-Spread");  // Pulsed rectified spread.
		
		configInput(GENERAL_FREQ_INPUT, "General freq CV");  // General frequency CV.
		configInput(GENERAL_SPREAD_INPUT, "General spread CV");  // General spread CV.

		configInput(C_FREQ_CV_INPUT, "C-Freq CV");  // Constant frequency CV.
		configInput(CR_FREQ_CV_INPUT, "CR-Freq CV");  // Constant rectified frequency CV.
		configInput(P_FREQ_CV_INPUT, "P-Freq CV");  // Pulsed frequency CV.
		configInput(PR_FREQ_CV_INPUT, "PR-Freq CV");  // Pulsed rectified frequency CV.
		configInput(C_SPREAD_CV_INPUT, "C-Spread CV");  // Constant spread CV.
		configInput(CR_SPREAD_CV_INPUT, "CR-Spread CV");  // Constant rectified spread CV.
		configInput(P_SPREAD_CV_INPUT, "P-Spread CV");  // Pulsed spread CV.
		configInput(PR_SPREAD_CV_INPUT, "PR-Spread CV");  // Pulsed rectified spread CV.

		configOutput(CONST_OUTPUT, "Constant");  // Constant output.
		configOutput(CONST_RECT_OUTPUT, "Constant rectified");  // Constant rectified output.
		configOutput(PULSE_OUTPUT, "Pulsed");  // Pulsed output.
		configOutput(PULSE_RECT_OUTPUT, "Pulsed rectified");  // Pulsed rectified output.
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
    bool shouldUpdate3(int ms) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPulseTime3);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }
    bool shouldUpdate4(int ms) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastPulseTime4);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }


	// Main logic.
	void process(const ProcessArgs& args) override {

        // Spread max and min controller.
        float spread_1 = params[C_SPREAD_PARAM].getValue();
        float spread_2 = params[CR_SPREAD_PARAM].getValue();
        float spread_3 = params[P_SPREAD_PARAM].getValue();
        float spread_4 = params[PR_SPREAD_PARAM].getValue();

        // Update frequency controller.
        float freq_1 = params[C_FREQ_PARAM].getValue();  
        float freq_2 = params[CR_FREQ_PARAM].getValue();  
        float freq_3 = params[P_FREQ_PARAM].getValue();  
        float freq_4 = params[PR_FREQ_PARAM].getValue();  
        
		ms1 = freq_1 * 100;
        ms2 = freq_2 * 100;
        ms3 = freq_3 * 100;
        ms4 = freq_4 * 100;
        //ms = 300;  // TEST.
        
        // Constant.
        if (shouldUpdate1(ms1)) {  
            std::uniform_real_distribution<float> voltage_top((spread_1-1.0f), (spread_1+1.0f));  // Top range.
            std::uniform_real_distribution<float> voltage_bottom(-(spread_1+1.0f), -(spread_1-1.0f));  // Bottom Range.
            std::uniform_real_distribution<float> choice(0.0f, 2.0f);  // Range choicing.
            float selected = choice(rng);  // Random from 0 to 2.

            float r1;  // Choice range.
            if (selected >= 1) {
                r1 = voltage_top(rng);
            } else {
                r1 = voltage_bottom(rng);
            }
            outputs[CONST_OUTPUT].setVoltage(r1);  // Set voltage.
			lastUpdateTime1 = std::chrono::steady_clock::now();
		}
		
		// Rectified constant.
		if (shouldUpdate2(ms2)) {  
            std::uniform_real_distribution<float> voltage_generated((spread_2-1.0f), (spread_2+1.0f));
            
			float r2;
			r2 = voltage_generated(rng);
            outputs[CONST_RECT_OUTPUT].setVoltage(r2);  // Set voltage.
			lastUpdateTime2 = std::chrono::steady_clock::now();
		}

		// Pulse.
		if (duration3.count() >= 600 && shouldUpdate3(ms3)) {
			std::uniform_real_distribution<float> voltage_top((spread_3-1.0f), (spread_3+1.0f));  // Top range.
			std::uniform_real_distribution<float> voltage_bottom(-(spread_3+1.0f), -(spread_3-1.0f));  // Bottom Range.
			std::uniform_real_distribution<float> choice(0.0f, 2.0f);  // Range choicing.
			float selected = choice(rng);  // Random from 0 to 2.
			float r3;  // Choice range.
			if (selected >= 1) {
				r3 = voltage_top(rng);
			} else {
				r3 = voltage_bottom(rng);
			}
			outputs[PULSE_OUTPUT].setVoltage(r3);  // Set voltage.
			lastUpdateTime3 = std::chrono::steady_clock::now();
			lastPulseTime3 = std::chrono::steady_clock::now();
		
		} else {
			outputs[PULSE_OUTPUT].setVoltage(0.0f);
		}

		// Rectified pulse.
		if (duration4.count() >= 600 && shouldUpdate4(ms4)) {
			std::uniform_real_distribution<float> voltage_generated((spread_4-1.0f), (spread_4+1.0f));
			float r4;  
			r4 = voltage_generated(rng);
			outputs[PULSE_OUTPUT].setVoltage(r4);  // Set voltage.
			lastUpdateTime4 = std::chrono::steady_clock::now();
			lastPulseTime4 = std::chrono::steady_clock::now();
		
		} else {
			outputs[PULSE_RECT_OUTPUT].setVoltage(0.0f);
		}
	}
};

// Visual components.
struct L_RandomWidget : ModuleWidget {
	L_RandomWidget(L_Random* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/L-Random.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// Knob params.
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(7.671, 50.755)), module, L_Random::C_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(20.272, 50.722)), module, L_Random::CR_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.744, 50.788)), module, L_Random::P_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(53.345, 50.755)), module, L_Random::PR_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(7.644, 74.831)), module, L_Random::C_SPREAD_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(20.245, 74.797)), module, L_Random::CR_SPREAD_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.717, 74.864)), module, L_Random::P_SPREAD_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(53.318, 74.831)), module, L_Random::PR_SPREAD_PARAM));

		// Inputs.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.919, 25.321)), module, L_Random::GENERAL_FREQ_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.092, 25.288)), module, L_Random::GENERAL_SPREAD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.624, 62.738)), module, L_Random::C_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.225, 62.705)), module, L_Random::CR_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.697, 62.772)), module, L_Random::P_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.298, 62.738)), module, L_Random::PR_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.598, 86.814)), module, L_Random::C_SPREAD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.198, 86.781)), module, L_Random::CR_SPREAD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.671, 86.847)), module, L_Random::P_SPREAD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.271, 86.814)), module, L_Random::PR_SPREAD_CV_INPUT));

		// Outputs.
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(7.62, 108.454)), module, L_Random::CONST_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(20.214, 108.454)), module, L_Random::CONST_RECT_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(40.746, 108.454)), module, L_Random::PULSE_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(53.34, 108.454)), module, L_Random::PULSE_RECT_OUTPUT));
	}
};

Model* modelL_Random = createModel<L_Random, L_RandomWidget>("L-Random");