#include "MyGame.h"
using namespace MyGame;
LightControl::LightControl(){}

void LightControl::highLight() {
	myLight.mSpecularIntensity = 1.0;
	myLight.mDiffuseIntensity = 0.5;
}
void LightControl::lowLight() {
	myLight.mSpecularIntensity = 0.3;
	myLight.mDiffuseIntensity = 0.2;
}