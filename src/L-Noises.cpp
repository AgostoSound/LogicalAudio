#include "plugin.hpp"


struct L_Noises : Module {
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

	L_Noises() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(NOISER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(NOISEL_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GATEL_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GATER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(VOLL_PARAM, 0.f, 1.f, 0.f, "");
		configParam(VOLR_PARAM, 0.f, 1.f, 0.f, "");
		configInput(LOGICA_INPUT, "");
		configInput(LOGICB_INPUT, "");
		configOutput(OUTL_OUTPUT, "");
		configOutput(OUTR_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.5, 38.359)), module, L_Noises::NOISER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.687, 38.858)), module, L_Noises::NOISEL_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.68, 58.395)), module, L_Noises::GATEL_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.522, 58.555)), module, L_Noises::GATER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.689, 75.798)), module, L_Noises::VOLL_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.352, 75.687)), module, L_Noises::VOLR_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.345, 14.097)), module, L_Noises::LOGICA_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.032, 14.289)), module, L_Noises::LOGICB_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.273, 95.218)), module, L_Noises::OUTL_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.323, 95.203)), module, L_Noises::OUTR_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.167, 19.522)), module, L_Noises::LEDA_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(50.315, 19.308)), module, L_Noises::LEDB_LIGHT));
	}
};


Model* modelL_Noises = createModel<L_Noises, L_NoisesWidget>("L-Noises");