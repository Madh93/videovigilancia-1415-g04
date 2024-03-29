#ifndef REC_HPP
#define REC_HPP

// Títulos de ventana
#define WINDOW_TITLE "REC"
#define WINDOW_RECORDING "Capturando - REC"
#define WINDOW_CRITICAL "Error - REC"
#define WINDOW_WARNING "Aviso - REC"

#include <QMainWindow>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QPainter>
#include <QSettings>
#include <QTime>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QBuffer>
#include <opencv2/opencv.hpp>

#include "acerca.hpp"
#include "capturebuffer.hpp"
#include "captura.pb.h"
#include "conexion.hpp"
#include "cvmatandqimage.h"
#include "dispositivos.hpp"

typedef std::vector<cv::Mat> ImagesType;
typedef std::vector<std::vector<cv::Point> > ContoursType;

namespace Ui {
    class Rec;
}

class Rec : public QMainWindow {

    Q_OBJECT

    private:

        Ui::Rec *ui;
        QCamera *camara;
        CaptureBuffer *buffer;
        QTcpSocket *socket;
        QLabel *label;
        cv::BackgroundSubtractorMOG2 backgroundSubtractor;
        QLabel statusIzda;
        QSettings preferencias;
        QPixmap pixmap;
        std::vector<cv::Rect> boxes;
        bool conectado;

        void activarFuncionalidades(bool cond);
        void crearLabel();
        void limpiarCamara();
        void limpiarSocket();
        void conectarConServidor();
        bool detectarMovimiento(QImage *imagen);

    private slots:

        void actualizarImagen(QImage imagen);
        void iniciarCamara();
        void desconectar();
        void socketError(QAbstractSocket::SocketError error);

        // Archivo
        void on_actionCapturar_triggered();
        void on_actionCerrar_triggered();
        void on_actionSalir_triggered();

        // Herramientas
        void on_actionTomarInstantanea_triggered();

        // Preferencias
        void on_actionConfigurarConexion_triggered();
        void on_actionDispositivos_triggered();
        void on_actionPantallaCompleta_toggled(bool cond);

        // Ayuda
        void on_actionAyuda_triggered();
        void on_actionAcercaDe_triggered();
        void on_actionAcercaDeQt_triggered();

    public:

        explicit Rec(QWidget *parent = 0);
        ~Rec();
};

#endif // REC_HPP
