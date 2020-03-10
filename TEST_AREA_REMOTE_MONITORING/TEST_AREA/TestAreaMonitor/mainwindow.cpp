#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QDebug"
#include <QTimer>
#include <QStackedWidget>
#include <QGraphicsView>
#include <QTime>
#include <QNetworkInterface>
#include <QDir>
#include "TcpServer/myserver.h"
#include "variable.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qDebug() << "Program started!";

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_update()));
    timer->start(50);


    for( ushort WhichZone = 1; WhichZone < (defZONECOUNT+1); WhichZone++)
    {
        ZoneUpdate(WhichZone);
    }

    //Alert.png location
    ui->Zone8_4_Alert_Icon->setIcon(QIcon(QDir::homePath()+"/Documents/QT_CODES/TEST_AREA/RaspberryPi3/photo/alert.png"));
    //ui->Zone8_4_Alert_Icon->setIcon(QIcon("/opt/TEST_AREA/alert.png"));           //for raspberry pi

    ui->Zone8_4_Alert_Icon->setStyleSheet("border-style: solid;");
    ui->Zone8_4_Alert_Icon->setIconSize(QSize(ui->Zone8_4_Alert_Icon->size().width(),ui->Zone8_4_Alert_Icon->size().height()));

    ConnectedCounter = 0;

    DataReceiveTimer = 0;
    ConnectionTimer = 1;                      //50 ms x 1 = 50 ms
    CommunicationStatus = defNeverConnect;



}


ushort MainWindow::ParseStr(QString str)
{
    #define def_max_variable    13
    #define def_first_queque    0

    ushort ans = defOK;

    ushort InitZoneNumber = 65535;
    ushort DataZoneNumber = 65535;

    QString Word[2];
    QStringList FullSentence = str.split(';');
    QString VariableList[def_max_variable] =
    {
        /*0*/   "inr",              // init zone (15) number
        /*1*/   "tip",              // init type
        /*2*/   "min",              // init minimum value
        /*3*/   "max",              // init maximum value
        /*4*/   "nmin",             // init nominal (green zone) minimum value
        /*5*/   "nmax",             // init nominal (green zone) maximum value
        /*6*/   "text",             // init parameter text
        /*7*/   "birim",            // init unit
        /*8*/   "visible",          // init zone (15) visible true or false

        /*9*/   "dnr",              // data zone (15) number
        /*10*/  "gval1",            // data general value 1
        /*11*/  "ccount",           // data connection count
        /*12*/  "val"               // data value
    };

    ans = defOK;
    for( ushort splitcounter = 0;splitcounter < FullSentence.size();splitcounter++ )
    {
        for( ushort ringcounter = 0;ringcounter < def_max_variable;ringcounter++ )
        {
            if( FullSentence[splitcounter].contains(VariableList[ringcounter]) == true )
            {
                if( FullSentence[splitcounter].split(':')[0] == VariableList[ringcounter] )
                {
                    Word[0] = FullSentence[splitcounter].split(':')[0];
                    Word[1] = FullSentence[splitcounter].split(':')[1];

                    switch (ringcounter)
                    {
                        case 0:
                                /*gelen string te inr degeri ilk sirada olmali*/
                                if ( splitcounter == def_first_queque )
                                {
                                    InitZoneNumber = Word[1].toUShort();

                                    if( InitZoneNumber <= defZONECOUNT )
                                    {
                                        WidgetInit[InitZoneNumber].WidgetNumber = InitZoneNumber;
                                        WidgetInit[InitZoneNumber].init = true;
                                    }
                                }
                        break;
                        case 1:
                                if( InitZoneNumber <= defZONECOUNT )
                                    WidgetInit[InitZoneNumber].type = Word[1];
                        break;
                        case 2:
                                if( InitZoneNumber <= defZONECOUNT )
                                     WidgetInit[InitZoneNumber].minValue = Word[1].toUShort();
                        break;
                        case 3:
                                if( InitZoneNumber <= defZONECOUNT )
                                    WidgetInit[InitZoneNumber].maxValue = Word[1].toUShort();
                        break;
                        case 4:
                                if( InitZoneNumber <= defZONECOUNT )
                                    WidgetInit[InitZoneNumber].nminValue = Word[1].toUShort();
                        break;
                        case 5:
                                if( InitZoneNumber <= defZONECOUNT )
                                    WidgetInit[InitZoneNumber].nmaxValue = Word[1].toUShort();
                        break;
                        case 6:
                                if( InitZoneNumber <= defZONECOUNT )
                                    WidgetInit[InitZoneNumber].label = Word[1];
                        break;
                        case 7:
                                if( InitZoneNumber <= defZONECOUNT )
                                    WidgetInit[InitZoneNumber].unit = Word[1];
                        break;
                        case 8:
                                if( InitZoneNumber <= defZONECOUNT )
                                {
                                    if( Word[1] == "true" )
                                        WidgetInit[InitZoneNumber].visible = true;

                                    else if( Word[1] == "false" )
                                    {
                                        WidgetInit[InitZoneNumber].visible = false;                                        
                                        WidgetInit[InitZoneNumber].init = false;
                                    }
                                }
                        break;
                        case 9:
                                /*gelen string te dnr degeri ilk sirada degilse gelen string sirasinda sorun var. node-red i kontrol et.*/
                                if ( splitcounter == def_first_queque )
                                {
                                    DataZoneNumber = Word[1].toUShort();

                                    if( WidgetInit[DataZoneNumber].init == false )
                                    {
                                        ans = defINIT;
                                        break;
                                    }
                                    //if( DataZoneNumber != 0 )
                                    //    WidgetData[0].commcount = "";

                                    if( DataZoneNumber <= defZONECOUNT )
                                        WidgetData[DataZoneNumber].WidgetNumber = DataZoneNumber;
                                }
                        break;
                        case 10:
                                if( DataZoneNumber <= defZONECOUNT )
                                    WidgetData[DataZoneNumber].gData1 = Word[1].toUShort();
                        break;
                        case 11:
                                if( DataZoneNumber <= defZONECOUNT )
                                    WidgetData[DataZoneNumber].commcount = Word[1];
                        break;
                        case 12:
                                if( DataZoneNumber <= defZONECOUNT )
                                    WidgetData[DataZoneNumber].Value = Word[1].toUShort();
                        break;
                        default:
                        break;
                    }

                    break;
                }
            }
        }
    }
    if( (InitZoneNumber > defZONECOUNT) && (DataZoneNumber > defZONECOUNT) )
        ans = defERROR;

    return ans;
}



MainWindow::~MainWindow()
{
    delete ui;
}

struct widgets
{
   QLabel* text_label;
   QLabel* text_unit;
   QLineEdit* text_value;

   QLabel* alfa_label;
   QLabel* alfa_unit;
   QLCDNumber* alfa_value;

   QLabel* gauge1_label;
   QGauge* gauge1;

   QStackedWidget* zone;

   ushort flag;
   ushort commlostflag;
}WIDGETS[(defZONECOUNT+1)];

/*
 * ui->Zone1                        -   QStackedWidget
 *
 * ui->Zone1_1_text_label           -   QLabel              QLabel* text_label;
 * ui->Zone1_1_text_unit            -   QLabel              QLabel* text_unit;
 * ui->Zone1_1_text_value           -   QLineEdit           QLineEdit* text_value;
 *
 * ui->Zone1_2_alfanumeric_label    -   QLabel              QLabel* alfa_label;
 * ui->Zone1_2_alfanumeric_unit     -   QLabel              QLabel* alfa_unit;
 * ui->Zone1_2_alfanumeric_value    -   QLCDNumber          QLCDNumber* alfa_value;
 *
 * ui->Zone1_3_gauge1_label         -   QLabel              QLabel* gauge1_label;
 * ui->Zone1_3_gauge_1_widget       -   QGauge              QGauge* gauge1;
 *
 * ui->Zone1                        -   QStackedWidget      QStackedWidget* zone;
 */


void MainWindow::ZoneStructDescription(ushort zone)
{
    switch(zone)
    {
        case 1:
                WIDGETS[1].text_label   = ui->Zone1_1_text_label;
                WIDGETS[1].text_unit    = ui->Zone1_1_text_unit;
                WIDGETS[1].text_value   = ui->Zone1_1_text_value;

                WIDGETS[1].alfa_label   = ui->Zone1_2_alfanumeric_label;
                WIDGETS[1].alfa_unit    = ui->Zone1_2_alfanumeric_unit;
                WIDGETS[1].alfa_value   = ui->Zone1_2_alfanumeric_value;

                WIDGETS[1].gauge1_label = ui->Zone1_3_gauge1_label;
                WIDGETS[1].gauge1       = ui->Zone1_3_gauge1_widget;

                WIDGETS[1].zone         = ui->Zone1;
        break;
        case 2:
                WIDGETS[2].text_label   = ui->Zone2_1_text_label;
                WIDGETS[2].text_unit    = ui->Zone2_1_text_unit;
                WIDGETS[2].text_value   = ui->Zone2_1_text_value;

                WIDGETS[2].alfa_label   = ui->Zone2_2_alfanumeric_label;
                WIDGETS[2].alfa_unit    = ui->Zone2_2_alfanumeric_unit;
                WIDGETS[2].alfa_value   = ui->Zone2_2_alfanumeric_value;

                WIDGETS[2].gauge1_label = ui->Zone2_3_gauge1_label;
                WIDGETS[2].gauge1       = ui->Zone2_3_gauge1_widget;

                WIDGETS[2].zone         = ui->Zone2;
        break;
        case 3:
                WIDGETS[3].text_label   = ui->Zone3_1_text_label;
                WIDGETS[3].text_unit    = ui->Zone3_1_text_unit;
                WIDGETS[3].text_value   = ui->Zone3_1_text_value;

                WIDGETS[3].alfa_label   = ui->Zone3_2_alfanumeric_label;
                WIDGETS[3].alfa_unit    = ui->Zone3_2_alfanumeric_unit;
                WIDGETS[3].alfa_value   = ui->Zone3_2_alfanumeric_value;

                WIDGETS[3].gauge1_label = ui->Zone3_3_gauge1_label;
                WIDGETS[3].gauge1       = ui->Zone3_3_gauge1_widget;

                WIDGETS[3].zone         = ui->Zone3;
        break;
        case 4:
                WIDGETS[4].text_label   = ui->Zone4_1_text_label;
                WIDGETS[4].text_unit    = ui->Zone4_1_text_unit;
                WIDGETS[4].text_value   = ui->Zone4_1_text_value;

                WIDGETS[4].alfa_label   = ui->Zone4_2_alfanumeric_label;
                WIDGETS[4].alfa_unit    = ui->Zone4_2_alfanumeric_unit;
                WIDGETS[4].alfa_value   = ui->Zone4_2_alfanumeric_value;

                WIDGETS[4].gauge1_label = ui->Zone4_3_gauge1_label;
                WIDGETS[4].gauge1       = ui->Zone4_3_gauge1_widget;

                WIDGETS[4].zone         = ui->Zone4;
        break;
        case 5:
                WIDGETS[5].text_label   = ui->Zone5_1_text_label;
                WIDGETS[5].text_unit    = ui->Zone5_1_text_unit;
                WIDGETS[5].text_value   = ui->Zone5_1_text_value;

                WIDGETS[5].alfa_label   = ui->Zone5_2_alfanumeric_label;
                WIDGETS[5].alfa_unit    = ui->Zone5_2_alfanumeric_unit;
                WIDGETS[5].alfa_value   = ui->Zone5_2_alfanumeric_value;

                WIDGETS[5].gauge1_label = ui->Zone5_3_gauge1_label;
                WIDGETS[5].gauge1       = ui->Zone5_3_gauge1_widget;

                WIDGETS[5].zone         = ui->Zone5;
        break;
        case 6:
                WIDGETS[6].text_label   = ui->Zone6_1_text_label;
                WIDGETS[6].text_unit    = ui->Zone6_1_text_unit;
                WIDGETS[6].text_value   = ui->Zone6_1_text_value;

                WIDGETS[6].alfa_label   = ui->Zone6_2_alfanumeric_label;
                WIDGETS[6].alfa_unit    = ui->Zone6_2_alfanumeric_unit;
                WIDGETS[6].alfa_value   = ui->Zone6_2_alfanumeric_value;

                WIDGETS[6].gauge1_label = ui->Zone6_3_gauge1_label;
                WIDGETS[6].gauge1       = ui->Zone6_3_gauge1_widget;

                WIDGETS[6].zone         = ui->Zone6;
        break;
        case 7:
                WIDGETS[7].text_label   = ui->Zone7_1_text_label;
                WIDGETS[7].text_unit    = ui->Zone7_1_text_unit;
                WIDGETS[7].text_value   = ui->Zone7_1_text_value;

                WIDGETS[7].alfa_label   = ui->Zone7_2_alfanumeric_label;
                WIDGETS[7].alfa_unit    = ui->Zone7_2_alfanumeric_unit;
                WIDGETS[7].alfa_value   = ui->Zone7_2_alfanumeric_value;

                WIDGETS[7].gauge1_label = ui->Zone7_3_gauge1_label;
                WIDGETS[7].gauge1       = ui->Zone7_3_gauge1_widget;

                WIDGETS[7].zone         = ui->Zone7;
        break;
        case 8:
                WIDGETS[8].text_label   = ui->Zone8_1_text_label;
                WIDGETS[8].text_unit    = ui->Zone8_1_text_unit;
                WIDGETS[8].text_value   = ui->Zone8_1_text_value;

                WIDGETS[8].alfa_label   = ui->Zone8_2_alfanumeric_label;
                WIDGETS[8].alfa_unit    = ui->Zone8_2_alfanumeric_unit;
                WIDGETS[8].alfa_value   = ui->Zone8_2_alfanumeric_value;

                WIDGETS[8].gauge1_label = ui->Zone8_3_gauge1_label;
                WIDGETS[8].gauge1       = ui->Zone8_3_gauge1_widget;

                WIDGETS[8].zone         = ui->Zone8;
        break;
        case 9:
                WIDGETS[9].text_label   = ui->Zone9_1_text_label;
                WIDGETS[9].text_unit    = ui->Zone9_1_text_unit;
                WIDGETS[9].text_value   = ui->Zone9_1_text_value;

                WIDGETS[9].alfa_label   = ui->Zone9_2_alfanumeric_label;
                WIDGETS[9].alfa_unit    = ui->Zone9_2_alfanumeric_unit;
                WIDGETS[9].alfa_value   = ui->Zone9_2_alfanumeric_value;

                WIDGETS[9].gauge1_label = ui->Zone9_3_gauge1_label;
                WIDGETS[9].gauge1       = ui->Zone9_3_gauge1_widget;

                WIDGETS[9].zone         = ui->Zone9;
        break;
        case 10:
                WIDGETS[10].text_label   = ui->Zone10_1_text_label;
                WIDGETS[10].text_unit    = ui->Zone10_1_text_unit;
                WIDGETS[10].text_value   = ui->Zone10_1_text_value;

                WIDGETS[10].alfa_label   = ui->Zone10_2_alfanumeric_label;
                WIDGETS[10].alfa_unit    = ui->Zone10_2_alfanumeric_unit;
                WIDGETS[10].alfa_value   = ui->Zone10_2_alfanumeric_value;

                WIDGETS[10].gauge1_label = ui->Zone10_3_gauge1_label;
                WIDGETS[10].gauge1       = ui->Zone10_3_gauge1_widget;

                WIDGETS[10].zone         = ui->Zone10;
        break;
        case 11:
                WIDGETS[11].text_label   = ui->Zone11_1_text_label;
                WIDGETS[11].text_unit    = ui->Zone11_1_text_unit;
                WIDGETS[11].text_value   = ui->Zone11_1_text_value;

                WIDGETS[11].alfa_label   = ui->Zone11_2_alfanumeric_label;
                WIDGETS[11].alfa_unit    = ui->Zone11_2_alfanumeric_unit;
                WIDGETS[11].alfa_value   = ui->Zone11_2_alfanumeric_value;

                WIDGETS[11].gauge1_label = ui->Zone11_3_gauge1_label;
                WIDGETS[11].gauge1       = ui->Zone11_3_gauge1_widget;

                WIDGETS[11].zone         = ui->Zone11;
        break;
        case 12:
                WIDGETS[12].text_label   = ui->Zone12_1_text_label;
                WIDGETS[12].text_unit    = ui->Zone12_1_text_unit;
                WIDGETS[12].text_value   = ui->Zone12_1_text_value;

                WIDGETS[12].alfa_label   = ui->Zone12_2_alfanumeric_label;
                WIDGETS[12].alfa_unit    = ui->Zone12_2_alfanumeric_unit;
                WIDGETS[12].alfa_value   = ui->Zone12_2_alfanumeric_value;

                WIDGETS[12].gauge1_label = ui->Zone12_3_gauge1_label;
                WIDGETS[12].gauge1       = ui->Zone12_3_gauge1_widget;

                WIDGETS[12].zone         = ui->Zone12;
        break;
        case 13:
                WIDGETS[13].text_label   = ui->Zone13_1_text_label;
                WIDGETS[13].text_unit    = ui->Zone13_1_text_unit;
                WIDGETS[13].text_value   = ui->Zone13_1_text_value;

                WIDGETS[13].alfa_label   = ui->Zone13_2_alfanumeric_label;
                WIDGETS[13].alfa_unit    = ui->Zone13_2_alfanumeric_unit;
                WIDGETS[13].alfa_value   = ui->Zone13_2_alfanumeric_value;

                WIDGETS[13].gauge1_label = ui->Zone13_3_gauge1_label;
                WIDGETS[13].gauge1       = ui->Zone13_3_gauge1_widget;

                WIDGETS[13].zone         = ui->Zone13;
        break;
        case 14:
                WIDGETS[14].text_label   = ui->Zone14_1_text_label;
                WIDGETS[14].text_unit    = ui->Zone14_1_text_unit;
                WIDGETS[14].text_value   = ui->Zone14_1_text_value;

                WIDGETS[14].alfa_label   = ui->Zone14_2_alfanumeric_label;
                WIDGETS[14].alfa_unit    = ui->Zone14_2_alfanumeric_unit;
                WIDGETS[14].alfa_value   = ui->Zone14_2_alfanumeric_value;

                WIDGETS[14].gauge1_label = ui->Zone14_3_gauge1_label;
                WIDGETS[14].gauge1       = ui->Zone14_3_gauge1_widget;

                WIDGETS[14].zone         = ui->Zone14;
        break;
        case 15:
                WIDGETS[15].text_label   = ui->Zone15_1_text_label;
                WIDGETS[15].text_unit    = ui->Zone15_1_text_unit;
                WIDGETS[15].text_value   = ui->Zone15_1_text_value;

                WIDGETS[15].alfa_label   = ui->Zone15_2_alfanumeric_label;
                WIDGETS[15].alfa_unit    = ui->Zone15_2_alfanumeric_unit;
                WIDGETS[15].alfa_value   = ui->Zone15_2_alfanumeric_value;

                WIDGETS[15].gauge1_label = ui->Zone15_3_gauge1_label;
                WIDGETS[15].gauge1       = ui->Zone15_3_gauge1_widget;

                WIDGETS[15].zone         = ui->Zone15;
        break;
        default:
        break;
    }
}
void MainWindow::CommunicationLostDescription(ushort zone)
{
    static int first_in = 0;
    if( ConnectionTimer > 0 )
    {
        ConnectionTimer--;
        if( ConnectionTimer == 0 )
        {
            first_in = 0;
            DataReceiveTimer = 0;
            if( CommunicationStatus == defNeverConnect )
            {
                //qDebug() << "(4) never communication";
                WidgetData[0].commcount = "never connections";
            }
            else if( CommunicationStatus == defDisconnected )
            {
                //qDebug() << "(1) communication disconnected";
                WidgetData[0].commcount = "0 connections";
            }
            else if( CommunicationStatus == defMultiConnected )
            {
                //qDebug() << "(2) multi communication occured";
                WidgetData[0].commcount = "multi connections";
            }
        }
    }

    if( CommunicationStatus == defConnected )
    {
        if( DataReceiveTimer > 0)
        {
            if( first_in == 0 )
            {
                //qDebug() << "(0) communication connected";
                WidgetData[0].commcount = "1 connection";
                first_in = 1;
            }

            DataReceiveTimer--;
            if( DataReceiveTimer == 0 )
            {
                //qDebug() << "(3) communication connected and no data input";
                WidgetData[0].commcount = "1 connection - no data";
                first_in = 0;
            }
        }
    }
}

void MainWindow::ZoneUpdate(ushort zone)
{
    static ushort ip_flag = 0;
    CommunicationLostDescription(zone);
    ZoneStructDescription(zone);

    if( ip_flag == 0 )
    {
        const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
        for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            {
                if( address.toString() != "" )
                    ip_flag = 1;
                 ui->Zone8_4_IP_ADDRESS->setText(address.toString());
            }
        }
    }

    if( WidgetData[0].commcount == "0 connections" )
    {
        for( ushort i = 1;i < (defZONECOUNT+1);i++ )
        {
            if( i != 8 )
                WIDGETS[i].zone->setVisible(false);
            WidgetInit[i].init = false;
            WidgetInit[i].type = "";
            WidgetData[i].Value = 0;
        }
        ui->Zone8_4_Alert->setText("CLIENT COMMMUNICATION LOST");
        WIDGETS[8].zone->setCurrentIndex(3);
        WIDGETS[8].zone->setVisible(true);
        WIDGETS[8].flag = 0;
    }
    else if( WidgetData[0].commcount == "multi connections" )
    {
        for( ushort i = 1;i < (defZONECOUNT+1);i++ )
        {
            if( i != 8 )
                WIDGETS[i].zone->setVisible(false);
            WidgetInit[i].init = false;
            WidgetInit[i].type = "";
            WidgetData[i].Value = 0;
        }
        ui->Zone8_4_Alert->setText("MULTI CONNECTION PROBLEM");
        WIDGETS[8].zone->setCurrentIndex(3);
        WIDGETS[8].zone->setVisible(true);
        WIDGETS[8].flag = 0;
    }
    else if( WidgetData[0].commcount == "never connections" )
    {
        for( ushort i = 1;i < (defZONECOUNT+1);i++ )
        {
            if( i != 8 )
                WIDGETS[i].zone->setVisible(false);
            WidgetInit[i].init = false;
            WidgetInit[i].type = "";
            WidgetData[i].Value = 0;
        }
        ui->Zone8_4_Alert->setText("NEVER CONNECTED");
        WIDGETS[8].zone->setCurrentIndex(3);
        WIDGETS[8].zone->setVisible(true);
        WIDGETS[8].flag = 0;
    }
    else if( WidgetData[0].commcount == "1 connection - no data" )
    {
        for( ushort i = 1;i < (defZONECOUNT+1);i++ )
        {
            if( i != 8 )
                WIDGETS[i].zone->setVisible(false);
            WidgetInit[i].init = false;
            WidgetInit[i].type = "";
            WidgetData[i].Value = 0;
        }
        ui->Zone8_4_Alert->setText("NO DATA INPUT");
        WIDGETS[8].zone->setCurrentIndex(3);
        WIDGETS[8].zone->setVisible(true);
        WIDGETS[8].flag = 0;
    }
    else if( WidgetData[0].commcount == "1 connection" )
    {
        if( WidgetInit[zone].type == "text" )
        {
            WIDGETS[zone].flag = 0;

            WIDGETS[zone].text_label->setText(WidgetInit[zone].label);
            WIDGETS[zone].text_unit->setText(WidgetInit[zone].unit);
            WIDGETS[zone].text_value->setText(QString::number(WidgetData[zone].Value,10));

            if( (WidgetData[zone].Value >= WidgetInit[zone].nminValue) && (WidgetData[zone].Value <= WidgetInit[zone].nmaxValue) )
                WIDGETS[zone].text_value->setStyleSheet("color: green;");
            else if( WidgetData[zone].Value > WidgetInit[zone].nmaxValue )
                WIDGETS[zone].text_value->setStyleSheet("color: red;");
            else if( WidgetData[zone].Value < WidgetInit[zone].nminValue )
                WIDGETS[zone].text_value->setStyleSheet("color: #A6A600;");

            WIDGETS[zone].zone->setCurrentIndex(0);
            WIDGETS[zone].zone->setVisible(WidgetInit[zone].visible);
        }
        else if( WidgetInit[zone].type == "alfa" )
        {
            WIDGETS[zone].flag = 0;

            WIDGETS[zone].alfa_label->setText(WidgetInit[zone].label);
            WIDGETS[zone].alfa_unit->setText(WidgetInit[zone].unit);
            WIDGETS[zone].alfa_value->display(WidgetData[zone].Value);


            if( (WidgetData[zone].Value >= WidgetInit[zone].nminValue) && (WidgetData[zone].Value <= WidgetInit[zone].nmaxValue) )
                WIDGETS[zone].alfa_value->setStyleSheet("color: green;");
            else if( WidgetData[zone].Value > WidgetInit[zone].nmaxValue )
                WIDGETS[zone].alfa_value->setStyleSheet("color: red;");
            else if( WidgetData[zone].Value < WidgetInit[zone].nminValue )
                WIDGETS[zone].alfa_value->setStyleSheet("color: #A6A600;");


            WIDGETS[zone].zone->setCurrentIndex(1);
            WIDGETS[zone].zone->setVisible(WidgetInit[zone].visible);
        }
        else if( WidgetInit[zone].type == "gauge1" )
        {
            WIDGETS[zone].flag = 0;

            WIDGETS[zone].gauge1_label->setText(WidgetInit[zone].label);

            WIDGETS[zone].gauge1->setUnits("");
            WIDGETS[zone].gauge1->setLabel(WidgetInit[zone].unit);
            WIDGETS[zone].gauge1->setMinValue(WidgetInit[zone].minValue);
            WIDGETS[zone].gauge1->setMaxValue(WidgetInit[zone].maxValue);
            WIDGETS[zone].gauge1->setThreshold(WidgetInit[zone].nmaxValue);
            WIDGETS[zone].gauge1->setValue(WidgetData[zone].Value);
            WIDGETS[zone].zone->setCurrentIndex(2);
            WIDGETS[zone].zone->setVisible(WidgetInit[zone].visible);
        }
        else
        {
            if( WIDGETS[8].flag == 0 )
            {
                WIDGETS[8].flag = 1;
                WIDGETS[8].zone->setVisible(false);
            }
        }
    }
}
ushort WhichZone = 1;

void MainWindow::timer_update()
{
    ZoneUpdate(WhichZone);

    if( ++WhichZone > defZONECOUNT )
        WhichZone = 1;

}
