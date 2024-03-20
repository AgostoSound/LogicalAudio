#include "plugin.hpp"
#include <chrono>


struct L_Rantics_test : Module {
	enum ParamId {
		BEAT_FRAC_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLOCK_INPUT,
		BPM_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT2_OUTPUT,
		OUT1_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LED2_LIGHT,
		LED1_LIGHT,
		LIGHTS_LEN
	};

	L_Rantics_test() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BEAT_FRAC_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CLOCK_INPUT, "");
		configInput(BPM_INPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT1_OUTPUT, "");
	}

	float phase = 0.f;
	float blinkPhase = 0.f;

	void process(const ProcessArgs& args) override {
		blinkPhase += args.sampleTime;
		if (blinkPhase >= 1.f)
			blinkPhase -= 1.f;
		lights[LED1_LIGHT].setBrightness(blinkPhase < 0.5f ? 1.f : 0.f);
		lights[LED2_LIGHT].setBrightness(blinkPhase < 0.5f ? 1.f : 0.f);
		
	}
};


struct L_Rantics_testWidget : ModuleWidget {
	L_Rantics_testWidget(L_Rantics_test* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/L-Rantics-test.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.239, 39.255)), module, L_Rantics_test::BEAT_FRAC_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.117, 24.843)), module, L_Rantics_test::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.613, 24.843)), module, L_Rantics_test::BPM_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.69, 88.162)), module, L_Rantics_test::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.966, 88.676)), module, L_Rantics_test::OUT1_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(34.60, 113.60)), module, L_Rantics_test::LED2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(27.10, 113.60)), module, L_Rantics_test::LED1_LIGHT));
	}
};


Model* modelL_Rantics_test = createModel<L_Rantics_test, L_Rantics_testWidget>("L-Rantics-test");