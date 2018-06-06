#ifndef AUTH_H
#define AUTH_H

#include <QObject>

class auth : public QObject
{
    Q_OBJECT
public:
    explicit auth(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AUTH_H