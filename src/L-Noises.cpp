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

	float pink_b0 = 0.0f, pink_b1 = 0.0f, pink_b2 = 0.0f;
	float brown = 0.0f;
	float noise_l = 0.0f;
	float noise_r = 0.0f;
	float smoothed_l = 0.f;
	float smoothed_r = 0.f;

// --------------------   Config module  -----------------------------------------
	L_Noises() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		static const std::vector<std::string> noise_labels = {"Pink", "White", "Brown"};
		configSwitch(NOISER_PARAM, 0.f, 2.f, 1.f, "Noise L mode", noise_labels);
		configSwitch(NOISEL_PARAM, 0.f, 2.f, 1.f, "Noise R mode", noise_labels);
		
		static const std::vector<std::string> gates_labels = {"AND", "OR", "XOR", "NAND", "NOR", "XNOR"};
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
	float generateNoise(float n_type, float white) {

		if (n_type == 1) {
			return white * 1.1f;
		}

		// Pink noise (simple filter).
		if (n_type == 0) {
			float pink = white + 0.75f * pink_b0 + 0.5f * pink_b1 + 0.25f * pink_b2;
			pink_b2 = pink_b1;
			pink_b1 = pink_b0;
			pink_b0 = white;
			return pink * 0.9f;
		}

		// Brown noise (white noise integration).
		if (n_type == 2) {
			brown += white * 0.02f;
			brown = clamp(brown, -5.f, 5.f);
			return brown;
		}

		return white;
	}

	// Logic gates.
	bool logicGatePass(int gate_type, bool a, bool b) {
		switch (gate_type) {
			case 0: return a && b;        // AND
			case 1: return a || b;        // OR
			case 2: return a != b;        // XOR
			case 3: return !(a && b);     // NAND
			case 4: return !(a || b);     // NOR
			case 5: return a == b;        // XNOR
			default: return false;
		}
	}


// --------------------   Main cycle logic  --------------------------------------

	void process(const ProcessArgs& args) override {

		// Reading inputs.
		float a_in = inputs[LOGICA_INPUT].getVoltage();
		float b_in = inputs[LOGICB_INPUT].getVoltage();
		float n_type_l = static_cast<int>(params[NOISEL_PARAM].getValue());
		float n_type_r = static_cast<int>(params[NOISER_PARAM].getValue());
		float gate_l = static_cast<int>(params[GATEL_PARAM].getValue());
		float gate_r = static_cast<int>(params[GATER_PARAM].getValue());
		float vol_l = params[VOLL_PARAM].getValue() / 100.f;
		float vol_r = params[VOLR_PARAM].getValue() / 100.f;
		bool a = a_in > 5.f;
		bool b = b_in > 5.f;

		// Set leds.
		lights[LEDA_LIGHT].setBrightness(a_in > 5.f ? 1.f : 0.f);
		lights[LEDB_LIGHT].setBrightness(b_in > 5.f ? 1.f : 0.f);

		// Generate noises.
		float white = random::normal();  // White noise.
		noise_l = generateNoise(n_type_l, white);
		noise_r = generateNoise(n_type_r, white);

		const float attackSamples = 12.f;  // Attack.
		float coeff = 1.f / attackSamples;
		coeff = clamp(coeff, 0.f, 1.f);

		// Logic gates.
		bool pass_l = logicGatePass(gate_l, a, b);
		bool pass_r = logicGatePass(gate_r, a, b);

		// Apply volume.
		float target_l = pass_l ? noise_l * vol_l : 0.f;
		float target_r = pass_r ? noise_r * vol_r : 0.f;

		// Only apply attack if noise type is brown (2).
		if (n_type_l == 2) {
			smoothed_l += (target_l - smoothed_l) * coeff;
			outputs[OUTL_OUTPUT].setVoltage(smoothed_l);
		} else {
			outputs[OUTL_OUTPUT].setVoltage(target_l);
		}

		if (n_type_r == 2) {
			smoothed_r += (target_r - smoothed_r) * coeff;
			outputs[OUTR_OUTPUT].setVoltage(smoothed_r);
		} else {
			outputs[OUTR_OUTPUT].setVoltage(target_r);
		}

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