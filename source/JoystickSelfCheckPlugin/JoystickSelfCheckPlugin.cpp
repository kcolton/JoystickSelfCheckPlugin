#include "JoystickSelfCheckPlugin.h"

BAKKESMOD_PLUGIN(JoystickSelfCheckPlugin, "Joystick Self-Check", "0.2", PLUGINTYPE_FREEPLAY)

#define INPUT_HISTORY_LENGTH 120

#define CVAR_JOYSTICK_VIZ_ENABLED "joystick_viz_enabled"
#define CVAR_JOYSTICK_VIZ_SIZE "joystick_viz_size"

void JoystickSelfCheckPlugin::onLoad()
{
	joystickVizSize = make_shared<int>(400);

	inputHistory.reserve(INPUT_HISTORY_LENGTH);

	cvarManager->log("JoystickSelfCheckPlugin loaded");

	gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput", std::bind(&JoystickSelfCheckPlugin::OnSetInput, this, std::placeholders::_1, std::placeholders::_2));

	gameWrapper->RegisterDrawable(std::bind(&JoystickSelfCheckPlugin::Render, this, std::placeholders::_1));

	cvarManager->registerCvar(CVAR_JOYSTICK_VIZ_ENABLED, "0", "Show Joystick Self-Check Visualization", true, true, 0.f, true, 1.f);
	cvarManager->registerCvar(CVAR_JOYSTICK_VIZ_SIZE, "400", "Joystick Visualization Size", true, true, 100, true, 1000).bindTo(joystickVizSize);
}

void JoystickSelfCheckPlugin::onUnload()
{
	cvarManager->log("JoystickSelfCheckPlugin unloaded");
	gameWrapper->UnregisterDrawables();
}

void JoystickSelfCheckPlugin::OnSetInput(CarWrapper cw, void * params)
{
	if (!gameWrapper->IsInFreeplay()) {
		return;
	}

	ControllerInput* ci = (ControllerInput*)params;

	if (cvarManager->getCvar(CVAR_JOYSTICK_VIZ_ENABLED).getBoolValue()) {
		if (inputHistory.size() >= INPUT_HISTORY_LENGTH) {
			int numToRemove = 1 + inputHistory.size() - INPUT_HISTORY_LENGTH;
			inputHistory.erase(inputHistory.begin(), inputHistory.begin() + numToRemove);
		}

		inputHistory.push_back(*ci);
	}
}

void JoystickSelfCheckPlugin::Render(CanvasWrapper canvas)
{
	if (!gameWrapper->IsInFreeplay() || !cvarManager->getCvar(CVAR_JOYSTICK_VIZ_ENABLED).getBoolValue()) {
		return;
	}

	Vector2 canvasSize = canvas.GetSize();
	Vector2 canvasCenter = { canvasSize.X / 2, canvasSize.Y / 2 };

	canvas.SetColor(255, 255, 255, 100);
	canvas.SetPosition(Vector2({ canvasCenter.X - *joystickVizSize / 2, canvasCenter.Y - *joystickVizSize / 2 }));
	canvas.DrawBox(Vector2({ *joystickVizSize, *joystickVizSize }));

	int i = 0;
	Vector2 prevPos;

	for (auto it = inputHistory.end(); it != inputHistory.begin(); --it)
	{
		ControllerInput input = *it;

		Vector2 currentPos = { canvasCenter.X + (int)(*joystickVizSize / 2 * input.Steer) - 3, canvasCenter.Y + (int)(*joystickVizSize / 2 * input.Pitch) - 3 };

		canvas.SetColor(255, 255, 255, 255 - i * 2);
		canvas.SetPosition(currentPos);
		canvas.FillBox(Vector2({ 5, 5 }));

		if (i > 0) {
			ControllerInput prevInput = inputHistory[inputHistory.size() - i - 1];
			Vector2 prevPos = { canvasCenter.X + (int)(*joystickVizSize / 2 * prevInput.Steer), canvasCenter.Y + (int)(*joystickVizSize / 2 * prevInput.Pitch) };

			canvas.DrawLine({ canvasCenter.X + (int)(*joystickVizSize / 2 * input.Steer), canvasCenter.Y + (int)(*joystickVizSize / 2 * input.Pitch) }, prevPos);
		}

		prevPos = currentPos;
		i++;
	}
}
