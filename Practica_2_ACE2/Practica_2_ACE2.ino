#include <SoftwareSerial.h>
#include <WiFiEsp.h>

char ssid[] = "HUAWEI GT3";
char pass[] = "Saijaman1234";
int variable = 1;
int status = WL_IDLE_STATUS;
char server[] = "pagina-p2.azurewebsites.net";

const int Trigger = 2;   //Pin digital 2 para el Trigger del sensor
const int Echo = 3;   //Pin digital 3 para el Echo del sensor
int pinReceptor = A0;
int sensorVal;
bool color_detectado = false;
int tiempoPausado=0;
boolean obstaculo = false;
boolean Esperando = false;
char deAmbrous;
const int motorActivado = 4; //Pin que le manda señal al arduino que tiene los motores
const int pinLinea = 5;
int valorLineaAnalogo1=0;
int valorLineaAnalogo2=0;
/*Parte de la comunicacion bluetooth*/
int estado =0;
boolean numeroDetectado = false;
boolean numeroCompletoEncontrado = false;
int numeroPaArduino =0;
String caracterNumero="";
String numeroCompleto="";
int timepoEsperaRecibido=2;//valores por defecto de tiempo y velocidad
int DistanciaRecibida=20;
int numeroRecibdoBT=0;
boolean opcionA = false;
boolean opcionB = false;
boolean opcionC = false;
boolean opcionD = false;
boolean opcionE = false;
boolean conectado = false;
boolean opcionTiempo = false;
boolean opcionDistancia= false;
SoftwareSerial mySerial(10,11); //RX,TX
SoftwareSerial BT1(19, 18); // RX | TX
WiFiEspServer servidor(443);

//++++++++++++++++++++++++++++++++++++++++

void setup() {
  Serial.begin(115200);

  Serial1.begin(9600);//iniciailzamos la comunicación
  mySerial.begin(9600);
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(motorActivado, OUTPUT); 
  pinMode(pinLinea,OUTPUT);
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0

    WiFi.init(&Serial1);
  while (status != WL_CONNECTED)
    {
        Serial.print("Conecting to wifi network: ");
        Serial.println(ssid);

        status = WiFi.begin(ssid, pass);
    }
  Serial.print("IP Address of ESP8266 Module is: ");
  Serial.println(WiFi.localIP());
  Serial.println("You're connected to the network");

  // Start the server
  servidor.begin();
  
}
WiFiEspClient clienteServidor;

void loop()
{
  int tiempo=timepoEsperaRecibido;
  leerBluetooth();
  tiempo=tiempo*850;//el tiempo se multiplica por mil porque en arduino 1000 de delay es un seg
  ultrasonico(DistanciaRecibida,tiempo);//Los parametros son la distancia y el tiempo de espera
  if(color_detectado==true){
        digitalWrite(5,HIGH);
        delay(100);
    }else{
          digitalWrite(5,LOW);
          delay(100);
      }
  
  String yourdata="{ \"Id_recorrido\":0, \"Tiempo\":"+String(variable)+", \"Velocidad\":"+String(variable)+", \"Distancia\":"+String(variable)+", \"Peso\":"+String(variable)+", \"Obstaculos\":"+String(variable)+", \"Tiempo_obstaculos\":"+String(variable)+", \"Id_tipo\":"+String(variable)+"}";

  if(conectado==false){
    clienteServidor.connect(server,80);
    conectado = true;
   }


  if(conectado == true){
    Serial.println("Entro al servidor");
      //Serial.println(jsonInicial);
      Serial.println(yourdata);
      //Serial.println(yourdata1);
      //Serial.println(yourdata2);
  int value = 2.5;
  String content = "{\"JSON_key\": " + String(value) + "}";
  clienteServidor.println("POST /api/recorrido HTTP/1.1");
  clienteServidor.println("Host: pagina-p2.azurewebsites.net");
  clienteServidor.println("Accept: */*");
  clienteServidor.println("Content-Length: " + String(yourdata.length()));
  clienteServidor.println("Content-Type: application/json");
  clienteServidor.println();
  clienteServidor.println(yourdata);
  
  //clienteServidor.println("POST /api/Recorrido HTTP/1.1");
  //clienteServidor.println("Host: pagina-arqui2.azurewebsites.net");
  //clienteServidor.println("Accept: */*");
  //clienteServidor.println("Content-Length: " + String(yourdata1.length()));
  //clienteServidor.println("Content-Type: application/json");
  //clienteServidor.println();
  //clienteServidor.println(yourdata1);
  //clienteServidor.println("POST /api/Recorrido HTTP/1.1");
  //clienteServidor.println("Host:  pagina-arqui2.azurewebsites.net");
  //clienteServidor.println("Accept: */*");
  //clienteServidor.println("Content-Length: " + String(yourdata2.length()));
  //clienteServidor.println("Content-Type: application/json");
  //clienteServidor.println();
  //clienteServidor.println(yourdata2); 
      //paAroo="^ MQ7: "+String(adc_MQ7)+ " PPM, MQ135: "+String(adc_MQ135)+ " PPM, UV: "+String(radiacion)+ " W/m2";
      //paAroo=String(adc_MQ7)+ ", "+String(adc_MQ135)+ ", "+String(radiacion);
      //blueSerial.print(paAroo);
      //Serial.println(paAroo);
      Serial.println("Done!");
      Serial.println("Disconnecting from server...");
      //clienteServidor.stop();
  }else{
    Serial.println("Fallo la conexion");
    Serial.println("Desconectando.");
    //clienteServidor.stop();
  }
  Serial.println("Salio");

   //delay(2000);

  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }

  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial1.write(inByte);
  }
  delay(500);
}
void leerBluetooth(){
  if(mySerial.available()>0){
    estado = mySerial.read();
    if(estado>=48&&estado<=57){
      numeroCompletoEncontrado=true;
      //Serial.print("Numero detectado, longitud ");
      //Serial.print(mySerial.available());
      //Serial.print(" ");
      convertirAscii(estado);
      delay(100);
      numeroDetectado=true;
    }else{
      if(numeroDetectado){
        numeroRecibdoBT = numeroCompleto.toInt();
        if(opcionTiempo){//aqui diferencia el numero del tiempo y distancia
          opcionTiempo=false;
          timepoEsperaRecibido = numeroRecibdoBT;
        }else if(opcionDistancia){
          opcionDistancia=false;
          DistanciaRecibida = numeroRecibdoBT;
        }
        Serial.print("Numero recibido: ");
        Serial.println(numeroRecibdoBT);
      }
      numeroCompletoEncontrado = false;
      numeroDetectado=false;
      numeroCompleto="";
      caracterNumero="";
    }
    
    //Serial.print("valido ");
    Serial.println(estado);   
  }
  switch (estado){
    case 'A':
    //INICIAR RECORRIDO
      if(!opcionA){
        opcionA= true;
      }else{
        opcionA=false;
      }  
    break;
    case 'B':
    //BITACORA
     if(!opcionB){
        opcionB= true;
      }else{
        opcionB=false;
      }
    break;
    case 'C':
    if(!opcionC){
        opcionC= true;
      }else{
        opcionC=false;
      }
    //digitalWrite(led1, LOW);
    break;
    case 'D':
    if(!opcionD){
        opcionD= true;
      }else{
        opcionD=false;
      }
    //digitalWrite(led1, LOW);
    break;
    case 'E':
    if(!opcionE){
        opcionE= true;
      }else{
        opcionE=false;
      }
    //digitalWrite(led1, LOW);
    break;
    case 'T':
      opcionTiempo=true;
      break;
    case 'M':
      opcionDistancia=true;
      break;
  }
  
}
void convertirAscii(int ascii){
  switch(ascii){   
    case 48:
      numeroCompleto+="0";
      caracterNumero="0";
      break;
    case 49:
      numeroCompleto+="1";
      caracterNumero="1";
      break;
    case 50:
      numeroCompleto+="2";
      caracterNumero="2";
      //Serial.print(" Deberia regresar un pinche 2!");
      break;
    case 51:
      numeroCompleto+="3";
      caracterNumero="3";
      break;
    case 52:
      numeroCompleto+="4";
      caracterNumero="4";
      break;
    case 53:
      numeroCompleto+="5";
      caracterNumero="5";
      break;
    case 54:
      numeroCompleto+="6";
      caracterNumero="6";
      break;
    case 55:
      numeroCompleto+="7";
      caracterNumero="7";
      break;
    case 56:
      numeroCompleto+="8";
      caracterNumero="8";
      break;
    case 57:
      numeroCompleto+="9";
      caracterNumero="9";
      break;
  }
}

void seguirColor(){
  
  valorLineaAnalogo1 = analogRead(A0);
  valorLineaAnalogo2 = analogRead(A1);
  if(valorLineaAnalogo1>=100  && valorLineaAnalogo1<=200 && valorLineaAnalogo2>=100  && valorLineaAnalogo2<=200){

    Serial.println("Color negro detectado");
    color_detectado=true;
  }else if(valorLineaAnalogo1>=870  && valorLineaAnalogo1<=999 && valorLineaAnalogo2>=870  && valorLineaAnalogo2<=999){
    Serial.println("Color blanco detectado");
    color_detectado=false;
  }else{
    
  }
}

void ultrasonico(long distancia, int tiempoEspera){
  
  long t; //tiempo que demora en llegar el eco
  long d; //distancia en centimetros
  
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;
  
  if(d<=distancia){
    Serial.print("Obstaculo Dectectado!!");//En esta parte va el codigo para que el vehiculo se debe quedar quieto
    Serial.println();
    digitalWrite(motorActivado, HIGH);
    delay(100);
    obstaculo = true;
    //Esperando = true;
    tiempoPausado=0;
  }else{
    if(obstaculo){
      if(tiempoPausado<tiempoEspera){
        Serial.print("En espera.....");//En esta parte el vehiculo debe mantenerse en equilibrio, sin avanzar
        Serial.println();
        digitalWrite(motorActivado, HIGH);
      }else if(tiempoPausado>=tiempoEspera){
        obstaculo = false;
      }
    }else{
      Serial.print("Distancia: ");//Aqui se puede mover con normalidad.
      seguirColor();     
      Serial.print(d);      //Enviamos serialmente el valor de la distancia
      Serial.print("cm");
      Serial.println();
      
      digitalWrite(motorActivado, LOW);
       
      
      digitalWrite(pinLinea,LOW);
    }
     
     
  }
  delay(100);          //Hacemos una pausa de 100ms
  
  tiempoPausado+=1000;
}
