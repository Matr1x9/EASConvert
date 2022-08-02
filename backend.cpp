#include "backend.h"

Backend::Backend(QObject *parent) : QObject(parent)
{

}

void Backend::startConvert(std::vector<Convert> &converts)
{

    QString errorText;
    int i = 1;
    int count = converts.size();
    for (auto &sc : converts)
    {
        emit sendLogMessage("[" + sc.getTitle() + "] (" + QString::number(i) + " from " + QString::number(count) + ")");
        emit sendLogMessage("Checking data...");
        if (!sc.checkData(errorText))
        {
            emit sendLogMessage("Error!");
            emit sendLogMessage(errorText);
            emit convertFinished(false);
            return;
        }

        emit sendLogMessage("Converting data...");
        if (!sc.convertData(errorText))
        {
            emit sendLogMessage("Error!");
            emit sendLogMessage(errorText);
            emit convertFinished(false);
            return;
        }

        emit sendLogMessage("Writing results...");
        if (!sc.writeResults(errorText))
        {
            emit sendLogMessage("Error!");
            emit sendLogMessage(errorText);
            emit convertFinished(false);
            return;
        }

        emit sendLogMessage("Done");
        i++;
    }


    emit convertFinished(true);
}
