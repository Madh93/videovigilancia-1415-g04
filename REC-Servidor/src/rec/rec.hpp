#ifndef REC_HPP
#define REC_HPP

// Títulos de ventana
#define WINDOW_TITLE_OFF "Offline - REC (Panel de Control)"
#define WINDOW_TITLE_ON "Online - REC (Panel de Control)"
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
#include <QDir>
#include <QtMath>
#include <QTcpServer>
#include <QTcpSocket>

#include "acerca.hpp"
#include "puertoescucha.hpp"
#include "usuario.h"
#include "ssl.hpp"

namespace Ui {
    class Rec;
}

class Rec : public QMainWindow {

    Q_OBJECT

    private:

        Ui::Rec *ui;
        QLabel *label;
        QSettings preferencias;
        QPixmap pixmap;
        QTcpServer *servidor;
        QTcpSocket *cliente;
        int bytes_a;
        int estado;
        QVector<usuario*> users;

        void activarFuncionalidades(bool cond);
        void crearLabel();
        void guardarImagen(QPixmap imagen, QString usuario, uint timestamp);

    private slots:

        // Archivo
        void on_actionIniciarServidor_triggered();
        void on_actionCerrar_triggered();
        void on_actionSalir_triggered();

        // Herramientas
        void on_actionTomarInstantanea_triggered();

        // Preferencias
        void on_actionEstablecerPuerto_triggered();
        void on_actionPantallaCompleta_toggled(bool cond);

        // Ayuda
        void on_actionAyuda_triggered();
        void on_actionAcercaDe_triggered();
        void on_actionAcercaDeQt_triggered();

        //conexion

        void nueva_conexion();
        void leer_datos();

    public:

        explicit Rec(QWidget *parent = 0);
        ~Rec();
};

#endif // REC_HPP
