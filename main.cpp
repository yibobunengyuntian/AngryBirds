#include "mainwin.h"

#include <QApplication>
#include <QFile>

#include "framewgt.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile styleFile(":Resource/UI/UI.qss"); // 如果放在资源文件中
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet); // 应用到整个应用程序
        styleFile.close();
    }
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
