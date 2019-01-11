

#include <QApplication>
#include <QObject>
#include <QPixmap>
#include <QSplashScreen>
#include <QTimer>
#include <QStyle>
#include <QStyleFactory>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

//   app.setAttribute(Qt::AA_DontUseNativeMenuBar);

  QPixmap pixmap(":/images/splash.png");
  QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

  QTimer t1;
  t1.setSingleShot(true);
#if QT_VERSION >= 0x050000
  t1.setTimerType(Qt::PreciseTimer);
#endif
  QObject::connect(&t1, SIGNAL(timeout()), &splash, SLOT(close()));

  splash.show();

  t1.start(3000);

  qApp->processEvents();

#if QT_VERSION >= 0x050000
#ifdef Q_OS_LINUX
  qApp->setStyle(QStyleFactory::create("Fusion"));
#endif
#endif
  qApp->setStyleSheet("QMessageBox { messagebox-text-interaction-flags: 5; }");

  class UI_Mainwindow MainWindow;

  return app.exec();
}





