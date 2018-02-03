#ifndef CMYNATIVEEVENTFILTER_H
#define CMYNATIVEEVENTFILTER_H

#include <QAbstractNativeEventFilter>
#include <QDebug>
#include <windows.h>

class CMyNativeEventFilter : public QAbstractNativeEventFilter
{
public:
    CMyNativeEventFilter();
    virtual bool nativeEventFilter(const QByteArray &eventType,
                                   void *message,
                                   long *result) Q_DECL_OVERRIDE;
    bool m_IsWindowHide;
};

#endif // CMYNATIVEEVENTFILTER_H
