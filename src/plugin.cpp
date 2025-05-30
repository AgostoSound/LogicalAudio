#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	p->addModel(modelL_Carrier);
	p->addModel(modelL_Rantics);
	p->addModel(modelL_Random);
	p->addModel(modelL_Panel);
	p->addModel(modelL_Noises);


	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
