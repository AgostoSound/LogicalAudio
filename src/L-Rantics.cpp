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

	bool lastTicL = false; // Tic status in the previous cycle.
	bool lastTicR = false; // Tic status in the previous cycle.
    float lastVoltage1 = 0.0f; // Last generated voltage.
    float lastVoltage2 = 0.0f; // Last generated voltage.
    float minVoltage = -1.0f;
    float maxVoltage = 1.0f;
	float l_volt;
	float r_volt;
	float L_lvl;
	float R_lvl;
	bool ticL = false;
	bool ticR = false;
	int factor;
	bool tics_disconected = false;


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


// --------------------   Functions  ---------------------------------------------

	// Obtain multiply factor for polarity.
	float getFactor(float polarity) {
		float random_range = rack::random::uniform() * (maxVoltage - minVoltage) + minVoltage;
		if (random_range >= 0) {factor = 1;}
		else {factor = -1;}
		if (polarity == 1.f) {factor = 1;}
    	return factor;
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
		// Get polarity switch value and turn on the LEDs.
		float polarity = params[POLARITY_PARAM].getValue();
		lights[UNIPOLAR_LED_LIGHT].setBrightness(polarity == 0.f ? 1.f : 0.f);
		lights[BIPOLAR_LED_LIGHT].setBrightness(polarity == 1.f ? 1.f : 0.f);		 

		// Get LvL knobs or cv values.
		if (inputs[L_CV_INPUT].isConnected()) {
			L_lvl = spread_convert_in(inputs[L_CV_INPUT].getVoltage());
		} else {L_lvl = params[L_SPREAD_PARAM].getValue();}
		if (inputs[L_CV_INPUT].isConnected()) {
			R_lvl = spread_convert_in(inputs[R_CV_INPUT].getVoltage());
		} else {R_lvl = params[R_SPREAD_PARAM].getValue();}

		tics_disconected = false;
		float ticSelector = params[TIC_SELECTOR_PARAM].getValue();

		static float timer = 0.f; // Temporizador interno.
		static float nextTicInterval = 0.f; // Intervalo de tiempo hasta el próximo tic.

		if (ticSelector == 0.f) {
			if (inputs[TIC1_INPUT].isConnected() && inputs[TIC2_INPUT].isConnected()) {
				ticL = inputs[TIC1_INPUT].getVoltage() >= 1.0f;
				ticR = inputs[TIC2_INPUT].getVoltage() >= 1.0f;
			} else if (inputs[TIC1_INPUT].isConnected()) {
				ticL = inputs[TIC1_INPUT].getVoltage() >= 1.0f;
				ticR = ticL;
			} else if (inputs[TIC2_INPUT].isConnected()) {
				ticR = inputs[TIC2_INPUT].getVoltage() >= 1.0f;
				ticL = ticR;
			} else {
				l_volt = 0.f;
				r_volt = 0.f;
				tics_disconected = true;
			}
		}
		else {
			// Modo tics aleatorios.
			timer += args.sampleTime; // Incrementar el temporizador.

			if (timer >= nextTicInterval) {
				// Generar tics aleatorios.
				ticL = random::uniform() > 0.5f; // 50% de probabilidad de tic.
				ticR = random::uniform() > 0.5f; // 50% de probabilidad de tic.
				timer = 0.f; // Reiniciar el temporizador.
				nextTicInterval = random::uniform() * 0.8f + 0.2f; // Nuevo intervalo aleatorio entre 0.4 y 1.2 segundos.
			} else {
				// No hay tics en este ciclo.
				ticL = false;
				ticR = false;
			}
		}
		

		if (ticL != lastTicL && tics_disconected == false) {  // If a change in the L tic signal is detected.
			if (ticL) {  // New clock tic.
				// Random voltage between -1V y +1V.
				float randomVoltage1 = rack::random::uniform() * (maxVoltage - minVoltage) + minVoltage;

				// Calculate final L voltage.
				l_volt = L_lvl + randomVoltage1;
				factor = getFactor(polarity); 
				l_volt = l_volt * factor;  

				// Update last voltage.
				lastVoltage1 = l_volt;
			} else {
				// Assign last L voltage.
				l_volt = lastVoltage1;
			}
		}
		lastTicL = ticL;  // Update tic state.

		if (ticR != lastTicR && tics_disconected == false) {  // If a change in the R tic signal is detected.
			if (ticR) {  // New clock tic.
				// Random voltage between -1V y +1V.
				float randomVoltage2 = rack::random::uniform() * (maxVoltage - minVoltage) + minVoltage;
				
				// Calculate final R voltage.
				r_volt = R_lvl + randomVoltage2;
				factor = getFactor(polarity); 
				r_volt = r_volt * factor;

				// Update last voltage.
				lastVoltage2 = r_volt;
			} else {
				// Assign last R voltage.
				r_volt = lastVoltage2;
			}
		}
		lastTicR = ticR;  // Update tic state.

		// OUTPUTS.
		outputs[OUT1_OUTPUT].setVoltage(l_volt);
		outputs[OUT2_OUTPUT].setVoltage(r_volt);
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