#include <QApplication>
#include "rec.hpp"
#include <unistd.h> //fork(), close(), setsid()...
#include <sys/stat.h> //umask()
#include <iostream>   //cerr()
#include <errno.h>  //errno
#include <fcntl.h>  //open()
#include <syslog.h>  //syslog(), openlog()
#include <pwd.h>    //passwd(), user(), getpwnam()
#include <grp.h>    //group(), getgrnam()


int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    /***********************/
    /* Crear Proceso Hijo */
    /*********************/

    pid_t pid;

    // Nos clonamos a nosotros mismos creando un proceso hijo
    pid = fork();

    // Si pid es < 0, fork() falló
    if (pid < 0) {
        // Mostrar la descripción del error y terminar
        std::cerr << strerror(errno) << std::endl;
        exit(10);
    }

    // Si pid es > 0, estamos en el proceso padre
    if (pid > 0)
        exit(0); // Terminar el proceso

    // Si la ejecución llega a este punto, estamos en el hijo

    /******************/
    /* Cambiar Umask */
    /****************/

    umask(0);

    /************************/
    /* Registro de Eventos */
    /**********************/

    // Abrir una conexión al demonio syslog
    openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);

    // Enviar un mensaje al demonio syslog
    syslog(LOG_NOTICE, "Demonio iniciado con éxito");


    /***********************/
    /* Crear nueva sesión */
    /*********************/

    // Intentar crear una nueva sesión
    if (setsid() < 0) {
        syslog(LOG_ERR, "No fue posible crear una nueva sesión\n");
        exit(11);
    }

    // Cambiar directorio de trabajo
    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "No fue posible cambiar el directorio de trabajo a /\n");
        exit(12);
    }

    /*************************/
    /* Reabrir descriptores */
    /***********************/

    // Cerrar los descriptores de la E/S estándar
    close(STDIN_FILENO);            // fd 0
    close(STDOUT_FILENO);           // fd 1
    close(STDERR_FILENO);           // fd 2

    // Abrir nuevos descriptores de E/S
    int fd0 = open("/dev/null", O_RDONLY);  // fd0 == 0
    int fd1 = open("/dev/null", O_WRONLY);  // fd0 == 1
    int fd2 = open("/dev/null", O_WRONLY);  // fd0 == 2


    /*************************/
    /* Cambiar personalidad */
    /***********************/

    // Cambiar el usuario efectivo del proceso a 'recd'
    passwd* user = getpwnam("recd");
    seteuid(user->pw_uid);

    // Cambiar el grupo efectivo del proceso a 'midemonio'
    group* group = getgrnam("recd");
    setegid(group->gr_gid);

    //Enviar un mensaje al demonio syslog
    syslog(LOG_NOTICE, "Demonio iniciado con éxito\n");


    /****************/
    /* Asignar PID */
    /**************/

    // Archivo que contiene identificador de proceso del demonio
    QFile file("/var/run/recd.pid");
    QTextStream out(&file);
    out << pid;
    file.close();


    return a.exec();
}
