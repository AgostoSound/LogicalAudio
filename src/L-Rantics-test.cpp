#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>


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
		LIGHTS_LEN
	};

	random::Xoroshiro128Plus rng;  // Pseudorandom number generator instance.
    std::chrono::steady_clock::time_point lastUpdateTime;  // Clock generator instance.
	int ms1;

	L_Rantics_test() {
		// Random states.
		uint64_t seed0 = std::random_device{}();
        uint64_t seed1 = std::random_device{}();
        rng.seed(seed0, seed1);

		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BEAT_FRAC_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CLOCK_INPUT, "Clock in");
		configInput(BPM_INPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT1_OUTPUT, "");
	}

	bool lastClockTrigger = false; // Estado del reloj en el ciclo anterior
    float lastVoltage = 0.0f; // Último voltaje generado

    float minVoltage = -5.0f;
    float maxVoltage = 5.0f;

	void process(const ProcessArgs& args) override {
		// Leer el estado del reloj
		bool clockTrigger = inputs[CLOCK_INPUT].getVoltage() >= 1.0f;

		// Si se detecta un cambio en la señal del reloj
		if (clockTrigger != lastClockTrigger) {
			// Si hay un nuevo tic del reloj
			if (clockTrigger) {
				// Generar un voltaje aleatorio entre -5V y +5V
				std::uniform_real_distribution<float> distribution(minVoltage, maxVoltage);
				float randomVoltage = distribution(rng);

				// Establecer el voltaje aleatorio en todas las salidas
				outputs[OUT1_OUTPUT].setVoltage(randomVoltage);

				// Guardar el último voltaje generado
				lastVoltage = randomVoltage;
			} else {
				// Si el reloj deja de tener tic, mantener el último voltaje generado
				outputs[OUT1_OUTPUT].setVoltage(lastVoltage);
			}
		}

		// Actualizar el estado del reloj
		lastClockTrigger = clockTrigger;
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

	}
};


Model* modelL_Rantics_test = createModel<L_Rantics_test, L_Rantics_testWidget>("L-Rantics-test");