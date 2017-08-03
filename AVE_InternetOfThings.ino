//AVE_InternetOfThings.ino
//Maquinaria Aurica
//Descripción:   ||Marca   ||Modelo  ||No se serie       ||Año de fabricación    

//Pata de Cabra  ||cat     ||815F    ||CAT0815FVBKL00404 ||2003
#include "SoftwareSerial.h" //dato GPS
#include <Separador.h>
Separador s;
SoftwareSerial datosgps(7,8); 
String GPS_Cv_F_S_Latitud( float,int=9,int=6,boolean=false);
String GPS_Cv_F_S_Longitud( float,int=10,int=6,boolean=false);
char aux_str[50];
int respuesta;
char direccion[300];
String latitud="";
String longitud="";
String direccion1;
int varGPS;
int varGPRS;

void setup() 
{     
     datosgps.begin(9600);
     Serial.begin(9600);
     //datosrecibidoss.reserve(200);
     delay(10);
     //String direccion1= "GET /tutoria/guardar.php?id="+String(bariable1)+"&ib="+String(bariable2)+"&latitud="+latitud+"&longitud="+longitud+" HTTP/1.1\r\nHost:avemeca.000webhostapp.com\r\nConnection: close\r\n\r\n";
    // String direccion1= "GET /tutoria/guardar.php?id="+String(bariable1)+"&ib="+String(bariable2)+"&latitud=23421&longitud=11111 HTTP/1.1\r\nHost:avemeca.000webhostapp.com\r\nConnection: close\r\n\r\n";
     //direccion1.toCharArray(direccion,300 );
     //String direccion1= "GET /tutoria/guardar.php?latitud="+latitud+"&longitud="+longitud+" HTTP/1.1\r\nHost:avemeca.000webhostapp.com\r\nConnection: close\r\n\r\n";
            //direccion1.toCharArray(direccion,300 );
}
void loop() 
{  

           varGPS=1;    
           //GPRS_Power_on();
           GPS_SOL_GPS();
           direccion1= "GET /tutoria/guardar.php?latitud="+latitud+"&longitud="+longitud+" HTTP/1.1\r\nHost:avemeca.000webhostapp.com\r\nConnection: close\r\n\r\n";
           direccion1.toCharArray(direccion,300 );
           GPRS_SOL_CONEXION();
         
}

         void GPRS_SOL_CONEXION()
         {
           Serial.println("Iniciar envio de datos");
          switch (varGPRS) {
              case 1:
                GPRS_Conectar();
                
              case 2:
                GPRS_PeticionHttp();
                
              case 3:
                GPRS_Desconectar();
                break;
              default:
                break;
          }
         }
         void GPRS_Power_on()
         {
             int respuestaPOn = 0;

             // Comprueba que el modulo SIM900 esta arrancado
             if (GPS_EnviarAT("AT", "OK", 2000) == 0)
             {
               Serial.println("Encendiendo el GPRS...");

               pinMode(9, OUTPUT);
               digitalWrite(9, HIGH);
               delay(1000);
               digitalWrite(9, LOW);

               // Espera la respuesta del modulo SIM900
               while (respuestaPOn == 0) {
                 // Envia un comando AT cada 2 segundos y espera la respuesta
                 respuestaPOn = GPS_EnviarAT("AT", "OK", 2000);
                 datosgps.println(respuestaPOn);
               }
             }
             varGPS=2;
         }
         
         void GPRS_Power_off()
         {
              	int respuestaPOf = 0;
              
                // Comprueba que el modulo SIM900 esta arrancado
                if (GPS_EnviarAT("AT", "OK", 2000) != 0)
                {
                  Serial.println("Apagando el GPRS...");
                  pinMode(9, OUTPUT);
                  digitalWrite(9, HIGH);
                  delay(1000);
                  digitalWrite(9, LOW);
                  // Espera la respuesta del modulo SIM900
                   while (respuestaPOf != 0) {
                    // Envia un comando AT cada 2 segundos y espera la respuesta
                   respuestaPOf = GPS_EnviarAT("AT", "OK", 2000);
                   datosgps.println(respuestaPOf);
                  }
                }
         }

         void GPRS_reiniciar()
         {
                Serial.println("Reiniciando...");
                GPRS_Power_off();
                GPRS_Power_on();
         }
         
         void GPRS_Conectar()
         {
               //GPS_EnviarAT("AT+CPIN=\"1867\"", "OK", 1000); //Introducimos el PIN de la SIM
                Serial.println("Conectando a la red...");
                //Comprobando la conexión a la red
                while ( GPS_EnviarAT("AT+CREG?", "+CREG: 0,1", 1000) == 0 )
                {
                }
                Serial.println("Conectado a la red.");
                GPS_EnviarAT("AT+CGATT=1\r", "OK", 1000); //Iniciamos la conexión GPRS
                //GPS_EnviarAT("AT+CSTT=\"http://www.telcel.com/\",\"telcel\",\"telcel\"", "OK", 3000); //Definimos el APN, usuario y clave a utilizar
                GPS_EnviarAT("AT+CSTT=\"internet.itelcel.com\",\"webgprs\",\"webgprs2003\"", "OK", 3000); //Definimos el APN, usuario y clave a utilizar
                GPS_EnviarAT("AT+CIICR", "OK", 3000); //Activamos el perfil de datos inalámbrico
                GPS_EnviarAT("AT+CIFSR", "", 3000); //Para obtener nuestra direccion IP
                varGPRS=2;
         }  
         void GPRS_Desconectar()
         {
                //GPS_EnviarAT("AT+CPIN=\"1867\"", "OK", 1000); //Introducimos el PIN de la SIM
                 Serial.println("Desconectando de la red...");
                 delay (5000);
                GPS_EnviarAT("AT+CIPCLOSE", "CLOSE OK", 10000); //Cerramos la conexion
                while (GPS_EnviarAT("AT+CIPSHUT", "OK", 10000)==0); //Cierra el contexto PDP del GPRS
                {

                }
                             
                 Serial.println("Desconectado de la red.");
                 varGPS=1;
           
         }     



         void GPRS_PeticionHttp()
         {
           if (GPS_EnviarAT("AT+CREG?", "+CREG: 0,1", 1000) == 1) //Comprueba la conexion a la red
           {
                  GPS_EnviarAT("AT+CIPSTART=\"TCP\",\"avemeca.000webhostapp.com\",\"80\"", "CONNECT OK", 5000); //Inicia una conexión TCP
                  
                  //Envíamos datos a través del TCP...
                   sprintf(aux_str, "AT+CIPSEND=%d", strlen(direccion));
         
         
                   if (GPS_EnviarAT(aux_str, ">", 10000) == 1)
                   { 
                          GPS_EnviarAT(direccion, "OK", 10000);
                    }
           }
           varGPRS=2;
        
         }







         void GPRS_PeticionHttplll()
         {
                 if (GPS_EnviarAT("AT+CREG?", "+CREG: 0,1", 1000) == 1) //Comprueba la conexion a la red
                 {
                        GPS_EnviarAT("AT+CIPSTART=\"TCP\",\"avemeca.000webhostapp.com\",\"80\"", "CONNECT OK", 5000); //Inicia una conexión TCP
                        //Envíamos datos a través del TCP...
                       //  sprintf(aux_str, "AT+CIPSEND=%d", strlen(direccion1));
                         //sprintf(aux_str, "AT+CIPSEND=%d", direccion1);
                         
               
                         if (GPS_EnviarAT("AT+CIPSEND",">", 1000) == 1)
                         { 
                                GPS_EnviarAT(direccion1, "OK", 1000);
                          }
                     varGPRS=2;
                 }
                else
                 {
                         // reiniciar();
                        GPRS_Conectar();
                 }
         }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

         void GPS_SOL_GPS()
         {
          Serial.println("iniciando GPS, espere por fabor");
          switch (varGPS) {
              case 1:
                GPRS_Power_on();
                //break;
              case 2:
                GPS_Activar_gps();
               // break;
              case 3:
                GPS_SOL_Coordenadas ();
               // break;
              case 4:
                GPS_Desactivar_gps();
                break;

              default:
                break;
          }
             
           //  GPRS_Power_on();
           //  GPS_Activar_gps();
           //  GPS_SOL_Coordenadas ();
           //  GPS_Desactivar_gps();
         }
         void GPS_SOL_Coordenadas()
         {
            Serial.println("Obteniedo Coordenadas...");  
               datosgps.print("AT+CGPSINF=0");//Solicita las coordenadas
               datosgps.print((char)10);
               datosgps.print((char)13);
              if (datosgps.available() != 0)
                {
               String datosrecibidoss = datosgps.readString();
               String Str_Latitud= s.separa(datosrecibidoss, ',', 1);
               String Str_Longitud= s.separa(datosrecibidoss, ',', 2);
               delay(5);
               float Flt_Latitud=Str_Latitud.toFloat();delay(20);
               Flt_Latitud=(Flt_Latitud/100)+0.2597827;delay(20);
               latitud= GPS_Cv_F_S_Latitud(Flt_Latitud);
               Serial.println("Latitud: " + latitud);
               float Flt_Longitud=Str_Longitud.toFloat();
               Flt_Longitud=(Flt_Longitud/-100)-0.154547;
               longitud=GPS_Cv_F_S_Longitud(Flt_Longitud);
               Serial.println("Longitud: "+ longitud);
                  }
                
                return latitud;
                return longitud;
                varGPS=4;

         }
           
         void GPS_Activar_gps()
         {
              GPS_EnviarAT("AT+CGPSPWR=1", "OK", 100); //activaos el gps
              Serial.println("Activando GPS...");
              delay (5000);
              //peguntar sobre el estado del gps, suele tardar un poco
              while ( GPS_EnviarAT("AT+CGPSSTATUS?", "+CGPSSTATUS: Location 3D Fix", 1000) == 0 )
              {
              }
              varGPS=3;

         }
         void GPS_Desactivar_gps()
         {
               GPS_EnviarAT("AT+CGPSPWR=0", "OK", 100); //desactivaos el gps
               Serial.println("Desactivando GPS...");
               GPS_EnviarAT("AT+CGPSSTATUS?", "+CGPSSTATUS: Location Unknown", 1000);
               delay (5000);
               
               //peguntar sobre el estado del gps para confirmar que se encuentre apagado

               varGPRS=1;
         }       
         int GPS_EnviarAT(String ATcommand, char* resp_correcta, unsigned int tiempo)
         {
         
                int x = 0;
                bool correcto = 0;
                char respuesta[100];
                unsigned long anterior;
         
                memset(respuesta, '\0', 100); // Inicializa el string
                delay(100);
                while ( datosgps.available() > 0) datosgps.read(); // Limpia el buffer de entrada
                datosgps.println(ATcommand); // Envia el comando AT
                x = 0;
                anterior = millis();
                // Espera una respuesta
                do {
                  // si hay datos el buffer de entrada del UART lee y comprueba la respuesta
                  if (datosgps.available() != 0)
                  {
                      respuesta[x] = datosgps.read();
                      x++;
                    // Comprueba si la respuesta es correcta
                    if (strstr(respuesta, resp_correcta) != NULL)
                    {
                      correcto = 1;
                    }
                  }
                }
                // Espera hasta tener una respuesta
                while ((correcto == 0) && ((millis() - anterior) < tiempo));
                Serial.println(respuesta);
         
                return correcto;
         }                
         String GPS_Cv_F_S_Longitud( float Flt_Longitud,int l,int d,boolean z)
         {
                char c[l+1];
                String slon;
                dtostrf(Flt_Longitud,l,d,c);
                slon=String(c);
                if(z){
                slon.replace(" ","0");
                }return slon;
         }               
         String GPS_Cv_F_S_Latitud( float Flt_Latitud,int l,int d,boolean z)
         {
                char c[l+1];
                String slat;
                dtostrf(Flt_Latitud,l,d,c);
                slat=String(c);
                if(z){
                slat.replace(" ","0");
                }return slat;
         }
