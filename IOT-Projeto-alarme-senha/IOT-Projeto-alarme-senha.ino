#include <Keypad.h>
#include <LiquidCrystal.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>

bool digitandoSenha = false;
String senhaDigitada;
String senhaEscondida;
String senha = "4321";

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {5, 4, 3, A0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

//LCD
const int rs = 9, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Teclado
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


void callback(char* topic, byte* payload, unsigned int length) {
  int c = atoi(payload);
  Serial.println(topic);
  Serial.println(c);
}

//Ethernet
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x77 };
EthernetClient ethClient;
PubSubClient client("m14.cloudmqtt.com", 12719, callback, ethClient);

long lastReconnectAttempt = 0;

boolean reconnect() {
  Serial.println("reconectando...");
  if (client.connect("projeto3", "arduino", "123")) {
    Serial.println("conectado");
    client.subscribe("verificaSenha");
  }
  return client.connected();
}

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Senha");

  Serial.println("iniciando...");
  Ethernet.begin(mac);
  delay(1500);
  lastReconnectAttempt = 0;
}

void limpaLCD() {
  lcd.setCursor(0, 1);
  lcd.print("              ");
  lcd.setCursor(0, 0);
  lcd.print("              ");
}

void loop() {

  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;

      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {

    client.loop();

    char key = keypad.getKey();

    if (key) {

      if (digitandoSenha && key != '#') {
        senhaDigitada += key;
        senhaEscondida += "*";
        Serial.println(senhaDigitada);
        limpaLCD();
        lcd.setCursor(0, 1);
        lcd.print(senhaEscondida);
      }

      if (key == '*') {
        senhaDigitada = "";
        senhaEscondida = "";
        digitandoSenha = true;
        Serial.println("Iniciando digitação da senha...");
        limpaLCD();
      }
      if (key == '#') {
        digitandoSenha = false;
        Serial.println("Finaliza senha");

        if (senhaDigitada == senha) {
          Serial.println("Alarme desativado");
          limpaLCD();
          lcd.setCursor(0, 0);
          lcd.print("Acesso");
          lcd.setCursor(0, 1);
          lcd.print("Liberado");
        }
        else {
          Serial.println("Acesso negado");
          limpaLCD();
          lcd.setCursor(0, 0);
          lcd.print("Acesso");
          lcd.setCursor(0, 1);
          lcd.print("Negado");
        }
      }
    } //if key
  } //else connected
} //loop
