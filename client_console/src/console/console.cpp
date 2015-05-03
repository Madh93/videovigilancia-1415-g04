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
   eleccion_= in_->readLine();


   if (eleccion_[0]=='1'){
    show_ip();
   } else if (eleccion_[0]=='2'){
    show_port();
   } else if (eleccion_[0] == '3'){
     show_device();
   } else if(eleccion_[0]=='4'){
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

     *out_ << "  \n\n Seleccion_e que dispositivo desea utilizar:" << endl;

     //Mostrar dispositivos disponibles (para poder seleccion_ar el que queramos segun la i que es el numero)
     for (int i=0; i< devices_.length();i++) {
         *out_<<" " << i <<"--"<< devices_[i] <<endl;
     }

     //Segun la eleccion_ comparamos para saber cual de los dispositivos es
     eleccion_  = in_->readLine();
     QString aux="";
     int selection_=-1;
     for(int i=0; i<devices_.length(); i++){
         aux.setNum(i);
         if(eleccion_.compare(aux)==0) {
             selection_=i;
         }
     }

     //Creamos variable del dispositivo, entramos a la configuracion y en dipositivo ponemos el elegido que se guardo
     //en la variable
     QString device_selec_;
     config_.setValue("DEVICE", devices_[selection_]);
     device_selec_=devices_[selection_];
     config_.sync();
     cout << "\n->Dispositivo seleccionado\n"<<endl;
     show_menu();

  }


