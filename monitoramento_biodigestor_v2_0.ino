#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// definição dos pinos:
#define PIN_DHT (9) // pino do sensor DHT
#define PIN_MQ4 (A0) // pino do sensor MQ4
#define PIN_DS18B20 (12) //pino do sensor de temperatura da fermentação
#define PIN_PRESSAO (A1) //pino para o sensor de pressão (não testei em casa o funcionamento)
#define PIN_PH (A2) //pino para o sensor de pH (não testei em casa o funcionamento)

// definição de constantes de hardware
#define RO_CLEAN_AIR_FACTOR (4.4) //RO_CLEAR_AIR_FACTOR=(RS no ar puro)/RO
#define RL_VALUE (21.7)    //resistência de carga em kilo ohms (inserir o valor da resistência utilizada na protoboard)

// definição de constantes de software
#define READ_SAMPLE_INTERVAL  (50)    //quantas amostras serão tomadas na operação normal
#define READ_SAMPLE_TIMES (5)     //intervalo de tempo (milissegundos) entre cada amostra na operação normal

// definição de variáveis:
float m = -0.318; // coef. angular
float beta = 1.133; // coef. linear
float Ro = 12.0; // colocar valor obtido em laboratório


DHT dht(PIN_DHT, DHT11); // define o pino e o tipo de DHT
OneWire oneWire(PIN_DS18B20); // Cria um objeto OneWire
DallasTemperature sensor(&oneWire); // Informa a referencia da biblioteca dallas temperature para Biblioteca onewire
DeviceAddress endereco_temp; // Cria um endereco temporario da leitura do sensor

void setup() {
  // Inicia e configura a Serial
  pinMode(PIN_MQ4, INPUT);
  dht.begin(); // inicializa o sensor DHT
  Serial.begin(9600); // 9600bps
}

void loop() {
  // Aguarda alguns segundos entre uma leitura e outra
  delay(2000); // 2 segundos (Datasheet)  


  float Tb = calcula_temperatura_biogas();
  float UR = calcula_umidade();
  float Tr = calcula_temperatura_reator();
  float P = calcula_pressao();
  float pH = calcula_pH();
  float ppm = calcula_CH4_ppm();
  String sep = ",";
  Serial.print(Tb + sep + UR + sep + Tr + sep + P + sep + pH + sep + ppm); // nova linha
}

/****************** MQCalculaResistencia ****************************************
Input:   raw_adc - valor bruto de leitura do adc, que representa a tensão
Output:  resistência calculada do sensor
Observações: A resistência de carga forma um divisor de tensão. Dada a tensão através
             do resistor de carga e sua resistência, a resistência do sensor pode ser obtida.
************************************************************************************/ 
float MQ4CalculaResistencia(int raw_adc) {
  float tensao_RL = raw_adc*(5.0/1023.0); //Convert analog values to voltage 
  return (float)RL_VALUE*((5.0/tensao_RL)-1.0); //retorna o valor de RS do sensor no ar puro durante o processo de calibração ou no biogás
}


/*****************************  MQleitura *********************************************
Input:   mq_pin - canal analógico
Output:  Rs do sensor
Remarks: Esta função usa a MQ4CalculaResistencia para calcular a resistência do sensor (Rs).
         O valor de RS muda em diferentes concentrações do gás alvo. O tempo de amostragem
         pode ser configurado alterando as definições iniciais.
************************************************************************************/ 
float MQleitura(int mq_pin)
{
  int i;
  float rs=0;

  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQ4CalculaResistencia(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  rs = rs/READ_SAMPLE_TIMES;

  return rs;  
}

float calcula_temperatura_biogas(){
  float temperatura; // variável para armazenar o valor de temperatura
  temperatura = dht.readTemperature(); // lê a temperatura em Celsius
  // Se ocorreu alguma falha durante a leitura
  if (isnan(temperatura)) {
    return 0.0;
    }
    else { // Se não
    // retorna o valor de temperatura
    return temperatura;
    }
}

float calcula_umidade(){
  float umidade; // variável para armazenar o valor de umidade
  umidade = dht.readHumidity(); // lê a umidade
  // Se ocorreu alguma falha durante a leitura
  if (isnan(umidade)) {
    return 0.0;
    }
    else { // Se não
    // retorna o valor de umidade
    return umidade;
    }
}

float calcula_temperatura_reator(){
  sensor.requestTemperatures(); // Envia comando para realizar a conversão de temperatura
  sensor.getAddress(endereco_temp,0);
  return sensor.getTempC(endereco_temp);
}

float calcula_pressao(){
  int raw_adc = analogRead(PIN_PRESSAO);
  float tensao_pressao =  raw_adc*(5.0/1023.0);
  return (157.0728 * tensao_pressao + 64.0460); // ajuste para converter a tensão em volts para pressão
}

float calcula_pH(){
  int raw_adc = analogRead(PIN_PH);
  float tensao_pH = raw_adc*(5.0/1023.0);
  return (-7.7837 * tensao_pH + 33.4608); // ajuste para converter a tensão em volts para pH

}

float calcula_CH4_ppm(){
  float razao_rs_ro = MQleitura(PIN_MQ4)/Ro;
  //float razao_rs_ro = a*calcula_umidade() + b*calcula_temperatura_biogas() + c;
  float ppm_log = (log10(razao_rs_ro)-beta)/m; //Get ppm value in linear scale according to the the ratio value  
  float ppm = pow(10, ppm_log); //Convert ppm value to log scale 
  float percentage = ppm/10000; //Convert to percentage
  return percentage;
}
