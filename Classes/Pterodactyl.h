#ifndef _PTERODACTYL_HPP_
#define _PTERODACTYL_HPP_

#include "cocos2d.h"
#include <array>
#include <stdlib.h>

USING_NS_CC;

class Pterodactyl : public cocos2d::Sprite 
{
public:
	Pterodactyl() {};
	~Pterodactyl() {};
	static Pterodactyl* create(bool vis = true);
	void initialize(bool vis = true);

	void run(int x, int y);
	void on() { running = true; }
	void off() { running = false; }
	bool isOn() { return running; }
	void attack() { hostile = true; cap = 6; }
	void peace() { hostile = false; cap = 3; }
	bool isHostile() { return hostile; }
	void setAnim(std::string state) { animstate = state; }
	std::string getAnim() { return animstate; }


private:
	int posx;
	int posy;
	float Xvelocity = 0;
	float Yvelocity = 0;
	float acceleration = .2f;
	std::string animstate = "";
	bool running = false;
	bool hostile = false;
	int cap = 3;
};

#endif //_PTERODACTYL_HPP_