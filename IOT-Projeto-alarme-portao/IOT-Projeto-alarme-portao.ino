#include <UIPEthernet.h>
#include <SPI.h>
#include <utility/logging.h>
#include <PubSubClient.h>
#include <Ultrasonic.h>

//Botão
const int Botao = 8; //botao no pino 8
int EstadoBotao = 0; //Variavel para ler o status do pushbutton

//Buzzer
const int Buzzer = 9;
int valorBuzzer = 500;

// Sensor ultrasônico
// ultrasonic(trigger, echo);
Ultrasonic ultrasonic(A5, A4); // Portas A5 e A4 para trigger e echo

// Endereço fisico do arduino na rede
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xF1, 0x62 };
void callback(char *topic, byte *payload, unsigned int length) {

}

EthernetClient ethClient;
// Cliente de conexão com o mqtt
PubSubClient client("m14.cloudmqtt.com", 14527, callback, ethClient);
int lastReconnectAttempt;

boolean reconnect() {
  Serial.println("reconectando...");
  if (client.connect("projeto3", "arduino", "123")) {
    Serial.println("conectado");
    client.subscribe("verificaSenha");
    client.subscribe("ativaAlarme");
    client.subscribe("presenca");
  }
  return client.connected();
}


void setup() {
  pinMode(Botao, INPUT); //Pino com botão será entrada
  pinMode(Buzzer, OUTPUT);
  Serial.begin(9600);
  Serial.println("iniciando...");
  Ethernet.begin(mac);
  delay(1500);
  lastReconnectAttempt = 0;
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
  }

  else {
    client.loop();

    EstadoBotao = digitalRead(Botao);
    // Lê o valor do sensor
    int distancia = ultrasonic.distanceRead();


    // Escreve o valor da distância no painel Serial
    Serial.print("Distance in CM: ");
    Serial.println(distancia);

    if ( distancia <= 10) {
      client.publish("presenca", "1");
    }
    else {
      //client.publish("presenca", "0");
    }

    if (EstadoBotao == HIGH) { //Se botão estiver pressionado (HIGH)
      Serial.println("Botão ligado");
      tone (Buzzer, valorBuzzer, 1000);
      client.publish("ativaAlarme", "1");
    }
    else { //se não estiver pressionado
      //Serial.println("Botão desligado");

    }
  } //else client loop
}  //void loop
