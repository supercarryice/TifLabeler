#ifndef MAPCANVAS_H
#define MAPCANVAS_H

#include <qgsmapcanvas.h>
#include <qgsrasterlayer.h>
#include <custominputdialog.h>

class MapCanvas : public QgsMapCanvas
{
public:
    MapCanvas(QWidget* parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public:
    bool isSelecting;
    bool startCrop;
    QPoint start_point;
    QPoint end_point;
    QString text;
    double start_lon;
    double start_lat;
    double end_lon;
    double end_lat;
    double now_lon;
    double now_lat;
    CustomInputDialog dialog;
};

#endif // MAPCANVAS_H
