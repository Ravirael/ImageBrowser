#pragma once

#include <QObject>

class SignalBlocker
{
    QObject &toBlock;
    bool prevState;
public:
    SignalBlocker(QObject &toBlock);
    SignalBlocker(QObject *toBlock);
    ~SignalBlocker();
};

