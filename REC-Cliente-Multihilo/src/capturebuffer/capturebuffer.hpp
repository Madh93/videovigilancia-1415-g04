#ifndef CAPTUREBUFFER_HPP
#define CAPTUREBUFFER_HPP

#include <QAbstractVideoSurface>

class CaptureBuffer : public QAbstractVideoSurface {

    Q_OBJECT

    signals:

        void transmitirImagen(QImage);

    public:

        CaptureBuffer(QObject *parent = 0);
        ~CaptureBuffer();

        QList<QVideoFrame::PixelFormat> supportedPixelFormats(
                QAbstractVideoBuffer::HandleType handleType =
                QAbstractVideoBuffer::NoHandle) const;
        bool present(const QVideoFrame &frame);
};

#endif // CAPTUREBUFFER_HPP
