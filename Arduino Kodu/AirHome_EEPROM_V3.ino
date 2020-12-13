/*
  MERAKLI MÜHENDİS YOUTUBE KANALI
  
  Youtube Kanalı için - https://bit.ly/2AEP5bf
  Facebook Sayfası için - https://bit.ly/3qVqAMC
  Discord Kanalımıza GEL!  - https://bit.ly/347FlSp
  
  Tüm sorularınızı discord kanalımızdan sorabilirsiniz. Orası teknik servis gibi :)
*/


#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

#define BLYNK_PRINT Serial

ESP8266WebServer server(80);
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// char auth[33], ssid[33], pass[33];
char authE[33], ssidE[33], passE[33];

char auth[33] = "Blynk_Auth_Token";
char ssid[33] = "WiFi_Ismi";
char pass[33] = "WiFi_Sifresi";

// Mod 2 için ayarlamalar - AP Modu
const char *ssidAP = "AirHome";
const char *passAP = "0123456789";


// Mod Ayarlama
// mod == 1 - normal mod
// mod == 2 - programlama modu
int mod = 1;


WidgetLED led1(V0);

bool pinChanged = false;
volatile int  pinValue   = 0;
const int buton = 12;
const int role = 13;
int deger;
int sicaklik = 0;

#define ONE_WIRE_BUS 14
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);


void checkPin()
{
  // Invert state, since button is "Active LOW"
  // pinValue = !digitalRead(buton);
  pinValue = !pinValue;
  Serial.println("Buton Degisti! ");

  // Mark pin value changed
  pinChanged = true;
  delayMicroseconds(500000);
}


BLYNK_CONNECTED() {
  Blynk.syncVirtual(V1);

  //Blynk.virtualWrite(V0, deger);
  //Serial.println(deger);
}


// Telefon üzerinden butona basıldığında çalışacak bölüm
BLYNK_WRITE(V1) {
  Serial.println(param.asInt());

  if (param.asInt())
    checkPin();

}


// Sıcaklık Okuma
BLYNK_READ(V2)
{
  DS18B20.requestTemperatures();
  Serial.println(DS18B20.getTempCByIndex(0));
  sicaklik = DS18B20.getTempCByIndex(0);
  Blynk.virtualWrite(V2, sicaklik);
}


// EEPROM'a Veri Yazma
void writeEEPROM(int ilkAdres, int uzunluk, char* yazilacakVeri) {
  EEPROM.begin(512);
  yield();
  Serial.println();
  Serial.print("writing EEPROM: ");
  //write to eeprom
  for (int i = 0; i < uzunluk; i++)
  {
    EEPROM.write(ilkAdres + i, yazilacakVeri[i]);
    Serial.print(yazilacakVeri[i]);
  }
  Serial.println();
  EEPROM.commit();
  EEPROM.end();
  for (int x = 0; x < 32; x++)
  {
    yazilacakVeri[x] = 0;
  }
}
// EEPROM'a Veri Yazma


// EEPROM'dan Veri Okuma
void readEEPROM(int ilkAdres, int maxUzunluk, char* okunanVeri) {
  EEPROM.begin(512);
  delay(10);
  for (int i = 0; i < maxUzunluk; i++)
  {
    okunanVeri[i] = char(EEPROM.read(ilkAdres + i));
  }
  EEPROM.end();
  Serial.print("ready reading EEPROM:");
  Serial.println(okunanVeri);

}
// EEPROM'dan Veri Okuma

// EEPROM'dan Veri Okuma V1
String veriOkumaEEPROM(int adres)
{
  EEPROM.begin(512);
  int i;
  char veri[33]; // max 32 byte
  int uzunluk = 0;
  unsigned char k;
  k = EEPROM.read(adres);
  while (k != '\0' && uzunluk < 32)
  {
    k = EEPROM.read(adres + uzunluk);
    veri[uzunluk] = k;
    uzunluk++;
  }
  veri[uzunluk] = '\0';
  return veri;
  EEPROM.end();
}
// EEPROM'dan Veri Okuma V1


void webpage()
{
  String webpage;

  webpage = "<html>";
  webpage += "<head><title>AirHome - Blynk</title>";
  webpage += "<style>";
  webpage += "body { background-color: #A3D133; font-family: Arial, Helvetica, Sans-Serif; Color: black;}";
  webpage += "</style>";
  webpage += "</head>";
  webpage += "<body>";
  webpage += "<h1><br>AirHome Blynk ve WiFi Bilgileri Giris Ekrani</h1>";
  webpage += "<form  name='frm'  method='POST'>";
  webpage += "Blynk Auth: <input type='text' name='girilenAuth'><br>"; // girilenAuth
  webpage += "WiFi SSID: <input type='text' name='girilenSSID'><br>"; // girilenSSID
  webpage += "WiFi Pass: <input type='text' name='girilenPass'><br>"; // girilenPass
  webpage += "<input type='submit' value='Kaydet'>";
  webpage += "</form>";
  webpage += "</body>";
  webpage += "</html>";

  server.send(200, "text/html", webpage);
}
void response() {
  if (server.hasArg("girilenAuth") && (server.arg("girilenAuth").length() > 0)) { // TODO check that it's not longer than 31 characters
    Serial.println("Girilen Bilgiler:\t");
    Serial.println(server.arg("girilenAuth"));
    Serial.println(server.arg("girilenSSID"));
    Serial.println(server.arg("girilenPass"));
    String girilenAuthString = server.arg("girilenAuth");
    String girilenSSIDString = server.arg("girilenSSID");
    String girilenPassString = server.arg("girilenPass");
    //    Serial.println(girilenAuthString);

    EEPROM.begin(512);
    char girilenAuthChar[33], girilenSSIDChar[33], girilenPassChar[33];
    girilenAuthString.toCharArray(girilenAuthChar, girilenAuthString.length() + 1); // String to Char
    girilenSSIDString.toCharArray(girilenSSIDChar, girilenSSIDString.length() + 1);
    girilenPassString.toCharArray(girilenPassChar, girilenPassString.length() + 1);
    EEPROM.put(0, girilenAuthChar);
    EEPROM.put(38, girilenSSIDChar);
    EEPROM.put(76, girilenPassChar);
    EEPROM.commit();
    EEPROM.end();

    delay(1000);
    Serial.println("*****************************");
    Serial.println("EEPROM'a kaydedilen veriler: ");
    EEPROM.begin(512);
    char eepromX[33], eepromY[33], eepromZ[33];
    Serial.println(EEPROM.get(0, eepromX));
    Serial.println(EEPROM.get(38, eepromY));
    Serial.println(EEPROM.get(76, eepromZ));
    EEPROM.end();

    server.send(200, "text/html", "<html><body><h1>Successful</h1><a href='/'>Home</a></body></html>");
  } else {
    server.send(400, "text/html", "<html><body><h1>HTTP Error 400</h1><p>Bad request. Please enter a value.</p></body></html>");
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("*****************************");

  pinMode(buton, INPUT);
  pinMode(role, OUTPUT);
  digitalWrite(role, LOW);

  if (digitalRead(buton) == HIGH)
  {
    mod = 1;
    Serial.println("Mod 1: Normal moda gecildi.");
  }
  if (digitalRead(buton) == LOW)
  {
    mod = 2;
    Serial.println("Mod 2: Programlama moduna gecildi.");
  }


  switch (mod)
  {
    case 1:   // Normal Mod

      Serial.println("Normal mod baslatiliyor...");

      EEPROM.begin(512);
      EEPROM.get(0, authE); // EEPROM'dan 0 adresli veri çekildi.
      EEPROM.get(38, ssidE);
      EEPROM.get(76, passE);
      Serial.println(authE);
      Serial.println(ssidE);
      Serial.println(passE);
      EEPROM.end();

      Serial.println("Blynk ile baglanti kuruluyor...");
      //  Blynk.begin(authX, ssidX, passX);
      //  Blynk.begin(auth, ssid, pass);
      Blynk.begin(authE, ssidE, passE);
      //  Blynk.begin(auth, ssid, pass);
      //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
      //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
      Serial.println("Blynk ile baglanti kuruldu.");

      led1.off(); // Lamba default olarak kapatıldı.

      DS18B20.begin();

      // Attach INT to our handler
      attachInterrupt(digitalPinToInterrupt(buton), checkPin, FALLING);

      break;

    case 2:

      Serial.println("Programlama modu baslatiliyor...");

      WiFi.softAP(ssidAP, passAP);
      WiFi.softAPConfig(local_ip, gateway, subnet);
      IPAddress myIP = WiFi.softAPIP();

      Serial.println("AP modu baslatildi.");
      Serial.println("ESP8266 IP Adress: ");
      Serial.println(myIP);
      server.on("/", HTTP_GET, webpage);
      server.on("/", HTTP_POST, response);
      server.begin();
      Serial.println("HTTP server baslatildi.");

      break;
  }


}


void loop()
{

  switch (mod)
  {

    case 1: // Normal Mod

      Blynk.run();


      if (pinChanged) {

        // Process the value
        if (pinValue) {
          led1.on();
          digitalWrite(role, HIGH);
          Serial.println("Role acildi.");
          // Blynk.notify("Lamba acıldı.");


        } else {
          led1.off();
          digitalWrite(role, LOW);
          Serial.println("Role kapatildi.");
          // Blynk.notify("Lamba kapatıldı.");
        }

        // Clear the mark, as we have processed the value
        pinChanged = false;
      }

      break;

    case 2: // Programlama Modu

      server.handleClient();

      break;

  }









  String okunanVeri = veriOkumaEEPROM(0);
  Serial.print("Okunan Veri:    ");
  Serial.println(okunanVeri);

  EEPROM.end();
  * /




}
