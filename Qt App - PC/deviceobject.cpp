#include "deviceobject.h"

DeviceObject::DeviceObject(QObject *parent) : QObject(parent)
{

}

DeviceObject::DeviceObject(const QString &name, const QString &color, QObject *parent)
    : QObject(parent), m_name(name), m_color(color)
{
}
