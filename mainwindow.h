#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <mapcanvas.h>
#include <qgscoordinatetransform.h>
#include <qgspointxy.h>
#include <gdal_priv.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QString rootPath, QWidget* parent = nullptr);
    int latlon2pix(double lon_min, double lat_min, double lon_max, double lat_max,
        int& leftx, int& lefty, int& rightx, int& righty);
    std::string qstringToString(QString qstr);
    int saveImg(int pix_leftx, int pix_lefty, int pix_rightx, int pix_righty);
    int saveToXml();
private slots:
    void saveTif();
    void showMousePoint(const QgsPointXY&p);
    void cropTif();
    void loadTif();
    void compressFile();
public:
    QgsCoordinateTransform trans;
    QgsCoordinateReferenceSystem viewCrs;

    MapCanvas* m_canvas;
    QString outputFileName;
    QString prefixPath;
    bool tifLoaded;
    QPoint selectionStart;
    QPoint selectionEnd;
    QRect selectionRect;
    QString m_fileName;
    GDALDataset* srcDataset;
    double adfGeoTransform[6];
};

#endif // MAINWINDOW_H
