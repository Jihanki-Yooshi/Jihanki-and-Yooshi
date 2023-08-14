/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read new NUID from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ListLib.h>

#define SS_PIN 53
#define RST_PIN 5
 
String ID = "";
String ID2 = "";
List<String> ID_R;
String ORDER[2] = {"",""};
int cost = 0;

// Pines envio de orden
int pinBTN_Add_P1 = 22;
int pinBTN_Add_P2 = 24;
int pinBTN_Add_P3 = 26;
int pinBTN_Send = 28;

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

LiquidCrystal_I2C lcd(0x27,16,2);

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];

void setup() { 

  // Declarar pinMode's
  pinMode(pinBTN_Add_P1,INPUT);
  pinMode(pinBTN_Add_P2,INPUT);
  pinMode(pinBTN_Add_P3,INPUT);
  pinMode(pinBTN_Send,INPUT);

  // Pantalla LCD
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(3,0);
  //lcd.rightToLeft();
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
   lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
   lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");

  // RFID
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    //key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  //printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
  Serial.println(F("====Listo Para Trabajar===="));
}
 
void loop() {

  int estBtnAddP1 = digitalRead(pinBTN_Add_P1);
  int estBtnAddP2 = digitalRead(pinBTN_Add_P2);
  int estBtnAddP3 = digitalRead(pinBTN_Add_P3);
  int estBtnSend = digitalRead(pinBTN_Send);

  if(estBtnSend == HIGH && ORDER[0] != ""){
    Serial.print("<" + ORDER[0] + "-" + ORDER[1] + ">");
    ORDER[0] = "";
    ORDER[1] = "";
    cost = 0;
    delay(300);
  } else if(estBtnSend == HIGH && ORDER[0] == ""){
    lcd.clear();
    lcd.print("Sin orden");
    delay(2000);
  }

  if(estBtnAddP1 == HIGH){
    cost = 0;
    Serial.println("1");
    ORDER[0] = "Sandwich";
    ORDER[1] = "Coca";
    cost+=50;
    delay(300);
  }

  if(estBtnAddP2 == HIGH){
    cost = 0;
    Serial.println("2");
    ORDER[0] = "Torta";
    ORDER[1] = "Agua";
    cost+=70;
    delay(300);
  }

  if(estBtnAddP3 == HIGH){
    cost = 0;
    Serial.println("3");
    ORDER[0] = "Papas";
    ORDER[1] = "Coca";
    cost+=40;
    delay(300);
  }

  if(ORDER[0] != ""){
    for(int i = 0; i < 2; i++){
      lcd.clear();
      lcd.print(ORDER[i]);
      lcd.setCursor(0,1);
      lcd.print(cost);
      delay(1000);
    }
  } else {
    lcd.clear();
    lcd.print("Nada");
    lcd.setCursor(0,1);
    lcd.print(0);
  }

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("A new card has been detected."));

    // Se vacian los ID
    ID = "";
    ID2 = "";

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    lcd.clear();
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    lcd.setCursor(0, 0);
    lcd.print(ID);

    int i = 0;

    // Registrar tarjeta si no se encuentra ya registrada
    for( i = 0; i < ID_R.Count(); i ++){
      if(ID_R[i] == ID){
        break;
      }
    }

    if(i == ID_R.Count()){
      ID_R.Add(ID);
    }

    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    lcd.setCursor(0, 1);
    lcd.print(ID2);
    Serial.println();

    Serial.print(ID_R[0]);

    sendOrder();
  }
  else Serial.println(F("Card read previously."));

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
    ID.concat(String(buffer[i] < 0x10 ? " 0" : " "));
    ID.concat(String(buffer[i], HEX));
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
    ID2.concat(String(buffer[i] < 0x10 ? " 0" : " "));
    ID2.concat(String(buffer[i], DEC));
  }
}

void sendOrder(){
  if(ORDER[0] != ""){
    Serial.print("<" + ORDER[0] + "-" + ORDER[1] + ">");
    ORDER[0] = "";
    ORDER[1] = "";
    cost = 0;
    delay(300);
  } else if(ORDER[0] == ""){
    lcd.clear();
    lcd.print("Sin orden");
    delay(2000);
  }
}

/*bool orderSizeVal(){
  if(ORDER.Count() < 2){
    return true;
  } else {
    return false;
  }
}

void showOrder(){
  for(int i = 0; i < 2; i++){
    lcd.clear();
    lcd.print(ORDER[i]);
  }
}*/
