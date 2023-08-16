
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

//Libreria para la lista
#include <ListLib.h>

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

long Counter = 0;

int flag1 = 0, flag2 = 0, timer = 200;

int Common = 0; //<Common=1; for Common anode> <Common=0; for Common cathode>
int On, Off, Off_C;
int DTime = 4; // Display timer

//Display Loop
unsigned long previousMillis = 0UL;
unsigned long interval = 3000UL;
unsigned int displayIndex = 0;
//Ya no es loop, por eso son estas
unsigned int previousReady = 0;


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
#define PURPLE  0x58EF
#define GREY    0x52AA


Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

#define BOXSIZE 40
#define PENRADIUS 3

//Variables
int oldcolor, currentcolor;
int currentOrder = 0;

//Variables para receptor serial
String inputString1;
String inputString2;
bool orderRecieved = false;
bool validEntry = false;
bool orderOne = true;

//Variedad de comida
String comida1 = "Hamburguesa";
String comida2 = "Torta";
String comida3 = "Pizza";
String comida4 = "Coca-Cola";
String orderItem1 = "";
String orderItem2 = "";

//Creacion de una estructura de datos (una clase)
//Para organizar las ordenes
/*Pero ni agarro xd
  class Order {
  public:
    String item1;
    String item2;
    int numOrder;

     void setItem1(String  item){
      item1 = item;
     }
     void getItem1(){
      return item1;
     }
     void setItem2(String  item){
      item2 = item;
     }
     void getItem2(){
      return item2;
     }
     void setNumOrder(String  item){
      item1 = item;
     }
     void getNumOrder(){
      return numOrder;
     }
  }
*/

//Variables de orden
//List<Order> ordenes; Prueba con una estructura de datos fallida
const int listCapacity = 20;
List<String> listOne(listCapacity);
List<String> listTwo(listCapacity);
List<int> ordenesListas(listCapacity);
String nullMsg[] = {"Orden lista", "O no hay"};

//Datos estaticos para probar
String comidaM1[] = {comida1, comida2};
String comidaM2[] = {comida1, comida3};
String comidaM3[] = {comida1, comida4};
String comidaM4[] = {comida3, comida2};
String comidaM5[] = {comida4, comida3};

  String comidaM6[] = {comida2, comida4};
  String comidaM7[] = {comida3, comida1};
  String comidaM8[] = {comida3, comida2};
  String comidaM9[] = {comida3, comida4};

//Comunicacion serial
char mystr[] = "HelloWorld";



void setup() {
  Serial.begin(9600);
  //Prueba serial
  Serial.write(mystr, 10);
  delay(1000);

  //Meter las ordenes estaticas de prueba
  /*prueba con estructura de datos fallida
    ordenes.Add({comida1, comida2, 0});
    ordenes.Add({comida1, comida3, 1});
    ordenes.Add({comida1, comida4, 2});
    ordenes.Add({comida3, comida4, 3});
    ordenes.Add({comida2, comida4, 4});
  */

  listOne.Add(comidaM1[0]);
  listTwo.Add(comidaM1[1]);

  listOne.Add(comidaM2[0]);
  listTwo.Add(comidaM2[1]);

  listOne.Add(comidaM3[0]);
  listTwo.Add(comidaM3[1]);

  listOne.Add(comidaM4[0]);
  listTwo.Add(comidaM4[1]);
  
  listOne.Add(comidaM5[0]);
  listTwo.Add(comidaM5[1]);

  listOne.Add(comidaM6[0]);
  listTwo.Add(comidaM6[1]);

  listOne.Add(comidaM7[0]);
  listTwo.Add(comidaM7[1]);

  listOne.Add(comidaM8[0]);
  listTwo.Add(comidaM8[1]);
  
  listOne.Add(comidaM9[0]);
  listTwo.Add(comidaM9[1]);
  
  Serial.print("TESTE");

  //Dar los dos primero valores a ser imprimidos en las ordenes
  orderItem1 = listOne[currentOrder];
  orderItem2 = listTwo[currentOrder];

  //Meterlos a las variables

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
  //Inicialización del LCD
  tft.reset();

  uint16_t identifier = tft.readID();

  //buscador de driver para el lcd
  //Comentar los serial print para el producto final para evitar interferencias.
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else {
    //Mensaje si no se encuentra algun driver
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    //Como no se encontró el driver, se cerrará el setup y la ejecución del programa se finalizará.
    return;
  }

  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(BLUE);
  tft.setTextSize(3);
  tft.print("Cargando...");

  currentcolor = RED;

  pinMode(40, OUTPUT);

  //Inilización de los displays de 7 segmentos
  // Indicate that system is ready
  for (int i = 9; i >= 0; i--) {
    showNumber(i);
    digitalWrite(c1Pin, Common);
    digitalWrite(c2Pin, Common);
    digitalWrite(c3Pin, Common);
    digitalWrite(c4Pin, Common);
    delay(500);
  }

  //Definición de la presión máxima y minima del touchscreen
  //En otras palabras, que tan debil o fuerte debe ser el pulso para
  //ser aceptado como una entrada.
#define MINPRESSURE 10
#define MAXPRESSURE 1000

  //Listo para usar el LCD
  tft.fillScreen(WHITE);
  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);
  tft.print("Toque este texto en la pantalla para poder iniciar");
}



void loop() {
/*
  if(ordenesListas.IsEmpty() ){
    Counter = 0;
  }else{
    Counter = ordenesListas[0] + 1;
  }
*/

  //Ordenes del jefe, esta ya no se usa
  //loopOrdenesListas();
  displayOrdenesListas();


  if(orderRecieved) {
    //Cuando la orden ya fue capturada por el Serial.Event()
    listOne.Add(inputString1);
    listTwo.Add(inputString2);
    inputString1 = "";
    inputString2 = "";
    orderRecieved = false;
    
  }

  //Lógica de los display de 7 segmentos.
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
    //Se refresca el display con el texto
    //text();


    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = map(p.y, TS_MINY, TS_MAXY, tft.height(), 0);

    Serial.print("("); Serial.print(p.x);
    Serial.print(", "); Serial.print(p.y);
    Serial.println(")");

    if (p.x < 80) {
      //Si le tocas dentro de la pantalla creo?
      //Creo que es para delimitar el rango en x?
      //Y tambien si la lista de ordenes no esta vacia
      oldcolor = currentcolor;

      if (p.y < 80) {
        //Si está en el rango del rectangulo inferior izquierdo (ANTERIOR)
        if (currentOrder > 0) {
          //Si la orden no es igual a 0
          currentOrder--;
          if (ordenesListas.Contains(currentOrder)) {
            //Cuando se da para atras pero esa orden ya está lista
            nullMsgSet();
          } else {
            orderItem1 = listOne[currentOrder];
            orderItem2 = listTwo[currentOrder];
          }
          Serial.print("Anterior");
        }
      } else if (p.y < 170 && currentOrder < listOne.Count() ) {
        //Si está en el rango del rectangulo inferior izquierdo (SIGUIENTE)
        //Y tambien si está dentro de los elementos que tiene la lista
        currentOrder++;
        if (ordenesListas.Contains(currentOrder)) {
          //Cuando se da adelante pero esa orden ya está lista
          nullMsgSet();
        } else {
          orderItem1 = listOne[currentOrder];
          orderItem2 = listTwo[currentOrder];
        }
        Serial.print("Siguiente");
      } else if (p.y < 240) {
        if (!ordenesListas.Contains(currentOrder)) {
          ordenesListas.Add(currentOrder);
          nullMsgSet();
        }
        Serial.print("Listo");
      }

    }
    text();
  }
}

//Poner texto nullo a la pantalla
//Osea, que no hay orden o está lista
void nullMsgSet() {
  orderItem1 = nullMsg[0];
  orderItem2 = nullMsg[1];
}



//Establecer el texto del LCD, junto
//con la orden en si
int text() {
  tft.fillScreen(WHITE);

  //Poner el cursor en el centro
  tft.setCursor(0, 0);
  tft.setTextColor(BLACK);
  tft.setTextSize(3);

  // Fondo para la línea "Orden numero: "
  tft.fillRect(0, 0, tft.width(), 25, PURPLE);
  tft.setTextColor(WHITE);
  tft.print("Orden numero: ");
  //INcrementarle en uno para el usuario tonto
  tft.println(currentOrder + 1);

  //Escribir la orden
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.println(orderItem1);
  tft.println(orderItem2);
  tft.println();

  // Dibujar los rectangulos en la parte inferior
  int rectHeight = tft.height() / 4;
  //Rectangulo izquierdo
  tft.fillRect(0, tft.height() - rectHeight, tft.width() / 3, rectHeight, GREY);
  //Rectangulo del centro
  tft.fillRect(tft.width() / 3 + 1, tft.height() - rectHeight, tft.width() / 3, rectHeight, BLACK);
  //Rectangulo derecho
  tft.fillRect(tft.width() / 3 * 2 + 2, tft.height() - rectHeight, tft.width() / 3, rectHeight, PURPLE);

  // Texto "<--" en la mitad izquierda (rojo)
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.setCursor(10, tft.height() - rectHeight + 10);
  tft.println("<--");

  // Texto "-->" en la mitad derecha negro)
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.setCursor(tft.width() / 3 + 10, tft.height() - rectHeight + 10);
  tft.println("-->");

  // Texto "Listo" (azul)
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(tft.width() / 3 * 2 + 15, tft.height() - rectHeight + 10);
  tft.println("Listo");

  //delay(1000);
}

//Serial receptor
void serialEvent() {
  Serial.print("serialEvent");
  
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    Serial.print(inChar);
    Serial.println("OrderOne: ");
    Serial.print(orderOne);
    if(inChar == '<'){
      //Verificar la entrada del texto para recibir
      validEntry = true;
      //Establece para escribir la primera parte de la orden
      orderOne = true;
      //Limpia los input strings
      inputString1 = "";
      inputString2 = "";
    }else if(inChar == '-' && validEntry){
      //Separador entre las dos ordenes
      orderOne = false;
      //delay(10);
    }else if(inChar == '>'  && validEntry){
      //Final de la orden completa, ya sea una o dos
      validEntry = false;
      orderOne = true;
      orderRecieved = true;
    }else if(validEntry) {
      //Entrada correcta del texto
      if(orderOne){
        //Entrada del texto de la primera orden
        inputString1 += inChar;
      }else{
        //Entrada del texto de la segunda orden
        inputString2 += inChar;
      }
    }
  }
  
}

//POs ya no se usa esta
void loopOrdenesListas(){
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > interval && !ordenesListas.IsEmpty()){
    /* The Arduino executes this code once every interval set in ms
    *  (interval = 1000 (ms) = 1 second). RN: 3000ms
    */
    Serial.print("Dispindex: ");
    Serial.println(displayIndex);
    Counter = ordenesListas[displayIndex] + 1;
    Serial.print("Displayn: ");
    Serial.println(Counter);
    if(displayIndex >= ordenesListas.Count() - 1){
      displayIndex = 0;
    }else if (displayIndex < ordenesListas.Count() - 1) {
      displayIndex++;
    }
    
    // Don't forget to update the previousMillis value
    previousMillis = currentMillis;
  }
}

//Esta de arriba que no se usa se cambiará por esta de abajo
void displayOrdenesListas(){
  if(!ordenesListas.IsEmpty()){
    Counter = ordenesListas[ordenesListas.Count()-1] + 1;
  }
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
