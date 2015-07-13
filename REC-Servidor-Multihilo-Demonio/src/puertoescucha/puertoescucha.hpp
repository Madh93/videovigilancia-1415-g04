#ifndef PUERTOESCUCHA_HPP
#define PUERTOESCUCHA_HPP

#include <QDialog>

namespace Ui {
    class PuertoEscucha;
}

class PuertoEscucha : public QDialog {

    Q_OBJECT

    private:
        Ui::PuertoEscucha *ui;

    public:
        explicit PuertoEscucha(quint16 actual = 0);
        ~PuertoEscucha();

        quint16 getPuerto();
};

#endif // PUERTOESCUCHA_HPP
