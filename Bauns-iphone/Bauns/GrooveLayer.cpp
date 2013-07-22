//
//  GrooveLayer.cpp
//  Bauns
//
//  Created by MOZE on 13-7-20.
//
//

#include "GrooveLayer.h"
#include "cocos2d.h"

using namespace cocos2d;

//
// Control the behaviours of cell
//
//
Cell::Cell(const char* c){
    initWithFile(c);
    this->autorelease();
    
    atRow = 0; atCol = 0 ;
    type = 0; isDropping = false;
    behaviour = NULL;
    functionCaller = NULL;
    isSearch = false;
}

CCRect Cell::rect(){
    CCSize s = getTexture()->getContentSize();
    return CCRectMake(-s.width / 2, -s.height / 2, s.width, s.height);
}
bool Cell::ccTouchBegan(CCTouch* touch, CCEvent* event){
    if (!isDropping && containsTouchLocation(touch)) {
        if (functionCaller!=NULL) {
//            CCLOG("touch at : %i %i",atRow,atCol);
            (functionCaller->*touchCall)(atRow,atCol);
        }
    }
    return false;
}
void Cell::touchDelegateRetain()
{
    this->retain();
}

void Cell::touchDelegateRelease()
{
    this->release();
}

bool Cell::containsTouchLocation(CCTouch* touch){
    return rect().containsPoint(convertTouchToNodeSpaceAR(touch));
}

void Cell::setAtRowCol(int atRow,int atCol){
    this->atRow = atRow;
    this->atCol = atCol;
}
void Cell::setTouchCall(CCObject* target,TOUCH_CALL touchCall){
    this->functionCaller = target ;
    this->touchCall = touchCall;
}
int Cell::getAtRow(){
    return this->atRow;
}
int Cell::getAtCol(){
    return this->atCol;
}
int Cell::getType(){
    return type;
}
void Cell::setType(int type){
    this->type = type;
}
void Cell::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSprite::onEnter();
}

void Cell::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}

void Cell::endDrop(){
    isDropping  = false;
}

bool Cell::isdropping(){
    return isDropping;
}
void Cell::setCellBehaviour(CellBehaviour *behaviour){
    this->behaviour = behaviour;
}
void Cell::setDropping(bool isDropping){
    this->isDropping = isDropping;
}

CellBehaviour* Cell::getCellBehaviour(){
    return behaviour;
}

Cell::~Cell(){
//    CCLOG("-------removed cell row: %i col: %i--------",atRow,atCol);
}

//
//
//
//
//
//
//

bool GrooveLayer::init(){
     moveEachCellTime = 0.07f;
     cellScaleTime = 0.2f;
     cellEachDelayTime = 0.02f;
        //
    cellRowCount = 10;cellColCount = 9;
     cellSize = 45;
     isDropping = false;
    
    ball = NULL;
    
    
    
    
    CCSprite* cellShadowImage = CCSprite::create("shadows.png");
    cellShadowImage->setPosition(ccp(240, 500));
    addChild(cellShadowImage,0);
    
    initCell();
    for (int i = 0; i<cellRowCount*cellColCount ;i++) {
        if(cells[i]!=NULL){
//            CCLOG("%i %i",cells[i]->getAtRow(),cells[i]->getAtCol());
            getCell(cells[i]->getAtRow(),cells[i]->getAtCol());
        }
    }
    
    this->schedule(schedule_selector(GrooveLayer::addCellOnTop), 4);
    
    ball = new Ball("ball.png");
    ball->setPosition(ccp(240,150));
    ball->setStartPosition(240, 150);
    CCPoint p = getCellPosition(2, 3);
    ball->setWillMoveTo(p.x+22,p.y+22);
    addChild(ball, 2);
    return true;
};
void GrooveLayer::addCellOnTop(){
    for (int i = 0; i<getCellColCount(); i++) {
        Cell* c = getRandomColorCell(this) ;
        addCell(getCellRowCount()-1, i, c);
    }
    drop();
}
void GrooveLayer::initCell(){
        //
    for (int i = 0; i<cellRowCount*cellColCount;i++) {
        cells[i] =NULL;
    }
    
        //
    for (int i = 0; i<cellRowCount*cellColCount-25 ;i++) {
        Cell* cell = getRandomColorCell(this);
        addCell(getRowIndex(i), getColIndex(i), cell);
    }

    drop();
    

};

Cell* GrooveLayer::getCell(int row,int col){
    int index = (cellColCount)*row + col ;
    Cell* cell =  cells[index];
    return cell;
};
int GrooveLayer::getRowNo(float percent){
    if ((int)percent* cellRowCount < percent* cellRowCount) {
        return percent* (int)cellRowCount;
    }
    return (int)percent* cellRowCount-1 ;
}
int GrooveLayer::getColNo(float percent){
    if ((int)percent* cellColCount < percent* cellColCount) {
        return percent* (int)cellColCount;
    }
    return (int)percent* cellColCount - 1 ;
}
bool GrooveLayer::isFull(int col){
    Cell* c = getCell(cellRowCount-1, col) ;
    if (c!=NULL && !c->isdropping()) {
        return true;
    }
    return false;
}
int GrooveLayer::getCellY(int row){
    return cellSize*row+400-112;
}

CCPoint GrooveLayer::getCellPosition(int row,int col){
    return ccp(cellSize*col +62,cellSize*row+400-112);
};

void GrooveLayer::addCell(int row,int col,Cell* cell){
    Cell* c = getCell(row, col);
    if (c!=NULL || ( c!=NULL && c->isdropping())) {
        return;
    }
    cell->setPosition((getCellPosition(row,col)));
    cell->setAtRowCol(row,col);
    cells[getIndex(row, col)] = cell;
    cell->setTouchCall(this, touch_call(GrooveLayer::hitCell));
    this->addChild(cell,1);
};

int GrooveLayer::getRowIndex(int number){
    return number / cellColCount;
}

int GrooveLayer::getColIndex(int number){
    return number % cellColCount;
}

int GrooveLayer::getIndex(int row,int col){
    return cellColCount*row+col;
}

void GrooveLayer::removeCellWithAction(int row,int col){
    Cell* cell = getCell(row, col);
    CCScaleTo* scaleAction = CCScaleTo::create(cellScaleTime, 0);
    CCCallFuncN* endAction = CCCallFuncN::create(this, callfuncN_selector(GrooveLayer::removeCell));
    
    CCSequence* action = CCSequence::createWithTwoActions(scaleAction, endAction);
    cell->runAction(action);

}

void GrooveLayer::removeCell(CCNode* node)
{
    Cell* cell = (Cell*)node;
    cells[getIndex(cell->getAtRow(), cell->getAtCol())]= NULL;
    cell->removeFromParent();
}

void GrooveLayer::hitCell(int row, int col){
    Cell* hitCell = getCell(row, col);
    if (hitCell==NULL) {
        return ;
    }
    
    
    CellBehaviour* b = hitCell->getCellBehaviour();
    if (b!=NULL) {
        b->onHit(row,col);
    }
    
    CCCallFunc* dropAction = CCCallFunc::create(this, callfunc_selector(GrooveLayer::drop));
    CCSequence* action = CCSequence::createWithTwoActions(CCDelayTime::create(cellScaleTime+0.05f), dropAction);
    this->runAction(action);
}
int GrooveLayer::getCellRowCount(){
    return cellRowCount;
}
int GrooveLayer::getCellColCount(){
    return cellColCount;
}

void GrooveLayer::addTint(Cell *cell){
    if (cell!=NULL) {
        CCSprite* tintSprite = CCSprite::create("tint.png");
        tintSprite->setPosition(ccp(cell->getTextureRect().size.width/2,cell->getTextureRect().size.height/2));
        CCFadeIn* fadeInAction = CCFadeIn::create(1);
        CCFadeOut* fadeOutAction = CCFadeOut::create(1);
        CCDelayTime* delayAction = CCDelayTime::create(1);
        
        CCSequence* s_action = CCSequence::create(fadeInAction,fadeOutAction,delayAction,NULL);
        CCRepeatForever* action = CCRepeatForever::create(s_action);
        tintSprite->runAction(action);
        cell->addChild(tintSprite);
    }
}
void GrooveLayer::drop(){
    if (isDropping) {
        return;
    }
//    CCLOG("drop");
    isDropping = true;
    for (int col = 0; col<cellColCount; col++) {
        int emptyRowIndex = -1;
        for (int _row = emptyRowIndex+1; _row<cellRowCount; _row++) {
            if (getCell(_row, col)==NULL) {
                emptyRowIndex = _row;
                break;
            }
        }
        float delayTime = 0;
        for (int row=emptyRowIndex+1; row<cellRowCount; row++) {
            Cell* cell = getCell(row, col);
            if (cell!=NULL && emptyRowIndex<cell->getAtRow() && emptyRowIndex>=0) {

                CCDelayTime* delayAction = CCDelayTime::create((delayTime));
                CCMoveTo* moveAction = NULL;
                if (cell->isdropping()) {
                    cell->stopActionByTag(0);
                    int _rowCount = (cell->getPositionY()- getCellY(emptyRowIndex)) / cellSize;
                    moveAction = CCMoveTo::create(moveEachCellTime*_rowCount, getCellPosition(emptyRowIndex, col));
                }else{
                    moveAction = CCMoveTo::create(moveEachCellTime*(row-emptyRowIndex), getCellPosition(emptyRowIndex, col));

                }
                
                CCCallFunc* endMoveAction = CCCallFunc::create(cell, callfunc_selector(Cell::endDrop));
//                CCLOG("%i %i move to %i %i",row,col,emptyRowIndex,col);
                CCSequence* action = CCSequence::create(delayAction,moveAction,endMoveAction,NULL);
                action->setTag(0);
                cell->runAction(action);
                cell->setDropping(true);
                cell->setAtRowCol(emptyRowIndex, col);
                cells[getIndex(emptyRowIndex, col)] = cell;
                emptyRowIndex++;
                delayTime+=cellEachDelayTime;
                cells[getIndex(row, col)] = NULL;
            }
        }
    }
    isDropping = false;
};
float GrooveLayer::getCellsPercentX(float percent){
    int startX = 40 ;
    return percent * cellSize * cellColCount + startX;
}

float GrooveLayer::getCellsPercentY(float percent){
    int startY = 500 - 472/2 ;
    return percent * cellSize * cellRowCount + startY;
}



ColorCellBehaviour::ColorCellBehaviour(GrooveLayer* groove){
    this->groove = groove;
}
void ColorCellBehaviour::onStart(){
};
void ColorCellBehaviour::onHit(int row, int col){
    Cell* cell = groove->getCell(row, col);
    CCArray* cellArray = CCArray::create();
    getLinkCell(cellArray,row,col,cell->getType());
    
    for (int i=0; i<cellArray->count(); i++) {
        Cell* cell = (Cell*)cellArray->objectAtIndex(i);
        if (cell!=NULL) {
            groove->removeCellWithAction(cell->getAtRow(), cell->getAtCol());
        }
    }
    cellArray->removeAllObjects();
    delete cellArray;
}

CCArray* ColorCellBehaviour::getLinkCell(cocos2d::CCArray *cellArray, int row, int col,int type){
    Cell* cell = groove->getCell(row, col);
    if (cell==NULL || cell->getType()!=type || cell->isdropping() || cell->isSearch) {
        return cellArray;
    }
    cell->isSearch = true;
    cellArray->addObject(cell);
        //down
    if (row>=1 && row<groove->getCellRowCount()) {
        this->getLinkCell(cellArray,row-1 , col,type);
    }
        //up
    if (row>=0 && row<groove->getCellRowCount()-1) {
        this->getLinkCell(cellArray, row+1, col,type);
    }
        //left
    if (col>=1 && col<groove->getCellColCount()) {
        this->getLinkCell(cellArray, row, col-1,type);
    }
        //right
    if (col>=0 && col<groove->getCellColCount()-1) {
        this->getLinkCell(cellArray, row, col+1,type);
    }
    return cellArray;
}






BoumbCellBehaviour::BoumbCellBehaviour(GrooveLayer* groove){
    this->groove = groove;
    power = 2;
}

void BoumbCellBehaviour::onStart(){};

void BoumbCellBehaviour::onHit(int row, int col){
    CCLOG("onHit : %i %i",row,col);
    Cell* cell = groove->getCell(row, col);
    CCArray* cellArray = CCArray::create();
    getLinkCell(cellArray,row,col,cell->getType());
    
    for (int i=0; i<cellArray->count(); i++) {
        Cell* cell = (Cell*)cellArray->objectAtIndex(i);
        if (cell!=NULL) {
            groove->removeCellWithAction(cell->getAtRow(), cell->getAtCol());
        }
    }
//    CCLOG("cellArray size: %i",cellArray->count());
    cellArray->removeAllObjects();
    delete cellArray;
    
}
CCArray* BoumbCellBehaviour::getLinkCell(CCArray* cellArray,int row,int col,int type){
    for (int r = row - power ; r<=(row + power) ; r++) {
        if(r<0 || r>= groove->getCellRowCount()){continue;}
        for (int c= col -power  ; c<=col + power ; c++) {
            if(c<0 || c>= groove->getCellColCount()){continue;}
            Cell* cell = groove->getCell(r, c);
            if (cell!=NULL && !cell->isdropping()) {
                cellArray->addObject(cell);
            }
        }
    }
    return cellArray;
}


CCRect Ball::rect(){
    CCSize s = getTexture()->getContentSize();
    return CCRectMake(-s.width / (3/2), -s.height / (3/2), 2*s.width, 2*s.height);
}
bool Ball::ccTouchBegan(CCTouch* touch, CCEvent* event){
    if (isTouched) {
        return true;
    }
    if (containsTouchLocation(touch)) {
        isTouched = true;
        return true;
    }
    return false;
}
void Ball::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent){
    CCPoint touchPoint = pTouch->getLocationInView();
    touchPoint = CCDirector::sharedDirector()->convertToGL(touchPoint);
    
//    if (touchBox.containsPoint(touchPoint)) {
//        setPosition(ccp(touchPoint.x,touchPoint.y));
//    }
    if (touchPoint.x>=touchBox.getMinX() && touchPoint.x<=touchBox.getMaxX()) {
        setPositionX(touchPoint.x);
        
        float _px = 1 - getPercentX() ;
        GrooveLayer* groove = (GrooveLayer*)this->getParent();
        float pX = groove->getCellsPercentX(_px);
        
        willMoveToX  = pX;
    }
    if (touchPoint.y>=touchBox.getMinY() && touchPoint.y<=touchBox.getMaxY()) {
        setPositionY(touchPoint.y);
        
        float _py = 1- getPercentY() ;
        GrooveLayer* groove = (GrooveLayer*)this->getParent();
        float pY = groove->getCellsPercentY(_py);
        
        willMoveToY  = pY;
    }

}
void Ball::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent){
//    CCLog("%f %f ",getPercentX() , getPercentY());
    if (isTouched) {
        moveTo();
        isTouched = false;
    }

}
void Ball::moveTo(){
//    CCMoveTo* move = CCMoveTo::create(1, ccp(willMoveToX,willMoveToY));
//    CCCallFunc* end = CCCallFunc::create(this, callfunc_selector(Ball::reset));
//    CCSequence* action = CCSequence::createWithTwoActions(move, end);
    CCMoveTo* fHalfMove = CCMoveTo::create(0.3f, ccp((willMoveToX+getPositionX())/2,(willMoveToY+getPositionY())/2));
    CCScaleTo* fHalfScale = CCScaleTo::create(0.3f, 1.4f);
    CCSpawn* fhalf = CCSpawn::createWithTwoActions(fHalfMove, fHalfScale);

    
    CCMoveTo* sHalfMove = CCMoveTo::create(0.4f, ccp((willMoveToX),(willMoveToY)));
    CCScaleTo* sHalfScale = CCScaleTo::create(0.4f, 0.5f);
    CCSpawn* shalf = CCSpawn::createWithTwoActions(sHalfMove, sHalfScale);
    
    CCCallFunc* endMove = CCCallFunc::create(this, callfunc_selector(Ball::onHit));
    
    CCSequence* action = CCSequence::create(fhalf,shalf,endMove,NULL);
    
    runAction(action);
}
void Ball::resetBall(){
    setScale(1);
    isTouched = false ;
    setPosition(ccp(startPointX,startPointY));
}
void Ball::setStartPosition(int x, int y){
    startPointX  = x;
    startPointY = y;
}
void Ball::touchDelegateRetain()
{
    this->retain();
}

void Ball::touchDelegateRelease()
{
    this->release();
}

bool Ball::containsTouchLocation(CCTouch* touch){
    return rect().containsPoint(convertTouchToNodeSpaceAR(touch));
}
void Ball::onEnter()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSprite::onEnter();
}

void Ball::onExit()
{
    CCDirector* pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCSprite::onExit();
}
Ball::Ball(const char* c){
    initWithFile(c);
    this->autorelease();
    initBall();
    
    isTouched = false;
}
void Ball::initBall(){
    touchBox = CCRectMake(60, 60, 360, 150);
}
float Ball::getPercentX(){
    float percent = (getPositionX()-touchBox.getMinX() )/ touchBox.size.width ;
    onShutPercentX = percent;
    if (percent<0 || percent>1) {
        onShutPercentX = 0;
        return 0;
    }
    return percent;
}
float Ball::getPercentY(){
    float percent = (getPositionY()-touchBox.getMinY()) / touchBox.size.height ;
    onShutPercentY = percent;
    if (percent<0 || percent>1) {
        onShutPercentY = 0;
        return 0;
    }
    return percent;
}

void Ball::draw(){
    if (isTouched) {
        CCSize s = getTexture()->getContentSize();
        ccDrawLine(ccp(s.width/2,s.height/2), ccp(willMoveToX-getPositionX()+s.width/2,willMoveToY-getPositionY()+s.height/2));
    }

    CCSprite::draw();
}

void Ball::setWillMoveTo(int x,int y ){
    willMoveToY = y;
    willMoveToX = x;
}
void Ball::onHit(){
    GrooveLayer* g =(GrooveLayer*)getParent();
//    CCLOG("perX % f perY %f",onShutPercentY,onShutPercentX);
//    CCLOG("hit %i %i",g->getRowNo(1-onShutPercentY), g->getColNo(1-onShutPercentX));
    g->hitCell(g->getRowNo(1-onShutPercentY), g->getColNo(1-onShutPercentX));
    
    resetBall();
}

Cell* getRandomColorCell(GrooveLayer* groove){
    int r = CCRANDOM_0_1()*4 ;
    Cell* cell = NULL;
    switch (r) {
        case BLUECELL:
            cell = new Cell("blueCell.png");
            break;
        case GREENCELL:
            cell = new Cell("greenCell.png");
            break;
        case REDCELL:
            cell = new Cell("redCell.png");
            break;
        case ORANGECELL:
            cell = new Cell("orangeCell.png");
            break;
        default:
            cell = new Cell("greenCell.png");
            break;
    }
    cell->setType(r);
    cell->setCellBehaviour(new ColorCellBehaviour(groove));
    return cell;
};

