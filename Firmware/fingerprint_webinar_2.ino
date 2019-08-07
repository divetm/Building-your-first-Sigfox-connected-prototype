#include <Adafruit_Fingerprint.h>
#include <Wire.h>
#include <hd44780.h>                       // main hd44780 header
#include <hd44780ioClass/hd44780_I2Cexp.h> // i2c expander i/o class header

//var where we will store the photoresistor's reading
float lightValue;
//var that stores the photoresistor's reading during setup (no finger), when lightValue>lightThreshold a finger is detected
float lightThreshold;

//define the pins where the fingerprint is connected (pin D2 - green cable & D3 - white one)
SoftwareSerial adaSerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&adaSerial);
float finger_id; // var where we'll store the fingerprint's ID once it's scanned
uint8_t id; // var where we'll store the ID the administrator can chose to modify (add/erase) when they scan their fingerprint

hd44780_I2Cexp lcd; // declare lcd object: auto locate & auto config expander chip

#ifdef SigfoxDeb
SoftwareSerial mySerial(0, 1); // RX, TX: creating a virtual serial on top of the native one to comunicate with Sigfox's module
#endif
char SigfoxReply[26]; // var where we'll store Sigfox's module's reply to the AT commands that we send to it
String buffer; // var where we'll store the payload that we'll send over Sigfox

void setup() {
  Serial.begin(9600);
  
  // fingerprint scanner set up 
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit fingerprint sensor ");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  finger.getTemplateCount();
  Serial.print("The sensor contains "); Serial.print(finger.templateCount); Serial.println(" registered fingerprints");

  // lcd display set up 
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
  mySerial.println("\n\n\n\n\n\n\n\n\rStart");
  #endif
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready");

  delay(5000);
  lightThreshold = analogRead(A0); // reading photoresistor's value in pin A0
}

// function used to identify a scanned fingerprint
// returns the fingerprint's ID or -1 if fingerprint is not registered or if an error occurs
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
  Serial.print(" identified with a confidence level of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}

// function used to register a new fingerprint in the scanner's memory
uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as ID #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Fingerprint correctly scanned");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Please scan the same finger once more");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Fingerprint correctly scanned");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for ID #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in this location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}

uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete from this location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}

void add_int(int var2)    // function to add an int to the buffer variable (payload that will be sent over Sigfox) - max 255
{
  byte* a2 = (byte*) &var2; // converting the int to bytes
  String str2;
  str2=String(a2[0], HEX);  // converting bytes to a string of bytes in HEX format
  // verifying that the string is complete (2 characters for one byte)
  if(str2.length()<2)
  {
    buffer+=0+str2;    // if not, we add a 0 to the string and add the result to the buffer
  }
  else
  {
    buffer+=str2;     // if yes, we simply add the string to the buffer
  }
}

void sendATCommandSigfox(String commandAT)
{
  //Serial.println(commandAT);
  digitalWrite(7, HIGH); // enable communication with Sigfox's module
  delay(1000);
  unsigned long x=0;
  #ifdef SigfoxDeb
  mySerial.print("\r\n\tSigfox-->");
  mySerial.println(commandAT); // printing AT command in virtual serial that communicates directly with Sigfox's module
  #endif
  while( Serial.available() > 0) Serial.read();
    x = 0;
  memset(SigfoxReply, '\0',sizeof(SigfoxReply)); // cleaning SigfoxReply var
  Serial.print(commandAT);
  Serial.print("\r\n");
  while(true) // reading the module's reply and storing it in SigfoxReply
  {
    if(Serial.available() != 0)
    {
      char c = Serial.read();
      delay(500);
      Serial.print(c);
      delay(500);   
      SigfoxReply[x] = c;
      x++;
      if (SigfoxReply[x-1]=='=') // when the reply is actually the downlink message it will start with "RX=[downlink payload]"
        // we just want to keep the [downlink payload part] so once it hits the "equal" we overwrite what has been written into
        // SigfoxReply up until then
      {
        x=0;
      }
      if (strstr(SigfoxReply, "ER") != NULL) // once the module's reply has been completely read, an ERROR message will start
        // to be read by our loop. So, when "ER" is added to SigfoxReply, it means we have completely read the module's reply
        // and we can break the loop
      {
        Serial.println(" ");
        #ifdef SigfoxDeb
        mySerial.print("Command OK\n\r");
        mySerial.println(SigfoxReply);
        #endif
        digitalWrite(7, LOW); // disable communication with Sigfox's module
        break;
      }
    }
  }
}

void send_buffer_get_name()
{
  int j;
  String name="";
  delay(1000);
  sendATCommandSigfox("AT$RC");
  delay(2000);
  //to request a downlink, we add ",1" to the buffer before sending it to Sigfox's module
  buffer += ",1\n";
  sendATCommandSigfox(buffer);
  delay(1000);
  Serial.println(SigfoxReply);
  for(j=0;j<=22;j+=2){ // data is transferred in HEX format over Sigfox, the reply is here translated into its ASCII format
    if (j==2){j=3;}
    if (j==5){j=6;}
    if (j==8){j=9;}
    if (j==11){j=12;}
    if (j==14){j=15;}
    if (j==17){j=18;}
    if (j==20){j=21;}
    char val = SigfoxReply[j] > 0x39 ? (SigfoxReply[j] - 'A') * 16 : (SigfoxReply[j] - '0') * 16;
    val += SigfoxReply[j+1] > 0x39 ? (SigfoxReply[j+1] - 'A') : (SigfoxReply[j+1] - '0');
    name += val;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello ");
  lcd.print(name);
}

// function that reads an input int different from 0
uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop(){
  lightValue = analogRead(A0); // reading the photoresistor's value on pin A0
  //Serial.println(lightValue);
  finger_id = -2;
  if (lightValue > lightThreshold) // if a finger is detected, the fingerprint scanner lights up
  {
    finger_id = getFingerprintIDez(); // fingerprint scanned
    delay(1000);
    buffer = "AT$SF="; // Creating the payload that will be sent through Sigfox's module
    add_int(finger_id); // fingerprint's ID is added to the payload
    if (finger_id != -1 && finger_id != -2 && finger_id != 1){ // if the fingerprint has been identified and is not the admin's
      add_int(0);
      add_int(0); // we add "0000" to the payload, so it is always the same size - simplifies decoding
      delay(1000);
      send_buffer_get_name(); // sending the payload over Sigfox and getting the name by downlink. Name shown on LCD display
      Serial.print("Welcome! Finger ID: ");Serial.println(finger_id);
      finger_id = -2;
    }
    else if (finger_id == 1){ // if the administrator's fingerprint has been scanned and identified
      int choice = -1;
      Serial.print("Welcome! Finger ID: ");Serial.println(finger_id);
      Serial.println("What do you wish to do? 1: Register a new fingerprint, 2: Erase an existing fingerprint");
      choice = readnumber();
      if (choice == 1){ // if the administrator chose to registera a new fingerprint
        add_int(choice); // adding the administrator's choice to the payload
        Serial.println("Ready to register a new fingerprint");
        Serial.println("Please input the ID # (from 1 to 127) that you would like to use for this fingerprint...");
        id = readnumber();
        if (id == 0) {// ID #0 not allowed, try again!
           return;
        }
        Serial.print("Registering ID #");
        Serial.println(id); // adding the new ID to the payload
        add_int(id);
        delay(500);
        getFingerprintEnroll(); // registering the new fingerprint
        delay(1000);
        sendATCommandSigfox(buffer); // sending the payload over Sigfox
      }
      else if (choice == 2){ // if the administrator chose to erase an existing fingerprint
        add_int(choice);
        Serial.println("Please input the existing ID # (from 1 to 127) that you wish to erase...");
        uint8_t id = readnumber();
        delay(1000);
        if (id == 0) {// ID #0 not allowed, try again!
           return;
        }
        Serial.print("Erasing ID #");
        Serial.println(id);
        add_int(id); // adding the erased ID to the payload
        delay(500);
        deleteFingerprint(id); // erasing the fingerprint's records from the sensor's memory
        delay(1000);
        sendATCommandSigfox(buffer); // sending the payload over Sigfox
      }
    }
  }
}
