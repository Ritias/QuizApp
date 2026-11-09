#include <QApplication>
#include <QWidget>
#include "Quiz.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QPixmap pixmap(QCoreApplication::applicationDirPath() + "/assets/pink_pen.png");
    qApp->setOverrideCursor(QCursor(pixmap));
    Quiz quiz;
    quiz.show();

    return app.exec();
    qApp->restoreOverrideCursor();
}
