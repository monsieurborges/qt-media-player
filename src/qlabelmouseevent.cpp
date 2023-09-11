#include "qlabelmouseevent.h"

QLabelMouseEvent::QLabelMouseEvent(QWidget *parent) : QLabel(parent)
{

}

void QLabelMouseEvent::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit mouseDoubleClick(event);
}

void QLabelMouseEvent::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
}

void QLabelMouseEvent::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event);
}
