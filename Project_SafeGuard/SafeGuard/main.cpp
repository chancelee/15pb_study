#include "mainwindow.h"
#include "cmynativeeventfilter.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    CMyNativeEventFilter filter_obj;
    a.installNativeEventFilter(&filter_obj);

    if (::RegisterHotKey((HWND)w.winId(), 0xa001, MOD_ALT, VK_F8)) {
        qDebug() << "Success Register Hotkey" << endl;
    } else {
        qDebug() << "Failed Register Hotkey" << endl;
    }

    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

    int result = a.exec();

    qDebug() << "Return";

    return result;
}
