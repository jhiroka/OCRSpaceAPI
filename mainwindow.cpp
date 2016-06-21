#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "flowlayout.h"

/**
 * @brief MainWindow that displays the interface for the program.
 * @param parent parent widget of mainwindow = 0
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);
    this->setWindowTitle("OCR Space API");
    this->setBaseSize(900,500);

    //create all layouts
    QVBoxLayout* vlayout1 = new QVBoxLayout;
    QHBoxLayout* hlayout1 = new QHBoxLayout;
    QHBoxLayout* hlayout2 = new QHBoxLayout;
    QHBoxLayout* hlayout3 = new QHBoxLayout;
    QVBoxLayout* vlayout2 = new QVBoxLayout;

    //create the widgets
    QWidget* widget = new QWidget;

    QPushButton* openFileButton = new QPushButton("Choose File");
    openFileButton->setFixedWidth(150);
    openFileButton->setToolTip("Max file size: 1MB");

    QLabel* chooseLang = new QLabel("Select Language:");

    languages << "Chinese (Simplified)" << "Chinese (Traditional)" << "Czech" << "Danish" << "Dutch" << "English"
              << "Finnish" << "French" << "German" << "Greek" << "Hungarian" << "Italian" << "Japanese" << "Korean"
              << "Norwegian" << "Polish" << "Portuguese" << "Spanish" << "Swedish" << "Turkish" << "Russian";

    languageChoices = new QListWidget(this);
    languageChoices->addItems(languages);
    languageChoices->setSortingEnabled(true);
    languageChoices->setSelectionMode(QAbstractItemView::SingleSelection);
    languageChoices->setFixedWidth(150);

    QPushButton* performOCR = new QPushButton("Perform OCR");
    performOCR->setFixedWidth(150);
    performOCR->setToolTip("Click to perform OCR and see results");

    QPushButton* exportToFile = new QPushButton("Export");
    exportToFile->setFixedWidth(100);
    exportToFile->setToolTip("Save the results to a text file");

    //scroll area stuff
    textScrollArea = new QScrollArea;
    imageScrollArea = new QScrollArea;
    QWidget* wrap1 = new QWidget(textScrollArea);
    QWidget* wrap2 = new QWidget(imageScrollArea);

    FlowLayout* flow1 = new FlowLayout(wrap1);
    FlowLayout* flow2 = new FlowLayout(wrap2);

    //Qlabels that hold OCR image and OCR'd text
    displayText = new QLabel(wrap1);
    displayImage = new QLabel(wrap2);
    displayImage->setFixedSize(350,400);

    flow2->addWidget(displayImage);
    flow1->addWidget(displayText);
    //set up scroll area for the OCR'd text
    wrap1->setLayout(flow1);
    wrap2->setLayout(flow2);
    textScrollArea->setWidget(wrap1);
    imageScrollArea->setWidget(wrap2);
    textScrollArea->setWidgetResizable(true);
    imageScrollArea->setWidgetResizable(true);

    //set signals and slots for buttons
    QObject::connect(openFileButton,SIGNAL(clicked()),this,SLOT(chooseFile()));
    QObject::connect(languageChoices,SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onItemClicked(QListWidgetItem*)));
    QObject::connect(performOCR,SIGNAL(clicked()),this,SLOT(recognize()));
    QObject::connect(exportToFile,SIGNAL(clicked()),this,SLOT(saveToFile()));

    //add everything to layouts
    vlayout1->addWidget(openFileButton);
    vlayout2->addWidget(chooseLang);
    vlayout2->addWidget(languageChoices);
    vlayout2->addWidget(performOCR);
    hlayout2->addWidget(imageScrollArea);
    hlayout2->addLayout(vlayout2);
    hlayout2->addWidget(textScrollArea);
    hlayout3->addWidget(exportToFile);
    hlayout3->setAlignment(Qt::AlignRight);

    //add everything to main layout
    vlayout1->addLayout(hlayout1);
    vlayout1->addLayout(hlayout2);
    vlayout1->addLayout(hlayout3);
    widget->setLayout(vlayout1);

    //set central widget
    this->setCentralWidget(widget);
}

/**
 * @brief Destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

 /**
 * @brief Function that chooses a file from the user's computer drive and opens it
 */
void MainWindow::chooseFile()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("Image Files (*.png *.jpg *.gif);;PDF Files (*.PDF)"));
    if(!fileName.isNull()) {
       originalImage->load(fileName);
       displayImage->setPixmap(QPixmap::fromImage(*originalImage).scaled(displayImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
       displayImage->hasScaledContents();
    }
    else {
        std::cout<<"failed to open file"<<std::endl;
    }
}

/**
 * @brief Function that saves the parsed results of OCR to .txt file
 *        on your computer
 */
void MainWindow::saveToFile() {
    QString saveAsName = QFileDialog::getSaveFileName(this, tr("Save As"), QDir::homePath(), tr("PlainText file (*.txt)"));
    if(!saveAsName.isNull()) {
        QFile outputFile(saveAsName);
        outputFile.open(QFile::WriteOnly);
        QTextStream out(&outputFile);
        out << text;
        outputFile.close();
    }
    else {
       // std::cout << "file not saved" << std::endl;
        qDebug() << "file not saved";
    }
}

/**
 * @brief Function that sets proper format of the multipart message
 * @param key parameter required to send to API
 * @param value
 * @return QHttpPart object to be added to multipart message
 */
QHttpPart partParameter(QString key, QString value) {
    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\""+ key +"\""));
    part.setBody(value.toLatin1());
    return part;
}

/**
 * @brief Function that sends the multipart message request to the API server.
 *        Shows the image being OCR'd in the left-hand side of the interface
 */
void MainWindow::recognize() {
    multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //set up http part message to send to api that contains image data
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/gif"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\"; filename=\"*.gif\""));

    QFile* file = new QFile(fileName);

    //debugging: make sure file was uploaded
    if(!file->open(QIODevice::ReadOnly)) {
        qDebug() << "# Could not upload/open file";
    }

    QByteArray fileContent(file->readAll());
    imagePart.setBody(fileContent);

    //append image data, api key, language, and overlay setting to multipart
    multipart->append(imagePart);
    multipart->append(partParameter("language", language));

    //alert user if no language was chosen
    if(language == "") {
        QMessageBox* message = new QMessageBox(this);
        message->setText("Please select a language");
        message->exec();
    }

    multipart->append(partParameter("isOverlayRequired","false"));
    multipart->append(partParameter("apikey","helloworld"));

    //OCR Space API url
    QUrl api_url("https://apifree2.ocr.space/parse/image");

    //create network request obj that contains the api url
    QNetworkRequest api_request(api_url);
    manager = new QNetworkAccessManager;
    reply = manager->post(api_request, multipart);

    QObject::connect(reply, SIGNAL(finished()), this, SLOT(networkData()));
    //debugging: make sure file was successfully opened
    qDebug() << file->size() << "bytes";

    imagePart.setBodyDevice(file);
    file->setParent(multipart);
    networkData();
}

/**
 * @brief Function that gets the language selected by the user
 * @param item the language within the QListWidget that is captured
 */
void MainWindow::onItemClicked(QListWidgetItem *item) {
    QString text = QString("%1").arg(item->text());
    language = text;
    qDebug() << text;
}

/**
 * @brief Function that retrieves network reply from the API, formats the
 *        response and inserts it in Qlabel object displayText to show the
 *        results on the interface.
 */
void MainWindow::networkData() {
    //test for network error
     QNetworkReply::NetworkError err = reply->error();
     if (err != QNetworkReply::NoError) {
         qDebug() << reply->errorString();
         return;
     }

    //store the network's response in a string
    QString response = (QString)reply->readAll();

    //network reply is stored in JSON format; get only the OCR'd text results
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray jsonArr = jsonObj["ParsedResults"].toArray();
    foreach(const QJsonValue& value, jsonArr) {
        QJsonObject obj = value.toObject();
        text.append(obj["ParsedText"].toString());
    }

    //display the text results on the interface
    displayText->setText(text);
    displayText->hasScaledContents();
    displayText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    qDebug() << text;
}


