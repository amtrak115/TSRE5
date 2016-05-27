#ifndef MAPWINDOW_H
#define	MAPWINDOW_H

#include <QtWidgets>
#include "OSM.h"

class QNetworkReply;
class QImage;
class IghCoordinate;
class LatitudeLongitudeCoordinate;
class PreciseTileCoordinate;
class QPushButton;

class MapWindow : public QDialog {
    Q_OBJECT
public:
    OSM dane;
    int tileX;
    int tileZ;
    
    static std::unordered_map<int, QImage*> mapTileImages;
    MapWindow();
    virtual ~MapWindow();
    void load(QByteArray* data = NULL);
    bool ok = false;
    void get();
    int exec();
    
public slots:
    void isData(QNetworkReply* r);
    void loadOSM();
    void colorComboActivated(QString val);

private:
    QLabel* imageLabel;
    float minlat, minlon, maxlat, maxlon;
    bool invert = false;
    IghCoordinate* igh = NULL;
    LatitudeLongitudeCoordinate* minLatlon = NULL;
    LatitudeLongitudeCoordinate* maxLatlon = NULL;
    PreciseTileCoordinate* aCoords = NULL;
    QPushButton *loadButton = NULL;
    void reload();
};

#endif	/* MAPWINDOW_H */
