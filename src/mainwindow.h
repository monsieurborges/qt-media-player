#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QMouseEvent>
#include <QShortcut>
#include <QTime>

#include <player.h>
#include <polygondrawer.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private slots:
    void openMedia();
    void openMedia2Mask();

    void btnPlayPauseClicked();
    void btnPlayPause_set_Play();
    void btnPlayPause_set_Pause();

    void btnNewPolygon();

    void updatePlayerUI(QImage img);
    void updateMaskUI(QImage img);
    void maskSavePolygon();

    void btnLoadPolygonPolygonClicked();
    void btnLoadMaskControlClicked();
    vector<Point> loadPolygon();

    QString getFormattedTime(int timeInSeconds);

    void frameSliderPressed();
    void frameSliderReleased();
    void frameSliderMoved(int position);

    void mouseDoubleClick(QMouseEvent *event);
    void mousePressed(QMouseEvent *event);
    void mouseMoved(QMouseEvent *event);

    void warningMessage(const QString &caption, const QString &message);
private:
    Ui::MainWindow *ui;
    void resizeEvent(QResizeEvent *event);

    Player *myPlayer;
    PolygonDrawer *polygonDrawer;
    QString lastDir;
};

#endif // MAINWINDOW_H
