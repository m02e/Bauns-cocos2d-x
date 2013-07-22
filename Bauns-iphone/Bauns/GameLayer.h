//
//  GameLayer.h
//  Bauns
//
//  Created by MOZE on 13-7-20.
//
//

#ifndef __Bauns__GameLayer__
#define __Bauns__GameLayer__

#include <iostream>
#include "cocos2d.h"
using namespace cocos2d;

class GameLayer :public CCLayer {
    
public:
    virtual bool init();
    CREATE_FUNC(GameLayer);
};

#endif /* defined(__Bauns__GameLayer__) */
