/*  This file is part of TSRE5.
 *
 *  TSRE5 - train sim game engine and MSTS/OR Editors. 
 *  Copyright (C) 2016 Piotr Gadecki <pgadecki@gmail.com>
 *
 *  Licensed under GNU General Public License 3.0 or later. 
 *
 *  See LICENSE.md or https://www.gnu.org/licenses/gpl.html
 */

#include "Game.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QString>
#include <QEventLoop>
//#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
//#include <QUrl>
//#include <QUrlQuery>
//#include "RouteEditorWindow.h"
//#include "LoadWindow.h"
//#include "CELoadWindow.h"
#include "SoundList.h"
#include "ShapeLib.h"
#include "EngLib.h"
#include <QtWidgets>
#include <QColor>
#include "TarFile.h"
#include "Renderer.h"

bool Game::ServerMode = false;
QString Game::serverLogin = "";
QString Game::serverAuth = "";
RouteEditorClient *Game::serverClient = NULL;
GeoWorldCoordinateConverter *Game::GeoCoordConverter = NULL;
TDB *Game::trackDB = NULL;
TDB *Game::roadDB = NULL;    
SoundList *Game::soundList = NULL;    
TerrainLib *Game::terrainLib = NULL;   

bool Game::UseWorkingDir = false;
QString Game::AppName = "TSRE5";
QString Game::AppVersion = "v0.7.012";
QString Game::AppDataVersion = "0.697";
QString Game::root = "C:/tsdata/Train Simulator/";
QString Game::route = "bbb1";
QString Game::routeName = "bbb";
QString Game::trkName = "bbb";
QString Game::season = "";
//QString Game::route = "traska";
//QString Game::route = "cmk";
QString Game::mainWindowLayout = "PWT";
QString Game::ceWindowLayout = "C1";
QString Game::ActivityToPlay = "";
Renderer *Game::currentRenderer = NULL;
bool Game::playerMode = false;
bool Game::useNetworkEng = false;
bool Game::useQuadTree = true;
bool Game::useTdbEmptyItems = true;
int Game::allowObjLag = 1000;
int Game::maxObjLag = 10;
bool Game::ignoreLoadLimits = false;
int Game::startTileX = 0;
int Game::startTileY = 0;
float Game::objectLod = 3000;
float Game::distantLod = 100000;
int Game::tileLod = 2;
int Game::start = 0;
bool Game::ignoreMissingGlobalShapes = false;
bool Game::deleteTrWatermarks = false;
bool Game::deleteViewDbSpheres = false;
bool Game::createNewRoutes = false;
bool Game::writeEnabled = false;
bool Game::writeTDB = false;
bool Game::systemTheme = false;
bool Game::toolsHidden = false;
bool Game::usenNumPad = false;
float Game::cameraFov = 55.0f;
float Game::cameraSpeedMin = 1.0;
float Game::cameraSpeedStd = 3.0;
float Game::cameraSpeedMax = 40.0;
float Game::mouseSpeed = 1.0;
bool Game::cameraStickToTerrain = false;
bool Game::mstsShadows = false;

bool Game::viewWorldGrid = true;
bool Game::viewTileGrid = true;
bool Game::viewTerrainGrid = false;
bool Game::viewTerrainShape = true;
bool Game::viewInteractives = true;
bool Game::viewTrackDbLines = true;
bool Game::viewTsectionLines = true;
bool Game::viewPointer3d = true;
bool Game::viewMarkers = false;
bool Game::viewSnapable = false;
bool Game::viewCompass = false;
bool Game::warningBox = false;
bool Game::leaveTrackShapeAfterDelete = false;
bool Game::renderTrItems = false;
int Game::newRouteX = -5000;
int Game::newRouteZ = 15000;
bool Game::consoleOutput = false;
int Game::fpsLimit = 0;
bool Game::ortsEngEnable = true;
bool Game::sortTileObjects = true;
int Game::oglDefaultLineWidth = 1;
bool Game::showWorldObjPivotPoints = false;
int Game::shadowMapSize = 2048;
int Game::shadowLowMapSize = 1024;
int Game::shadowsEnabled = 1;
float Game::sunLightDirection[] = {-1.0,2.0,1.0};
int Game::textureQuality = 1;
float Game::snapableRadius = 20;
bool Game::snapableOnlyRot = false;
float Game::trackElevationMaxPm = 700.0;
bool Game::proceduralTracks = false;
bool Game::fullscreen = false;
bool Game::hudEnabled = false;
float Game::hudScale = 1.0;
bool Game::markerLines = false;

bool Game::loadAllWFiles = false;
bool Game::autoFix = false;
bool Game::gui = true;

QString Game::geoPath = "hgst";

//RouteEditorWindow* Game::window = NULL;
//LodWindow* Game::loadWindow = NULL;
ShapeLib *Game::currentShapeLib = NULL;
EngLib *Game::currentEngLib = NULL;
Route *Game::currentRoute = NULL;
GameObj *Game::currentSelectedGameObj = NULL;
QColor *Game::colorConView = NULL;
QColor *Game::colorShapeView = NULL;

QString Game::StyleMainLabel = "#770000";
QString Game::StyleGreenButton = "#55FF55";
QString Game::StyleRedButton = "#FF5555";
QString Game::StyleYellowButton = "#FFFF55";
QString Game::StyleGreenText = "#009900";
QString Game::StyleRedText = "#990000";

QString Game::imageMapsUrl;
int Game::mapImageResolution = 4096;

bool Game::autoNewTiles = false;
bool Game::autoGeoTerrain = false;

bool Game::useSuperelevation = false;

bool Game::soundEnabled = false;

int Game::AASamples = 0;
bool Game::AARemoveBorder = false;
float Game::PixelRatio = 1.0;

float Game::fogDensity = 0.7;
float Game::shadow1Res = 2000.0;
float Game::shadow1Bias = 0.0025;
float Game::shadow2Res = 4000.0;
float Game::shadow2Bias = 0.002;
//float fogColor[4]{0.5, 0.75, 1.0, 1.0};
float Game::fogColor[4] = {230.0/255.0,248.0/255,255.0/255.0, 1.0};
float Game::skyColor[4] = {230.0/255.0,248.0/255,255.0/255.0, 1.0};

int Game::DefaultElevationBox = 0;
float Game::DefaultMoveStep = 0.25;

bool Game::seasonalEditing = false;
int Game::numRecentItems = 11;
bool Game::useOnlyPositiveQuaternions = false;

QStringList Game::objectsToRemove;
QString Game::routeMergeString;
    
QHash<QString, int> Game::TextureFlags {
        {"None", 0x0},
        {"Snow", 0x1},
        {"SnowTrack", 0x2},
        {"Spring", 0x4},
        {"Autumn", 0x8},
        {"Winter", 0x10},
        {"SpringSnow", 0x20},
        {"AutumnSnow", 0x40},
        {"WinterSnow", 0x80},
        {"Night", 0x100},
        {"Underground", 0x40000000}
    };

void Game::InitAssets() {
    QString path;
    bool newInstallation = false;
    path = "./tsre_assets/";
    QFile appFile3(path);
    if (!appFile3.exists()){
        QDir().mkdir(path);
    }
    
    path = "./tsre_appdata/";
    
    QFile appFile1(path);
    if (!appFile1.exists()){
        newInstallation = true;
        QMessageBox msgBox;
        msgBox.setWindowTitle("TSRE");
        msgBox.setText("Welcome in TSRE!\n\nThis is experimental version.\nUsing it may seriously damage your data."
                       "\nMake backup first!\n\n\nNow TSRE will download app data.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        QDir().mkdir(path);
    }
    
    path += Game::AppDataVersion;
    
    QFile appFile2(path);
    if (!appFile2.exists()){
        qDebug() << "no appdata";
        DownloadAppData(path);
    }
    if (!appFile2.exists()){
        qDebug() << "appdata failed to load";
        return;
    }
}

void Game::load() {
    
    QString sh;
    QString path;
    
    path = "settings.txt";
    QFile file(path);
    
    if (!file.exists()){
        qDebug() << "creating new settings file";
        CreateNewSettingsFile();
    }
    
    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "settings file fails to open";
        return;
    }
    
    qDebug() << path;

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString line;
    QStringList args;
    QString val;
    while (!in.atEnd()) {
        line = in.readLine();
        //qDebug() << line;
        if(line.startsWith("#", Qt::CaseInsensitive)) continue;
        //args = line.split("=");
        args.clear();
        args.push_back(line.section('=', 0, 0));
        args.push_back(line.section('=', 1));
        //qDebug() << args[0] << args[1];
        if(args.count() < 2) continue;
        val = args[0].trimmed();
        //qDebug() << args[0].trimmed() << " "<< args[1].trimmed();
        if(val == "consoleOutput"){
            if(args[1].trimmed().toLower() == "true")
                Game::consoleOutput = true;
            else
                Game::consoleOutput = false;
        }
        if(val == "gameRoot")
            root = args[1].trimmed();
        if(val == "routeName")
            route = args[1].trimmed();

        if(val == "startTileX"){
            Game::start++;
            startTileX = args[1].trimmed().toInt();
        }
        if(val == "startTileY"){
            Game::start++;
            startTileY = args[1].trimmed().toInt();
        }
        if(val == "deleteTrWatermarks"){
            if(args[1].trimmed().toLower() == "true")
                deleteTrWatermarks = true;
            else
                deleteTrWatermarks = false;
        }
        if(val == "deleteViewDbSpheres"){
            if(args[1].trimmed().toLower() == "true")
                deleteViewDbSpheres = true;
            else
                deleteViewDbSpheres = false;
        }
        if(val == "createNewIfNotExist"){
            if(args[1].trimmed().toLower() == "true")
                createNewRoutes = true;
            else
                createNewRoutes = false;
        }
        if(val == "writeEnabled"){
            if(args[1].trimmed().toLower() == "true")
                writeEnabled = true;
            else
                writeEnabled = false;
        }
        if(val == "writeTDB"){
            if(args[1].trimmed().toLower() == "true")
                writeTDB = true;
            else
                writeTDB = false;
        }
        if(val == "systemTheme"){
            if(args[1].trimmed().toLower() == "true")
                systemTheme = true;
            else
                systemTheme = false;
        }
        if(val == "toolsHidden"){
            if(args[1].trimmed().toLower() == "true")
                toolsHidden = true;
            else
                toolsHidden = false;
        }
        if(val == "usenNumPad"){
            if(args[1].trimmed().toLower() == "true")
                usenNumPad = true;
            else
                usenNumPad = false;
        }
        if(val == "tileLod"){
            tileLod = args[1].trimmed().toInt();
        }
        if(val == "objectLod"){
            objectLod = args[1].trimmed().toInt();
        }
        if(val == "maxObjLag"){
            maxObjLag = args[1].trimmed().toInt();
        }
        if(val == "allowObjLag"){
            allowObjLag = args[1].trimmed().toInt();
        }
        if(val == "fpsLimit"){
            fpsLimit = args[1].trimmed().toInt();
        }
        if(val == "cameraFov"){
            cameraFov = args[1].trimmed().toFloat();
        }
        if(val == "warningBox"){
            if(args[1].trimmed().toLower() == "true")
                warningBox = true;
            else
                warningBox = false;
        }
        if(val == "leaveTrackShapeAfterDelete"){
            if(args[1].trimmed().toLower() == "true")
                leaveTrackShapeAfterDelete = true;
            else
                leaveTrackShapeAfterDelete = false;
        }
        if(val == "renderTrItems"){
            if(args[1].trimmed().toLower() == "true")
                renderTrItems = true;
            else
                renderTrItems = false;
        }
        if(val == "geoPath")
            geoPath = args[1].trimmed();
        if(val == "colorConView")
            colorConView = new QColor(args[1].trimmed());
        if(val == "colorShapeView")
            colorShapeView = new QColor(args[1].trimmed());
        if(val == "ortsEngEnable"){
            if(args[1].trimmed().toLower() == "true")
                ortsEngEnable = true;
            else
                ortsEngEnable = false;
        }
        if(val == "sortTileObjects"){
            if(args[1].trimmed().toLower() == "true")
                sortTileObjects = true;
            else
                sortTileObjects = false;
        }
        if(val == "ignoreMissingGlobalShapes"){
            if(args[1].trimmed().toLower() == "true")
                ignoreMissingGlobalShapes = true;
            else
                ignoreMissingGlobalShapes = false;
        }
        if(val == "snapableOnlyRot"){
            if(args[1].trimmed().toLower() == "true")
                snapableOnlyRot = true;
            else
                snapableOnlyRot = false; 
        }
        if(val == "oglDefaultLineWidth"){
            oglDefaultLineWidth = args[1].trimmed().toInt();
        }
        if(val == "shadowsEnabled"){
            shadowsEnabled = args[1].trimmed().toInt();
        }
        if(val == "shadowMapSize"){
            shadowMapSize = args[1].trimmed().toInt();
            if(shadowMapSize == 8192){
                shadow1Res = 3000.0;
                shadow1Bias = 0.0004;
            }
            if(shadowMapSize == 4096){
                shadow1Res = 2500.0;
                shadow1Bias = 0.0007;
            }
        }
        if(val == "shadowLowMapSize"){
            shadowLowMapSize = args[1].trimmed().toInt();
            if(shadowLowMapSize >= 2048){
                shadow2Res = 4000.0;
                shadow2Bias = 0.001;
            }
        }
        if(val == "textureQuality"){
            textureQuality = args[1].trimmed().toInt();
        }
        if(val == "imageMapsUrl"){
            imageMapsUrl = args[1].trimmed();
        }
        if(val == "mapImageResolution"){
            mapImageResolution = args[1].trimmed().toInt();
        }
        if(val == "AASamples"){
            AASamples = args[1].trimmed().toInt();
        }
        if(val == "cameraStickToTerrain"){
            if(args[1].trimmed().toLower() == "true")
                cameraStickToTerrain = true;
            else
                cameraStickToTerrain = false; 
        }
        if(val == "proceduralTracks"){
            if(args[1].trimmed().toLower() == "true")
                proceduralTracks = true;
            else
                proceduralTracks = false; 
        }
        if(val == "fullscreen"){
            if(args[1].trimmed().toLower() == "true")
                fullscreen = true;
            else
                fullscreen = false; 
        }
        if(val == "soundEnabled"){
            if(args[1].trimmed().toLower() == "true")
                soundEnabled = true;
            else
                soundEnabled = false; 
        }
        
        if(val == "cameraSpeedMin"){
            cameraSpeedMin = args[1].trimmed().toFloat();
        }
        if(val == "cameraSpeedStd"){
            cameraSpeedStd = args[1].trimmed().toFloat();
        }
        if(val == "cameraSpeedMax"){
            cameraSpeedMax = args[1].trimmed().toFloat();
        }
        if(val == "mouseSpeed"){
            mouseSpeed = args[1].trimmed().toFloat();
        }
        if(val == "trackElevationMaxPm"){
            trackElevationMaxPm = args[1].trimmed().toFloat();
        }
        if(val == "mainWindowLayout"){
            mainWindowLayout = args[1].trimmed();
        }
        if(val == "ceWindowLayout"){
            ceWindowLayout = args[1].trimmed();
        }
        if(val == "useQuadTree"){
            if(args[1].trimmed().toLower() == "true")
                useQuadTree = true;
            else
                useQuadTree = false; 
        }
        if(val == "playerMode"){
            if(args[1].trimmed().toLower() == "true")
                playerMode = true;
            else
                playerMode = false; 
        }
        if(val == "useNetworkEng"){
            if(args[1].trimmed().toLower() == "true")
                useNetworkEng = true;
            else
                useNetworkEng = false; 
        }
        if(val == "fogDensity"){
            fogDensity = args[1].trimmed().toFloat();
        }
        if(val == "fogColor"){
            QColor tcolor(args[1].trimmed());
            fogColor[0] = tcolor.redF();
            fogColor[1] = tcolor.greenF();
            fogColor[2] = tcolor.blueF();
        }
        if(val == "skyColor"){
            QColor tcolor(args[1].trimmed());
            skyColor[0] = tcolor.redF();
            skyColor[1] = tcolor.greenF();
            skyColor[2] = tcolor.blueF();
        }
        if(val == "defaultElevationBox"){
            DefaultElevationBox = args[1].trimmed().toInt();
        }
        if(val == "defaultMoveStep"){
            DefaultMoveStep = args[1].trimmed().toFloat();
        }
        if(val == "hudEnabled"){
            if(args[1].trimmed().toLower() == "true")
                hudEnabled = true;
            else
                hudEnabled = false; 
        }
        if(val == "hudScale"){
            hudScale = args[1].trimmed().toFloat();
        }
        if(val == "useTdbEmptyItems"){
            if(args[1].trimmed().toLower() == "true")
                useTdbEmptyItems = true;
            else
                useTdbEmptyItems = false; 
        }
        if(val == "useWorkingDir"){
            if(args[1].trimmed().toLower() == "true")
                UseWorkingDir = true;
            else
                UseWorkingDir = false; 
        }
        if(val == "numRecentItems"){
            numRecentItems = args[1].trimmed().toInt();
        }
        if(val == "season"){
            season = args[1].trimmed();
        }
        if(val == "markerLines"){
            if(args[1].trimmed().toLower() == "true")
                markerLines = true;
            else
                markerLines = false; 
        }
        if(val == "seasonalEditing"){
            if(args[1].trimmed().toLower() == "true")
                seasonalEditing = true;
            else
                seasonalEditing = false; 
        }
        if(val == "loadAllWFiles"){
            if(args[1].trimmed().toLower() == "true")
                loadAllWFiles = true;
            else
                loadAllWFiles = false; 
        }
        if(val == "autoFix"){
            if(args[1].trimmed().toLower() == "true")
                autoFix = true;
            else
                autoFix = false; 
        }
        if(val == "useOnlyPositiveQuaternions"){
            if(args[1].trimmed().toLower() == "true")
                useOnlyPositiveQuaternions = true;
            else
                useOnlyPositiveQuaternions = false; 
        }
        if(val == "routeMergeString")
            routeMergeString = args[1].trimmed();
        if(val == "objectsToRemove")
            objectsToRemove = args[1].trimmed().split(":");
        if(val == "serverLogin"){
            serverLogin = args[1].trimmed();
        }
        if(val == "serverAuth"){
            serverAuth = args[1].trimmed();
        }
        
    }
}
/*
bool Game::loadRouteEditor(){
    
    window = new RouteEditorWindow();
    if(Game::fullscreen){
        window->setWindowFlags(Qt::CustomizeWindowHint);
        window->setWindowState(Qt::WindowMaximized);
    } else {
        window->resize(1280, 800);
    }
    
    loadWindow = new LoadWindow();
    QObject::connect(window, SIGNAL(exitNow()),
                      loadWindow, SLOT(exitNow()));
    
    QObject::connect(loadWindow, SIGNAL(showMainWindow()),
                      window, SLOT(show()));
    
    if(Game::checkRoot(Game::root) && (Game::checkRoute(Game::route) || Game::createNewRoutes)){
        Game::window->show();
    } else {
        Game::loadWindow->show();
    }
}

bool Game::loadConEditor(){
    //if(Game::warningBox){
    //    QMessageBox msgBox;
    //    msgBox.setText("This is experimental version.\nUsing it may seriously damage your consists."
    //                   "\nMake backup first!\n\nTo disable this window, set 'warningBox = false' in settings.txt.");
    //    msgBox.setIcon(QMessageBox::Warning);
    //    msgBox.exec();
    //}
    CELoadWindow* ceLoadWindow = new CELoadWindow();
    ceLoadWindow->show();
    //ConEditorWindow* cwindow = new ConEditorWindow();
    //cwindow->resize(1280, 720);
    //cwindow->show();
}
*/
bool Game::checkRoot(QString dir){
    QString path;
    path = dir + "/routes";
    path.replace("//", "/");
    QFile file(path);
    if (!file.exists()) {
        qDebug () << "/routes not exist: "<< file.fileName();
        return false;
    }
    path = dir + "/global";
    path.replace("//", "/");
    file.setFileName(path);
    if (!file.exists()) {
        qDebug () << "/global not exist: "<< file.fileName();
        return false;
    }
    path = dir + "/global/tsection.dat";
    path.replace("//", "/");
    file.setFileName(path);
    if (!file.exists()) {
        qDebug () << "/global/tsection.dat not exist: "<< file.fileName();
        return false;
    }
    
    return true;
}

bool Game::checkCERoot(QString dir){
    QString path;
    path = dir + "/trains";
    path.replace("//", "/");
    QFile file(path);
    if (!file.exists()) {
        qDebug () << "/trains not exist: "<< file.fileName();
        return false;
    }
    path = dir + "/trains/trainset";
    path.replace("//", "/");
    file.setFileName(path);
    if (!file.exists()) {
        qDebug () << "/trains/trainset not exist: " << file.fileName();
        return false;
    }
    path = dir + "/trains/consists";
    path.replace("//", "/");
    file.setFileName(path);
    if (!file.exists()) {
        qDebug () << "/trains/consists not exist: " << file.fileName();
        return false;
    }
    
    return true;
}

bool Game::checkRoute(QString dir){
    QFile file;
    file.setFileName(Game::root+"/routes/"+dir+"/"+dir+".trk");
    if(file.exists()){
        Game::trkName = dir;
        return true;
    }
    QDir folder(Game::root+"/routes/"+dir+"/");
    folder.setNameFilters(QStringList() << "*.trk");
    folder.setFilter(QDir::Files);
    foreach(QString dirFile, folder.entryList()){
        Game::trkName = dirFile.split(".")[0];
        //qDebug() << Game::trkName;
        return true;
    }
    
    //qDebug() << file.fileName();
    //qDebug() << file.exists();
    return false;
}

bool Game::checkRemoteRoute(QString dir){
    QFile file;
    file.setFileName(Game::root+"/routes/"+dir);
    if(file.exists()){
        //Game::trkName = dir;
        return true;
    }
    return false;
}

template<class T>
void Game::check_coords(T&& x, T&& z, float* p) {
    if (p[0] >= 1024) {
        p[0] -= 2048;
        x++;
    }
    if (p[0] < -1024) {
        p[0] += 2048;
        x--;
    }
    if (p[2] >= 1024) {
        p[2] -= 2048;
        z++;
    }
    if (p[2] < -1024) {
        p[2] += 2048;
        z--;
    }
}
template void Game::check_coords(int& x, int& z, float* p);
template void Game::check_coords(float& x, float& z, float* p);

template<class T, class K>
void Game::check_coords(T&& x, T&& z, K&& px, K&& pz) {
    if (px >= 1024) {
        px -= 2048;
        x++;
    }
    if (px < -1024) {
        px += 2048;
        x--;
    }
    if (pz >= 1024) {
        pz -= 2048;
        z++;
    }
    if (pz < -1024) {
        pz += 2048;
        z--;
    }
}
template void Game::check_coords(int& x, int& z, int& px, int& pz);
template void Game::check_coords(int& x, int& z, float& px, float& pz);
template void Game::check_coords(float& x, float& z, float& px, float& pz);

void Game::CreateNewSettingsFile(){
    QFile file;
    QTextStream out;
    QString filepath;

    filepath = "./settings.txt";
    file.setFileName(filepath);
    //qDebug() << filepath;
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file);
    out << "consoleOutput = false\n";
    out << "\n";
    out << "# main directory of your game data\n";
    out << "gameRoot = F:/train simulator\n";
    out << "\n";
    out << "# route directory name to load on startup by default\n";
    out << "routeName = asdasasdasd1233\n";
    out << "\n";
    out << "# optional start tile\n";
    out << "#startTileX = -5306\n";
    out << "#startTileY = 14961\n";
    out << "\n";
    out << "# route edit\n";
    out << "#createNewIfNotExist = true\n";
    out << "writeEnabled = true\n";
    out << "writeTDB = true\n";
    out << "#deleteTrWatermarks = true\n";
    out << "#deleteViewDbSpheres = true\n";
    out << "\n";
    out << "# geo data\n";
    out << "geoPath = F:/hgst\n";
    out << "\n";
    out << "# misc\n";
    out << "#systemTheme = true\n";
    out << "#colorConView = #FF0000\n";
    out << "#colorShapeView = #00FF00\n";
    out << "#toolsHidden = true\n";
    out << "usenNumPad = true\n";
    out << "tileLod = 2\n";
    out << "objectLod = 4000\n";
    out << "maxObjLag = 10\n";
    out << "allowObjLag = 1000\n";
    out << "#cameraFov = 20.0\n";
    out << "leaveTrackShapeAfterDelete = false\n";
    out << "#renderTrItems = true\n";
    out << "#useImperial = false\n";
    out << "#ortsEngEnable = false\n";
    out << "#oglDefaultLineWidth = 2\n";
    out << "shadowsEnabled = 1\n";
    out << "#shadowMapSize = 8192\n";
    out << "#textureQuality = 4\n";
    out << "ignoreMissingGlobalShapes = true\n";
    out << "snapableOnlyRot = false\n";
    out << "imageMapsUrl = \n";
    out << "#AASamples = 16\n";
    out << "#mapImageResolution = 2048\n";
    out << "#cameraStickToTerrain = true\n";
    out << "#mouseSpeed = 0.1\n";
    out << "#mainWindowLayout = W\n";
    out << "#ceindowLayout = CU1\n";
    out << "#useQuadTree = false\n";
    out << "#fogColor = #D0D0FF\n";
    out << "#fogDensity = 0.5\n";
    out << "#defaultElevationBox = 0\n";
    out << "#defaultMoveStep = 0.25\n";
    out.flush();
    file.close();
}

void Game::CheckForOpenAl(){
    
    QString openalfilename;
    QString Url;
    
    QFile file("./"+openalfilename);
    if(file.exists())
        return;
    
#ifdef Q_PROCESSOR_X86_64
    openalfilename = "OpenAL32.dll";
    Url = "http://koniec.org/tsre5/data/openal/Win64/soft_oal.dll";
#endif

#ifdef Q_PROCESSOR_X86_32
    openalfilename = "openal32.dll";
    Url = "http://koniec.org/tsre5/data/openal/Win32/soft_oal.dll";
#endif
    
    QNetworkAccessManager* mgr = new QNetworkAccessManager();
    //connect(mgr, SIGNAL(finished(QNetworkReply*)), this, SLOT(isData(QNetworkReply*)));
    qDebug() << "Wait ..";

    QNetworkRequest req;
    req.setUrl(QUrl(Url));
    qDebug() << req.url();
    QNetworkReply* r = mgr->get(req);
    QEventLoop loop;
    QObject::connect(r, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    qDebug() << "Network Reply Loop End";
    QByteArray data = r->readAll();

    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
}

void Game::DownloadAppData(QString path){
    QDir().mkdir(path);
    
    // Download and extract AppData
    QNetworkAccessManager* mgr = new QNetworkAccessManager();
    qDebug() << "Wait ..";
    QString Url = "http://koniec.org/tsre5/data/appdata/"+ Game::AppDataVersion + ".tar";
    qDebug() << Url;
    QNetworkRequest req;//(QUrl(Url));
    req.setUrl(QUrl(Url));
    qDebug() << req.url();
    QNetworkReply* r = mgr->get(req);
    QEventLoop loop;
    QObject::connect(r, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    
    qDebug() << "Network Reply Loop End";
    QByteArray data = r->readAll();
    FileBuffer *fileData = new FileBuffer((unsigned char*)data.data(), data.length());
    TarFile tarFile(fileData);
    tarFile.extractTo("./tsre_appdata/");
    
    // Create bat file for Consist Editor.
    QString conBatFile = QFileInfo(QCoreApplication::applicationFilePath()).fileName()+" --conedit";
    QFile file1("./ConsistEditor.bat");
    file1.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out;
    out.setDevice(&file1);
    out << conBatFile;
    out.flush();
    file1.close();
    
    // Create bat file for Shape Viewer.
    conBatFile = QFileInfo(QCoreApplication::applicationFilePath()).fileName()+" --shapeview";
    QFile file2("./ShapeViewer.bat");
    file2.open(QIODevice::WriteOnly | QIODevice::Text);
    out.setDevice(&file2);
    out << conBatFile;
    out.flush();
    file2.close();
}