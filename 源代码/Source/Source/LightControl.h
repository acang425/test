#pragma once
namespace MyGame {
	class LightControl
	{
	public:
		DirectionalLight myLight;

		LightControl();

		void highLight();
		void lowLight();
	};
}

