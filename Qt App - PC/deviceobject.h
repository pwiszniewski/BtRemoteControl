#ifndef DEVICEOBJECT_H
#define DEVICEOBJECT_H

#include <QObject>

class DeviceObject : public QObject
{
    Q_OBJECT
public:
    explicit DeviceObject(QObject *parent = nullptr);
    DeviceObject(const QString &name, const QString &color, QObject *parent=nullptr);
    QString name() const {return m_name;}
    void setName(const QString &name) {m_name = name;}
    QString color() const {return m_color;}
    void setColor(const QString &color) {m_color = color;}
signals:

public slots:

private:
    QString m_name;
    QString m_color;
};

#endif // DEVICEOBJECT_H
