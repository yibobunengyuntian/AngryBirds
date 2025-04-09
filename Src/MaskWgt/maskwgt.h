#ifndef MASKWGT_H
#define MASKWGT_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>

class MaskWgt : public QWidget
{
    Q_OBJECT
public:
    explicit MaskWgt(QWidget *parent = nullptr);

    ~MaskWgt() = default;

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // MASKWGT_H
