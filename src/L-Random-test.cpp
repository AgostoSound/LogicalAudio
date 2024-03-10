#include "plugin.hpp"
#include "random.hpp"
#include <random>
#include <chrono>

struct L_Random_test : Module {
    enum ParamId {
        SPREAD_PARAM,
        FREQ_PARAM,
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
        configParam(SPREAD_PARAM, 1.f, 9.f, 1.f, "SPREAD");
        configParam(FREQ_PARAM, 1.f, 9.f, 1.f, "FREQ");
        configOutput(OR_OUT_OUTPUT, "RANDOM");
    }

    // Función para verificar si ha pasado un segundo desde la última actualización
    bool shouldUpdate(int ms) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime);
        return duration.count() >= ms; // 1000 ms = 1 segundo
    }

    // Main logic.
    void process(const ProcessArgs& args) override {
        
        // Spread max and min controller.
        float spread = params[SPREAD_PARAM].getValue();

        // Update frequency controller.
        float freq = params[FREQ_PARAM].getValue();  
        ms = freq * 100;
        //ms = 300;  // TEST.
        
        // If an update is necessary.
        if (shouldUpdate(ms)) {  
            std::uniform_real_distribution<float> uniform_top((spread-1.0f), (spread+1.0f));  // Top range.
            std::uniform_real_distribution<float> uniform_bottom(-(spread+1.0f), -(spread-1.0f));  // Bottom Range.
            std::uniform_real_distribution<float> choice(0.0f, 2.0f);  // Range choicing.
            float selected = choice(rng);  // Random from 0 to 2.

            float r;  // Choice range.
            if (selected >= 1) {
                r = uniform_top(rng);
            } else {
                r = uniform_bottom(rng);
            }
            
            outputs[OR_OUT_OUTPUT].setVoltage(r);  // Set voltage.
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
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.614, 25.036)), module, L_Random_test::FREQ_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.458, 77.602)), module, L_Random_test::OR_OUT_OUTPUT));
    }
};

Model* modelL_Random_test = createModel<L_Random_test, L_Random_testWidget>("L-Random-test");
