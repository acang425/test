#include<iostream>
#include "MyConsoleEngine.h"
#include"test.h"
#include"MyGame.h"
using namespace MyGame;
int main() 
{

/*

	
	test q;
	q.init();

*/

	std::string s = "gameover\n";
	BaseLogic maingame;
	//maingame.gameBegin();
	maingame.wholeGame();
	maingame.testgame();

	system("pause");
	return 0;
}
