#include <RtcDS1302.h>
#include <Servo.h>


// CONNECTIONS: Reloj
// DS1302 CLK/SCLK --> 5
// DS1302 DAT/IO --> 4
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND

// CONNECTIONS: Sensor y bocina
const int Trigger = 6;   // Pin digital 2 para el Trigger del sensor
const int Echo = 7;      // Pin digital 3 para el Echo del sensor
const int DistanciaLimite = 30;  // Distancia límite para emitir el sonido en centímetros
const int ZumbadorPin = 13;  // Pin digital 4 para el zumbador o altavoz

//CONNECTIONS: Servomotor
const int servoPin = 9;   // Pin PWM para el servomotor

Servo myServo;            // Objeto Servo

ThreeWire myWire(4,5,2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

void setup () 
{
  pinMode(Trigger, OUTPUT);  // Pin como salida
  pinMode(Echo, INPUT);      // Pin como entrada
  pinMode(ZumbadorPin, OUTPUT);  // Pin del zumbador como salida
  digitalWrite(Trigger, LOW);  // Inicializamos el pin con 0

  myServo.attach(servoPin);  // Asociar el objeto Servo al pin del servomotor

  //Reloj
    Serial.begin(57600);

    Serial.print("compiled: ");
    Serial.print(__DATE__);
    Serial.println(__TIME__);

    Rtc.Begin();

    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    printDateTime(compiled);
    Serial.println();


    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing

        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);
    }

    if (Rtc.GetIsWriteProtected())
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void loop () 
{
  /*Modulo de Reloj*/
  RtcDateTime now = Rtc.GetDateTime();
  String tiempo = printDateTime(now);

  Serial.println();
  Serial.print(tiempo);
  Serial.println();

  if (!now.IsValid())
  {
      // Common Causes:
      //    1) the battery on the device is low or even missing and the power line was disconnected
      Serial.println("RTC lost confidence in the DateTime!");
  }

  //Metodos
  checkDistance(); 
  checkHours(tiempo);

  delay(1000); // ten seconds
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

String printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u:%02u:%02u"),
            //dt.Month(),
            //dt.Day(),
            //dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    //Serial.print(datestring);
    return datestring;
}

void checkHours(const String tiempo){
  //ver si es hora de comer
  if (tiempo.equals("05:33:50") || tiempo.equals("05:44:00")) {
    //activamos el servomotor
    Serial.println("Los strings son iguales");
    moveServo();
  }

}


void checkDistance(){
  /*Sensor Ultrasonico*/
  long t;  // Tiempo que demora en llegar el eco
  long d;  // Distancia en centímetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          // Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);

  t = pulseIn(Echo, HIGH);  // Obtenemos el ancho del pulso
  d = t / 59;               // Escalamos el tiempo a una distancia en cm

  Serial.print("Distancia: ");
  Serial.print(d);          // Enviamos serialmente el valor de la distancia
  Serial.println(" cm");

  //Podriamos poner esto en una funcion para que 
  //solo suene una vez por un periodo de timepo en lugar de eternamente.
  if (d <= DistanciaLimite) {
    // Emitir sonido
    tone(ZumbadorPin, 1000);
  } else {
    // Detener el sonido
    noTone(ZumbadorPin);
  }
}

void moveServo(){
  // Mover el servomotor a 0 grados (posición inicial)
  myServo.write(0);
  delay(1000);  // Esperar 1 segundo
  
  // Mover el servomotor a 90 grados (posición media)
  myServo.write(90);
  delay(1000);  // Esperar 1 segundo
  
  // Mover el servomoztor a 180 grados (posición final)
  myServo.write(180);
}

