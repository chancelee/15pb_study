#ifndef CMYGRUB_H
#define CMYGRUB_H

#include <QWidget>
#include <vector>
#include <Windows.h>
#include <QTreeWidget>
#include <QTimer>
using std::vector;

namespace Ui {
class CMyGrub;
}

class CMyGrub : public QWidget
{
    Q_OBJECT

    typedef struct _SOFTINFO {
        WCHAR szSoftName[50];
        WCHAR szSoftVer[50];
        WCHAR szSoftDate[20];
        WCHAR szSoftSize[MAX_PATH];
        WCHAR strSoftInsPath[MAX_PATH];
        WCHAR strSoftUniPath[MAX_PATH];
        WCHAR strSoftVenRel[50];
        WCHAR strSoftIco[MAX_PATH];
        WCHAR strMidReg[MAX_PATH];
    }SOFTINFO, *PSOFTINFO;

    typedef struct _GRUBINFO {
        WCHAR szSoftName[MAX_PATH];
        WCHAR strSoftPath[MAX_PATH];
    }GRUBINFO, *PGRUBINFO;

public:
    explicit CMyGrub(QWidget *parent = 0);
    ~CMyGrub();
    bool EnumReg();
    bool EnumGrub();

private slots:
    void on_treeWidget_SoftUninstall_customContextMenuRequested(const QPoint &pos);

    void on_treeWidget_Boot_customContextMenuRequested(const QPoint &pos);

    void trigerUninstall();
    void trigerGrub();
    void FlushGrub();

private:
    Ui::CMyGrub *ui;
    vector<SOFTINFO> m_vectSoftInfo;
    vector<GRUBINFO> m_vectGrubInfo;
    QTreeWidgetItem* m_curItem_uninstall;
    QTreeWidgetItem* m_curItem_grub;
    QAction* m_act_uninstall;
    QAction* m_act_grub;
    QTimer* m_timer;
};

#endif // CMYGRUB_H
