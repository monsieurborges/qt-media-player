#include "polygondrawer.h"

PolygonDrawer::PolygonDrawer(QObject *parent)
    : QThread(parent)
{
    stop = true;

    mousePosition = QPoint(0, 0);

    // Sets the filters used in the file dialog
    mimeTypeFilters << "video/mp4" << "video/avi";
}

PolygonDrawer::~PolygonDrawer()
{
    mutex.lock();
    stop = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

bool PolygonDrawer::loadVideo(QString filename)
{
    capture = new cv::VideoCapture(filename.toStdString());

    if (capture->isOpened()) {
        frameRate   = int(capture->get(CV_CAP_PROP_FPS));
        frameWidth  = int(capture->get(CV_CAP_PROP_FRAME_WIDTH));
        frameHeight = int(capture->get(CV_CAP_PROP_FRAME_HEIGHT));
        return true;
    }
    else {
        return false;
    }
}

bool PolygonDrawer::isOpened()
{
    if (capture == nullptr) {
        return false;
    }

    return capture->isOpened();
}

QImage PolygonDrawer::getFrame()
{
    if (capture->read(frame)) {
        if (frame.channels() == 3) {
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);

            img = QImage(static_cast<const unsigned char*>(RGBframe.data),
                         RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
        } else {
            img = QImage(static_cast<const unsigned char*>(frame.data),
                         frame.cols,frame.rows,QImage::Format_Indexed8);
        }
    } else {
        img = QImage(); // Null image
    }

    return img;
}

QImage PolygonDrawer::image()
{
    return img;
}

int PolygonDrawer::getFrameWidth()
{
    return frameWidth;
}

int PolygonDrawer::getFrameHeight()
{
    return frameHeight;
}

void PolygonDrawer::setCurrentFrame(int frameNumber)
{
    capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}

double PolygonDrawer::getFrameRate()
{
    return frameRate;
}

double PolygonDrawer::getCurrentFrame()
{
    return capture->get(CV_CAP_PROP_POS_FRAMES);
}

double PolygonDrawer::getNumberOfFrames()
{
    return capture->get(CV_CAP_PROP_FRAME_COUNT);
}

void PolygonDrawer::Play()
{
    stop = false;
    polygonPoints.clear();
    start(TimeCriticalPriority);
}

void PolygonDrawer::setMousePosition(QPoint pos)
{
    this->mousePosition = pos;
}

void PolygonDrawer::run()
{
    QPoint textPosition;
    int delay = (1000 / frameRate);

    while(!stop) {
        Mat canvas;
        frame.copyTo(canvas);

        // Text Position
        if (mousePosition.x() > (frameWidth - 120)) {
            textPosition.setX(frameWidth - 120);
        } else {
            textPosition.setX(mousePosition.x());
        }

        if (mousePosition.y() < 30) {
            textPosition.setY(30);
        } else {
            textPosition.setY(mousePosition.y());
        }

        if (polygonPoints.size() > 0) {
            // Draw all line segments
            const Point *pts = (const cv::Point *) Mat(polygonPoints).data;
            const int npts = Mat(polygonPoints).rows;

            cv::polylines(canvas,
                          &pts,
                          &npts,
                          1,
                          false,
                          Scalar(0, 255, 0),
                          2);

            // Draw the current segment
            cv::line(canvas,
                     polygonPoints.back(),
                     cv::Point(mousePosition.x(), mousePosition.y()),
                     Scalar(0, 255, 0),
                     3);
        }

        // Draw the mouse position
        cv::putText(canvas,
                    QString("%1:%2").arg(mousePosition.x()).arg(mousePosition.y()).toStdString(),
                    cv::Point(textPosition.x(), textPosition.y()),
                    cv::FONT_HERSHEY_COMPLEX,
                    0.7,
                    Scalar(0, 255, 0),
                    1,
                    cv::LINE_AA);

        cv::cvtColor(canvas, RGBframe, CV_BGR2RGB);

        img = QImage(static_cast<const unsigned char*>(RGBframe.data),
                     RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);

        emit processedImage(img);
        this->msleep(delay);
    }
}

void PolygonDrawer::Stop()
{
    stop = true;
}

void PolygonDrawer::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, nullptr);
}

bool PolygonDrawer::isStopped()
{
    return this->stop;
}

void PolygonDrawer::addPointToPolygon(QPoint pt)
{
    polygonPoints.push_back(cv::Point(pt.x(), pt.y()));
}

void PolygonDrawer::removePointFromPolygon()
{
    if (polygonPoints.size() > 0) {
        polygonPoints.pop_back();
    }


}

void PolygonDrawer::finishPolygonEditing()
{
    this->Stop();
    while(this->isRunning());

    Mat canvas;
    Mat overlay;
    frame.copyTo(canvas);
    frame.copyTo(overlay);

    // Fill polygon
    if (polygonPoints.size() > 0) {
        // Draw all line segments
        const Point *pts = (const cv::Point *) Mat(polygonPoints).data;
        const int npts = Mat(polygonPoints).rows;

        cv::fillPoly(overlay,
                     &pts,
                     &npts,
                     1,
                     Scalar(0, 0, 128));

        cv::addWeighted(overlay,
                        0.4,
                        canvas,
                        1 - 0.4,
                        0,
                        canvas);

        cv::cvtColor(canvas, RGBframe, CV_BGR2RGB);

        img = QImage(static_cast<const unsigned char*>(RGBframe.data),
                     RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);

        emit processedImage(img);
    }
}

void PolygonDrawer::setPolygonPoints(vector<Point> pt)
{
    this->Stop();
    while(this->isRunning());

    polygonPoints = pt;

    finishPolygonEditing();
}

vector<Point> PolygonDrawer::getPolygonPoints()
{
    return polygonPoints;
}
