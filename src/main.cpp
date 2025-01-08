#include <Adafruit_Sensor.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <DHT.h>
#include <DHT_U.h>

// ============================ CONFIGURAÇÕES DO USUÁRIO ============================

// Configuração Wi-Fi
const char* ssid = "Desktop_F2925018";  // Nome da rede Wi-Fi
const char* password = "jr010203";      // Senha da rede Wi-Fi

// Configuração do servidor
const char* serverName = "http://104.154.230.185:3000/api/sensors"; // URL do servidor

// Configuração dos sensores
#define DHTPIN D2       // Pino conectado ao DHT22
#define DHTTYPE DHT22   // Modelo do sensor
#define ANALOG_PIN A0   // Pino analógico para MQ ou Ruído
#define DIGITAL_PIN D1  // Pino digital como simulação para o outro sensor

// Escolha do sensor no ANALOG_PIN (1 para MQ, 0 para Ruído)
#define SENSOR_ANALOG_IS_MQ 0 // Altere para 0 se o ANALOG_PIN for para Ruído

// Configuração do LED interno
#define LED_BUILTIN D4 // LED interno do ESP8266

// ============================ FIM DAS CONFIGURAÇÕES ============================

DHT dht(DHTPIN, DHTTYPE);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Sincronizar com UTC a cada 60s
WiFiClient wifiClient; // Cliente WiFi necessário para HTTPClient

// Declaração das funções
float lerTemperaturaDHT22();
float lerUmidadeDHT22();
float lerSensorAnalogico();
String obterTempoEpoch();

void setup() {
  Serial.begin(115200);
  Serial.println("Inicializando...");

  // Configurar LED interno
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // LED apagado inicialmente

  // Configuração de pino digital como simulação
  pinMode(DIGITAL_PIN, INPUT);

  // Inicializar o DHT22
  dht.begin();
  delay(2000); // Aguarda estabilização do sensor

  // Conectar ao Wi-Fi com LED piscando
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW);  // Liga LED
    delay(250);
    digitalWrite(LED_BUILTIN, HIGH); // Desliga LED
    delay(250);
    Serial.print(".");
  }

  // LED aceso constante após conexão
  digitalWrite(LED_BUILTIN, LOW); // LED ligado
  Serial.println("\nConectado ao Wi-Fi!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicializar o cliente NTP
  timeClient.begin();
}

void loop() {
  // Atualizar o cliente NTP
  timeClient.update();

  // Ler os sensores
  float temperature = lerTemperaturaDHT22();       // Ler temperatura
  float humidity = lerUmidadeDHT22();             // Ler umidade
  float analogValue = lerSensorAnalogico();       // Ler o sensor no pino A0

  // Se o sensor analógico for MQ, "ruído" é null; caso contrário, "CO" é null
  String coValue = SENSOR_ANALOG_IS_MQ ? String(analogValue) : "null";
  String noiseValue = SENSOR_ANALOG_IS_MQ ? "null" : String(analogValue);

  // Obter horário no formato Epoch
  String epochTime = obterTempoEpoch();

  // Formatar os dados no JSON
  String payload = "{";
  payload += "\"time\": \"" + epochTime + "\",";
  payload += "\"temp\": " + (isnan(temperature) ? "null" : String(temperature)) + ",";
  payload += "\"umidade\": " + (isnan(humidity) ? "null" : String(humidity)) + ",";
  payload += "\"co\": " + coValue + ",";
  payload += "\"ruido\": " + noiseValue;
  payload += "}";

  Serial.print("JSON Enviado: ");
  Serial.println(payload);

  // Enviar os dados ao servidor
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(wifiClient, serverName); // Novo formato: Passar o WiFiClient e a URL
    http.addHeader("Content-Type", "application/json"); // Cabeçalho da requisição

    int httpResponseCode = http.POST(payload); // Enviar os dados

    // Exibir a resposta
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Resposta do servidor: ");
      Serial.println(response);
    } else {
      Serial.print("Erro na requisição: ");
      Serial.println(httpResponseCode);
    }
    http.end(); // Finalizar a conexão HTTP
  } else {
    Serial.println("Wi-Fi desconectado!");
  }

  delay(20000); // Esperar 20 segundos antes de enviar novamente
}

// Implementações das funções
float lerTemperaturaDHT22() {
  float temperature = dht.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Erro ao ler temperatura do DHT22!");
    return NAN;
  }
  return temperature;
}

float lerUmidadeDHT22() {
  float humidity = dht.readHumidity();
  if (isnan(humidity)) {
    Serial.println("Erro ao ler umidade do DHT22!");
    return NAN;
  }
  return humidity;
}

float lerSensorAnalogico() {
  int analogValue = analogRead(ANALOG_PIN);
  if (SENSOR_ANALOG_IS_MQ) {
    return map(analogValue, 0, 1023, 0, 1000); // Mapear valores para MQ
  } else {
    return map(analogValue, 0, 1023, 0, 100); // Mapear valores para Ruído
  }
}

String obterTempoEpoch() {
  return String(timeClient.getEpochTime());
}
