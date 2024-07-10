#include "mainwindow.h"
#include <QSplashScreen>
#include <QTimer>
#include <QPixmap>
#include <QScreen>
#include <QPainter>
#include <QApplication>
#include <QWidget>
#include <QFont>
int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    MainWindow w;
    // Load the splash screen image from resources
    QPixmap pixmap(":/splash/logo.jpeg");

    // Get the primary screen size
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    QSize screenSize = screenGeometry.size();

    // Create a white background pixmap
    QPixmap whiteBackground(screenSize);
    whiteBackground.fill(Qt::white);

    //setting the splash screen image onto the white background
    QSize desiredSize(450, 450);    //Resizing the image
    QPainter painter(&whiteBackground);
    QPixmap scaledPixmap = pixmap.scaled(desiredSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int x = (screenSize.width() - scaledPixmap.width()) / 2;
    int y = (screenSize.height() - scaledPixmap.height()) / 2;
    painter.drawPixmap(x, y, scaledPixmap);
    painter.end();



    // Changing the backgroung to white of splash screen
    QSplashScreen splash(whiteBackground);

    // Centering the splash screen
    int centerX = (screenGeometry.width() - screenSize.width()) / 2;
    int centerY = (screenGeometry.height() - screenSize.height()) / 2;
    splash.move(centerX, centerY);
    splash.showFullScreen();

    QFont font("Poppins", 25); //Changing the font of loading message
    splash.setFont(font);   //Setting the font

    // Display a loading message
    splash.showMessage("Loading Expense Manager...", Qt::AlignBottom | Qt::AlignCenter, Qt::black);

    // Simulate some loading time
    QTimer::singleShot(3000, &splash, &QSplashScreen::close); // Close the splash screen after 3 seconds
    QTimer::singleShot(3000, &w, &QWidget::showFullScreen); // Show the main window after 3 seconds

    return app.exec();
}
