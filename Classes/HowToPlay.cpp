#include "HowToPlay.h"

// CODE TO TRY
std::string IPADDRESS = "";
int PLAYERNUM = 1;
//////////////

Scene* HowToPlay::createHowToPlay()
{
	auto HTPScene = Scene::create();
	auto HTPlayer = HowToPlay::create();

	HTPScene->addChild(HTPlayer);

	return HTPScene;
}

// CODE TO TRY
Scene* HowToPlay::createHowToPlay(std::string ip, int pn)
{
	IPADDRESS = ip;
	PLAYERNUM = pn;

	auto HTPScene = Scene::create();
	auto HTPlayer = HowToPlay::create();

	HTPScene->addChild(HTPlayer);

	return HTPScene;
}
///////////////


bool HowToPlay::init()
{
	if (!Layer::init())
	{
		return false;
	}

	float winSizeWidth = CCDirector::sharedDirector()->getWinSize().width / 2;
	float winSizeHeight = CCDirector::sharedDirector()->getWinSize().height / 2;

	auto begin_button = MenuItemImage::create("res//sprites//ui//GameBeginNP.png", "res//sprites//ui//GameBeginP.png", CC_CALLBACK_1(HowToPlay::begin, this));
	begin_button->setPosition(Vec2(winSizeWidth + 12, winSizeHeight - 125));
	begin_button->setScale(0.5f);

	auto layer = Menu::create(begin_button, NULL);
	layer->setPosition(Point::ZERO);
	this->addChild(layer, 1);

	std::string file = "res//maps//how_to_play.tmx";
	auto str = String::createWithContentsOfFile(FileUtils::getInstance()->fullPathForFilename(file.c_str()).c_str());
	HTPScreen = cocos2d::TMXTiledMap::createWithXML(str->getCString(), "");
	this->addChild(HTPScreen, 0);

	Directions = Sprite::create("res//sprites//ui//how_to_play.png");
	Directions->setScale(0.5f);
	Directions->setPosition(Vec2((int)winSizeWidth, (int)winSizeHeight));
	this->addChild(Directions, 0);

	return true;
}


void HowToPlay::begin(cocos2d::Ref* pSender)
{
	experimental::AudioEngine::stopAll();
	auto scene = ClientDemo::createScene(IPADDRESS, PLAYERNUM); // CODE TO TRY
	CCDirector::getInstance()->replaceScene(scene);
	//CCDirector::getInstance()->replaceScene(TransitionFade::create(0.9, scene, Color3B(255, 255, 255)));

}


void HowToPlay::menuCloseCallback(Ref* pSender)
{
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}