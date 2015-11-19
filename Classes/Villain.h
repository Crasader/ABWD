#ifndef _VILLAIN_HPP_
#define _VILLAIN_HPP_

#include "cocos2d.h"
#include "Player.h"

class Villain : public cocos2d::Sprite
{

public:
	Villain(){};
	~Villain(){};

	
	static Villain* create();
	//static Villain* create(int playernum);
	//gameloop villain function
	void runAI(std::vector<Player*>* players);
	//update villain knowledge
	void calculations();
	//determines which behavior the ai should use
	void chooseBehavior();
	//walk to target
	void walk();
	//charge at target
	void charge();
	//charge charge
	void chargeCharge();
	//the index of the ai's current target
	int getTarget();
	//the current action type the villain uses, use to determine animations and whatnot
	//number key located at the end of file
	int getBehavior();

private:


	int target = 0;
	int behavior = 0;
	int behavior_timer = 0;
	int walk_speed = 1;
	int charge_speed = 4;
	std::vector<Player*>* player_list;
	std::vector<int> distance;
	
	bool behavior_unlocked = true;

};
/*
behavior code
code 0 - ai will walk towards the specified target parameter with no special actions
code 1 - ai does a charge sequence
*/

#endif // _VILLAIN_HPP_
