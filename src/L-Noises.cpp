#include "plugin.hpp"
#include "rack.hpp"


// General structure.
struct L_Noises : Module {
// --------------------   Visual components namespace  ---------------------------
	enum ParamId {
		NOISER_PARAM,
		NOISEL_PARAM,
		GATEL_PARAM,
		GATER_PARAM,
		VOLL_PARAM,
		VOLR_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		LOGICA_INPUT,
		LOGICB_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTL_OUTPUT,
		OUTR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LEDA_LIGHT,
		LEDB_LIGHT,
		LIGHTS_LEN
	};

// --------------------   Set initial values  ------------------------------------
	std::vector<std::__cxx11::basic_string<char>> gates_labels = {"AND", "OR", "XOR", "NAND", "NOR", "XNOR"};
	std::vector<std::__cxx11::basic_string<char>> noise_labels = {"Pink", "White", "Brown"};
	float pink_b0 = 0.0f, pink_b1 = 0.0f, pink_b2 = 0.0f;
	float brown = 0.0f;
	float noise_l = 0.0f;
	float noise_r = 0.0f;

// --------------------   Config module  -----------------------------------------
	L_Noises() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configSwitch(NOISER_PARAM, 0.f, 2.f, 1.f, "Noise L mode", noise_labels);
		configSwitch(NOISEL_PARAM, 0.f, 2.f, 1.f, "Noise R mode", noise_labels);
		configSwitch(GATEL_PARAM, 0.f, 5.f, 1.f, "Gate L", gates_labels);
		configSwitch(GATER_PARAM, 0.f, 5.f, 1.f, "Gate R", gates_labels);
		configParam(VOLL_PARAM, 0.f, 100.f, 1.f, "Vol L");
		configParam(VOLR_PARAM, 0.f, 100.f, 1.f, "Vol R");
		configInput(LOGICA_INPUT, "Signal A");
		configInput(LOGICB_INPUT, "Signal B");
		configOutput(OUTL_OUTPUT, "Noise L");
		configOutput(OUTR_OUTPUT, "Noise R");
	}

// --------------------   Functions  ---------------------------------------------
	
	// Generate white noise.
	float generateNoises(float n_type) {

		// White noise.
		float white = random::normal();

		if (n_type == 1) {
			float noiseWhite = white * 1.1f;
			return noiseWhite;
		}

		// Pink noise (simple filter).
		if (n_type == 0) {
			float pink = white + 0.75f * pink_b0 + 0.5f * pink_b1 + 0.25f * pink_b2;
			pink_b2 = pink_b1;
			pink_b1 = pink_b0;
			pink_b0 = white;

			float noisePink = pink * 0.9f;
			return noisePink;
		}

		// Brown noise (white noise integration).
		if (n_type == 2) {
			brown += white * 0.02f;
			brown = clamp(brown, -5.f, 5.f);
		
			float noiseBrown = brown;
			return noiseBrown;
		}
	}

// --------------------   Main cycle logic  --------------------------------------

	void process(const ProcessArgs& args) override {

		// Reading inputs.
		float a_in = inputs[LOGICA_INPUT].getVoltage();
		float b_in = inputs[LOGICB_INPUT].getVoltage();
		float n_type_l = static_cast<int>(params[NOISEL_PARAM].getValue());
		float n_type_r = static_cast<int>(params[NOISER_PARAM].getValue());

		// Set leds
		if (a_in == 10.0) {
			lights[LOGICA_INPUT].setBrightness(1);
			lights[LEDA_LIGHT].setBrightness(1);
		} else {
			lights[LOGICA_INPUT].setBrightness(0);
			lights[LEDA_LIGHT].setBrightness(0);
		}
		if (b_in == 10.0) {
			lights[LOGICB_INPUT].setBrightness(1);
			lights[LEDB_LIGHT].setBrightness(1);
		} else {
			lights[LOGICB_INPUT].setBrightness(0);
			lights[LEDB_LIGHT].setBrightness(0);
		}


		if (inputs[OUTL_OUTPUT].isConnected()) {
			noise_l = generateNoises(n_type_l);
		} else {noise_l = 0.f;}

		if (inputs[OUTR_OUTPUT].isConnected()) {
			noise_r = generateNoises(n_type_r);
		} else {noise_r = 0.f;}


		// Set outputs.
		outputs[OUTL_OUTPUT].setVoltage(noise_l);
		outputs[OUTR_OUTPUT].setVoltage(noise_r);

	}
};


struct L_NoisesWidget : ModuleWidget {
	L_NoisesWidget(L_Noises* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/L-Noises.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(Vec(45.5, 49.472)), module, L_Noises::NOISER_PARAM));
		addParam(createParamCentered<CKSSThreeHorizontal>(mm2px(Vec(15.687, 49.971)), module, L_Noises::NOISEL_PARAM));

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(15.68, 69.508)), module, L_Noises::GATEL_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(45.522, 69.668)), module, L_Noises::GATER_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(15.689, 86.8)), module, L_Noises::VOLL_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(45.352, 86.8)), module, L_Noises::VOLR_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.345, 25.21)), module, L_Noises::LOGICA_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.032, 25.402)), module, L_Noises::LOGICB_INPUT));

		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(13.273, 106.331)), module, L_Noises::OUTL_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(48.323, 106.316)), module, L_Noises::OUTR_OUTPUT));

		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(20.167, 30.635)), module, L_Noises::LEDA_LIGHT));
		addChild(createLightCentered<MediumLight<WhiteLight>>(mm2px(Vec(50.315, 30.421)), module, L_Noises::LEDB_LIGHT));

	}
};


Model* modelL_Noises = createModel<L_Noises, L_NoisesWidget>("L-Noises");