#ifndef CESAR_OPERATION_H
#define CESAR_OPERATION_H

#include <QObject>
#include <QDebug>

class CesarOperation : public QObject
{
    Q_OBJECT

public:
    explicit CesarOperation(QObject *parent = nullptr);
    ~CesarOperation();


private slots:

private:


};

#endif // CESAR_OPERATION_H
