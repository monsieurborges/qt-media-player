#ifndef PLAYER_H
#define PLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

class Player : public QThread
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

    vector<Point> polygonPoints;

signals:
    void processedImage(const QImage &image);

protected:
    void run();
    void msleep(int ms);

public:
    Player(QObject *parent = nullptr);
    ~Player();

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

    void setPolygonPoints(vector<Point> pt);

    QStringList mimeTypeFilters;
};

#endif // PLAYER_H
