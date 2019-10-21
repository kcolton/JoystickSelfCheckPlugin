#pragma once
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"


class JoystickSelfCheckPlugin : public BakkesMod::Plugin::BakkesModPlugin {
private:
	std::vector<ControllerInput> inputHistory;
	shared_ptr<int> joystickVizSize;

public:
	void onLoad() override;
	void onUnload() override;

	void OnSetInput(CarWrapper cw, void * params);
	void Render(CanvasWrapper canvas);
};
