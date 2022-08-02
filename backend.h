#ifndef BACKEND_H
#define BACKEND_H

#include "convert.h"
#include <QObject>

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);
public slots:
    void startConvert(std::vector<Convert> &converts);
signals:
    void sendLogMessage(QString message);
    void convertFinished(bool success);
};

#endif // BACKEND_H
