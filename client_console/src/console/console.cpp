 #include "console.hpp"

using namespace std;

Console::Console(QObject *parent):

    QObject(parent),
    in_(NULL),
    out_(NULL),
    config_("client.ini", QSettings::IniFormat) {

    in_= new QTextStream(stdin, QIODevice::ReadOnly);
    out_= new QTextStream(stdout, QIODevice::WriteOnly);
    ip_=config_.value("IP","127.0.0.1").toString();
    port_=config_.value("PORT","15000").toString();
    devices_=QCamera::availableDevices();
}


Console::~Console(){

    delete in_;
    delete out_;

    in_=NULL;
    out_=NULL;
}

void Console::show_menu(void) {

   std::cout<<"======================="<<std::endl;
   std::cout<<"| MENÚ CONSOLA CLIENTE |"<<std::endl;
   std::cout<<"======================="<<std::endl;
   std::cout<<"1. Configurar IP" <<std::endl;
   std::cout<<"2. Configurar Puerto" <<std::endl;
   std::cout<<"3. Configurar Dispositivo de grabación" <<std::endl;
   std::cout<<"4. Salir" <<std::endl;

   std::cout<<"\nSeleccione la opción deseada"<<std::endl;
   eleccion= in_->readLine();


   if (eleccion[0]=='1'){
    show_ip();
   } else if (eleccion[0]=='2'){
    show_port();
   } else if (eleccion[0] == '3'){
     show_device();
   } else if(eleccion[0]=='4'){
     exit(0);
   } else
     std::cout<<"\n¡ERROR! Opción invalida, vuelva a intentarlo.\n"<<std::endl;
     show_menu();

}

  void Console::show_ip(){

      QString response_;
      *out_ << "Introduzca IP: " <<endl;
      ip_=in_->readLine();
      std::cout<<"\n->IP introducida\n"<<std::endl;

      config_.setValue("IP",ip_);
      config_.sync();

      show_menu();
  }

  void Console::show_port(){

      *out_ << "Introduzca Puerto: " << endl;
      port_=in_->readLine();
      std::cout<<"\n->Puerto introducido\n"<<std::endl;

      config_.setValue("PORT",port_);
      config_.sync();

      show_menu();
  }

  void Console::show_device(){

      std::cout<<"HOLA DEVICE"<<std::endl;

      /**out_ << "Introduzca Dispositivo: " <<endl;
      devices_=in_->readLine();

      config_.setValue("DEVICE",devices_);
      config_.sync();*/

      show_menu();
  }


