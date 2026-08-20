/*
 * ============================================================
 *  Carrinho-robo - Project-based Maker Lab (FIAP)
 *  Aula 16 - Controle sem fio (WiFi) - opcao 1
 *
 *  BASE: codigo apresentado nos slides 8 a 13 da Aula 16
 *        (fonte citada no slide: randomnerdtutorials.com/esp32-wi-fi-car-robot-arduino)
 *        Mantidos os nomes de variaveis, a pinagem da tabela do slide 7
 *        e a estrutura de rotas/handlers do original.
 *
 *  ACRESCIMOS DO GRUPO (todos marcados com "// [GRUPO]"):
 *    1. sensor ultrassonico HC-SR04
 *    2. parada automatica com obstaculo a menos de 20 cm
 *    3. leitura da distancia exibida na propria pagina
 *    4. failsafe: para os motores se a pagina parar de responder
 *  Ver docs/testes.md para o roteiro de teste incremental.
 * ============================================================
 */

#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid     = "COLOQUE_SEU_SSID";      // ✏️ preencher
const char* password = "COLOQUE_SUA_SENHA";     // ✏️ preencher

// Create an instance of the WebServer on port 80
WebServer server(80);

// Motor 1 - pinagem da tabela do slide 7
int motor1Pin1 = 27;
int motor1Pin2 = 26;
int enable1Pin = 14;

// Motor 2
int motor2Pin1 = 33;
int motor2Pin2 = 25;
int enable2Pin = 32;

// Setting PWM properties
const int freq = 30000;
const int resolution = 8;
int dutyCycle = 0;

String valueString = String(0);

// [GRUPO] pinos do sensor ultrassonico
const int trigPin = 5;
const int echoPin = 18;              // ECHO devolve 5 V -> divisor 1k/2k ate o GPIO

// [GRUPO] estado usado pela trava de seguranca e pelo failsafe
const int DIST_PARADA_CM = 20;
const unsigned long TIMEOUT_COMANDO_MS = 1500;
int distanciaCm = -1;
bool andandoParaFrente = false;
unsigned long ultimaLeitura = 0;
unsigned long ultimoComando = 0;

// ============================================================
// [GRUPO] leitura do HC-SR04
// ============================================================
int medirDistancia() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duracao = pulseIn(echoPin, HIGH, 25000UL);   // timeout ~4 m
  if (duracao == 0) return -1;                               // sem eco
  int cm = duracao / 58;
  return (cm > 300) ? -1 : cm;
}

// [GRUPO] true quando ha obstaculo perto demais
bool caminhoBloqueado() {
  return (distanciaCm > 0 && distanciaCm < DIST_PARADA_CM);
}

// ============================================================
//  PAGINA WEB  (handleRoot do slide 9, com a linha da distancia)
// ============================================================
void handleRoot() {
  const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; }
    .button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none;
              user-select: none; background-color: #4CAF50; border: none; color: white;
              padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px;
              cursor: pointer; }
    .button2 { background-color: #555555; }
    #distancia { font-size: 22px; font-weight: bold; }
  </style>
  <script>
    function moveForward() { fetch('/forward'); }
    function moveLeft()    { fetch('/left'); }
    function stopRobot()   { fetch('/stop'); }
    function moveRight()   { fetch('/right'); }
    function moveReverse() { fetch('/reverse'); }
    function updateMotorSpeed(pos) {
      document.getElementById('motorSpeed').innerHTML = pos;
      fetch(`/speed?value=${pos}`);
    }
    // [GRUPO] atualiza a distancia e serve de heartbeat para o failsafe
    setInterval(() => {
      fetch('/distancia').then(r => r.text()).then(t => {
        document.getElementById('distancia').innerHTML = (t == -1) ? '--' : t;
      });
    }, 400);
  </script>
</head>
<body>
  <h1>ESP32 Motor Control</h1>
  <p><button class="button" onclick="moveForward()">FORWARD</button></p>
  <div style="clear: both;">
    <p>
      <button class="button" onclick="moveLeft()">LEFT</button>
      <button class="button button2" onclick="stopRobot()">STOP</button>
      <button class="button" onclick="moveRight()">RIGHT</button>
    </p>
  </div>
  <p><button class="button" onclick="moveReverse()">REVERSE</button></p>
  <p>Motor Speed: <span id="motorSpeed">0</span></p>
  <input type="range" min="0" max="100" step="25" id="motorSlider"
         oninput="updateMotorSpeed(this.value)" value="0"/>
  <p>Distancia: <span id="distancia">--</span> cm</p>
</body>
</html>)rawliteral";
  server.send(200, "text/html", html);
}

// ============================================================
//  HANDLERS DE MOVIMENTO  (slides 10 e 11)
// ============================================================
void handleForward() {
  Serial.println("Forward");
  ultimoComando = millis();                  // [GRUPO]
  if (caminhoBloqueado()) {                  // [GRUPO] nao deixa avancar contra o obstaculo
    Serial.println("Bloqueado pelo sensor");
    handleStop();
    return;
  }
  andandoParaFrente = true;                  // [GRUPO]
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
  server.send(200);
}

void handleLeft() {
  Serial.println("Left");
  ultimoComando = millis();                  // [GRUPO]
  andandoParaFrente = false;                 // [GRUPO]
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
  server.send(200);
}

void handleRight() {
  Serial.println("Right");
  ultimoComando = millis();                  // [GRUPO]
  andandoParaFrente = false;                 // [GRUPO]
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
  server.send(200);
}

void handleReverse() {
  Serial.println("Reverse");
  ultimoComando = millis();                  // [GRUPO]
  andandoParaFrente = false;                 // [GRUPO]
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
  server.send(200);
}

void handleStop() {
  Serial.println("Stop");
  ultimoComando = millis();                  // [GRUPO]
  andandoParaFrente = false;                 // [GRUPO]
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
  server.send(200);
}

// [GRUPO] usada pelo loop, quando nao ha requisicao para responder
void pararMotores() {
  andandoParaFrente = false;
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

// ============================================================
//  VELOCIDADE  (handleSpeed do slide 11)
// ============================================================
void handleSpeed() {
  ultimoComando = millis();                  // [GRUPO]
  if (server.hasArg("value")) {
    valueString = server.arg("value");
    int value = valueString.toInt();
    if (value == 0) {
      ledcWrite(enable1Pin, 0);
      ledcWrite(enable2Pin, 0);
      pararMotores();
    } else {
      dutyCycle = map(value, 25, 100, 200, 255);
      ledcWrite(enable1Pin, dutyCycle);
      ledcWrite(enable2Pin, dutyCycle);
      Serial.println("Motor speed set to " + String(value));
    }
  }
  server.send(200);
}

// [GRUPO] rota nova: devolve a distancia lida para a pagina
void handleDistancia() {
  ultimoComando = millis();
  server.send(200, "text/plain", String(distanciaCm));
}

// ============================================================
//  SETUP  (slide 12)
// ============================================================
void setup() {
  Serial.begin(115200);

  // Set the Motor pins as outputs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // [GRUPO] pinos do sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configure PWM Pins  (API do core ESP32 3.x, como no slide)
  ledcAttach(enable1Pin, freq, resolution);
  ledcAttach(enable2Pin, freq, resolution);
  // Core 2.x? usar no lugar:
  //   ledcSetup(0, freq, resolution); ledcAttachPin(enable1Pin, 0);
  //   ledcSetup(1, freq, resolution); ledcAttachPin(enable2Pin, 1);

  // Initialize PWM with 0 duty cycle
  ledcWrite(enable1Pin, 0);
  ledcWrite(enable2Pin, 0);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Define routes  (slide 13)
  server.on("/",          handleRoot);
  server.on("/forward",   handleForward);
  server.on("/left",      handleLeft);
  server.on("/stop",      handleStop);
  server.on("/right",     handleRight);
  server.on("/reverse",   handleReverse);
  server.on("/speed",     handleSpeed);
  server.on("/distancia", handleDistancia);   // [GRUPO]

  // Start the server
  server.begin();
}

// ============================================================
//  LOOP  (slide 13 + acrescimos do grupo)
// ============================================================
void loop() {
  server.handleClient();

  // [GRUPO] leitura periodica do sensor, sem travar o servidor
  if (millis() - ultimaLeitura >= 60) {
    ultimaLeitura = millis();
    distanciaCm = medirDistancia();
  }

  // [GRUPO] trava de seguranca: so interrompe quem esta indo para frente
  if (andandoParaFrente && caminhoBloqueado()) {
    pararMotores();
    Serial.printf("OBSTACULO a %d cm - parado\n", distanciaCm);
  }

  // [GRUPO] failsafe: perdeu contato com o navegador -> para
  if (millis() - ultimoComando > TIMEOUT_COMANDO_MS) {
    pararMotores();
  }
}
