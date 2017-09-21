
#define SDA 9
#define SCL 7
#define RST 5

uint8_t byte_data = 0;
uint8_t bit_cnt = 0;
uint16_t byte_cnt = 0;

void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  Serial.println("setup()");

  pinMode(RST, OUTPUT);
  pinMode(SCL, OUTPUT);
  pinMode(SDA, INPUT);

  digitalWrite(RST, LOW);
  digitalWrite(SCL, LOW);
  delay(1000);
  digitalWrite(RST, HIGH);
  Serial.print("0"); Serial.print(": ");
}

void loop() {
  // put your main code here, to run repeatedly:

  uint8_t d = 0;

  if(byte_cnt > 4096)
  {
    delay(100);
    return;
  }

  
  
  digitalWrite(SCL, LOW);
  delay(1);
  uint8_t bit = digitalRead(SDA);
  byte_data |= bit << bit_cnt;
  digitalWrite(SCL, HIGH);
  delay(1);

  bit_cnt++;
  if(bit_cnt >= 7)
  {
    bit_cnt = 0;
    byte_cnt++;
    Serial.print(byte_data, HEX);
    byte_data = 0;
    if(byte_cnt % 16 == 0)
    {
      Serial.println();
      Serial.print(byte_cnt, HEX); Serial.print(": ");
    }
    delay(10);
  }
}
