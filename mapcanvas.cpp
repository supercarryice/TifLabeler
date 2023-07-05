#include <mapcanvas.h>
#include <QMessageBox>
//#define ENABLE_DEBUG_OUTPUT

MapCanvas::MapCanvas(QWidget* parent):QgsMapCanvas (parent)
{
    isSelecting = false;
    startCrop = false;
    start_point = QPoint();
    end_point = QPoint();
    text = QString();
    start_lon = 0.0;
    start_lat = 0.0;
    end_lon = 0.0;
    end_lat = 0.0;
    now_lon = 0.0;
    now_lat = 0.0;
}

void MapCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && startCrop)
    {
        // get screen position
        start_point = event->pos();
        start_lon = now_lon;
        start_lat = now_lat;
        isSelecting = true;
#ifdef ENABLE_DEBUG_OUTPUT
        std::cout << "start" << std::endl;
        std::cout << start_point.x() << std::endl;
        std::cout << start_point.y() << std::endl;
        qDebug() << start_lon;
        qDebug() << start_lat;
#endif // ENABLE_DEBUG_OUTPUT
    }

    QgsMapCanvas::mousePressEvent(event);
}

void MapCanvas::mouseMoveEvent(QMouseEvent *event)
{
    if (isSelecting && startCrop)
    {
        end_point = event->pos();
        update();
    }

    QgsMapCanvas::mouseMoveEvent(event);
}

void MapCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isSelecting && startCrop)
    {
        end_point = event->pos();
        isSelecting = false;
        startCrop = false;
        end_lon = now_lon;
        end_lat = now_lat;
#ifdef ENABLE_DEBUG_OUTPUT
        std::cout << "end" << std::endl;
        std::cout << end_point.x() << std::endl;
        std::cout << end_point.y() << std::endl;
        std::cout << end_lon << std::endl;
        std::cout << end_lat << std::endl;
#endif // ENABLE_DEBUG_OUTPUT
        if (dialog.exec() == QDialog::Accepted)
        {
            text = dialog.getText();
#ifdef ENABLE_DEBUG_OUTPUT
            qDebug() << "what input? " << text;
#endif // ENABLE_DEBUG_OUTPUT
        }
        QMessageBox::information(this, "成功", "文件裁剪成功");
    }

    QgsMapCanvas::mouseReleaseEvent(event);
}

void MapCanvas::paintEvent(QPaintEvent *event)
{
    QgsMapCanvas::paintEvent(event);
    if (isSelecting && startCrop)
    {
        QPainter painter(this->viewport());
        painter.setPen(Qt::red);
        painter.drawRect(QRect(start_point, end_point));
    }
}
