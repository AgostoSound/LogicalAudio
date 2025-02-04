#include "plugin.hpp"


struct L_Rantics : Module {
	enum ParamId {
		L_SPREAD_PARAM,
		R_SPREAD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SELECT_INPUT,
		CLOCK_INPUT,
		BPM_INPUT,
		BEAT_FRAC_INPUT,
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
		LIGHTS_LEN
	};

	L_Rantics() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(L_SPREAD_PARAM, 0.f, 1.f, 0.f, "");
		configParam(R_SPREAD_PARAM, 0.f, 1.f, 0.f, "");
		configInput(SELECT_INPUT, "");
		configInput(CLOCK_INPUT, "");
		configInput(BPM_INPUT, "");
		configInput(BEAT_FRAC_INPUT, "");
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.571, 73)), module, L_Rantics::L_SPREAD_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(48.496, 73)), module, L_Rantics::R_SPREAD_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.531, 35)), module, L_Rantics::SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.471, 35)), module, L_Rantics::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.391, 35)), module, L_Rantics::BPM_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.504, 52)), module, L_Rantics::BEAT_FRAC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.466, 87)), module, L_Rantics::L_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.258, 87)), module, L_Rantics::R_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.273, 106)), module, L_Rantics::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.383, 106)), module, L_Rantics::OUT2_OUTPUT));
	}
};


Model* modelL_Rantics = createModel<L_Rantics, L_RanticsWidget>("L-Rantics");