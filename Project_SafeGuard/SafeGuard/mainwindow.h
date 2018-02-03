#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cmynativeeventfilter.h"
#include "cmyram.h"
#include "cmyrocket.h"
#include <QAbstractNativeEventFilter>
#include <windows.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public CMyNativeEventFilter
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void PriviToken();
//    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow *ui;

private slots:
    void ShowProcess();
    void ShowService();
    void ShowRam();
    void ShowPE();
    void ShowRocket();
    void ShowCloud();
    void ShowTrash();
    void ShowGrub();
    void ShowVirus();
    void DestroyRocket();
    void my_shutdown();
    void my_reboot();
    void my_logout();
    void my_sleep();\
    void my_lock();

private:
    CMyRam* m_ram_obj;
    CMyRocket* m_rocket_obj;
};

#endif // MAINWINDOW_H
