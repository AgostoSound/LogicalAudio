#include "plugin.hpp"

// Smoother to avoid clicks in gates.
struct SmoothGate {
	float gain = 0.0f;
	bool isOpen = false;
	int counter = 0;
	static const int attackSamples = 12;

	float process(bool gateOpen, float input) {
		if (gateOpen != isOpen) {
			isOpen = gateOpen;
			counter = 0;
		}

		if (counter < attackSamples) {
			counter++;
			float t = (float)counter / attackSamples;
			gain = isOpen ? t : 1.0f - t;
		} else {
			gain = isOpen ? 1.0f : 0.0f;
		}

		return gain * input;
	}
};


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

	SmoothGate andGate, nandGate, orGate, notGate, xorGate, xnorGate;

	// Initial configuration.
	L_Carrier() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(A_IN_INPUT, "A");
		configInput(B_IN_INPUT, "B");
		configInput(AUDIO_IN_INPUT, "Audio");
		configOutput(AND_OUT_OUTPUT, "And");
		configOutput(OR_OUT_OUTPUT, "Or");
		configOutput(XOR_OUT_OUTPUT, "Xor");
		configOutput(NAND_OUT_OUTPUT, "Nand");
		configOutput(NOT_OUT_OUTPUT, "Nor");
		configOutput(XNOR_OUT_OUTPUT, "Xnor");
	}

	// Main logic.
	void process(const ProcessArgs& args) override {

		// Reading inputs.
		float audio_in = inputs[AUDIO_IN_INPUT].getVoltage();
		float a_in = inputs[A_IN_INPUT].getVoltage();
		float b_in = inputs[B_IN_INPUT].getVoltage();

		bool aHigh = (a_in == 10.0f);
		bool bHigh = (b_in == 10.0f);
		
		// LEDs
		lights[A_LED_LIGHT].setBrightness(aHigh ? 1.f : 0.f);
		lights[CIRCUIT_A_LED_LIGHT].setBrightness(aHigh ? 1.f : 0.f);
		lights[B_LED_LIGHT].setBrightness(bHigh ? 1.f : 0.f);
		lights[CIRCUIT_B_LED_LIGHT].setBrightness(bHigh ? 1.f : 0.f);

		// Smooth outputs.
		bool andResult = aHigh && bHigh;
		bool nandResult = !andResult;
		bool orResult = aHigh || bHigh;
		bool norResult = !orResult;
		bool xorResult = aHigh ^ bHigh;
		bool xnorResult = !xorResult;

		outputs[AND_OUT_OUTPUT].setVoltage(andGate.process(andResult, audio_in));
		outputs[NAND_OUT_OUTPUT].setVoltage(nandGate.process(nandResult, audio_in));
		outputs[OR_OUT_OUTPUT].setVoltage(orGate.process(orResult, audio_in));
		outputs[NOT_OUT_OUTPUT].setVoltage(notGate.process(norResult, audio_in));
		outputs[XOR_OUT_OUTPUT].setVoltage(xorGate.process(xorResult, audio_in));
		outputs[XNOR_OUT_OUTPUT].setVoltage(xnorGate.process(xnorResult, audio_in));

		// LED XOR
		lights[CIRCUIT_XOR_LED_LIGHT].setBrightness(xorResult ? 1.f : 0.f);

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