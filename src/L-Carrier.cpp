#include "plugin.hpp"


struct L_Carrier : Module {

	// Declare parameters.
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		A_IN_INPUT,
		B_IN_INPUT,
		AUDIO_IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AND_OUT_OUTPUT,
		OR_OUT_OUTPUT,
		XOR_OUT_OUTPUT,
		NAND_OUT_OUTPUT,
		NOT_OUT_OUTPUT,
		XNOR_OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		CIRCUIT_A_LED_LIGHT,
		CIRCUIT_XOR_LED_LIGHT,
		CIRCUIT_B_LED_LIGHT,
		A_LED_LIGHT,
		B_LED_LIGHT,
		LIGHTS_LEN
	};

	// Initial configuration.
	L_Carrier() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(A_IN_INPUT, "");
		configInput(B_IN_INPUT, "");
		configInput(AUDIO_IN_INPUT, "");
		configOutput(AND_OUT_OUTPUT, "");
		configOutput(OR_OUT_OUTPUT, "");
		configOutput(XOR_OUT_OUTPUT, "");
		configOutput(NAND_OUT_OUTPUT, "");
		configOutput(NOT_OUT_OUTPUT, "");
		configOutput(XNOR_OUT_OUTPUT, "");
	}

	// Main logic.
	void process(const ProcessArgs& args) override {

		// Reading inputs.
		float audio_in = inputs[AUDIO_IN_INPUT].getVoltage();
		float a_in = inputs[A_IN_INPUT].getVoltage();
		float b_in = inputs[B_IN_INPUT].getVoltage();
		
		// Set leds
		if (a_in == 10.0) {
			lights[A_LED_LIGHT].setBrightness(1);
			lights[CIRCUIT_A_LED_LIGHT].setBrightness(1);
		} else {
			lights[A_LED_LIGHT].setBrightness(0);
			lights[CIRCUIT_A_LED_LIGHT].setBrightness(0);
		}
		if (b_in == 10.0) {
			lights[B_LED_LIGHT].setBrightness(1);
			lights[CIRCUIT_B_LED_LIGHT].setBrightness(1);
		} else {
			lights[B_LED_LIGHT].setBrightness(0);
			lights[CIRCUIT_B_LED_LIGHT].setBrightness(0);
		}

		// Set outputs.
		if (a_in == 10.0 && b_in == 10.0) {
			outputs[AND_OUT_OUTPUT].setVoltage(audio_in);  // AND
			outputs[NAND_OUT_OUTPUT].setVoltage(0);
		} else {
			outputs[NAND_OUT_OUTPUT].setVoltage(audio_in);  // NAND
			outputs[AND_OUT_OUTPUT].setVoltage(0);
		}

		if (a_in == 10.0 || b_in == 10.0) {
			outputs[OR_OUT_OUTPUT].setVoltage(audio_in);  // OR
			outputs[NOT_OUT_OUTPUT].setVoltage(0);
		} else {
			outputs[NOT_OUT_OUTPUT].setVoltage(audio_in);  // NOR
			outputs[OR_OUT_OUTPUT].setVoltage(0);
		}

		if (a_in == 10.0 ^ b_in == 10.0) {
			outputs[XOR_OUT_OUTPUT].setVoltage(audio_in);  // XOR
			outputs[XNOR_OUT_OUTPUT].setVoltage(0);
			lights[CIRCUIT_XOR_LED_LIGHT].setBrightness(1);
		} else {
			outputs[XNOR_OUT_OUTPUT].setVoltage(audio_in);  // XNOR
			outputs[XOR_OUT_OUTPUT].setVoltage(0);
			lights[CIRCUIT_XOR_LED_LIGHT].setBrightness(0);
		}

	}
};


struct L_CarrierWidget : ModuleWidget {
	L_CarrierWidget(L_Carrier* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/L-Carrier.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.228, 39.30)), module, L_Carrier::A_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.968, 39.30)), module, L_Carrier::B_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.457, 62.92)), module, L_Carrier::AUDIO_IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.736, 87.5)), module, L_Carrier::AND_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.458, 87.5)), module, L_Carrier::OR_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.112, 87.5)), module, L_Carrier::XOR_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.878, 105.0)), module, L_Carrier::NAND_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.533, 105.0)), module, L_Carrier::NOT_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.126, 105.0)), module, L_Carrier::XNOR_OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(8.43, 15.41)), module, L_Carrier::CIRCUIT_A_LED_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(53.0, 22.59)), module, L_Carrier::CIRCUIT_XOR_LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.43, 30.29)), module, L_Carrier::CIRCUIT_B_LED_LIGHT));
		addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(24.41, 45.7)), module, L_Carrier::A_LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(43.16, 45.7)), module, L_Carrier::B_LED_LIGHT));
	}
};


Model* modelL_Carrier = createModel<L_Carrier, L_CarrierWidget>("L-Carrier");