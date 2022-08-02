#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QScreen>
#include <QGuiApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    backend(new Backend(nullptr)),
    threadForBackend(new QThread(this))
{
    ui->setupUi(this);
    move(QGuiApplication::screens().at(0)->geometry().center() - frameGeometry().center());
    initBackendHandler();
    setConnections();
}

MainWindow::~MainWindow()
{
    threadForBackend->quit();
    threadForBackend->wait();
    threadForBackend->deleteLater();

    delete ui;
}

void MainWindow::initBackendHandler()
{
    backend->moveToThread(threadForBackend);
    connect(threadForBackend, &QThread::started, this, [&](){ qDebug() << "Backend thread started"; });
    connect(threadForBackend, &QThread::finished, backend, &QObject::deleteLater);
    threadForBackend->start();
}

void MainWindow::setConnections()
{
    qRegisterMetaType<Convert>("Convert&");
    qRegisterMetaType<std::vector<Convert>>("std::vector<Convert>&");

    connect(backend, &Backend::sendLogMessage, this, &MainWindow::writeLog);
    connect(backend, &Backend::convertFinished, this, &MainWindow::convertFinished);
    connect(this, &MainWindow::startConvert, backend, &Backend::startConvert);
}


void MainWindow::on_toolButton_clicked()
{
    ui->line_inputFolder->setText(QFileDialog::getExistingDirectory(0, (""), QDir::currentPath()));
}


void MainWindow::on_toolButton_2_clicked()
{
    ui->line_outputFolder->setText(QFileDialog::getExistingDirectory(0, (""), QDir::currentPath()));
}


void MainWindow::writeLog(QString message)
{
    ui->list_log->addItem(message);
}

void MainWindow::unblockUI(bool unblock)
{
    ui->b_convert->setEnabled(unblock);
}

void MainWindow::convertFinished(bool success)
{
    if (!success)
        writeLog("Convert failed");
    unblockUI(true);
}


void MainWindow::on_b_convert_clicked()
{
    if (!QDir().exists(ui->line_inputFolder->text()))
    {
        writeLog("Input folder doesn't exist");
        return;
    }

    unblockUI(false);
    converts.clear();
    ui->list_log->clear();

    if (ui->radio_single->isChecked())
    {
        Convert newConvert(ui->line_inputFolder->text(), ui->line_outputFolder->text());
        newConvert.setConvertD(ui->check_d->isChecked());
        newConvert.setConvertS(ui->check_s->isChecked());
        newConvert.setWriteHeader(ui->check_writeHeader->isChecked());

        if (ui->check_s->isChecked())
        {
            if (ui->groupBox_height->isChecked())
                newConvert.setH(ui->spin_height->value());
            if (ui->groupBox_inPoint->isChecked())
                newConvert.setHeightInPoint(ui->spin_x->value(), ui->spin_y->value());
        }

        converts.push_back(newConvert);
    }

    if (ui->radio_multi->isChecked())
    {
        QDir const source(ui->line_inputFolder->text());
        QStringList const folders = source.entryList(QDir::NoDot | QDir::NoDotDot | QDir::Dirs);

        for (QString const& name: folders)
        {
            QString const fullPathName = ui->line_inputFolder->text() + QDir::separator() + name;

            //check is it folder with results
            QDir const res(fullPathName);
            QStringList const files = res.entryList(QStringList() << "*.sim", QDir::Files);
            if (files.size() == 0)
                continue;

            Convert newConvert(fullPathName, ui->line_outputFolder->text());
            newConvert.setConvertD(ui->check_d->isChecked());
            newConvert.setConvertS(ui->check_s->isChecked());
            newConvert.setWriteHeader(ui->check_writeHeader->isChecked());

            if (ui->check_s->isChecked())
            {
                if (ui->groupBox_height->isChecked())
                    newConvert.setH(ui->spin_height->value());
                if (ui->groupBox_inPoint->isChecked())
                    newConvert.setHeightInPoint(ui->spin_x->value(), ui->spin_y->value());
            }

            converts.push_back(newConvert);
        }
    }

    emit startConvert(converts);
}


void MainWindow::on_check_s_stateChanged(int arg1)
{
    bool isVisible = true;
    if (arg1 == 0)
       isVisible = false;

    ui->groupBox_height->setVisible(isVisible);
    ui->groupBox_inPoint->setVisible(isVisible);
}

