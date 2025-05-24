#pragma once
#include <rack.hpp>

using namespace rack;

// Declare the Plugin, defined in plugin.cpp
extern Plugin* pluginInstance;

// Declare each Model, defined in each module source file
extern Model* modelL_Carrier;
extern Model* modelL_Rantics;
extern Model* modelL_Random;
extern Model* modelL_Panel;
extern Model* modelL_Noises;
