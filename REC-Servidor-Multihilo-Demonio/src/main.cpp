#include <QApplication>
#include <QDebug>
#include <unistd.h> //fork(), close(), setsid()...
#include <sys/stat.h> //umask()
#include <iostream>   //cerr()
#include <errno.h>  //errno
#include <fcntl.h>  //open()
#include <syslog.h>  //syslog(), openlog()
#include <pwd.h>    //passwd(), user(), getpwnam()
#include <grp.h>    //group(), getgrnam()

#include "rec.hpp"
#include "demonio.hpp"


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

    /*********************/
    /* Cambiar Permisos */
    /*******************/
    umask(0);

    /************************/
    /* Registro de Eventos */
    /**********************/

    // Abrir una conexión al demonio syslog
    openlog(argv[0], LOG_NOWAIT | LOG_PID, LOG_USER);

    // Enviar un mensaje al demonio syslog
    syslog(LOG_NOTICE, "Iniciando demonio REC...");


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
        syslog(LOG_ERR, "No fue posible cambiar el directorio de trabajo a '/'\n");
        exit(12);
    }

    /***************************/
    /* Redirigir descriptores */
    /*************************/

    // Cerrar los descriptores de la E/S estándar
    close(STDIN_FILENO);            // fd 0
    close(STDOUT_FILENO);           // fd 1
    close(STDERR_FILENO);           // fd 2

    // Abrir nuevos descriptores de E/S
    int fd0 = open("/dev/null", O_RDONLY);  // fd0 == 0
    int fd1 = open("/dev/null", O_WRONLY);  // fd0 == 1
    int fd2 = open("/dev/null", O_WRONLY);  // fd0 == 2
    Q_UNUSED(fd0);
    Q_UNUSED(fd1);
    Q_UNUSED(fd2);


    /*************************/
    /* Cambiar personalidad */
    /***********************/

    // Cambiar el usuario efectivo del proceso
    passwd* usuario = getpwnam("alumno");   // USUARIO PROVISIONAL!
    if (!usuario) {
        std::cerr << "No existe el usuario en el sistema." << std::endl;
        return 0;
    } else
        seteuid(usuario->pw_uid);

    // Cambiar el grupo efectivo del proceso
    group* grupo = getgrnam("alumno");    // GRUPO PROVISIONAL!
    if (!grupo) {
        std::cerr << "No existe el grupo en el sistema." << std::endl;
        return 0;
    } else
        setegid(grupo->gr_gid);

    // Enviar un mensaje al demonio syslog
    syslog(LOG_NOTICE, "Demonio REC iniciado con éxito");

    /****************/
    /* Asignar PID */
    /**************/

    // Archivo que contiene identificador de proceso del demonio
    QFile file("/var/run/recd.pid");
    QTextStream out(&file);
    out << pid;
    file.close();


    //sleep(20);
    setupUnixSignalHandlers();


    // Cuando el demonio termine, cerrar la conexión con
    // el servicio syslog
    syslog(LOG_NOTICE, "Demonio REC finalizado con éxito");
    closelog();

    return a.exec();
}
