#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "Villain.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/asio.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <boost/thread/thread.hpp>
#include "ConfigFileInput.hpp"
#include "ServerPositionPacket.hpp"
#include "UDPInterface.hpp"

USING_NS_CC;
using boost::asio::ip::udp;

class HelloWorld : public cocos2d::Layer
{
private:
	cocos2d::experimental::TMXTiledMap* tileMap;

	Player* player1;
	Player* player2;
	std::vector<Player*> players;
	Villain* villain;

	std::vector<cocos2d::Sprite*> objects;


	cocos2d::Animate* walkanim;
	cocos2d::Animate* walkanim2;

public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	void KeyDown(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void KeyRelease(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void update(float dt);
	float xmove = 0;
	float ymove = 0;
	
	int playernum;
	ConfigFileInput setupdata;

	boost::asio::io_service* io_service_p;
	boost::asio::io_service io_service_;
	//UDPInterface* myudpinterfacep;
	udp::socket* myudpsocketp;
	
	udp::endpoint myendpoint;

	void processPacket(ServerPositionPacket p);
	void doReceive();

	enum { max_length = 1024 };
	char indata[max_length];
	char outdata[max_length];
	std::string outstringbuffer;

	~HelloWorld();
	
};

#endif // __HELLOWORLD_SCENE_H__
