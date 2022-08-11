#ifndef CONVERT_H
#define CONVERT_H

#include <QString>
#include <QDir>
#include <QDataStream>
#include <QTextStream>
#include <QDateTime>

class Convert
{
public:
    Convert() {};
    Convert(QString _inputFolder, QString _outputFolder);

    bool checkData(QString &errorText);
    bool convertData(QString &errorText);
    bool writeResults(QString &errorText);
    void setH(double _h);
    void setHeightInPoint(double x, double y);
    void setConvertS(bool val);
    void setConvertD(bool val);
    void setWriteHeader(bool val);
    QString getTitle();
private:
    struct Triangle
    {
        double x;
        double y;
        double z;
    };
    QString inputFolder;
    QString outputFolder;
    QString title;

    QString pathToActFile;
    QString pathToLastPlSim;
    QString pathToLastOsSim;
    QString pathToLastCSim;
    QString pathToXYZFile;

    bool sInHeight = false;
    double h;

    bool sInPoint = false;
    double s_pointX;
    double s_pointY;

    bool convertS = false;
    bool convertD = false;

    bool writeHeader = false;

    std::vector<QStringList> resultsPl;
    std::vector<QStringList> resultsOs;
    std::vector<QStringList> resultsS;

    bool getPathToSimFiles(const QString filename, QString &targetPath, QString &errorText);

    bool convertSData(QString &errorText);
    bool convertPlData(QString &errorText, QString filename, std::vector<QStringList> &results);
    bool writeAllResults(QString &errorText, QString &pathToFile, const std::vector<QStringList> &results);
    bool writeSResults(QString &errorText);
    bool writeOSResults(QString &errorText);
    bool writePLResults(QString &errorText);
    std::vector<double> getTriangleMassCenter(const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z);
};

#endif // CONVERT_H
