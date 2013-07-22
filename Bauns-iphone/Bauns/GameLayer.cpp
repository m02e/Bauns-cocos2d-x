//
//  GameLayer.cpp
//  Bauns
//
//  Created by MOZE on 13-7-20.
//
//

#include "GameLayer.h"
#include "GrooveLayer.h"

bool GameLayer::init(){
    
    CCSprite* bgImage = CCSprite::create("background.png");
    bgImage->setPosition(ccp(240,400));
    addChild(bgImage);
    
    GrooveLayer* grooveLayer = GrooveLayer::create();
    addChild(grooveLayer);

    return true;
}

