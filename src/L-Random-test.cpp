#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>

struct L_Random_test : Module {
    enum ParamId {
        SPREAD_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        INPUTS_LEN
    };
    enum OutputId {
        OR_OUT_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    // Declaración del generador de números pseudoaleatorios como miembro de la clase
    random::Xoroshiro128Plus rng;
    std::chrono::steady_clock::time_point lastUpdateTime;
    int ms; // Declaración de la variable ms

    L_Random_test() : rng() {
        uint64_t seed0 = std::random_device{}();
        uint64_t seed1 = std::random_device{}();
        rng.seed(seed0, seed1);
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(SPREAD_PARAM, 1.f, 9.f, 1.f, "");
        configOutput(OR_OUT_OUTPUT, "");
    }

    // Función para verificar si ha pasado un segundo desde la última actualización
    bool shouldUpdate(int ms) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }

    // Main logic.
    void process(const ProcessArgs& args) override {
        
        // Controla voltage maximo y minimo.
        float spread = params[SPREAD_PARAM].getValue();  // Compute the frequency from the pitch parameter.

        // Controla frecuencia de actualizacion, mejorar.
        // float spread = params[SPREAD_PARAM].getValue();  // Compute the frequency from the pitch parameter.
        //ms = spread * 100;
        ms = 300;  // TEST.
        if (shouldUpdate(ms)) {
            // Triangle distribution.
            //float r = 2.0f * (std::sqrt(uniform(rng)) - std::sqrt(1.0f - uniform(rng)));

            std::uniform_real_distribution<float> uniform_top((spread-1.0f), (spread+1.0f));
            std::uniform_real_distribution<float> uniform_bottom(-(spread+1.0f), -(spread-1.0f));   
            std::uniform_real_distribution<float> choice(0.0f, 2.0f);

            float selected = choice(rng); // Genera un número aleatorio que será 0 o 1

            float r;
            if (selected >= 1) {
                // Utiliza la distribución uniforme top
                r = uniform_top(rng);
            } else {
                // Utiliza la distribución uniforme bottom
                r = uniform_bottom(rng);
            }

            //float scaled_r = r * spread; // Escalar el valor a tu intervalo deseado
            
            outputs[OR_OUT_OUTPUT].setVoltage(r);
            lastUpdateTime = std::chrono::steady_clock::now();
        }
    }
};

struct L_Random_testWidget : ModuleWidget {
    L_Random_testWidget(L_Random_test* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/L-Random-test.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.614, 25.036)), module, L_Random_test::SPREAD_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.458, 77.602)), module, L_Random_test::OR_OUT_OUTPUT));
    }
};

Model* modelL_Random_test = createModel<L_Random_test, L_Random_testWidget>("L-Random-test");
