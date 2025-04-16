#ifndef LEVELWGT_H
#define LEVELWGT_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>

class LevelWgt: public QWidget
{
    Q_OBJECT
public:
    LevelWgt(QWidget *parent = nullptr);

    void setLevel(const QVariantList &level);

protected:
    void resizeEvent(QResizeEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

signals:
    void selectedLevel(uint level);
    void home();

private:
    QWidget *m_pCenterWgt = nullptr;
    QPushButton *m_pBtnHome = nullptr;
    QScrollArea *m_pScrollArea;
    QList<QPushButton*> m_buttons;
};

#endif // LEVELWGT_H
