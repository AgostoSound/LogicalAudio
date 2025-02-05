#include "plugin.hpp"


struct L_Rantics : Module {
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

	L_Rantics() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configParam(L_SPREAD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(R_SPREAD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(POLARITY_PARAM, 0.f, 1.f, 0.f, "Polarity");
		configParam(TIC_SELECTOR_PARAM, 0.f, 1.f, 0.f, "Tic Selector");

		configInput(TIC1_INPUT, "");
		configInput(TIC2_INPUT, "");
		configInput(L_CV_INPUT, "");
		configInput(R_CV_INPUT, "");

		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT2_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {

	}
};


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
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.466, 87.3)), module, L_Rantics::L_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.258, 87.3)), module, L_Rantics::R_CV_INPUT));

		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(13.273, 106.29)), module, L_Rantics::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ3410Port>(mm2px(Vec(48.383, 106.29)), module, L_Rantics::OUT2_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.742, 41.372)), module, L_Rantics::UNIPOLAR_LED_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.267, 41.372)), module, L_Rantics::BIPOLAR_LED_LIGHT));
		
	}
};


Model* modelL_Rantics = createModel<L_Rantics, L_RanticsWidget>("L-Rantics");