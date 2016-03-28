#include "signalblocker.h"

SignalBlocker::SignalBlocker(QObject &toBlock) : toBlock(toBlock)
{
    prevState = toBlock.signalsBlocked();
    toBlock.blockSignals(true);
}

SignalBlocker::SignalBlocker(QObject *toBlock) : SignalBlocker(*toBlock)
{

}

SignalBlocker::~SignalBlocker()
{
    toBlock.blockSignals(prevState);
}

