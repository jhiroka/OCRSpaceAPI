#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <iostream>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QCheckBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <iostream>
#include <QEvent>
#include <QDebug>
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QString>
#include <QHttpMultiPart>
#include <QFile>
#include <QScriptEngine>
#include <QDir>
#include <QVariantMap>
#include <QByteArray>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStringList>
#include <QObject>
#include <QIcon>
#include <QMessageBox>
#include <QScrollArea>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QImage* greyScale(QImage* origin);

public slots:
    void chooseFile();
    void recognize();
    void networkData();
    void saveToFile();
    void onItemClicked(QListWidgetItem* item);

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString saveLocation;
    QString text;
    QString language;
    QListWidget* languageChoices;
    QStringList languages;

    QImage* originalImage = new QImage;
    QLabel* displayImage;
    QLabel* displayText;
    QNetworkReply* reply;
    QNetworkAccessManager* manager;
    QHttpMultiPart* multipart;
    QScrollArea* textScrollArea;
    QScrollArea* imageScrollArea;
};

#endif // MAINWINDOW_H
