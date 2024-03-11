#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>
#include <array>
#include <vector>

using namespace rack;

struct L_Random : Module {
	enum ParamId {
		L_C_FREQ_PARAM,
		L_CR_FREQ_PARAM,
		R_C_FREQ_PARAM,
		R_CR_FREQ_PARAM,
		L_C_SPREAD_PARAM,
		L_CR_SPREAD_PARAM,
		R_C_SPREAD_PARAM,
		R_CR_SPREAD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		GENERAL_FREQ_INPUT,
		GENERAL_SPREAD_INPUT,
		L_C_FREQ_CV_INPUT,
		L_CR_FREQ_CV_INPUT,
		R_C_FREQ_CV_INPUT,
		R_CR_FREQ_CV_INPUT,
		L_C_SPREAD_CV_INPUT,
		L_CR_SPREAD_CV_INPUT,
		R_C_SPREAD_CV_INPUT,
		R_CR_SPREAD_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		L_CONST_OUTPUT,
		L_CONST_RECT_OUTPUT,
		R_CONST_OUTPUT,
		R_CONST_RECT_OUTPUT,
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

		configParam(L_C_FREQ_PARAM, 1.f, 9.f, 0.f, "Freq");  // Left bipolar frequency.
		configParam(L_CR_FREQ_PARAM, 1.f, 9.f, 0.f, "Freq");  // Left unipolar frequency.
		configParam(R_C_FREQ_PARAM, 1.f, 9.f, 0.f, "Freq");  // Right bipolar frequency.
		configParam(R_CR_FREQ_PARAM, 1.f, 9.f, 0.f, "Freq");  // Right unipolar frequency.
		configParam(L_C_SPREAD_PARAM, 1.f, 4.f, 1.f, "Spread");  // Left bipolar spread.
		configParam(L_CR_SPREAD_PARAM, 1.f, 9.f, 1.f, "Spread");  // Left unipolar spread.
		configParam(R_C_SPREAD_PARAM, 1.f, 4.f, 1.f, "Spread");  // Right bipolar spread.
		configParam(R_CR_SPREAD_PARAM, 1.f, 9.f, 1.f, "Spread");  // Right unipolar spread.
		
		configInput(GENERAL_FREQ_INPUT, "General freq CV");  // General frequency CV.
		configInput(GENERAL_SPREAD_INPUT, "General spread CV");  // General spread CV.

		configInput(L_C_FREQ_CV_INPUT, "Freq CV");  // Left bipolar CV.
		configInput(L_CR_FREQ_CV_INPUT, "Freq CV");  // Left unipolar CV.
		configInput(R_C_FREQ_CV_INPUT, "Freq CV");  // Right bipolar CV.
		configInput(R_CR_FREQ_CV_INPUT, "Freq CV");  // Right unipolar CV.
		configInput(L_C_SPREAD_CV_INPUT, "Spread CV");  // Left bipolar CV.
		configInput(L_CR_SPREAD_CV_INPUT, "Spread CV");  // Left unipolar CV.
		configInput(R_C_SPREAD_CV_INPUT, "Spread CV");  // Right bipolar CV.
		configInput(R_CR_SPREAD_CV_INPUT, "Spread CV");  // Right unipolar CV.

		configOutput(L_CONST_OUTPUT, "Left bipolar");  // Left bipolar output.
		configOutput(L_CONST_RECT_OUTPUT, "Left unipolar");  // Left unipolar output.
		configOutput(R_CONST_OUTPUT, "Right bipolar");  // Right bipolar output.
		configOutput(R_CONST_RECT_OUTPUT, "Right unipolar");  // Right unipolar output.
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
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime3);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }
    bool shouldUpdate4(int ms) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime4);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }


	// Main logic.
	void process(const ProcessArgs& args) override {
		float freq_1;
		float freq_2;
		float freq_3;
		float freq_4;
		float spread_1;
		float spread_2;
		float spread_3;
		float spread_4;

		if (inputs[GENERAL_SPREAD_INPUT].isConnected()) {
			// Spread max and min controller.
			float spread_1 = inputs[GENERAL_SPREAD_INPUT].getVoltage();
			float spread_2 = inputs[GENERAL_SPREAD_INPUT].getVoltage();
			float spread_3 = inputs[GENERAL_SPREAD_INPUT].getVoltage();
			float spread_4 = inputs[GENERAL_SPREAD_INPUT].getVoltage();
			
		} else {
			// Spread max and min controller.
			float spread_1 = params[L_C_SPREAD_PARAM].getValue();
			float spread_2 = params[L_CR_SPREAD_PARAM].getValue();
			float spread_3 = params[R_C_SPREAD_PARAM].getValue();
			float spread_4 = params[R_CR_SPREAD_PARAM].getValue();
		}

		if (inputs[GENERAL_FREQ_INPUT].isConnected()) {
			// Update frequency controller.
			float freq_1 = inputs[GENERAL_FREQ_INPUT].getVoltage();  
			float freq_2 = inputs[GENERAL_FREQ_INPUT].getVoltage();  
			float freq_3 = inputs[GENERAL_FREQ_INPUT].getVoltage();  
			float freq_4 = inputs[GENERAL_FREQ_INPUT].getVoltage(); 
			
		} else {
        	// Update frequency controller.
			float freq_1 = params[L_C_FREQ_PARAM].getValue();  
			float freq_2 = params[L_CR_FREQ_PARAM].getValue();  
			float freq_3 = params[R_C_FREQ_PARAM].getValue();  
			float freq_4 = params[R_CR_FREQ_PARAM].getValue(); 
		}
        
		ms1 = freq_1 * 100;
        ms2 = freq_2 * 100;
        ms3 = freq_3 * 100;
        ms4 = freq_4 * 100;
        //ms = 300;  // TEST.
        
        // L Constant.
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
            outputs[L_CONST_OUTPUT].setVoltage(r1);  // Set voltage.
			lastUpdateTime1 = std::chrono::steady_clock::now();
		}
		
		// Rectified L constant.
		if (shouldUpdate2(ms2)) {  
            std::uniform_real_distribution<float> voltage_generated((spread_2-1.0f), (spread_2+1.0f));
            
			float r2;
			r2 = voltage_generated(rng);
            outputs[L_CONST_RECT_OUTPUT].setVoltage(r2);  // Set voltage.
			lastUpdateTime2 = std::chrono::steady_clock::now();
		}

		// R Constant.
        if (shouldUpdate3(ms3)) {  
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
            outputs[R_CONST_OUTPUT].setVoltage(r3);  // Set voltage.
			lastUpdateTime3 = std::chrono::steady_clock::now();
		}
		
		// Rectified R constant.
		if (shouldUpdate4(ms4)) {  
            std::uniform_real_distribution<float> voltage_generated((spread_4-1.0f), (spread_4+1.0f));
            
			float r4;
			r4 = voltage_generated(rng);
            outputs[R_CONST_RECT_OUTPUT].setVoltage(r4);  // Set voltage.
			lastUpdateTime4 = std::chrono::steady_clock::now();
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
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(7.671, 50.755)), module, L_Random::L_C_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(20.272, 50.722)), module, L_Random::L_CR_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.744, 50.788)), module, L_Random::R_C_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(53.345, 50.755)), module, L_Random::R_CR_FREQ_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(7.644, 74.831)), module, L_Random::L_C_SPREAD_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(20.245, 74.797)), module, L_Random::L_CR_SPREAD_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(40.717, 74.864)), module, L_Random::R_C_SPREAD_PARAM));
		addParam(createParamCentered<RoundSmallBlackKnob>(mm2px(Vec(53.318, 74.831)), module, L_Random::R_CR_SPREAD_PARAM));

		// Inputs.
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.919, 25.321)), module, L_Random::GENERAL_FREQ_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.092, 25.288)), module, L_Random::GENERAL_SPREAD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.624, 62.738)), module, L_Random::L_C_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.225, 62.705)), module, L_Random::L_CR_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.697, 62.772)), module, L_Random::R_C_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.298, 62.738)), module, L_Random::R_CR_FREQ_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.598, 86.814)), module, L_Random::L_C_SPREAD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.198, 86.781)), module, L_Random::L_CR_SPREAD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(40.671, 86.847)), module, L_Random::R_C_SPREAD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.271, 86.814)), module, L_Random::R_CR_SPREAD_CV_INPUT));

		// Outputs.
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(7.62, 108.454)), module, L_Random::L_CONST_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(20.214, 108.454)), module, L_Random::L_CONST_RECT_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(40.746, 108.454)), module, L_Random::R_CONST_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(53.34, 108.454)), module, L_Random::R_CONST_RECT_OUTPUT));
	}
};

Model* modelL_Random = createModel<L_Random, L_RandomWidget>("L-Random");