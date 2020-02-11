#ifndef STATEINDICATOR_H
#define STATEINDICATOR_H

#include <QWidget>
#include <QPainter>

/*!
 * \brief The stateIndicator class
 * wskaźnik aktualnego stanu urządzenia
 */
class stateIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit stateIndicator(QWidget *parent = nullptr);
    void setOn();
    void setOff();
    void setDisabled();
protected:
    void paintEvent(QPaintEvent *) override;
private:
    QColor color;
signals:

public slots:
};

#endif // STATEINDICATOR_H
