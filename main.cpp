#include "mainwin.h"

#include <QApplication>
#include <QFile>

#include "framewgt.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Utils::loadStyle(":Resource/UI/style.qss");

    QFont font("华文琥珀");
    a.setFont(font);

    MainWin *w = new MainWin;
    w->resize(600, 400);
    FrameWgt fw(w);
    // fw.setHiddenTitleBar(true);
    // fw.setTitleBarColor(QColor::fromString("#92bd6c"));
    fw.setBackgroundColor(Qt::white);
    fw.setTitleTextColor(QColor::fromString("#92bd6c"));
    fw.setShadowColor(QColor::fromString("#92bd6c"));
    fw.show();

    return a.exec();
}
