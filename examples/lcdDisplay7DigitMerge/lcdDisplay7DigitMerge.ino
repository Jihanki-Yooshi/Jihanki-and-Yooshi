/*
Esta es la prueba con el dispolay de 7 segmentos, el lcd y
cosas de la comunicación serial.
La ultima prueba de este sketch funciona el display
de 7 segmentos y el touchscreen.
Unico "problema" que se ve es que el display se refresca
cuando el touch se refresca.
*/

/*Librerias viejas
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
*/
//Librerias viejas
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif

#define aPin 9   //Antes 9        
#define bPin 13  //             _____
#define cPin 5   //            |  A  |
#define dPin 3   //          F |_____| B
#define ePin 2   //            |  G  |
#define fPin 10  //          E |_____| C
#define gPin 6   //Antes 9      D     O dot
//decimal 4
#define c1Pin 8   // Common pin for digit 1
#define c2Pin 11  // Common pin for digit 2
#define c3Pin 12  // Common pin for digit 3
#define c4Pin 7   // Common pin for digit 4

#define bt_up     48
#define bt_down   46
#define bt_reset  44

long Counter = 1;

int flag1 = 0, flag2 = 0, timer = 200;

int Common = 0; //<Common=1; for Common anode> <Common=0; for Common cathode>
int On, Off, Off_C;
int DTime = 4; // Display timer


//LCD:
#define YP A9  // must be an analog pin, use "An" notation!
#define XM A10  // must be an analog pin, use "An" notation!
#define YM 23   // can be a digital pin
#define XP 22   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

#define LCD_CS A9
#define LCD_CD A10
#define LCD_WR A11
#define LCD_RD A12
// optional
#define LCD_RESET A8

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3

//Variables
int oldcolor, currentcolor;
int i = 0;

String comida1 = "Hamburguesa";
String comida2 = "Hot-Dog";
String comida3 = "Pizza";
String comida4 = "Sushi";
String comidaM1 = comida1;
String comidaM2 = comida2;

//Comunicacion serial
char mystr[] = "Hello";

void setup() {
  Serial.begin(9600);
  //Prueba serial
  Serial.write(mystr,5);
  delay(1000);
  
  
  //Displa
  pinMode(bt_up,    INPUT_PULLUP);
  pinMode(bt_down,  INPUT_PULLUP);
  pinMode(bt_reset, INPUT_PULLUP);

  pinMode(aPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  pinMode(cPin, OUTPUT);
  pinMode(dPin, OUTPUT);
  pinMode(ePin, OUTPUT);
  pinMode(fPin, OUTPUT);
  pinMode(gPin, OUTPUT);

  pinMode(c1Pin, OUTPUT);
  pinMode(c2Pin, OUTPUT);
  pinMode(c3Pin, OUTPUT);
  pinMode(c4Pin, OUTPUT);

  if (Common == 1) {
    On = 0, Off = 1, Off_C = 0; //for Common anode
  }
  else {
    On = 1, Off = 0, Off_C = 1; //for Common cathode
  }

  //LCD
  tft.reset();
  
  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(WHITE);

  currentcolor = RED;
 
  pinMode(40, OUTPUT);

  // Indicate that system is ready
  for (int i = 9; i >= 0; i--) {
    showNumber(i);
    digitalWrite(c1Pin, Common);
    digitalWrite(c2Pin, Common);
    digitalWrite(c3Pin, Common);
    digitalWrite(c4Pin, Common);
    delay(1000);
  }

  //Algo mas del lcd
  #define MINPRESSURE 10
  #define MAXPRESSURE 1000
  
}

void loop() {
  if (digitalRead (bt_up) == 0) {
    if (flag1 == 0) {
      flag1 = 1;
      Counter = Counter + 1;
      if (Counter > 9999) {
        Counter = 0;
      }
    }
  } else {
    flag1 = 0;
  }

  if (digitalRead (bt_down) == 0) {
    if (flag2 == 0) {
      flag2 = 1;
      Counter = Counter - 1;
      if (Counter < 0) {
        Counter = 9999;
      }
    }
  } else {
    flag2 = 0;
  }

  if (digitalRead (bt_reset) == 0) {
    if (timer < 200) {
      timer = timer + 1;
    }
    if (timer == 200) {
      Counter = 0;
    }
  } else {
    timer = 0;
  }


  showNumber((Counter / 1000) % 10);
  //Serial.print((Counter / 1000) % 10);
  digitalWrite(c1Pin, Common);
  delay(DTime);
  digitalWrite(c1Pin, Off_C);

  showNumber((Counter / 100) % 10);
  //Serial.print((Counter / 100) % 10);
  digitalWrite(c2Pin, Common);
  delay(DTime);
  digitalWrite(c2Pin, Off_C);

  showNumber((Counter / 10) % 10);
  //Serial.print((Counter / 10) % 10);
  digitalWrite(c3Pin, Common);
  delay(DTime);
  digitalWrite(c3Pin, Off_C);

  showNumber(Counter % 10);
  //Serial.print(Counter % 10);
  digitalWrite(c4Pin, Common);
  delay(DTime);
  digitalWrite(c4Pin, Off_C);

  //LCD
  digitalWrite(40, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(40, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    text();


    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);
    
    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");
    
    if (p.x < 80) {
       oldcolor = currentcolor;

       if (p.y < 80) { 
         comidaM1 = comida1;
         comidaM2 = comida2;
         Serial.print("Anterior");
       } else if (p.y < 170) {
         i++;
         comidaM1 = comida3;
         comidaM2 = comida4;
         Serial.print("Siguiente");
       } else if (p.y < 240) {
         Serial.print("Listo");
       }

    }
    
  }
}

//LCd
int text() {
  tft.fillScreen(WHITE);

  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);

  // Fondo azul para la línea "Orden numero: "
  tft.fillRect(0, 0, tft.width(), 25, BLUE);
  tft.setTextColor(WHITE);
  tft.print("Orden numero: ");
  tft.println(i);

  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println(comidaM1);
  tft.println(comidaM2);
  tft.println();

  // Dibujar el rectángulo morado en la parte inferior
  int rectHeight = tft.height() / 4;
  tft.fillRect(0, tft.height() - rectHeight, tft.width() / 3, rectHeight, RED);
  tft.fillRect(tft.width() / 3 + 1, tft.height() - rectHeight, tft.width() / 3, rectHeight, BLACK);
  tft.fillRect(tft.width() / 3 * 2 + 2, tft.height() - rectHeight, tft.width() / 3, rectHeight, BLUE);

  // Texto "Eliminar" en la mitad izquierda (rojo)
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(10, tft.height() - rectHeight + 10);
  tft.println("<--");

  // Texto "Siguiente" en la mitad derecha (verde)
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(tft.width() / 3 + 10, tft.height() - rectHeight + 10);
  tft.println("-->");

  // Texto "Listo" (azul)
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(tft.width() / 3 * 2 + 15, tft.height() - rectHeight + 10);
  tft.println("Listo");

  //delay(1000);
}

//7Display
void showNumber(int x) {

  switch (x) {
    case 1: one();   break;
    case 2: two();   break;
    case 3: three(); break;
    case 4: four();  break;
    case 5: five();  break;
    case 6: six();   break;
    case 7: seven(); break;
    case 8: eight(); break;
    case 9: nine();  break;
    default: zero(); break;
  }
}

void one() {
  digitalWrite( aPin, Off); //
  digitalWrite( bPin, On);  //      |
  digitalWrite( cPin, On);  //      |
  digitalWrite( dPin, Off); //      |
  digitalWrite( ePin, Off); //      |
  digitalWrite( fPin, Off);
  digitalWrite( gPin, Off);
}

void two() {
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  //      |
  digitalWrite( cPin, Off); //  ____|
  digitalWrite( dPin, On);  // |
  digitalWrite( ePin, On);  // |____
  digitalWrite( fPin, Off);
  digitalWrite( gPin, On);
}

void three() {
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  //      |
  digitalWrite( cPin, On);  //  ____|
  digitalWrite( dPin, On);  //      |
  digitalWrite( ePin, Off); //  ____|
  digitalWrite( fPin, Off);
  digitalWrite( gPin, On);
}

void four() {
  digitalWrite( aPin, Off); //
  digitalWrite( bPin, On);  // |    |
  digitalWrite( cPin, On);  // |____|
  digitalWrite( dPin, Off); //      |
  digitalWrite( ePin, Off); //      |
  digitalWrite( fPin, On);
  digitalWrite( gPin, On);
}

void five() {
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, Off); // |
  digitalWrite( cPin, On);  // |____
  digitalWrite( dPin, On);  //      |
  digitalWrite( ePin, Off); //  ____|
  digitalWrite( fPin, On);
  digitalWrite( gPin, On);
}

void six() {
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, Off); // |
  digitalWrite( cPin, On);  // |____
  digitalWrite( dPin, On);  // |    |
  digitalWrite( ePin, On);  // |____|
  digitalWrite( fPin, On);
  digitalWrite( gPin, On);
}

void seven() {
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  //      |
  digitalWrite( cPin, On);  //      |
  digitalWrite( dPin, Off); //      |
  digitalWrite( ePin, Off); //      |
  digitalWrite( fPin, Off);
  digitalWrite( gPin, Off);
}

void eight() {
  digitalWrite( aPin, On); //  ____
  digitalWrite( bPin, On); // |    |
  digitalWrite( cPin, On); // |____|
  digitalWrite( dPin, On); // |    |
  digitalWrite( ePin, On); // |____|
  digitalWrite( fPin, On);
  digitalWrite( gPin, On);
}

void nine() {
  digitalWrite( aPin, On);  //  ____
  digitalWrite( bPin, On);  // |    |
  digitalWrite( cPin, On);  // |____|
  digitalWrite( dPin, On);  //      |
  digitalWrite( ePin, Off); //  ____|
  digitalWrite( fPin, On);
  digitalWrite( gPin, On);
}

void zero() {
  digitalWrite( aPin, On); //  ____
  digitalWrite( bPin, On); // |    |
  digitalWrite( cPin, On); // |    |
  digitalWrite( dPin, On); // |    |
  digitalWrite( ePin, On); // |____|
  digitalWrite( fPin, On);
  digitalWrite( gPin, Off);
}
