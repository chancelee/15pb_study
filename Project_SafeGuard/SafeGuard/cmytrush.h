#ifndef CMYTRUSH_H
#define CMYTRUSH_H

#include <QMenuBar>
#include <QWidget>
#include <windows.h>

namespace Ui {
class CMyTrush;
}

class CMyTrush : public QWidget
{
    Q_OBJECT

public:
    explicit CMyTrush(QWidget *parent = 0);
    LARGE_INTEGER EnumFoldAndDeleteFile(WCHAR* lpFoldPath, WCHAR* lpFileType);
    ~CMyTrush();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void trigerBrowserClean();
    void trigerBrowserSearch();

private:
    Ui::CMyTrush *ui;
    bool IsDocSelected;
    QMenuBar* m_menuBar;
    QMenu* m_menu;
    QAction* m_act_BrowserClean;
    QAction* m_act_BrowserSearch;

};

#endif // CMYTRUSH_H
