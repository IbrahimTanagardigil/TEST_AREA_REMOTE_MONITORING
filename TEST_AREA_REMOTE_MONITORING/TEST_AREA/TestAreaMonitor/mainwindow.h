#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#define defOK       0
#define defERROR    1
#define defINIT     2


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ushort ParseStr(QString tex);

private slots:
    void CommunicationLostDescription(ushort zone);
    void ZoneStructDescription(ushort zone);
    void ZoneUpdate(ushort zone);
    void timer_update();

private:
    Ui::MainWindow *ui;

};

extern ushort WhichZone;

#endif // MAINWINDOW_H
