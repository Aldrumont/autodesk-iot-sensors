
# Projeto ESP32: Monitoramento com Sensores

Este projeto utiliza um ESP32 para monitorar quatro sensores e enviar dados para um servidor em tempo real. Ele inclui sensores de temperatura/umidade, gás e ruído, conectados ao ESP32.

## Sensores Utilizados

1. **DHT22** - Sensor de temperatura e umidade.
2. **MQ-7** - Sensor de gás monóxido de carbono (CO).
3. **Sensor de Ruído** - Medidor de intensidade de som (analógico).
4. **LED Interno** - Indicador de status.

---

## Conexões dos Sensores

### GPIOs Configurados

| Sensor         | GPIO      | Observações                                   |
|----------------|-----------|-----------------------------------------------|
| **DHT22**      | GPIO4     | Alimentado com 3.3V                          |
| **MQ-7**       | GPIO33    | Deve ser alimentado com 5V (conecte ao VIN). |
| **Ruído**      | GPIO32    | Alimentado com 3.3V                          |
| **LED Interno**| GPIO2     | Indicador de status                          |

### Alteração dos GPIOs

Se for necessário alterar os pinos, modifique as seguintes linhas no código:

```cpp
#define DHTPIN 4      // GPIO conectado ao DHT22
#define MQ_PIN 33     // GPIO para o sensor MQ
#define NOISE_PIN 32  // GPIO para o sensor de ruído
#define LED_BUILTIN 2 // LED interno do ESP32
```

---

## Alimentação dos Sensores

- **DHT22**: Alimentação recomendada: **3.3V**.
- **MQ-7**: Necessita de **5V**. Conecte ao pino VIN do ESP32 para alimentação estável.
- **Sensor de Ruído**: Alimentação recomendada: **3.3V**.

⚠️ **Atenção**: O sensor MQ-7 deve ser alimentado exclusivamente com 5V para funcionar corretamente. Utilize o pino VIN do ESP32, que fornece 5V, quando o ESP32 está alimentado por USB ou fonte externa.

---

## Configuração do Intervalo de Leitura

O intervalo entre as leituras é configurável no início do código. Por padrão, está definido como **20 segundos**:

```cpp
const unsigned long interval = 20000; // Intervalo em milissegundos
```

Se necessário, ajuste o valor para um intervalo desejado (em milissegundos).

---

## Configuração Wi-Fi e Servidor

### Configuração de Wi-Fi

Altere as credenciais da rede Wi-Fi no código:

```cpp
const char* ssid = "NomeDaRedeWiFi";
const char* password = "SenhaDaRedeWiFi";
```

### Configuração do Servidor

Defina o endereço do servidor para onde os dados serão enviados:

```cpp
const char* serverName = "http://IP_DO_SERVIDOR:PORTA/api/sensors";
```

---

## Comportamento em Caso de Erro

Caso algum sensor não consiga ser lido, o valor correspondente será enviado como `null` no payload JSON. Por exemplo:

```json
{
  "time": "1736482060",
  "temp": null,
  "umidade": null,
  "co": 23.00,
  "ruido": 43.00
}
```

Isso garante que o servidor ainda receba os dados disponíveis, mesmo se houver falhas em sensores específicos.

---

## Funcionamento

1. O ESP32 conecta à rede Wi-Fi e sincroniza o horário com um servidor NTP.
2. Os sensores são lidos periodicamente.
3. Os dados são enviados ao servidor no formato JSON, contendo:
   - Temperatura (em graus Celsius).
   - Umidade (em porcentagem).
   - Monóxido de carbono (CO) em ppm.
   - Nível de ruído (em escala arbitrária).

---

Se tiver dúvidas ou precisar de suporte, entre em contato! 🚀
