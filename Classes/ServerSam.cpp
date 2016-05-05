
#include "ServerSam.h"
#include <math.h>
#include <stdlib.h>
#include "cocos2d.h"
#include <iostream>


ServerSam* ServerSam::create(ServerDemo* ptr, bool vis)
{
	ServerSam* pSprite = new ServerSam();
	pSprite->serverptr = ptr;

	if (pSprite->initWithFile("\\res\\sprites\\animations\\sam\\sam_walk_front0.png"))
	{
		pSprite->autorelease();

		pSprite->initialize(vis);

		return pSprite;
	}

	CC_SAFE_DELETE(pSprite);
	return NULL;
}

void ServerSam::initialize(bool vis)
{
	this->getTexture()->setAliasTexParameters();
	this->setPosition(Vec2(-250, -150));
	this->setAnchorPoint(Vec2(0.5, 0.0));
	this->setVisible(vis);
}


void ServerSam::setPriority(std::vector<std::vector<char>> tiles, std::vector<std::vector<char>> dry, int time) {
	for (int i = 0; i < 4; i++) {
		priority[i] = 0;
		idle = true;
	}
	for (unsigned int i = 0; i < tiles.size(); i++) {
		for (unsigned int j = 0; j < tiles[i].size(); j++ ) {
			if (tiles[i][j] == 1 && dry[i][j] != time) {
				priority[0]++;
				idle = false;
			}
			else if (tiles[i][j] == 2 && dry[i][j] != time) {
				priority[1]++;
				idle = false;
			}
			else if (tiles[i][j] == 3 && dry[i][j] != time) {
				priority[2]++;
				idle = false;
			}
			else if (tiles[i][j] == 4 && dry[i][j] != time) {
				priority[3]++;
				idle = false;
			}
		}
	}
}

void ServerSam::runAI(std::vector<Player*>* players)
{

	player_list = players;
	calculations();
	teleport_cd--;
	candy->run();
	if (candy->getTime() == 1) {
		candyOn();
	}
	if(candy->getTime() == 1)
	{
		serverptr->enqueueMessage(ServerMessage(9, 0, 0, candy->getOwner()+1));
	}
	if (!(candy->active())) {
		candy->setOwner(-1);
	}
	if (behavior_unlocked) {
		chooseBehavior();
	}
	switch (behavior) {
	case 0:
		walk();
		break;
	case 1:
		pteraSummon();
		break;
	case 2:
		charge();
		break;
	case 3:
		chargeTeleport();
		break;
	case 4:
		teleport();
		break;
	case 5:
		wait();
		break;
	case 6:
		munch();
		break;
	case 7:
		munching();
		break;
	}
	if (behavior != 1 && b_ptera) {
		ptera->run(this->getPositionX(), this->getPositionY());
	}
}

void ServerSam::calculations() {
	distance.erase(distance.begin(), distance.end());
	int ServerSamX = this->getPositionX();
	int ServerSamY = this->getPositionY();
	for (unsigned int i = 0; i < player_list->size(); i++) {
		int temp = sqrt(pow((ServerSamX - player_list->at(i)->getPositionX()), 2) + pow((ServerSamY - player_list->at(i)->getPositionY()), 2));
		distance.push_back(temp);
	}
}


void ServerSam::chooseBehavior() {
	int choose;
	if (idle) {
		choose = 0;
	}
	else {
		choose = behaviors.at((rand() % behaviors.size()));
	}
	switch (choose) {
		//wait
	case 0:
		behavior_timer = idle_time;
		behavior = 5;
		setAnim("samdown");
		break;
		//walk
	case 1:
		behavior_timer = walk_time;
		behavior = 0;
		break;
		//ptera
	case 2:
		behavior_timer = 400;
		secondary_time = 30;
		behavior = 1;
		break;
		//teleport
	case 3:
		if (teleport_cd <= 0) {
			int total = priority[0] + priority[1] + priority[2] + priority[3];
			int guess = rand() % (total + 1);
			int guess1 = rand() % (total - priority[0] + 1);
			int guess2 = rand() % (total - priority[0] - priority[1] + 1);
			int guess3 = rand() % (total - priority[0] - priority[1] - priority[2] + 1);

			if (guess < priority[0]) {
				target = 0;
				x = player_list->at(target)->getPositionX();
				y = player_list->at(target)->getPositionY();
				behavior_timer = charge_teleport_time;
				behavior = 3;
			}
			else if (guess1 < priority[1]) {
				target = 1;
				x = player_list->at(target)->getPositionX();
				y = player_list->at(target)->getPositionY();
				behavior_timer = charge_teleport_time;
				behavior = 3;
			}
			else if (guess2 < priority[2]) {
				target = 2;
				x = player_list->at(target)->getPositionX();
				y = player_list->at(target)->getPositionY();
				behavior_timer = charge_teleport_time;
				behavior = 3;
			}
			else if (guess3 < priority[3]) {
				target = 3;
				x = player_list->at(target)->getPositionX();
				y = player_list->at(target)->getPositionY();
				behavior_timer = charge_teleport_time;
				behavior = 3;
			}
			else {
				behavior_timer = teleport_fail_timer;
				behavior = 0;
			}
		}
		break;
		//candy
	case 4:
		behavior = 6;
		behavior_timer = 120;
		secondary_time = 30;
		break;
	}
	behavior_unlocked = false;
}


void ServerSam::walk() {
	if (timeCheck()) {
		behavior_timer--;
		int temp = -1;
		bool anything = false;
		for (unsigned int i = 0; i < distance.size(); i++) {
			int temp1 = distance[i];
			if (temp == -1 && priority[i] > 0) { 
				temp = temp1; 
				target = i; 
				anything = true;
			}
			else if (temp > temp1 && priority[i] > 0) { 
				temp = temp1; 
				target = i; 
			}
		}
		if (anything) {
			double playerx, playery, samx, samy;
			playerx = player_list->at(target)->getPositionX();
			playery = player_list->at(target)->getPositionY();
			samx = this->getPositionX();
			samy = this->getPositionY();
			double theta;
			if (playerx >= samx) {
				theta = atan((playery - samy) / (playerx - samx)) * 180 / 3.14159;
			}
			else if (playery > samy) {
				theta = 180 + (atan((playery - samy) / (playerx - samx)) * 180 / 3.14159);
			}
			else {
				theta = -180 + atan((playery - samy) / (playerx - samx)) * 180 / 3.14159;
			}
			if (priority[target] > 0 && distance[target] > 2) {

				this->setPositionX(this->getPositionX() + walk_speed*(cos(theta * 3.14159 / 180)));
				this->setPositionY(this->getPositionY() + walk_speed*(sin(theta * 3.14159 / 180)));

				if (theta > 45 && theta < 135) {
					setAnim("samup");
				}
				else if (theta >= 135 || theta <= -135) {
					setAnim("samleft");
				}
				else if (theta <= 45 && theta >= -45) {
					setAnim("samright");
				}
				else if (theta < -45 && theta > -135) {
					setAnim("samdown");
				}

			}
			else {
				behavior_timer = 0;
				behavior = 0;
				walk_speed = 2;
				setAnim("samdown");
			}
		}
		else {
			behavior_timer = 0;
			behavior = 0;
		}
	}
	else {
		behavior_unlocked = true;
		walk_speed = 2;
	}
}


void ServerSam::charge() {
	if (timeCheck()) {
		behavior_timer--;
	}
	else {
		behavior_unlocked = true;
	}
}


void ServerSam::chargeCharge() {
	if (timeCheck()) {
		behavior_timer--;
	}
	else {
		behavior = 2;
		behavior_timer = 300;
	}
}


void ServerSam::teleport() {
	if (timeCheck()) {
		
		behavior_timer--;
	}
	else {
		
		behavior = 0;
		behavior_timer = 150;
		teleport_cd = 150;
	}
}

void ServerSam::chargeTeleport() {
	if (timeCheck()) {
		setAnim("samwarp");
		behavior_timer--;
	}
	else {
		setAnim("samappear");
		this->setPositionX(x);
		this->setPositionY(y);
		behavior_timer = teleport_arrival_time;
		behavior = 4;
	}
}

void ServerSam::wait() {
	if (timeCheck()) {
		behavior_timer--;
	}
	else {
		behavior_timer = 0;
		behavior_unlocked = true;
	}
}

void ServerSam::pteraSummon() {
	if (timeCheck()) {
		animstate = "samwhistle";
		if (secondary_time <= 0) {
			ptera->attack();
		}
		else {
			secondary_time--;
		}
		int temp = -1;
		for (unsigned int i = 0; i < distance.size(); i++) {
			int temp1 = distance[i];
			if (temp < temp1 && priority[i] > 0) { temp = temp1; target = i; }
		}
		ptera->run(player_list->at(target)->getPositionX(), player_list->at(target)->getPositionY());
		behavior_timer--;
	}
	else {
		behavior = 0;
		behavior_timer = 300;
		ptera->peace();
	}
}
//munchFunction
void ServerSam::munch() {
	bool resume = true;
	if (flag) {
		
		int theta = (rand() % 360);
		int newX = abs((this->getPositionX() - candy_spawn_distance*(cos(theta * 3.14159 / 180))));
		int newY = abs(this->getPositionY() - candy_spawn_distance*(sin(theta * 3.14159 / 180)));

		//make sure new coords are within map boundaries
		auto testx = ((lvm->levelmap->getMapSize().width - 1) * lvm->levelmap->getTileSize().width);
		auto testy = (lvm->levelmap->getMapSize().height - 1) * lvm->levelmap->getTileSize().height;
		if (newX > testx || newY > testy)
		{
			resume = false;
		}
		else
		{
			//int tileX = abs((lvm->levelmap->getMapSize().width * lvm->levelmap->getTileSize().width) - newX - 1) / (lvm->levelmap->getTileSize().width); 
			int tileX = newX / lvm->levelmap->getTileSize().width;
			int tileY = ((lvm->levelmap->getMapSize().height * lvm->levelmap->getTileSize().height) - newY) / (lvm->levelmap->getTileSize().height);
			Vec2 tileCoord = Vec2(tileX, tileY);

			if (blockage != NULL)
			{
				int bkTile = blockage->getTileGIDAt(tileCoord);
				if (bkTile)
				{
					auto tilemapvals = lvm->levelmap->getPropertiesForGID(bkTile).asValueMap();
					if (!tilemapvals.empty())
					{
						auto w = tilemapvals["Collidable"].asString();

						if ("true" == w) {
							resume = false;
						}
					}
				}
			}
			else 
			{
				resume = false;
			}
		}
		if (resume) {
			flag = false;
			candy->setPosition(newX, newY);
			candy->setStatus(true);

			//If sam is closer to candy cane, make her wait long enough for closest player to catch up:

			//get sam's distance from candy; d=sqrt((x_2 - x_1)^2 + (y_2 - y_1)^2)
			sam_candy_dist = (int)(sqrt((int)(pow((newX - this->getPositionX()), 2)) + (int)(pow((newY - this->getPositionY()), 2))));
			//find player closest to candy cane
			for (unsigned int i = 0; i < player_list->size(); i++) 
			{
				if (player_list->at(i)->isVisible())
				{
					player_dist_check = sqrt(pow((newX - player_list->at(i)->getPositionX()), 2) + pow((newY - player_list->at(i)->getPositionY()), 2));
					//if player i is closer to candy than anyone checked previously, 
					//set as closest player distance to candy
					if (player_dist_check < player_candy_dist)
					{
						player_candy_dist = player_dist_check;
					}
				}
			}
			//if sam is closer to candy than any player, make sam wait
			//to pursue candy for the amount of time it would take the
			//closest player to catch up
            if (sam_candy_dist < player_candy_dist) 
			{
				//candy_wait = (how much closer sam is to candy than player) / how many pixels a player can move per frame
				candy_wait = ((player_candy_dist - sam_candy_dist) / 2) + reaction_time_weight;
			}
			else
			{
				//below is a multipler that makes sam's speed fast enough to get to candy at the same time as player
				walk_speed *= ((player_list->at(0)->getSpeed() * sam_candy_dist) / (player_candy_dist * walk_speed));
				candy_wait = reaction_time_weight;
			}
			}
	}
	if (resume) {
		if (candy_wait <= 0)
		{
			double theta = 0;
			if (candy->getPositionX() >= this->getPositionX()) {
				theta = atan((candy->getPositionY() - this->getPositionY()) / (candy->getPositionX() - this->getPositionX())) * 180 / 3.14159;
			}
			else if (candy->getPositionX() > this->getPositionY()) {
				theta = 180 + (atan((candy->getPositionY() - this->getPositionY()) / (candy->getPositionX() - this->getPositionX())) * 180 / 3.14159);
			}
			else {
				theta = -180 + atan((candy->getPositionY() - this->getPositionY()) / (candy->getPositionX() - this->getPositionX())) * 180 / 3.14159;
			}

			this->setPositionX(this->getPositionX() + walk_speed*(cos(theta * 3.14159 / 180)));
			this->setPositionY(this->getPositionY() + walk_speed*(sin(theta * 3.14159 / 180)));

			if (theta > 45 && theta < 135) {
				setAnim("samup");
			}
			else if (theta >= 135 || theta <= -135) {
				setAnim("samleft");
			}
			else if (theta <= 45 && theta >= -45) {
				setAnim("samright");
			}
			else if (theta < -45 && theta > -135) {
				setAnim("samdown");
			}
		}
		else
		{
			candy_wait--;
		}
		for (unsigned int i = 0; i < player_list->size(); i++) {
			if (abs(player_list->at(i)->getPositionX() - candy->getPositionX()) < 10 && abs(player_list->at(i)->getPositionY() - candy->getPositionY()) < 10 && candy->notCollected()) {
				candy->setStatus(false);
				candy->setOwner(i);
				candy->start();
				candy->setPosition(-1000, -1000);
				flag = true;
				behavior_timer = 150;
				behavior = 5;
				pl_candy_score++;
				walk_speed = 2; //set sam speed back to normal
				player_candy_dist = 9999; //set this back to default value
				//check sam-player candy acquisition ratio + adjust reaction time weight
				//every 2 times the candy is picked up
				if (sam_candy_score + pl_candy_score % 2 == 0)
				{
					int check = sam_candy_score - pl_candy_score;
					//if sam is getting more candy than players, 
					//make her wait longer before running to candy
					if (check > 0) {
						incReactW();
					}
					//if sam is getting less candy, make her wait less
					else if (check < 0) {
						lowerReactW();
					}
				}
				serverptr->enqueueMessage(ServerMessage(8, 0, 0, i + 1));
			}
		}
		if (abs(this->getPositionX() - candy->getPositionX()) < 10 && abs(this->getPositionY() - candy->getPositionY()) < 10 && candy->notCollected()) {
			candy->setStatus(false);
			candy->setPosition(-1000, -1000);
			behavior = 7;
			behavior_timer = 150;
			flag = true;
			walk_speed = 4;
			sam_candy_score++;
			player_candy_dist = 9999; //set this back to default value
			//check sam-player candy acquisition ratio + adjust reaction time weight
			//every 2 times the candy is picked up
			if (sam_candy_score + pl_candy_score % 2 == 0)
			{
				int check = sam_candy_score - pl_candy_score;
				//if sam is getting more candy than players, 
				//make her wait longer before running to candy
				if (check > 0) {
					incReactW();
				}
				//if sam is getting less candy, make her wait less
				else if (check < 0) {
					lowerReactW();
				}
			}
		}
	}
	else {
		behavior_unlocked = true;
		flag = true;
	}
}

void ServerSam::munching() {
	if (timeCheck()) {
		behavior_timer--;
		setAnim("sammunch");
	}
	else {
		behavior_timer = sprint_timer;
		behavior = 0;
	}
}

int ServerSam::getTarget() {
	return target;
}

int ServerSam::getBehavior() {
	return behavior;
}

bool ServerSam::timeCheck() {
	return behavior_timer > 0;
}

void ServerSam::linkPtera(Pterodactyl* pterodactyl) {
	ptera = pterodactyl;
	ptera->on();
}

void ServerSam::linkCandy(Candy* candies) {
	candy = candies;
}

void ServerSam::walkOff() {
	if (b_walk) {
		b_walk = false;
		for (unsigned int i = 0; i < behaviors.size(); i++) {
			if (behaviors[i] == 1) {
				behaviors.erase(behaviors.begin() + i);
				break;
			}
		}
	}
}


void ServerSam::pteraOff() {
	if (b_ptera) {
		ptera->setVisible(false);
		b_ptera = false;
		for (unsigned int i = 0; i < behaviors.size() != 2; i++) {
			if (behaviors[i] == 2) {
				behaviors.erase(behaviors.begin() + i);
				break;
			}
		}
	}
}

void ServerSam::teleportOff() {
	if (b_teleport) {
		b_teleport = false;
		for (unsigned int i = 0; i < behaviors.size(); i++) {
			if (behaviors[i] == 3) {
				behaviors.erase(behaviors.begin() + i);
				break;
			}
		}
	}
}

void ServerSam::candyOff() {
	if (b_candy) {
		b_candy = false;
		for (unsigned int i = 0; i < behaviors.size() != 4; i++) {
			if (behaviors[i] == 4) {
				behaviors.erase(behaviors.begin() + i);
				break;
			}
		}
	}
}

void ServerSam::attachLevel(LevelManager* levelmanager) {
	lvm = levelmanager;
	blockage = lvm->levelmap->getLayer("Collision");
}

void ServerSam::walkOn() {
	if (!b_walk) {
		b_walk = true;
		behaviors.push_back(1);
	}
}

void ServerSam::teleportOn() {
	if (!b_teleport) {
		b_teleport = true;
		behaviors.push_back(3);
	}
}

void ServerSam::candyOn() {
	if (!b_candy) {
		b_candy = true;
		behaviors.push_back(4);
	}
}

void ServerSam::pteraOn() {
	if (!b_ptera) {
		ptera->setVisible(true);
		b_ptera = true;
		behaviors.push_back(2);
	}
}

void ServerSam::lowerReactW(void) {
	//5 frames is bottom limt of reaction time weight.
	//5 frames at 30 fps is 0.167s and fastest possible human reaction time is 0.15s
	if (reaction_time_weight > 5) {
		reaction_time_weight--;
	}
}

void ServerSam::incReactW(void) {
	//15 frames is upper limt of reaction time weight.
	//15 frames at 30 fps is 0.5s which is kind of the upper outlier of normal human reaction time
	if (reaction_time_weight < 15) {
		reaction_time_weight++;
	}
}

//For lovely statistics on human reaction time, check out
//http://www.humanbenchmark.com/tests/reactiontime/statistics