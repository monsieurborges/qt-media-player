#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Instantiate myPlayer and polygonDrawer class
    myPlayer      = new Player();
    polygonDrawer = new PolygonDrawer();


    // GUI settings
    ui->setupUi(this);
    ui->videoCanvas->setPixmap(QPixmap());

    this->showMaximized();
    
    // Connect SIGNAL and SLOTS
    connect(ui->btnOpenMedia, SIGNAL(clicked()), this, SLOT(openMedia()));
    connect(ui->btnOpenMedia2Mask, SIGNAL(clicked()), this, SLOT(openMedia2Mask()));
    connect(ui->btnLoadMaskControl, SIGNAL(clicked()), this, SLOT(btnLoadMaskControlClicked()));
    connect(ui->btnPlayPause, SIGNAL(clicked()), this, SLOT(btnPlayPauseClicked()));

    connect(myPlayer, SIGNAL(processedImage(QImage)), this, SLOT(updatePlayerUI(QImage)));
    connect(myPlayer, SIGNAL(started()), this, SLOT(btnPlayPause_set_Pause()));
    connect(myPlayer, SIGNAL(finished()), this, SLOT(btnPlayPause_set_Play()));

    connect(ui->frameSlider, SIGNAL(sliderPressed()), this, SLOT(frameSliderPressed()));
    connect(ui->frameSlider, SIGNAL(sliderReleased()), this, SLOT(frameSliderReleased()));
    connect(ui->frameSlider, SIGNAL(sliderMoved(int)), this, SLOT(frameSliderMoved(int)));

    connect(ui->maskCanvas, SIGNAL(mouseDoubleClick(QMouseEvent *)), this, SLOT(mouseDoubleClick(QMouseEvent *)));
    connect(ui->maskCanvas, SIGNAL(mousePressed(QMouseEvent *)), this, SLOT(mousePressed(QMouseEvent *)));
    connect(ui->maskCanvas, SIGNAL(mouseMoved(QMouseEvent *)), this, SLOT(mouseMoved(QMouseEvent *)));

    connect(ui->btnNewPolygon, SIGNAL(clicked()), this, SLOT(btnNewPolygon()));
    connect(ui->btnSavePolygon, SIGNAL(clicked()), this, SLOT(maskSavePolygon()));
    connect(ui->btnLoadPolygon, SIGNAL(clicked()), this, SLOT(btnLoadPolygonPolygonClicked()));
    connect(polygonDrawer, SIGNAL(processedImage(QImage)), this, SLOT(updateMaskUI(QImage)));

    // Keyboard shortcuts
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close())); // Quit app macOS
    new QShortcut(QKeySequence(Qt::ALT + Qt::Key_F4), this, SLOT(close())); // Quit app Windows
}

MainWindow::~MainWindow()
{
    delete myPlayer;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);

    if (event->key() == Qt::Key_Escape) {
        if (polygonDrawer->isRunning()) {
            polygonDrawer->removePointFromPolygon();
        }
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        if (polygonDrawer->isRunning()) {
            polygonDrawer->finishPolygonEditing();
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // Update video canvas
    if (myPlayer->isOpened() && !myPlayer->isRunning()) {
        ui->videoCanvas->setAlignment(Qt::AlignCenter);
        ui->videoCanvas->setPixmap(QPixmap::fromImage(myPlayer->image()).scaled(
                                       ui->videoCanvas->size(),
                                       Qt::KeepAspectRatio,
                                       Qt::FastTransformation));
    }
}

void MainWindow::openMedia()
{
    if (lastDir.isEmpty()) {
        lastDir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).value(0, QDir::homePath());
    }

    myPlayer->Stop();

    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open video"));
    fileDialog.setMimeTypeFilters(myPlayer->mimeTypeFilters);
    fileDialog.setDirectory(lastDir);

    if (fileDialog.exec() == QDialog::Accepted) {
        if (myPlayer->loadVideo(fileDialog.selectedFiles().first())) {
            ui->btnPlayPause->setEnabled(true);
            ui->frameSlider->setEnabled(true);

            ui->frameSlider->setMaximum(int(myPlayer->getNumberOfFrames()));
            ui->labelVideoTime->setText(getFormattedTime(int(myPlayer->getNumberOfFrames() / myPlayer->getFrameRate())));

            // Update video canvas
            this->updatePlayerUI(myPlayer->getFrame());

            lastDir = fileDialog.directory().path();
        } else {
            QMessageBox msgBox;
            msgBox.setText("The selected video could not be opened!");
            msgBox.exec();
        }
    }
}

void MainWindow::openMedia2Mask()
{
    if (lastDir.isEmpty()) {
        lastDir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).value(0, QDir::homePath());
    }

    polygonDrawer->Stop();

    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open video"));
    fileDialog.setMimeTypeFilters(polygonDrawer->mimeTypeFilters);
    fileDialog.setDirectory(lastDir);

    if (fileDialog.exec() == QDialog::Accepted) {
        if (polygonDrawer->loadVideo(fileDialog.selectedFiles().first())) {
            ui->btnLoadPolygon->setEnabled(true);
            ui->btnNewPolygon->setEnabled(true);
            ui->btnSavePolygon->setEnabled(true);
            ui->btnEditPolygonInfo->setEnabled(true);

            // Update mask canvas
            ui->maskCanvas->setAlignment(Qt::AlignCenter);
            ui->maskCanvas->setPixmap(QPixmap::fromImage(polygonDrawer->getFrame()));

            lastDir = fileDialog.directory().path();
        } else {
            QMessageBox msgBox;
            msgBox.setText("The selected video could not be opened!");
            msgBox.exec();
        }
    }
}

void MainWindow::btnPlayPauseClicked()
{
    if (!myPlayer->isRunning()) {
        myPlayer->Play();
    } else {
        myPlayer->Stop();
    }
}

void MainWindow::btnPlayPause_set_Play()
{
    ui->btnPlayPause->setIcon(QIcon(":/pics/MediaPlay.png"));
}

void MainWindow::btnPlayPause_set_Pause()
{
    ui->btnPlayPause->setIcon(QIcon(":/pics/MediaPause.png"));
}

void MainWindow::btnNewPolygon()
{
    polygonDrawer->Play();
}

void MainWindow::updatePlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->videoCanvas->setAlignment(Qt::AlignCenter);
        ui->videoCanvas->setPixmap(QPixmap::fromImage(img).scaled(ui->videoCanvas->size(), Qt::KeepAspectRatio, Qt::FastTransformation));

        ui->frameSlider->setValue(int(myPlayer->getCurrentFrame()));
        ui->labelVideoTime->setText(getFormattedTime(int(myPlayer->getCurrentFrame() / myPlayer->getFrameRate())));
    }
}

void MainWindow::updateMaskUI(QImage img)
{
    if (!img.isNull())
    {
        ui->maskCanvas->setAlignment(Qt::AlignCenter);
        ui->maskCanvas->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::maskSavePolygon()
{
    QString data;
    QString currentTime(QDateTime::currentDateTime().toString(Qt::SystemLocaleShortDate));

    data.append(tr("# ENGIE Multi-Object Tracking Analytics\n"));
    data.append(tr("# File created on %1\n\n").arg(currentTime));
    data.append(tr("[Polygon]\n"));

    vector<Point> points(polygonDrawer->getPolygonPoints());

    for (unsigned long i = 0; i < points.size(); i++) {
        data.append(tr("%1 %2\n").arg(points[i].x).arg(points[i].y));
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Save Polygon", lastDir, "Text file (*.txt)");

    if (filePath.isEmpty())
        return;

    if (!filePath.contains(".txt"))
        filePath.append(".txt");

    QFile outFile(filePath);

    if (outFile.open(QIODevice::WriteOnly | QIODevice::Text) == false) {
        warningMessage(tr("Mask Save Polygon"), tr("File %1 could not be create.\t").arg(filePath));
        return;
    }

    outFile.write(data.toUtf8());
    outFile.close();
}

void MainWindow::btnLoadPolygonPolygonClicked()
{
    vector<Point> points = loadPolygon();

    if (!points.empty()) {
        polygonDrawer->setPolygonPoints(points);
    }
}

void MainWindow::btnLoadMaskControlClicked()
{
    vector<Point> points = loadPolygon();

    if (!points.empty()) {
        myPlayer->setPolygonPoints(points);
    }
}

vector<Point> MainWindow::loadPolygon()
{
    if (polygonDrawer->isRunning())
        polygonDrawer->Stop();

    vector<Point> points;

    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("Open Polygon"));
    fileDialog.setMimeTypeFilters(QStringList("text/plain"));
    fileDialog.setDirectory(lastDir);

    if (fileDialog.exec() == QDialog::Accepted) {
        QFile inFile(fileDialog.selectedFiles().first());

        if (inFile.open(QIODevice::ReadOnly) == false) {
            QMessageBox msgBox;
            msgBox.setText(tr("File %1 could not be opened.\t").arg(inFile.fileName()));
            msgBox.exec();
            return points;
        }

        QTextStream in(&inFile);
        QStringList fields;
        QString line;
        QString section("[Polygon]");
        int lineNumber = 0;
        bool ok = false;

        // Find section [Polygon]
        in.seek(0);
        do {
            line = in.readLine();
            lineNumber++;

            if (line.contains(section))
                ok = true;
        } while ((ok == false) && !in.atEnd());

        if (ok == false) {
            warningMessage("Mask Polygon Load", tr("File %1\n\nSection %2 not found.\t").arg(inFile.fileName()).arg(section));
            return points;
        }

        // Read data
        while (!in.atEnd()) {
            line   = in.readLine();
            lineNumber++;

            // Is a comment?
            while (line.startsWith('#', Qt::CaseInsensitive) && !in.atEnd()) {
                line = in.readLine();
                lineNumber++;
            }

            // Is other section or line without data?
            if (line.count() == 0) {
                if (points.empty())
                    warningMessage("Mask Polygon Load", tr("File %1\n\nData not found.\t").arg(inFile.fileName()));

                return points;
            }

            fields = line.split(" ");
            points.push_back(Point(fields.at(0).toInt(&ok), fields.at(1).toInt(&ok)));

            if (ok == false) {
                warningMessage("Mask Polygon Load", tr("File %1\n\nInvalid data at line %2").arg(inFile.fileName()).arg(lineNumber));
                points.clear();
                return points;
            }
        }

        return points;
    }
}

QString MainWindow::getFormattedTime(int timeInSeconds)
{
    int seconds = int(timeInSeconds) % 60;
    int minutes = int((timeInSeconds / 60) % 60);
    int hours   = int((timeInSeconds / (60*60) % 24));

    QTime t(hours, minutes, seconds);
    if (hours == 0) {
        return t.toString("mm:ss");
    } else {
        return t.toString("h:mm:ss");
    }
}

void MainWindow::frameSliderPressed()
{
    myPlayer->Stop();
    frameSliderMoved(ui->frameSlider->value());
}

void MainWindow::frameSliderReleased()
{
    myPlayer->Play();
}

void MainWindow::frameSliderMoved(int position)
{
    if (position >= int(myPlayer->getNumberOfFrames())) {
        position = int(myPlayer->getNumberOfFrames() - 1);
    }

    myPlayer->setCurrentFrame(position);
    ui->labelVideoTime->setText(getFormattedTime(position / int(myPlayer->getFrameRate())));

    ui->videoCanvas->setAlignment(Qt::AlignCenter);
    ui->videoCanvas->setPixmap(QPixmap::fromImage(myPlayer->getFrame()).scaled(
                                   ui->videoCanvas->size(),
                                   Qt::KeepAspectRatio,
                                   Qt::FastTransformation));
}

void MainWindow::mouseDoubleClick(QMouseEvent *event)
{
    statusBar()->showMessage(tr("Double Click"), 3000);
}

void MainWindow::mousePressed(QMouseEvent *event)
{
    if (polygonDrawer->isRunning()) {
        if (event->button() == Qt::LeftButton) {
            statusBar()->showMessage(tr("Left button pressed"), 3000);

            polygonDrawer->addPointToPolygon(event->pos());

        } else if (event->button() == Qt::RightButton) {
            statusBar()->showMessage(tr("Right button pressed"), 3000);
        }
    }
}

void MainWindow::mouseMoved(QMouseEvent *event)
{
    if (polygonDrawer->isRunning()) {
        statusBar()->showMessage(tr("x=%1, y=%2").arg(event->pos().x()).arg(event->pos().y()), 3000);
        polygonDrawer->setMousePosition(event->pos());
    }
}

void MainWindow::warningMessage(const QString &caption, const QString &message)
{
    QMessageBox::warning(this, caption, message);
}
