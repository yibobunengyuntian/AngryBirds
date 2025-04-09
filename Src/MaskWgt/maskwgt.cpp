#include "maskwgt.h"

MaskWgt::MaskWgt(QWidget *parent)
    : QWidget(parent)
{
    // 设置窗口标志，确保无边框
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    // 设置窗口背景为透明
    setAttribute(Qt::WA_TranslucentBackground);
}

void MaskWgt::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing); // 启用抗锯齿

    // 创建一个路径
    QPainterPath path;

    // 设置圆角半径
    int radius = 10;

    // 绘制顶部两个直角边
    path.moveTo(0, 0);
    path.lineTo(width(), 0);
    path.lineTo(width(), height() - radius);

    // 绘制右下角圆角
    path.arcTo(width() - 2 * radius, height() - 2 * radius, 2 * radius, 2 * radius, 0, -90);

    // 绘制左下角圆角
    path.arcTo(0, height() - 2 * radius, 2 * radius, 2 * radius, -90, -90);

    // 绘制左上角直角边
    path.lineTo(0, radius);

    // 设置背景颜色
    painter.fillPath(path, QBrush(QColor(0, 0, 0, 20))); // 半透明黑色
}
