#include "cmynativeeventfilter.h"

CMyNativeEventFilter::CMyNativeEventFilter():
    m_IsWindowHide(true){
}


bool CMyNativeEventFilter::
nativeEventFilter(const QByteArray &eventType, void *message, long *result) {
    MSG* pMsg = reinterpret_cast<MSG*>(message);

    if((pMsg->message == WM_HOTKEY) && (pMsg->wParam == 0xa001)) {
        if (m_IsWindowHide == TRUE) {
            ShowWindow(pMsg->hwnd, SW_HIDE);
            m_IsWindowHide = FALSE;
        } else {
            ShowWindow(pMsg->hwnd, SW_SHOW);
            m_IsWindowHide = TRUE;
        }
        return true;
    }

    return false;
}
