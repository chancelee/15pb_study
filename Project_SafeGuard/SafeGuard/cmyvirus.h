#ifndef CMYVIRUS_H
#define CMYVIRUS_H

#include <QAction>
#include <QMenuBar>
#include <QWidget>
#include <windows.h>
#include "csnapshot.h"

namespace Ui {
class CMyVirus;
}

class CMyVirus : public QWidget
{
    Q_OBJECT

public:
    explicit CMyVirus(QWidget *parent = 0);
    ~CMyVirus();
    void StorageMd5();
    LARGE_INTEGER EnumFoldAndDeleteVirus(WCHAR* lpFoldPath, WCHAR* lpFileType);

private slots:
    /* Md5 Button */
    void on_pushButton_clicked();
    /* Doc Button */
    void on_pushButton_2_clicked();
    /* Process/Disk Button */
    void on_pushButton_3_clicked();

    void trigerLightning();
    void trigerSnapShot();

private:
    Ui::CMyVirus *ui;
    QString m_md5;
    QMenuBar* m_menuBar;
    QMenu* m_menu;
    QAction* m_act_snapshot;
    QAction* m_act_lightning;
    CSnapShot m_snap_obj;
    BOOL TerminateProcessFromID(DWORD dwId);
};

#endif // CMYVIRUS_H
