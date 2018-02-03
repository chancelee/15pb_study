#ifndef CMYPROCESS_H
#define CMYPROCESS_H

#include <QAction>
#include <QTreeWidget>
#include <QWidget>
#include <vector>
#include <Windows.h>

using std::vector;

namespace Ui {
class CMyProcess;
}

class CMyProcess : public QWidget
{
    Q_OBJECT

public:
    explicit CMyProcess(QWidget *parent = 0);
    ~CMyProcess();
    BOOL TerminateProcessFromID(DWORD dwId);

private slots:
    void on_treeWidget_Process_customContextMenuRequested(const QPoint &pos);
    void trigerTerminateProcess();

private:
    Ui::CMyProcess *ui;
    QAction* m_act_process;
    QTreeWidgetItem* m_curItem_terminate;
};

#endif // CMYPROCESS_H
