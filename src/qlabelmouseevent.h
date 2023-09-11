#ifndef QLABELMOUSEEVENT_H
#define QLABELMOUSEEVENT_H

#include <QLabel>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>

class QLabelMouseEvent : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelMouseEvent(QWidget *parent=nullptr);

    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void mouseDoubleClick(QMouseEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);
public slots:

};

#endif // QLABELMOUSEEVENT_H
