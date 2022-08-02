#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QDebug>
#include <QFileDialog>

#include "backend.h"
#include "convert.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void writeLog(QString message);
    void unblockUI(bool unblock);
    void convertFinished(bool success);

    void on_b_convert_clicked();

    void on_check_s_stateChanged(int arg1);

signals:
    void startConvert(std::vector<Convert> &converts);

private:
    Ui::MainWindow *ui;
    Backend *backend;           ///< обработчик событий gui
    QThread *threadForBackend;  ///< Поток для backend

    std::vector<Convert> converts;

    void initBackendHandler();
    void setConnections();
};
#endif // MAINWINDOW_H
