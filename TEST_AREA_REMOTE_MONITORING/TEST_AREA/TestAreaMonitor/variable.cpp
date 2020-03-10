#include "variable.h"
#include <QNetworkInterface>

unsigned short CommunicationStatus = 0;
unsigned short oldCommunicationStatus = 0;
unsigned short ConnectedCounter = 0;

unsigned short ConnectionTimer = 0;
unsigned short DataReceiveTimer = 0;

WidgetDatas WidgetData[defZONECOUNT+1];
WidgetInitiliaze WidgetInit[defZONECOUNT+1];


