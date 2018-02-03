#include "cmypushbutton.h"
#include <QCloseEvent>
#include <QDebug>

CMyPushButton::CMyPushButton(QWidget* parent):
    QPushButton(parent) {
}

CMyPushButton::~CMyPushButton() {
}

void CMyPushButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_Drag = true;
        m_DragPosition = event->globalPos() - this->pos();
        event->accept();
    }
}

void CMyPushButton::mouseMoveEvent(QMouseEvent *event) {
    if (m_Drag && (event->buttons() && Qt::LeftButton)) {
        move(event->globalPos() - m_DragPosition);
        event->accept();
    }
}

void CMyPushButton::mouseReleaseEvent(QMouseEvent *event) {
    char buf[128] = {};

    sprintf_s(buf, 128, "(%d, %d), (%d, %d)",
              event->pos().x(), event->pos().y(),
              event->globalPos().x(), event->globalPos().y());

    qDebug() << QString(buf) << endl;

    m_Drag = false;
}
