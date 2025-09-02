
#include "CytronMotorDriver.h" /// bilbioteka do obsługi sterownika silników Cytron
#include <LiquidCrystal.h> /// biblioteka od wyswietlacza

int odczytanaWartoscADC = 0;
int cyfryADC = 0;
/// string odebraneDane = "";
float okresInt = 250; /// okres jako int w ms
int wypelnienieProcentowe = 0;
float wypelnienie = 0.0;
#define motor1A 10 // 
#define motor1B 11 //
#define przycisk1 13 // M1A
#define przycisk2 12/// M1B
#define krancowka 17 // inaczej A3 na płytce
///#define potencjometr A0
#define enkoderA 2 /// enkoder sygnał A
#define enkoderB 3  /// sygnał B

/// impulsy
volatile long pozycja = 0; /// zmienna volatile - może się zmieniać podczas wyk. programu
int cyfryPozycja = 0;
///////// Silniczek
CytronMD motor2(PWM_PWM, motor1A, motor1B); // PWM 1A = Pin 10, PWM 1B = Pin 11.

///////// Wyświetlacz LCD
LiquidCrystal lcd(8,9,4,5,6,7); //// wyświetlacz 2 RS, 3 Enable, 4 do 7 - D4-D7 (dane)


//// REGULATOR P
float Kp = 5.0;
volatile int uSter = 0;
float uSterFLT = 0.0;
int uSterMax = 0;
int uSterMin = 22; 
volatile long eUchyb = 0;
int pozZadana = 1500;


void setup() {
  //Serial.begin(9600); /// rozpoczęcie komunikacji UART z komputerem
  //// Wyświetlacz - ustawienia wstępne
  lcd.begin(16,2); /// typ wyświetlacza - 16 znaków, 2 linijki
  lcd.setCursor(0,0);
  lcd.print("0000"); /// wartosc z przetwornika A/C
  lcd.setCursor(6,0);
  lcd.print("sterMAX");
  lcd.setCursor(0,1);
  lcd.print("0000"); /// pozycja
  lcd.setCursor(12,1);
  lcd.print("poz");
  pinMode(przycisk1,INPUT_PULLUP); /// PRZYCISKI i krancówka jako input
  pinMode(przycisk2,INPUT_PULLUP);/// z rezystorem podciągającym
  pinMode(krancowka, INPUT_PULLUP);
  pinMode(enkoderA, INPUT);
  pinMode(enkoderB, INPUT);

  ////// przerwanie
attachInterrupt(0,przerwanie,CHANGE);
/* można to zrobić tak:
attachInterrupt(digitalPinToInterrupt(pin), ISR, mode); (recommended) /// tutaj po porstu nr pinu
attachInterrupt(interrupt, ISR, mode); (not recommended)
interript to będzie nr. przerwania: 0 lub 1  - odpowiada to pinowi 2 i 3
*/

}
/// mamy do dyspozycji przetwornik 10 bitowy, tj. od 0 do 1023 - takie
/// wartosci moze przyjmować - wiec musze zrobic skalowanie do 0.0-5.OV
void loop() {
odczytanaWartoscADC = analogRead(A0); /// odczytaj wartosc 0 - 1023 z przetwornika w formie inta
wypelnienie = ((float(odczytanaWartoscADC))/1023.0) * okresInt; // zwraca nam wartosc od 0.0 do 1.0 (* okresInt)
wypelnienieProcentowe = int(wypelnienie); // zamiana na int 0 - okres Int
//Serial.println(String("Wypelnienie % : ")+ wypelnienieProcentowe);

//////////////////////////////// poprawne wyświetlanie wartości z ADC
odczytanaWartoscADC = wypelnienieProcentowe;
cyfryADC =String(odczytanaWartoscADC).length(); // dlugosc (ilosc cyfr) akt. wartosci ADC
lcd.setCursor(0,0);
switch (cyfryADC)
{
  case 1: /// liczba 1 cyfrowa
  lcd.print(String("000")+odczytanaWartoscADC); /// "doklejanie" zer tam gdzie trzeba
  break;
  case 2: /// liczba 2 cyfrowa itd.
  lcd.print(String("00")+odczytanaWartoscADC);
  break;
  case 3:
  lcd.print(String("0")+odczytanaWartoscADC);
  break;
  case 4: /// 4 cyfrowa
  lcd.print(odczytanaWartoscADC); /// tutaj wartosc 4 cyfrowa wypelnia wszystkie pola
  break;                          //// zera są tutaj niepotrzebne
  default:
  lcd.print("0000");
  break;
}


/// Regulator P//////////////////////////////////////////////////////////////////////////
uSterMax = wypelnienieProcentowe;
//
if ((digitalRead(przycisk1)==0) && (digitalRead(przycisk2)==0)) /// dojazd do pozycji wyzwalany dy są 2 przyciski wcisniete
{
  eUchyb = pozZadana - pozycja; /// UCHYB Regulacji -  "e"
uSter = int((double(eUchyb)/5000.0) * Kp*uSterMax); // uchyb razy wzmocnienie Kp regulatora
//e                                                                   ///spoko zakres uchybu, kiedy moze sie zmieniac wartosc to od 1 do 1000 - powyzej juz lepiej zrobic
//d                                                                      // ograniczenie
//
//////// ograniczenia sygnału sterującego
if ((uSter>=uSterMax) || (uSter<=-uSterMax)) /// gdy jest osiągana górna granica MAX, to nie pozwoli na przekroczenie
{
  if (uSter >=0)
  {  uSter = uSterMax;
  }
  else
  {
    uSter = -uSterMax;
  }
}

if ((uSter >= uSterMin) || (uSter<=-uSterMin))  /// Sprawdzenie czy sygnał jest powyżej minimalnego
{
   motor2.setSpeed(uSter);
}
else      /// gdy jest osiągana dolna granica u MIN
{
  if (uSter >=0)
  { 
     uSter = uSterMin;
  }
  else
  {
    uSter = -uSterMin;
  }
}

//if ((pozycja <= pozZadana) && (pozycja>=pozZadana))
  if ((pozycja == pozZadana))
{
  motor2.setSpeed(0);
}

}
/////////////////////// POZYCJA /////////////// wyświetlanie
  lcd.setCursor(1,1);
cyfryPozycja = String(pozycja).length();
switch (cyfryPozycja)
{
  case 1: /// liczba 1 cyfrowa
  lcd.print(String("00000")+abs(pozycja)); /// "doklejanie" zer tam gdzie trzeba
  break;
  case 2: /// liczba 2 cyfrowa itd.
  lcd.print(String("0000")+abs(pozycja));
  break;
  case 3:
  lcd.print(String("000")+abs(pozycja));
  break;
  case 4: /// 4 cyfrowa
  lcd.print(String("00")+abs(pozycja));/// tutaj wartosc 4 cyfrowa wypelnia wszystkie pola
  break;                             //// zera są tutaj niepotrzebne
  case 5:
  lcd.print(String("0")+abs(pozycja));
  break;
  default:
  lcd.print("00xx0");
  lcd.print(abs(pozycja));
  break;
}

if (pozycja >= 0)
  {
    lcd.setCursor(0,1);
    lcd.print(" ");
  }
  else
  {
    lcd.setCursor(0,1);
    lcd.print("-");
  }



/*
//////////////// Ruch przyciskami
if ((digitalRead(przycisk1)==0) &&(digitalRead(przycisk2) == 1) ) /// jedź w prawo - pozycja powinna się zwiększać
{
  motor2.setSpeed(wypelnienieProcentowe);  
}
else if ((digitalRead(przycisk2) == 0) && (digitalRead(przycisk1) == 1))
{
  motor2.setSpeed(-wypelnienieProcentowe); /// ruch w przeciwna strone - w lewo
}
else if ((digitalRead(przycisk1)==1) &&(digitalRead(przycisk2) == 1))
{
  motor2.setSpeed(0);
}
if (digitalRead(krancowka)==0)/// bazowanie za pomocą krańcówki
{
  motor2.setSpeed(0);
  delay(50);
  pozycja = 0; /// ustawienie pozycji na "zero"
}

/*


if ((digitalRead(przycisk1)==0) && (digitalRead(przycisk2)==0))
{
  if (pozycja < 2000)
  {
    motor2.setSpeed(wypelnienieProcentowe);
  }
  else if (pozycja > 2050)
  {
    motor2.setSpeed(-wypelnienieProcentowe);
  }
  else
  {
    motor2.setSpeed(0);
  }
}
*/


if (digitalRead(krancowka)==0)/// bazowanie za pomocą krańcówki
{
  motor2.setSpeed(0);
  delay(500);
  pozycja = 0; /// ustawienie pozycji na "zero"
}

}
//// procedura przerwania ISR (interrupt service routine), która jest wywołaywana
void przerwanie()
{
  if (digitalRead(enkoderA)==digitalRead(enkoderB))
  {
    pozycja = pozycja - 1;
  }
 // else if ((digitalRead(enkoderA)!=digitalRead(enkoderB)
  else
  {
    pozycja = pozycja + 1;
  }
}



