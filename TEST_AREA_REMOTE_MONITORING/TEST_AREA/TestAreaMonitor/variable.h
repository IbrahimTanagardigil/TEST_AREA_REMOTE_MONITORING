#ifndef VARIABLE_H
#define VARIABLE_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkInterface>


#define defZONECOUNT    15

#define defMultiConnected   3
#define defDisconnected     2
#define defConnected        1
#define defNeverConnect     0


#define defNoConnection     0
#define defOneConnection    1


extern unsigned short CommunicationStatus;
extern unsigned short oldCommunicationStatus;
extern unsigned short ConnectedCounter;

extern unsigned short ConnectionTimer;
extern unsigned short DataReceiveTimer;

struct WidgetInitiliaze
{
    QString type;
    QString label;
    QString unit;

    unsigned short WidgetNumber;
    unsigned short minValue;
    unsigned short maxValue;
    unsigned short nminValue;
    unsigned short nmaxValue;
    bool visible;
    bool init;
};
extern struct WidgetInitiliaze WidgetInit[defZONECOUNT+1];  //buff[0] kullanılmıyor.

struct WidgetDatas
{
    QString commcount;

    unsigned short WidgetNumber;
    unsigned short Value;
    unsigned short gData1;
};
extern struct WidgetDatas WidgetData[defZONECOUNT+1];

#endif // VARIABLE_H
