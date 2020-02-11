#include "stateindicator.h"

stateIndicator::stateIndicator(QWidget *parent) : QWidget(parent)
{
    color = Qt::gray;
}
/*!
 * \brief stateIndicator::setOn
 * Ustawienie koloru na zielony
 */
void stateIndicator::setOn()
{
    color = Qt::green;
    update();
}
/*!
 * \brief stateIndicator::setOff
 * Ustawienie koloru na czerwony
 */
void stateIndicator::setOff()
{
    color = Qt::red;
    update();
}
/*!
 * \brief stateIndicator::setDisabled
 * Ustawienie koloru na szary
 */
void stateIndicator::setDisabled()
{
    color = Qt::gray;
    update();
}

void stateIndicator::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color);
    painter.drawEllipse(0, 0, width(), height());
}
