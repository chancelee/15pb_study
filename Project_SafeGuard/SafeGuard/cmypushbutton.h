#ifndef CMYPUSHBUTTON_H
#define CMYPUSHBUTTON_H

#include <QPushButton>
#include <QWidget>

class CMyPushButton : public QPushButton {
    Q_OBJECT

public:
    explicit CMyPushButton(QWidget* parent = 0);
    ~CMyPushButton();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    bool m_Drag; //��¼����Ƿ���
    QPoint m_DragPosition;//��¼���λ��
};

#endif // CMYPUSHBUTTON_H
