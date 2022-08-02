#include "convert.h"
#include <cmath>




Convert::Convert(QString _inputFolder, QString _outputFolder)
{
    inputFolder  = _inputFolder;
    outputFolder = _outputFolder;
    QDir d = QFileInfo(_inputFolder).absoluteFilePath();
    title = d.dirName();
}

bool Convert::checkData(QString &errorText)
{
    QDir().mkdir(outputFolder);
    if (!QDir().exists(inputFolder))
    {
        errorText = "input folder doesn't exist";
        return false;
    }

    if (!getPathToSimFiles("act.cfg", pathToActFile, errorText))
        return false;

    if (!getPathToSimFiles("xyz.sim", pathToXYZFile, errorText))
        return false;


    int index = 2;
    for (index = 1; index <= 99999; ++index)
    {
        QString str = QString::number(index).rightJustified(4,'0');
        if (!getPathToSimFiles("d_os_" + str + ".sim", pathToLastOsSim, errorText))
            break;
    }

    index-=1;
    QString str = QString::number(index).rightJustified(4,'0');
    if (!getPathToSimFiles("d_os_" + str + ".sim", pathToLastOsSim, errorText))
        return false;

    if (!getPathToSimFiles("d_pl_" + str + ".sim", pathToLastPlSim, errorText))
        return false;

    if (!getPathToSimFiles("c_" + str + ".sim", pathToLastCSim, errorText))
        return false;

    return true;
}

bool Convert::convertData(QString &errorText)
{
    if (convertD)
    {
        if (!convertPlData(errorText, pathToLastPlSim, resultsPl))
            return false;
        if (!convertPlData(errorText, pathToLastOsSim, resultsOs))
            return false;
    }

    if (convertS)
        if (!convertSData(errorText))
            return false;

    return true;
}

bool Convert::writeResults(QString &errorText)
{
    if (convertD)
    {
        if (!writePLResults(errorText))
            return false;
        if (!writeOSResults(errorText))
            return false;
    }

    if (convertS)
        if (!writeSResults(errorText))
            return false;

    return true;
}

bool Convert::writeSResults(QString &errorText)
{
    QString path = outputFolder + QDir::separator() + title + "_s.dat";

    if (!QDir().exists(outputFolder))
        QDir().mkdir(outputFolder);

    if (!writeAllResults(errorText, path, resultsS))
    {
        errorText = "Can't write s result -> " + path;
        return false;
    }
    return true;
}

bool Convert::writeOSResults(QString &errorText)
{
    QString path = outputFolder + QDir::separator() + title + "_os.dat";

    if (!QDir().exists(outputFolder))
        QDir().mkdir(outputFolder);
    if (!writeAllResults(errorText, path, resultsOs))
    {
        errorText = "Can't write s result -> " + path;
        return false;
    }
    return true;
}

bool Convert::writePLResults(QString &errorText)
{
    QString path = outputFolder + QDir::separator() + title + "_pl.dat";

    if (!QDir().exists(outputFolder))
        QDir().mkdir(outputFolder);

    if (!writeAllResults(errorText, path, resultsPl))
    {
        errorText = "Can't write s result -> " + path;
        return false;
    }
    return true;
}

void Convert::setH(double _h)
{
    sInHeight = true;
    h = _h;
}

void Convert::setHeightInPoint(double x, double y)
{
    sInPoint = true;
    s_pointX = x;
    s_pointY = y;
}

void Convert::setConvertS(bool val)
{
    convertS = val;
}

void Convert::setConvertD(bool val)
{
    convertD = val;
}

void Convert::setWriteHeader(bool val)
{
    writeHeader = val;
}

QString Convert::getTitle()
{
    return title;
}

bool Convert::getPathToSimFiles(const QString filename, QString &targetPath, QString &errorText)
{
    QString path = inputFolder + QDir::separator() + filename;

    if (!QFile().exists(path))
    {
        errorText = "Can't find file -> " + path;
        return false;
    }

    targetPath = path;
    return true;
}

bool Convert::convertSData(QString &errorText)
{
    QFile file(pathToLastCSim);
    if(!file.open(QIODevice::ReadOnly))
    {
        errorText = "Can't open sim file -> " + pathToLastCSim;
        return false;
    }

    QFile file_xyz(pathToXYZFile);
    if(!file_xyz.open(QIODevice::ReadOnly))
    {
        errorText = "Can't open sim file -> " + pathToXYZFile;
        return false;
    }

    QDataStream in;
    in.setDevice(&file);
    in.setVersion(QDataStream::Qt_5_15);
    in.setByteOrder(QDataStream::LittleEndian);

    QDataStream xyz;
    xyz.setDevice(&file_xyz);
    xyz.setVersion(QDataStream::Qt_5_15);
    xyz.setByteOrder(QDataStream::LittleEndian);

    QFile file_act(pathToActFile);
    if(!file_act.open(QIODevice::ReadOnly))
    {
        errorText = "Can't open act file -> " + pathToActFile;
        file.close();
        file_xyz.close();
        return false;
    }

    QTextStream in_act(&file_act);
    std::vector<double> acts;
    while (!in_act.atEnd())
    {
        QString line = in_act.readLine();
        bool ok;
        double val = line.toDouble(&ok);
        if (!ok)
        {
            errorText = "Wrong value in act file -> " + line;
            file.close();
            file_act.close();
            file_xyz.close();
            return false;
        }
        acts.push_back(val);
    }

    if (acts.size() == 0)
    {
        errorText = "Act file is empty -> " + pathToActFile;
        file.close();
        file_act.close();
        file_xyz.close();
        return false;
    }

    double act = acts.at(0);

    int x_count;
    int y_count;
    int z_count;
    double val;

    double t;
    int x_count_s;
    int y_count_s;
    int z_count_s;
    int nucCount;


    xyz >> x_count >> y_count >> z_count;

    in >> t;
    in >> x_count_s >> y_count_s >> z_count_s >> nucCount;

    int xyzCount = x_count_s * y_count_s * z_count_s;

    if (xyzCount != (x_count * y_count *z_count))
    {
        errorText = "Coordinates in xyz and s sim files are different";
        file.close();
        file_act.close();
        file_xyz.close();
        return false;
    }

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;

    for (int i = 0; i < x_count; i++)
    {
        xyz >> val;
        x.push_back(val);
    }

    for (int i = 0; i < y_count; i++)
    {
        xyz >> val;
        y.push_back(val);
    }

    for (int i = 0; i < z_count; i++)
    {
        xyz >> val;
        z.push_back(val);
    }

    std::vector<double> vals;

    for (int i = 0; i < nucCount; i++)
        for (int j = 0; j < xyzCount; j++)
        {
            in >> val;
            vals.push_back(val);
        }


    int indexVals = -1;
    for (int index_z = 0; index_z < z_count_s; index_z++)
        for (int index_y = 0; index_y < y_count_s; index_y++)
            for (int index_x = 0; index_x < x_count_s; index_x++)
            {
                indexVals++;
                if (sInHeight)
                {
                    if (z[index_z] < (h-1) || z[index_z] > (h+1))
                        continue;
                }

                if (sInPoint)
                {
                    if (x[index_x] < s_pointX - 0.1 || x[index_x] > s_pointX + 0.1)
                        continue;
                    if (y[index_y] < s_pointY - 0.1 || y[index_y] > s_pointY + 0.1)
                        continue;
                }

                QStringList inres;
                inres.push_back(QString::number(x[index_x],'E', 5));
                inres.push_back(QString::number(y[index_y],'E', 5));
                inres.push_back(QString::number(z[index_z],'E', 5));

                double overallAct = 0;
                for (int k = 0; k < nucCount; k++)
                {
                    inres.push_back(QString::number(vals[indexVals + ( k * xyzCount)],'E', 5));
                    overallAct += vals[indexVals + ( k * xyzCount)];
                }

                double dose = 1.37315E-13 * act * overallAct;
                inres.push_back(QString::number(dose,'E', 5));

                resultsS.push_back(inres);
            }

    if (writeHeader)
    {
        std::vector<QStringList> temp;
        QStringList list;
        list.push_back(title + "; t = " + QString::number(t));
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back(QString::number(nucCount + 3));
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back("Координата X, м");
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back("Координата Y, м");
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back("Координата Z, м");
        temp.insert(temp.begin(), list);
        list.clear();

        for (int j = 0; j < nucCount; j++)
        {
            list.push_back("Объемная активность, Бк/м3");
            temp.insert(temp.begin(), list);
            list.clear();
        }

        list.push_back("Доза, Зв");
        temp.insert(temp.begin(), list);
        list.clear();

        for (const auto &strlist : temp)
            resultsS.insert(resultsS.begin(), strlist);
    }


    file.close();
    file_act.close();
    file_xyz.close();


    return true;
}

bool Convert::convertPlData(QString &errorText, QString filename, std::vector<QStringList> &results)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
    {
        errorText = "Can't open sim file -> " + filename;
        return false;
    }

    QDataStream in;
    in.setDevice(&file);
    in.setVersion(QDataStream::Qt_5_15);
    in.setByteOrder(QDataStream::LittleEndian);

    double t;
    in >> t;

    int recordsCount;
    int columnsCount;

    in >> recordsCount >> columnsCount;

    for (int i = 0; i < recordsCount; i++)
    {
        QStringList inres;
        // вершина треугольника. Координаты и все значения нуклидов

        std::vector<double> list1;
        for (int j = 0; j < columnsCount; j++)
        {
            double val;
            in >> val;
            list1.push_back(val);
        }

        for (int j = 0; j < columnsCount; j++)
            inres.push_back(QString::number(list1[j],'E', 5));

        results.push_back(inres);
    }

    if (writeHeader)
    {
        std::vector<QStringList> temp;
        QStringList list;
        list.push_back(title + "; t = " + QString::number(t));
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back(QString::number(columnsCount));
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back("Координата X, м");
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back("Координата Y, м");
        temp.insert(temp.begin(), list);
        list.clear();

        list.push_back("Координата Z, м");
        temp.insert(temp.begin(), list);
        list.clear();

        for (int j = 3; j < columnsCount; j++)
        {
            list.push_back("Осаждения,  Бк/м2");
            temp.insert(temp.begin(), list);
            list.clear();
        }

        for (const auto &strlist : temp)
            results.insert(results.begin(), strlist);
    }

    file.close();
    return true;
}

bool Convert::writeAllResults(QString &errorText, QString &pathToFile, const std::vector<QStringList> &results)
{
    QFile file(pathToFile);
    if(!file.open(QIODevice::WriteOnly))
    {
        errorText = "Can't create file -> " + pathToFile;
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    for (const auto &str : results)
    {
        QString tempString;
        for (const auto &col : str)
            tempString+= col + "\t";

        out << tempString << "\r\n";
    }
    file.close();
    return true;
}
