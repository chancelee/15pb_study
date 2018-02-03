#ifndef CMYSERVICE_H
#define CMYSERVICE_H

#include <QAction>
#include <QTreeWidget>
#include <QWidget>
#include <windows.h>

namespace Ui {
class CMyService;
}

class CMyService : public QWidget
{
    Q_OBJECT

public:
    explicit CMyService(QWidget *parent = 0);
    ~CMyService();
    void ServiceRoundRobin();

private slots:
    void trigerTerminateService();
    void trigerStartService();
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CMyService *ui;
    QAction* m_act_service_terminate;
    QAction* m_act_service_start;
    QTreeWidgetItem* m_curItem_terminate;
};

#endif // CMYSERVICE_H
