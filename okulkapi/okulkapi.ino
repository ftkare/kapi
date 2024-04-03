#include <Keypad.h>
// en son test edilen Keypad.h surumu: 3.1

// onemli notlar:
//  * degisken isimleri ingilizce olursa daha iyi olur
//  * uzun isimler kullanmamaya dikkat edin ki koda bir sey
//    ekleyecegimiz zaman dikkatimizi degiskenleri dogru yazmaya
//    degil sistemi dogru yapmaya verebilelim.
//  * bu notlari silmeyin hatta gerekirse ek duzenlemeler yapin ki
//    gelecekte bu koda bakan rastgele bir pfalli ne yapmasi gerektigini
//    anlayabilsin.
//  * kodda halihazirda kullanilan yazim stilini takip edin lutfen,
//    begenmeseniz bile.
//  * lutfen chatgpt kullanirsaniz edindiginiz kodun
//    nasil calistigini anlamadan direkt kopyala yapistir yapmayin,
//    sonra cok sacma hatalara yol acabiliyor.
//  * yazdiginiz koda yorum satiri eklemeyi unutmayin ki kutuphaneyi/dili
//    bilmeyen birisi bile anlayip hatalari kontrol edebilsin
//
// -- cinar

// dokunma sensoru kullanilabilir mi
#define ALLOW_TOUCH false
// keypad sifresi uzunlugu (null byte, dogrulama tusu vb. haric)
#define PASSLEN 11

const int RELAY_PIN = 2; // kilit
const int TOUCH_PIN = 3; // touchpad

// keypad duzeni
const byte ROWS = 4; 
const byte COLS = 4; 
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {13, 12, 11, 10}; 
byte colPins[COLS] = {9, 8, 7, 6}; 
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

// sifre uzunlugu
const char* password = "01234567890"; // lutfen PASSLEN karaktere sahip olsun
                                      // ve sadece 0123456789 karakterlerinden olussun

char enteredPassword[PASSLEN+2];  // ne olur ne olmaz diye +2
int passInd = 0;
bool doorOpen = false;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);
  // enteredPassword sifirlanmali, ne olur ne olmaz
  memset(enteredPassword, 0, sizeof(enteredPassword));
}

void loop() {
  checkKeypad();
  checkTouchSensor();
}

void checkTouchSensor() {
  if (digitalRead(TOUCH_PIN) == HIGH && ALLOW_TOUCH) {
    Serial.println("TouchSensor'e Dokunuldu");
    doorOpen = !doorOpen;
    digitalWrite(RELAY_PIN, doorOpen ? HIGH : LOW);
    delay(500);
  }
}

void passIndCtrl() {
  if (passInd >= PASSLEN)
  {
    // eger sifreye fazla karakter girildiyse her seyi temizle
    memset(enteredPassword, 0, sizeof(enteredPassword));
    passInd = 0;
  }
  if (passInd < 0)
  {
    // bir sekilde 0'in altina dusmus (muhtemelen ust uste B'ye basildi), sifirlayalim
    memset(enteredPassword, 0, sizeof(enteredPassword));
    passInd = 0;
  }
}

void checkKeypad() {
  char value = customKeypad.getKey();
  if (value) {
    if (value == '*') {
      return; // bir sey eklenebilir
    } else if (value == '#') {
      return; // bir sey eklenebilir
    } else if (value == 'A') {
      return; // bir sey eklenebilir
    } else if (value == 'D') {
      enteredPassword[passInd] = '\0';
      if (strcmp(enteredPassword, password) == 0) {
        doorOpen = !doorOpen;
        digitalWrite(RELAY_PIN, doorOpen ? HIGH : LOW);
        Serial.println("Giris yapildi!");
      } else {
        // sifre hatali
        Serial.println("Giris reddedildi!");
      }
      // buffer'i ve indisi sifirla
      memset(enteredPassword, 0, sizeof(enteredPassword));
      passInd = 0;
    } else if (value == 'B') {
      // girilen son karakteri sil
      passInd--;
      passIndCtrl();
      enteredPassword[passInd] = 0;
      Serial.print("Entered: ");
      Serial.println(enteredPassword);
    } else if (value == 'C') {
      // girilen her seyi temizle
      memset(enteredPassword, 0, sizeof(enteredPassword));
      passInd = 0;
      Serial.print("Entered: ");
      Serial.println(enteredPassword);
    } else {
      // 0123456789 girildi
      passIndCtrl();
      enteredPassword[passInd] = value;
      passInd++;
      Serial.print("Entered: ");
      Serial.println(enteredPassword);
    }
  }
}
