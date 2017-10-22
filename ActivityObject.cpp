/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#include "ActivityObject.h"
#include "Consist.h"
#include "Game.h"
#include "GLUU.h"
#include <QDebug>
#include <QMenu>
#include "TDB.h"
#include "GLMatrix.h"
#include "ParserX.h"
#include "FileBuffer.h"
#include "GLMatrix.h"
#include "Activity.h"

ActivityObject::~ActivityObject() {
}

ActivityObject::ActivityObject(){
    typeObj = GameObj::activityobj;
    id = -1;
}

ActivityObject::ActivityObject(int tid){
    typeObj = GameObj::activityobj;
    id = tid;
}

ActivityObject::ActivityObject(const ActivityObject& orig){
    typeObj = orig.typeObj;
    id = orig.id;
    con = orig.con;
    direction = orig.direction;
    objectType = orig.objectType;
    objectTypeId = orig.objectTypeId;
    parentActivity = orig.parentActivity;
    selected = orig.selected;
    selectionValue = orig.selectionValue;
    failedSignal = orig.failedSignal;
    modified = orig.modified;
    Vec4::copy((float*)tile, (float*)orig.tile);
    Vec4::copy((float*)restrictedSpeedZoneStart, (float*)orig.restrictedSpeedZoneStart);
    Vec4::copy((float*)restrictedSpeedZoneEnd, (float*)orig.restrictedSpeedZoneEnd);
}

void ActivityObject::setPosition(int x, int z, float* p){
    if(Game::trackDB == NULL)
        return;
    
    if(con == NULL)
        return;

    float tp[3];
    float tpos[3];
    float posT[2];
    
    Vec3::copy(tp, p);
    Game::check_coords(x, z, tp);
    posT[0] = x;
    posT[1] = z;

    int ok = Game::trackDB->findNearestPositionOnTDB(posT, tp, NULL, tpos);
    if(ok >= 0){
        tile[0] = posT[0];
        tile[1] = -posT[1];
        tile[2] = tp[0];
        tile[3] = -tp[2];
        con->isOnTrack = false;
        modified = true;
    }
}

void ActivityObject::toggleDirection(){
    if(con == NULL)
        return;
    
    direction = abs(direction - 1);
    con->isOnTrack = false;
    modified = true;
}

void ActivityObject::setParentActivity(Activity* a){
    parentActivity = a;
}

QString ActivityObject::getParentName(){
    if(parentActivity == NULL)
        return "";
    return parentActivity->header->name;
}

bool ActivityObject::select(int value){
    selected = true;
    selectionValue = value;
    if(con != NULL){
        con->select(value);
    }
    return true;
}

int ActivityObject::getSelectedElementId(){
    if(con != NULL){
        return con->selectedIdx;
    }
    return 0;
}

bool ActivityObject::getElementPosition(int id, float *posTW){
    if(con != NULL){
        return con->getWagonWorldPosition(id, posTW);
    }
    return false;
}

bool ActivityObject::unselect(){
    selected = false;
    selectionValue = -1;
    if(con != NULL){
        con->unselect();
    }
    return false;
}

void ActivityObject::remove(){
    if(parentActivity == NULL)
        return;
    parentActivity->deleteObject(id);
}

bool ActivityObject::isUnSaved(){
    if(con != NULL)
        if(con->isUnSaved())
            return true;
    return modified;
}

void ActivityObject::setModified(bool val){
    modified = val;
    if(con != NULL)
        con->setModified(val);
}

void ActivityObject::render(GLUU* gluu, float* playerT, int renderMode, int index){
    int selectionColor = 0;
    if(renderMode == gluu->RENDER_SELECTION){
        selectionColor = 11 << 20;
        selectionColor |= index << 8;
    }
    
    if(objectTypeId == ActivityObject::WAGONLIST ){
        if(con != NULL){
            if (!con->isOnTrack)
                con->initOnTrack(tile, direction);
            con->renderOnTrack(gluu, playerT, selectionColor);
        }
    }
    if(objectTypeId == ActivityObject::RESTRICTEDSPEEDZONE ){
        
    }
    if(objectTypeId == ActivityObject::FAILEDSIGNAL ){
        
    }
}

void ActivityObject::pushContextMenuActions(QMenu *menu){
    if(contextMenuActions["ToggleDirection"] == NULL){
        contextMenuActions["ToggleDirection"] = new QAction(tr("&Toggle Direction")); 
        QObject::connect(contextMenuActions["ToggleDirection"], SIGNAL(triggered()), this, SLOT(menuToggleDirection()));
    }
    menu->addAction(contextMenuActions["ToggleDirection"]);
}

void ActivityObject::menuToggleDirection(){
    toggleDirection();
}

void ActivityObject::load(FileBuffer* data) {
    QString sh;
    while (!((sh = ParserX::NextTokenInside(data).toLower()) == "")) {

        if (sh == ("objecttype")) {
            objectType = ParserX::GetStringInside(data);
            ParserX::SkipToken(data);
            continue;
        }
        if (sh == ("train_config")) {
            objectTypeId = WAGONLIST;
            con = new Consist();
            if (con->load(data)) {
                con->loaded = 1;
                con->setMaxVelocityFixed(true);
                con->initPos();
            }
            //ParserX::SkipToken(data);
            ParserX::SkipToken(data);
            continue;
        }
        if (sh == ("direction")) {
            direction = ParserX::GetNumber(data);
            ParserX::SkipToken(data);
            continue;
        }
        if (sh == ("id")) {
            id = ParserX::GetNumber(data);
            ParserX::SkipToken(data);
            continue;
        }
        if (sh == ("tile")) {
            tile[0] = ParserX::GetNumber(data);
            tile[1] = ParserX::GetNumber(data);
            tile[2] = ParserX::GetNumber(data);
            tile[3] = ParserX::GetNumber(data);
            ParserX::SkipToken(data);
            continue;
        }
        if (sh == ("startposition")) {
            restrictedSpeedZoneStart[0] = ParserX::GetNumber(data);
            restrictedSpeedZoneStart[1] = ParserX::GetNumber(data);
            restrictedSpeedZoneStart[2] = ParserX::GetNumber(data);
            restrictedSpeedZoneStart[3] = ParserX::GetNumber(data);
            ParserX::SkipToken(data);
            continue;
        }
        if (sh == ("endposition")) {
            restrictedSpeedZoneEnd[0] = ParserX::GetNumber(data);
            restrictedSpeedZoneEnd[1] = ParserX::GetNumber(data);
            restrictedSpeedZoneEnd[2] = ParserX::GetNumber(data);
            restrictedSpeedZoneEnd[3] = ParserX::GetNumber(data);
            ParserX::SkipToken(data);
            continue;
        }
        qDebug() << "#activityObject - undefined token: " << sh;
        ParserX::SkipToken(data);
        continue;
    }
}

void ActivityObject::save(QTextStream* out) {
    if(objectTypeId == RESTRICTEDSPEEDZONE){
        *out << "			ActivityRestrictedSpeedZone (\n";
        *out << "				StartPosition ( "<<restrictedSpeedZoneStart[0]<<" "<<restrictedSpeedZoneStart[1]<<" "<<restrictedSpeedZoneStart[2]<<" "<<restrictedSpeedZoneStart[3]<<" )\n";
        *out << "				EndPosition ( "<<restrictedSpeedZoneEnd[0]<<" "<<restrictedSpeedZoneEnd[1]<<" "<<restrictedSpeedZoneEnd[2]<<" "<<restrictedSpeedZoneEnd[3]<<" )\n";
        *out << "			)\n";
        return;
    }
    if(objectTypeId == FAILEDSIGNAL){
        *out << "			ActivityFailedSignal ( " << failedSignal << " )\n";
        return;
    }
    
    *out << "			ActivityObject (\n";
    *out << "				ObjectType ( "<<ParserX::AddComIfReq(objectType)<<" )\n";
    if(con != NULL){
        *out << "				Train_Config (\n";
        con->save("				", out);
        *out << "				)\n";
    }
    *out << "				Direction ( "<<direction<<" )\n";
    *out << "				ID ( "<<id<<" )\n";
    *out << "				Tile ( "<<tile[0]<<" "<<tile[1]<<" "<<tile[2]<<" "<<tile[3]<<" )\n";
    *out << "			)\n";
}