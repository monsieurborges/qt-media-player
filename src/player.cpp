#include "player.h"

Player::Player(QObject *parent)
    : QThread(parent)
{
    stop = true;

    // Sets the filters used in the file dialog
    mimeTypeFilters << "video/mp4" << "video/avi";
}

Player::~Player()
{
    mutex.lock();
    stop = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

bool Player::loadVideo(QString filename)
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

bool Player::isOpened()
{
    if (capture == nullptr) {
        return false;
    }

    return capture->isOpened();
}

QImage Player::getFrame()
{
    if (capture->read(frame)) {
        Mat overlay;
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
                            frame,
                            1 - 0.4,
                            0,
                            frame);
        }

        // Convert frame to QImage
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

QImage Player::image()
{
    return img;
}

int Player::getFrameWidth()
{
    return frameWidth;
}

int Player::getFrameHeight()
{
    return frameHeight;
}

void Player::setCurrentFrame(int frameNumber)
{
    capture->set(CV_CAP_PROP_POS_FRAMES, frameNumber);
}

double Player::getFrameRate()
{
    return frameRate;
}

double Player::getCurrentFrame()
{
    return capture->get(CV_CAP_PROP_POS_FRAMES);
}

double Player::getNumberOfFrames()
{
    return capture->get(CV_CAP_PROP_FRAME_COUNT);
}

void Player::Play()
{
    // If last frame, set to the first frame
    if (capture->get(CV_CAP_PROP_POS_FRAMES) >= capture->get(CV_CAP_PROP_FRAME_COUNT)) {
        capture->set(CV_CAP_PROP_POS_FRAMES, 0);
    }

    stop = false;
    start(TimeCriticalPriority);
}

void Player::run()
{
    int delay = (1000 / frameRate);

    while(!stop) {
        img = this->getFrame();

        if (img.isNull()) {
            stop = true;
        } else {
            emit processedImage(img);
            this->msleep(delay);
        }
    }
}

void Player::Stop()
{
    stop = true;
}

void Player::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, nullptr);
}

bool Player::isStopped()
{
    return this->stop;
}

void Player::setPolygonPoints(vector<Point> pt)
{
    this->Stop();
    while(this->isRunning());

    polygonPoints = pt;
}
