#ifndef POLYGONDRAWER_H
#define POLYGONDRAWER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class PolygonDrawer : public QThread
{
    Q_OBJECT

private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    VideoCapture *capture = nullptr;
    QImage img;
    Mat RGBframe;
    Mat frame;
    int frameRate;
    int frameWidth;
    int frameHeight;

    QPoint mousePosition;
    vector<Point> polygonPoints;

signals:
    void processedImage(const QImage &image);

public slots:
    void setMousePosition(QPoint pos);

protected:
    void run();
    void msleep(int ms);

public:
    PolygonDrawer(QObject *parent = nullptr);
    ~PolygonDrawer();

    bool loadVideo(QString filename);
    bool isOpened();
    QImage getFrame();
    QImage image();
    int getFrameWidth();
    int getFrameHeight();

    void setCurrentFrame(int frameNumber);

    double getFrameRate();
    double getCurrentFrame();
    double getNumberOfFrames();

    void Play();
    void Stop();
    bool isStopped();

    void addPointToPolygon(QPoint pt);
    void removePointFromPolygon();
    void finishPolygonEditing();
    void setPolygonPoints(vector<Point> pt);
    vector<Point> getPolygonPoints();

    QStringList mimeTypeFilters;
};

#endif // POLYGONDRAWER_H
