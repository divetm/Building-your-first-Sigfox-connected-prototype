#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

//variable donde guardaremos el valor de la fotoresistencia
float lightValue;
//variable ajustable que define el valor limite de la fotoresistencia que representa la presencia de un dedo sobre el lector
float lightThreshold;

//definiendo los pines sobre los cuales esta conectado el lector de huellas (Aqui son los pines D2 - cable verde y D3 - blanco)
SoftwareSerial adaSerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&adaSerial);
float finger_id; // variable donde guardaremos el ID del dedo escaneado
uint8_t id; // variable donde guardaremos el ID de una nueva huella o de una huella que será borrada

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

#ifdef SigfoxDeb
SoftwareSerial mySerial(0, 1); // RX, TX
#endif
char RespuestaSigfox[26];
String bufer; // variable donde guardaremos nuestro payload

void setup() {
  Serial.begin(9600);
  
  // lo que sigue es el setup del lector de huellas
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit lector de huellas ");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("¡Lector de huellas detectado!");
  } else {
    Serial.println("No se pudo detectar un lector de huellas :(");
    while (1) { delay(1); }
  }
  finger.getTemplateCount();
  Serial.print("El lector contiene "); Serial.print(finger.templateCount); Serial.println(" huellas registradas");

  int status;
  status = lcd.begin(16, 2);
  if(status) // non zero status means it was unsuccesful
  {
    status = -status; // convert negative status value to positive number

    // begin() failed so blink error code using the onboard LED if possible
    hd44780::fatalError(status); // does not return
  }
  lcd.print("Hello, World!");
  #ifdef SigfoxDeb
  mySerial.begin(9600);
  #endif
  pinMode(7, OUTPUT);
  #ifdef SigfoxDeb
  mySerial.println("\n\n\n\n\n\n\n\n\rInicio");
  #endif
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready");

  delay(5000);
  lightThreshold = analogRead(A0); // leyendo el valor de la fotoresistencia conectada al pin A0
}

// función que usamos para identificar una huella escaneada por el lector
// regresa el ID de la huella o -1 si la huella no existe o que hubo un error
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  int seconds = 0;
  while (p == FINGERPRINT_NOFINGER && seconds <= 10)
  {
    p = finger.getImage();
    seconds += 1;
    delay(1000);
  }
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("ID #"); Serial.print(finger.fingerID); 
  Serial.print(" identificado con una confianza de "); Serial.println(finger.confidence);
  return finger.fingerID; 
}

// función para registrar una nueva huella en el lector
uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Esperando por dedo valido para registrar bajo el ID #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Scan completado");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de communicación");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error en el proceso del scan");
      break;
    default:
      Serial.println("Error desconocido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen no lo suficientemente nítida");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de communicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se pudo identificar los atributos de la huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se pudo identificar los atributos de la huella");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }
  
  Serial.println("Levante el dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Por favor, vuelva a escanear el mismo dedo");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Scan completado");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de communicación");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Error en el proceso del scan");
      break;
    default:
      Serial.println("Error desconocido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagen convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagen no lo suficientemente nítiday");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Error de communicación");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("No se pudo identificar los atributos de la huella");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("No se pudo identificar los atributos de la huella");
      return p;
    default:
      Serial.println("Error desconocido");
      return p;
  }
  
  // OK converted!
  Serial.print("Creando modelo para #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("¡Huellas correspondientes!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de communicación");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Las huellas no son iguales");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Guardado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("No se pudo guardar en este lector");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error escribiendo al flash");
    return p;
  } else {
    Serial.println("Error desconocido");
    return p;
  }   
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Borrado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Error de comunicación");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("No se pudo borrar de este lector");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error escribiendo al flash");
    return p;
  } else {
    Serial.print("Error desconocido: 0x"); Serial.println(p, HEX);
    return p;
  }   
}

void add_int(int var2)    //funcion para agregar enteros al payload (hasta 255)
{
  byte* a2 = (byte*) &var2; //convertimos el dato a bytes
  String str2;
  str2=String(a2[0], HEX);  //convertimos el valor a hex en string 
  //verificamos si nuestro byte hex en string esta completo
  if(str2.length()<2)
  {
    bufer+=0+str2;    //si no, se agrega un cero
  }
  else
  {
    bufer+=str2;     //si esta completo, se copia tal cual
  }
}

void enviarcomandoATSigfox(String comandoAT)
{
  //Serial.println(comandoAT);
  delay(1000);
  unsigned long x=0;
  #ifdef SigfoxDeb
  mySerial.print("\r\n\tSigfox-->");
  mySerial.println(comandoAT);
  #endif
  while( Serial.available() > 0) Serial.read();
    x = 0;
  memset(RespuestaSigfox, '\0',sizeof(RespuestaSigfox)); 
  Serial.print(comandoAT);
  Serial.print("\r\n");
  while(true)
  {
    if(Serial.available() != 0)
    {
      char c = Serial.read();
      delay(500);
      Serial.print(c);
      delay(500);   
      RespuestaSigfox[x] = c;
      x++;
      if (RespuestaSigfox[x-1]=='=')
      {
        x=0;
      }
      if (strstr(RespuestaSigfox, "ER") != NULL)
      {
        Serial.println(" ");
        #ifdef SigfoxDeb
        mySerial.print("Comando OK\n\r");
        mySerial.println(RespuestaSigfox);
        #endif
        break;
      }
    }
  }
}

void enviar_bufer_get_nombre()
{
  int j;
  String nombre="";
  digitalWrite(7, HIGH);
  delay(1000);
  enviarcomandoATSigfox("AT$RC");
  delay(2000);
  //para hacer una peticion de downlink, se agrega ",1" al final del comando de enviar mensaje
  bufer += ",1\n";
  enviarcomandoATSigfox(bufer);
  digitalWrite(7, LOW);
  delay(1000);
  Serial.println(RespuestaSigfox);
  for(j=0;j<=22;j+=2){
    if (j==2){j=3;}
    if (j==5){j=6;}
    if (j==8){j=9;}
    if (j==11){j=12;}
    if (j==14){j=15;}
    if (j==17){j=18;}
    if (j==20){j=21;}
    char val = RespuestaSigfox[j] > 0x39 ? (RespuestaSigfox[j] - 'A') * 16 : (RespuestaSigfox[j] - '0') * 16;
    val += RespuestaSigfox[j+1] > 0x39 ? (RespuestaSigfox[j+1] - 'A') : (RespuestaSigfox[j+1] - '0');
    nombre += val;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hola ");
  lcd.print(nombre);
}

// función para leer un número en entrada en la terminal que no sea 0
uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop(){
  lightValue = analogRead(A0); // leyendo el valor de la fotoresistencia conectada al pin A0
  //Serial.println(lightValue);
  finger_id = -2;
  if (lightValue > lightThreshold) // si un dedo esta sobre el lector de huellas, este se prende
  {
    finger_id = getFingerprintIDez(); // scan de la huella
    delay(1000);
    bufer = "AT$SF="; // Creando el payload que le enviaremos a Sigfox
    add_int(finger_id); // el ID de la huella se agrega al payload
    if (finger_id != -1 && finger_id != -2 && finger_id != 1){ // si la huella está registrada como la de un niño
      add_int(0);
      add_int(0); // agregamos 0000 al payload para que cada envio sea del mismo tamaño
      delay(1000);
      enviar_bufer_get_nombre(); // enviamos el payload a Sigfox
      Serial.print("Bienvenido! Finger ID: ");Serial.println(finger_id);
      finger_id = -2;
    }
    else if (finger_id == 1){ // el ID 1 del lector tiene que ser el de la huella del administrador
      int choice = -1;
      Serial.print("Bienvenido! Finger ID: ");Serial.println(finger_id);
      Serial.println("¿Qué quiere hacer? 1: Registrar una huella nueva, 2: Borrar huella existente");
      choice = readnumber();
      if (choice == 1){
        add_int(choice);
        Serial.println("¡Listo para registrar une huella nueva!");
        Serial.println("Por favor ingrese el ID # (entre 1 y 127) que quiere usar para registrar esta huella...");
        id = readnumber();
        if (id == 0) {// ID #0 not allowed, try again!
           return;
        }
        Serial.print("Registrando al ID #");
        Serial.println(id);
        add_int(id);
        delay(500);
        getFingerprintEnroll();
        delay(1000);
        enviarcomandoATSigfox(bufer);
      }
      else if (choice == 2){
        add_int(choice);
        Serial.println("Por favor ingrese el ID existiente # (entre 1 y 127) que quiere borrar...");
        uint8_t id = readnumber();
        delay(1000);
        if (id == 0) {// ID #0 not allowed, try again!
           return;
        }
        Serial.print("Borrando el ID #");
        Serial.println(id);
        add_int(id);
        delay(500);
        deleteFingerprint(id);
        delay(1000);
        enviarcomandoATSigfox(bufer);
      }
    }
  }
}
