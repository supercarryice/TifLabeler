#include "mainwindow.h"
#include "zipfile.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#pragma execution_character_set("utf-8")
//#define ENABLE_DEBUG_OUTPUT

MainWindow::MainWindow(QString rootPath, QWidget* parent) : QMainWindow(parent), tifLoaded(false)
{
    setWindowTitle("Tif标注工具");

    prefixPath = rootPath;
    //prefixPath = "C:/Users/eric/Desktop/qtCode/";

    // button widget
    QPushButton* select_button = new QPushButton("选择文件", this);
    connect(select_button, &QPushButton::clicked, this, &MainWindow::loadTif);
    QPushButton* crop_button = new QPushButton("裁剪文件", this);
    connect(crop_button, &QPushButton::clicked, this, &MainWindow::cropTif);
    QPushButton* save_button = new QPushButton("保存文件", this);
    connect(save_button, &QPushButton::clicked, this, &MainWindow::saveTif);
    QPushButton* mkzip_button = new QPushButton("压缩文件", this);
    connect(mkzip_button, &QPushButton::clicked, this, &MainWindow::compressFile);

    // create window
    m_canvas = new MapCanvas();
    m_canvas->setCanvasColor(Qt::white);

    // create layout
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(select_button);
    layout->addWidget(crop_button);
    layout->addWidget(save_button);
    layout->addWidget(mkzip_button);
    layout->addWidget(m_canvas);
    m_canvas->setMouseTracking(true);

    // create mainwindow and set layout
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    viewCrs = QgsCoordinateReferenceSystem(900913, QgsCoordinateReferenceSystem::EpsgCrsId);
    QgsCoordinateReferenceSystem wgs84 = QgsCoordinateReferenceSystem(4326, QgsCoordinateReferenceSystem::EpsgCrsId);
    trans = QgsCoordinateTransform(viewCrs, wgs84, QgsProject::instance());
    connect(m_canvas, SIGNAL(xyCoordinates(QgsPointXY)), this, SLOT(showMousePoint(QgsPointXY)));
}

int MainWindow::latlon2pix(double lon_min, double lat_min, double lon_max, double lat_max, int &leftx, int &lefty, int &rightx, int &righty)
{
    int status = -1;
    double dTemp = adfGeoTransform[1] * adfGeoTransform[5] - adfGeoTransform[2] * adfGeoTransform[4];
    if (dTemp == 0)
    {
        return -1;
    }
    leftx = (adfGeoTransform[5] * (lon_min - adfGeoTransform[0]) - adfGeoTransform[2] * (lat_min - adfGeoTransform[3])) / dTemp + 0.5;
    lefty = (adfGeoTransform[1] * (lat_min - adfGeoTransform[3]) - adfGeoTransform[4] * (lon_min - adfGeoTransform[0])) / dTemp + 0.5;

    rightx = (adfGeoTransform[5] * (lon_max - adfGeoTransform[0]) - adfGeoTransform[2] * (lat_max - adfGeoTransform[3])) / dTemp + 0.5;
    righty = (adfGeoTransform[1] * (lat_max - adfGeoTransform[3]) - adfGeoTransform[4] * (lon_max - adfGeoTransform[0])) / dTemp + 0.5;


    status = 0;
#ifdef ENABLE_DEBUG_OUTPUT
    qDebug() << "dTemp: " << dTemp;
    for (int i = 0; i < 6; i++) {
        std::cout << adfGeoTransform[i] << " ";
    }
#endif // ENABLE_DEBUG_OUTPUT
    return status;
}

std::string MainWindow::qstringToString(QString qstr)
{
    QByteArray qbytes = qstr.toUtf8();
    const char* cstr = qbytes.constData();
    std::string str(cstr);
    return str;
}

int MainWindow::saveImg(int pix_leftx, int pix_lefty, int pix_rightx, int pix_righty)
{
    QMessageBox saveMessageBox;
    saveMessageBox.setWindowTitle("保存中");
    //saveMessageBox.setText("请稍后");
    saveMessageBox.setIcon(QMessageBox::Information);
    saveMessageBox.setStandardButtons(QMessageBox::NoButton);
    saveMessageBox.setStyleSheet("QLabel{"
                         "min-width: 100px;"
                         "min-height: 50px; "
                         "}");
    saveMessageBox.show();

    int status = -1;
    int xOff = std::min(pix_leftx, pix_rightx);
    int yOff = std::min(pix_lefty, pix_righty);
    int xSize = abs(pix_rightx - pix_leftx);
    int ySize = abs(pix_righty - pix_lefty);

    if (xOff < 0 || yOff < 0)
    {
        qDebug() << "Error: crop position invalid";
        return status;
    }

    if (xSize <= 0 || ySize <= 0)
    {
        qDebug() << "Error: crop size invalid";
        return status;
    }

    QString posQString = QString::number(m_canvas->start_lon) + "_" + QString::number(m_canvas->start_lat);
    QString labelQString = m_canvas->text;
    outputFileName = prefixPath + "crop/" + m_fileName + "_" + posQString + "_" + labelQString;
    QString outputTifPath = outputFileName + ".tif";
    // only for TIF file

    GDALDataset* dstDataset = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
        outputTifPath.toUtf8().constData(), xSize, ySize, srcDataset->GetRasterCount(), srcDataset->GetRasterBand(1)->GetRasterDataType(), nullptr
    );

    GDALDriver* pngDriver = GetGDALDriverManager()->GetDriverByName("PNG");
    if (!pngDriver) {
        std::cerr << "错误：无法加载PNG驱动" << std::endl;
    }
    else {
        std::cout << "成功加载PNG驱动" << std::endl;
    }

    if (dstDataset == nullptr) {
        qDebug() << "错误：无法创建Tif文件";
        GDALClose(srcDataset);
        return status;
    }
    else {
        qDebug() << "成功创建临时Tif文件";
    }

    double newGeoTransform[6];
    newGeoTransform[0] = adfGeoTransform[0] + xOff * adfGeoTransform[1];
    newGeoTransform[1] = adfGeoTransform[1];
    newGeoTransform[2] = adfGeoTransform[2];
    newGeoTransform[3] = adfGeoTransform[3] + yOff * adfGeoTransform[5];
    newGeoTransform[4] = adfGeoTransform[4];
    newGeoTransform[5] = adfGeoTransform[5];
    dstDataset->SetGeoTransform(newGeoTransform);

    int inputBandCount = srcDataset->GetRasterCount();
    for (int bandIndex = 1; bandIndex <= inputBandCount; bandIndex++) {
        GDALRasterBand* inputBand = srcDataset->GetRasterBand(bandIndex);
        GDALRasterBand* outputBand = dstDataset->GetRasterBand(bandIndex);
        // int * int overflow?
        unsigned char* buffer = new unsigned char[xSize * ySize];
        inputBand->RasterIO(
            GF_Read, xOff, yOff, xSize, ySize, buffer, xSize, ySize, GDT_Byte, 0, 0
        );
        outputBand->RasterIO(
            GF_Write, 0, 0, xSize, ySize, buffer, xSize, ySize, GDT_Byte, 0, 0
        );
        delete[] buffer;
    }


    std::string pngFilePath = qstringToString(outputFileName) + ".png";
    GDALDataset* pngDataset = pngDriver->CreateCopy(pngFilePath.c_str(), dstDataset, FALSE, nullptr, nullptr, nullptr);
    if (!pngDataset) {
        std::cerr << "错误：无法创建PNG文件" << std::endl;
        GDALClose(pngDataset);
    }
    else {
        std::cout << "成功创建PNG文件" << std::endl;
    }

    saveMessageBox.close();

    status = 0;
    return status;
}

int MainWindow::saveToXml()
{
    int status = -1;
    QString xmlFilePath = outputFileName + ".xml";
#ifdef ENABLE_DEBUG_OUTPUT
    qDebug() << xmlFilePath;
#endif // ENABLE_DEBUG_OUTPUT
    QFile file(xmlFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "错误：无法打开xml文件";
        return status;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);

    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Root");

    // save geo information
    xmlWriter.writeStartElement("LeftTop_Geolocation");
    xmlWriter.writeAttribute("lontitude", QString::number(m_canvas->start_lon,'f',6));
    xmlWriter.writeAttribute("latitude", QString::number(m_canvas->start_lat,'f',6));
    xmlWriter.writeEndElement();

    xmlWriter.writeStartElement("RightDown_Geolocation");
    xmlWriter.writeAttribute("lontitude", QString::number(m_canvas->end_lon,'f',6));
    xmlWriter.writeAttribute("latitude", QString::number(m_canvas->end_lat,'f',6));
    xmlWriter.writeEndElement();

    // save label
    xmlWriter.writeTextElement("Label", m_canvas->text);

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    file.close();

    //set position to 0
    m_canvas->start_lon = 0;
    m_canvas->start_lat = 0;
    m_canvas->end_lon = 0;
    m_canvas->end_lat = 0;

    status = 0;
    return status;
}

void MainWindow::saveTif()
{
    if (m_canvas->start_lon == 0 && m_canvas->start_lat == 0 && m_canvas->end_lon == 0 && m_canvas->end_lat == 0) {
        QMessageBox errorBox;;
        errorBox.setIcon(QMessageBox::Critical);
        errorBox.setWindowTitle("错误");
        errorBox.setText("裁剪后再保存");
        errorBox.exec();
        return;
    }
    int pix_leftx = 0;
    int pix_lefty = 0;
    int pix_rightx = 0;
    int pix_righty = 0;
    if (latlon2pix(m_canvas->start_lon, m_canvas->start_lat, m_canvas->end_lon, m_canvas->end_lat,
        pix_leftx, pix_lefty, pix_rightx, pix_righty) == 0) {
#ifdef ENABLE_DEBUG_OUTPUT
        std::cout << "left pos: " << pix_leftx << " " << pix_lefty << std::endl;
        std::cout << "right pos: " << pix_rightx << " " << pix_righty << std::endl;
#endif // ENABLE_DEBUG_OUTPUT
        std::cout << "成功计算像素位置" << std::endl;
    }
    else {
        std::cout << "错误：无法计算像素位置" << std::endl;
    }

    if (saveImg(pix_leftx, pix_lefty, pix_rightx, pix_righty) == 0) {
        std::cout << "成功保存PNG图像" << std::endl;
    }
    else {
        std::cout << "错误：无法保存PNG图像" << std::endl;
    }

    if (saveToXml() == 0) {
        std::cout << "成功保存xml文件" << std::endl;
    }
    else {
        std::cout << "错误：无法保存xml文件" << std::endl;
    }

    QMessageBox::information(this, "成功", "裁剪文件保存成功");
}

void MainWindow::showMousePoint(const QgsPointXY&p)
{
    QgsPointXY xy = trans.transform(p, QgsCoordinateTransform::ForwardTransform);
    m_canvas->now_lon = xy.x();
    m_canvas->now_lat = xy.y();
}

void MainWindow::cropTif()
{
    if (!tifLoaded) {
        QMessageBox errorBox;;
        errorBox.setIcon(QMessageBox::Critical);
        errorBox.setWindowTitle("错误");
        errorBox.setText("加载后再进行裁剪");
        errorBox.exec();
        return;
    }
    m_canvas->startCrop = true;
}

void MainWindow::loadTif()
{
    // select tif file
    QString filePath = QFileDialog::getOpenFileName(this, "选择Tif文件", "", "TIFF Files (*.tif *.tiff)");
    QFileInfo fileInfo(filePath);
    m_fileName = fileInfo.baseName();
    // read Tif information
    srcDataset = static_cast<GDALDataset*>(GDALOpen(filePath.toUtf8().constData(), GA_ReadOnly));

    if (srcDataset == nullptr) {
        qDebug() << "错误：无法打开Tif源文件";
        return;
    }
    else {
        qDebug() << "成功打开源文件";
    }

    srcDataset->GetGeoTransform(adfGeoTransform);

    if (!filePath.isEmpty())
    {
        // load tif file from QgsRasterLayerRegistry as layer
        QgsRasterLayer* layer = new QgsRasterLayer(filePath, QFileInfo(filePath).completeBaseName());
        if (layer && layer->isValid())
        {
            // add layer to map
            QgsProject::instance()->addMapLayer(layer);
            m_canvas->setExtent(layer->extent());
            m_canvas->setLayers(QList<QgsMapLayer*>() << layer);
            m_canvas->refresh();
            tifLoaded = true;
        }
        else
        {
            QMessageBox::critical(this, "错误", "无法加载Tif文件");
        }
    }
}

void MainWindow::compressFile()
{
    QString zipPath = QFileDialog::getSaveFileName(nullptr, "保存压缩文件", QDir::homePath() + "/cropImage.zip", "ZIP Files (*.zip)");
    miniz_cpp::zip_file zipFile;

#ifdef ENABLE_DEBUG_OUTPUT
    qDebug() << zipPath;
#endif // ENABLE_DEBUG_OUTPUT

    QString folderPath = prefixPath + "crop/";
    QDir folderDir(folderPath);
    folderDir.setFilter(QDir::Files);

    QStringList fileNames = folderDir.entryList();

    for (const QString& fileName : fileNames) {
        QString filePath = folderPath + fileName;
        std::string strFilePath = qstringToString(filePath);
        QFile file(filePath);
        QString fileSuffix = QFileInfo(filePath).suffix();

        // donot save tmp tif file
        if (fileSuffix == "tif") {
            continue;
        }

        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "无法压缩文件: " << fileName;
            continue;
        }

        zipFile.write(strFilePath);

    }

    zipFile.save(qstringToString(zipPath));

    QMessageBox::information(this, "成功", "文件压缩成功");
}
