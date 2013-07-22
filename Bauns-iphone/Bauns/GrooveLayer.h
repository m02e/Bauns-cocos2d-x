//
//  GrooveLayer.h
//  Bauns
//
//  Created by MOZE on 13-7-20.
//
//

#ifndef __Bauns__GrooveLayer__
#define __Bauns__GrooveLayer__

#include <iostream>
#include "cocos2d.h"
#include "GameLayer.h"
using namespace cocos2d;
    //
typedef void (CCObject::*TOUCH_CALL)(int r,int c);
#define touch_call(_func) (TOUCH_CALL)(&_func)


#define BLUECELL 0
#define REDCELL 1
#define GREENCELL 2
#define ORANGECELL 3
#define BOMBCELL 4

//
// Control the behaviours of cell
//
//
class CellBehaviour :public CCObject{
    
public:
    virtual void onStart()=0;
    virtual void onHit(int row,int col)=0;
    virtual CCArray* getLinkCell(CCArray* cellArray,int row,int col,int type)=0;
};

//
//
//
//
//
class  BallListener :public CCObject{
    
public:
    virtual void onHit(int r,int w) = 0;
};

//
//
//
//
class Cell:public CCSprite,public CCTargetedTouchDelegate{

private:
    CellBehaviour* behaviour ;
    int atRow,atCol;
    int type;
    bool isDropping;
        CCRect rect();
        //
    CCObject* functionCaller;
    TOUCH_CALL touchCall ;
        //
    bool containsTouchLocation(CCTouch* touch);
public:
    bool isSearch  ;
    Cell(const char* c);
    
    virtual void onEnter();
    virtual void onExit();
    void setAtRowCol(int atRow,int atCol);
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);

    
    void setTouchCall(CCObject* target,TOUCH_CALL touchCall);
    void endDrop();
    int getAtRow();
    int getAtCol();
    int getType();
    void setType(int type);
    void setCellBehaviour(CellBehaviour* behaviour);
    bool isdropping();
    void setDropping(bool isDropping);
    virtual void touchDelegateRetain();
    virtual void touchDelegateRelease();
    CellBehaviour* getCellBehaviour();
    ~Cell();

};

    //
    //
    //
    //
class Ball:public CCSprite ,public CCTargetedTouchDelegate{
    
private:
    int startPointX , startPointY ;
    int willMoveToX ,willMoveToY ;
    CCRect rect();
    CCRect touchBox ;
    bool containsTouchLocation(CCTouch* touch);
    bool isTouched ;
    void initBall();
    float onShutPercentX ,onShutPercentY ;
        //
public:
    virtual void onEnter();
    virtual void onExit();
    virtual void touchDelegateRetain();
    virtual void touchDelegateRelease();
    virtual bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    virtual void  ccTouchMoved (CCTouch *pTouch, CCEvent *pEvent);
    virtual void  ccTouchEnded (CCTouch *pTouch, CCEvent *pEvent);
        //    virtual void  ccTouchCancelled (CCTouch *pTouch, CCEvent *pEvent);
    
    Ball(const char* c);
        //    virtual void draw();
    void moveTo();
    void resetBall();
    float getPercentX();
    float getPercentY();
    void setWillMoveTo(int x,int y);
    void setStartPosition(int x,int y);
    virtual void draw();
    void onHit();
    
    
};
//
//
//
//
//
class GrooveLayer :public CCLayer{
 
private:
        //
    float moveEachCellTime ;
    float cellScaleTime ;
    float cellEachDelayTime  ;
        //
    int cellRowCount ,cellColCount ;
    Cell* cells[90];
    int cellSize ;
    bool isDropping ;
    void initCell();
    int getRowIndex(int number);
    int getColIndex(int number);
    int getIndex(int row,int col);
    void addCell(int row,int col,Cell* cell);
    
    Ball* ball;
    int getCellY(int row);
    void removeCell(CCNode* node);
    
    void addCellOnTop();
    
    bool isFull(int col);
    
    void addTint(Cell* cell);
    void removeTint(Cell* cell);
public:
    int getCellRowCount();
    int getCellColCount();
    virtual bool init();
    Cell* getCell(int row,int col);
    void drop();
    void hitCell(int row,int col);
    void removeCellWithAction(int row,int col);
    CCPoint getCellPosition(int row,int col);
    
    float getCellsPercentX(float percent);
    float getCellsPercentY(float percent);
    
    int getRowNo(float percent);
    int getColNo(float percent);
    CREATE_FUNC(GrooveLayer);
};

//
//
//
//
//
class ColorCellBehaviour:public CellBehaviour {
private:
    GrooveLayer* groove ;
public:
    ColorCellBehaviour(GrooveLayer* layer);
    virtual void onStart();
    virtual void onHit(int row,int col);
    virtual CCArray* getLinkCell(CCArray* cellArray,int row,int col,int type);
    
};


//
//
//
//
//
class BoumbCellBehaviour:public CellBehaviour {
private:
    GrooveLayer* groove ;
    int power;
public:
    BoumbCellBehaviour(GrooveLayer* groove);
    virtual void onStart();
    virtual void onHit(int row,int col);
    virtual CCArray* getLinkCell(CCArray* cellArray,int row,int col,int type);
    
};


//
//
//
//
Cell* getRandomColorCell(GrooveLayer* groove);
#endif /* defined(__Bauns__GrooveLayer__) */
