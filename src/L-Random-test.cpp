#include "plugin.hpp"
#include "random.hpp"
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

    L_Random_test() : rng() {
        uint64_t seed0 = std::random_device{}();
        uint64_t seed1 = std::random_device{}();
        rng.seed(seed0, seed1);
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(SPREAD_PARAM, 0.f, 1.f, 0.f, "");
        configOutput(OR_OUT_OUTPUT, "");
    }

    // Función para verificar si ha pasado un segundo desde la última actualización
    bool shouldUpdate() {
        float pitch = params[FRECUENCY_PARAM].getValue();  // Compute the frequency from the pitch parameter.

        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime);
        return duration.count() >= 1000; // 1000 ms = 1 segundo
    }

    // Main logic.
    void process(const ProcessArgs& args) override {
        if (shouldUpdate()) {

            float pitch = params[FRECUENCY_PARAM].getValue();  // Compute the frequency from the pitch parameter.

            std::uniform_real_distribution<float> uniform(-5.f, 5.f);
            float r = uniform( rng);
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
