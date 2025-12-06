#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myServo;

// === Pins ===
const int servoPin = 3;
const int buzzerPin = 10;
const byte S0 = 4, S1 = 5, S2 = 7, S3 = 6, OUT_PIN = 8;

// === Servo Angles ===
const int SERVO_IDLE_ANGLE = 145;
const int SERVO_PRESENT_ANGLE = 0;

// === Constants ===
const int samplesPerChannel = 10;
const unsigned long pulseTimeout = 120000UL;
float CLEAR_BASELINE = 0.0f;
const float CLEAR_FLOOR_FACTOR = 1.25f;
const float CLEAR_ABS_MIN_HZ = 900.0f;
const float NEUTRAL_THRESHOLD = 0.03f;
const unsigned long STABILITY_TIME = 1500; 
const unsigned long COOLDOWN_TIME = 1500;  
const float COLOR_TOLERANCE = 0.025;      

// === Structs ===
struct RGBC { float r, g, b, c; };
struct ColorRef { String name; float r, g, b; int num; };


ColorRef COLORS[6] = {
  {"Brown",  0.355, 0.288, 0.357, 4},
  {"Green",  0.281, 0.323, 0.393, 3},
  {"Blue",   0.251, 0.296, 0.449, 1},
  {"Pink",   0.463, 0.206, 0.331, 2},
  {"Red",    0.475, 0.220, 0.305, 5},
  {"Purple", 0.283, 0.270, 0.448, 6}
};


// === Function declarations ===
unsigned long readPulseOnce();
float channelHz(bool s2, bool s3);
RGBC readRGBC();
RGBC readAverageColor(int n = 5);
float measureClearBaseline(unsigned long ms);
bool isAmbient(float rn, float gn, float bn, float clearHz);
String classifyClosest(float rn, float gn, float bn, float &bestDist);
int colorToNumber(String color);
void buzzColor(int times);

// === Setup ===
void setup() {
  Serial.begin(115200);
  pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
  pinMode(OUT_PIN, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  myServo.attach(servoPin, 500, 2400);
  myServo.write(SERVO_IDLE_ANGLE);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");
  delay(1000);

  CLEAR_BASELINE = measureClearBaseline(1500);
  lcd.clear();
  lcd.print("Sensor Ready!");
  delay(1000);
}

// === Main loop ===
void loop() {
  static String lastStableColor = "";
  static String currentColor = "";
  static unsigned long stableStart = 0;
  static float lastDist = 0;
  static bool confirmed = false;
  static unsigned long lastActionTime = 0;

  if (millis() - lastActionTime < COOLDOWN_TIME) {
    lcd.clear(); lcd.print("Cooldown...");
    delay(100);
    return;
  }

  RGBC v = readAverageColor(4);
  float sum = v.r + v.g + v.b + 1e-6f;
  float rn = v.r / sum;
  float gn = v.g / sum;
  float bn = v.b / sum;

  if (isAmbient(rn, gn, bn, v.c)) {
    lcd.clear(); lcd.print("Waiting...");
    myServo.write(SERVO_IDLE_ANGLE);
    stableStart = 0; confirmed = false; lastStableColor = "";
    delay(150);
    return;
  }

  float bestDist;
  String color = classifyClosest(rn, gn, bn, bestDist);
  int colorNum = colorToNumber(color);

  Serial.print("r:"); Serial.print(rn,3);
  Serial.print(" g:"); Serial.print(gn,3);
  Serial.print(" b:"); Serial.print(bn,3);
  Serial.print(" -> "); Serial.print(color);
  Serial.print(" dist="); Serial.println(bestDist,3);

  unsigned long now = millis();

  // --- Smart stability check ---
  if (color == currentColor && fabs(bestDist - lastDist) < COLOR_TOLERANCE) {
    if (stableStart == 0) stableStart = now;
    unsigned long held = now - stableStart;

    lcd.clear();
    lcd.print("Holding: "); lcd.print(color);
    lcd.setCursor(0,1);
    lcd.print((held / 1000.0), 1);
    lcd.print("s");

    // If held stable long enough, confirm
    if (!confirmed && held >= STABILITY_TIME) {
      confirmed = true;
      lcd.clear();
      lcd.print("Detected:");
      lcd.setCursor(0,1);
      lcd.print(color);
      lcd.print(" (");
      lcd.print(colorNum);
      lcd.print(")");

      Serial.print("✅ Confirmed: ");
      Serial.println(color);

      delay(300); 

      myServo.write(SERVO_PRESENT_ANGLE);
      delay(300);
      myServo.write(SERVO_IDLE_ANGLE);

      buzzColor(colorNum);

      lastActionTime = millis();
      stableStart = 0;
      confirmed = false;
      lastStableColor = color;
      currentColor = "";
    }
  } 
  else {
    currentColor = color;
    lastDist = bestDist;
    stableStart = now;
    confirmed = false;
  }

  delay(100);
}

// === Helper functions ===
RGBC readAverageColor(int n) {
  RGBC avg = {0,0,0,0};
  for (int i=0;i<n;i++){
    RGBC v = readRGBC();
    avg.r+=v.r; avg.g+=v.g; avg.b+=v.b; avg.c+=v.c;
    delay(20);
  }
  avg.r/=n; avg.g/=n; avg.b/=n; avg.c/=n;
  return avg;
}

unsigned long readPulseOnce() {
  unsigned long p=pulseIn(OUT_PIN,LOW,pulseTimeout);
  if(p==0)p=pulseIn(OUT_PIN,HIGH,pulseTimeout);
  return p;
}

float channelHz(bool s2,bool s3){
  digitalWrite(S2,s2?HIGH:LOW);
  digitalWrite(S3,s3?HIGH:LOW);
  delayMicroseconds(250);
  double acc=0;int got=0;
  for(int i=0;i<samplesPerChannel;i++){
    unsigned long p=readPulseOnce();
    if(p>0){acc+=1e6/(double)p;got++;}
  }
  if(got==0)return 0;
  return acc/got;
}

RGBC readRGBC(){
  RGBC v;
  v.r=channelHz(false,false);
  v.b=channelHz(false,true);
  v.g=channelHz(true,true);
  v.c=channelHz(true,false);
  return v;
}

float measureClearBaseline(unsigned long ms){
  unsigned long t0=millis(); double acc=0; int n=0;
  while(millis()-t0<ms){
    digitalWrite(S2,HIGH); digitalWrite(S3,LOW);
    delayMicroseconds(200);
    unsigned long p=pulseIn(OUT_PIN,LOW,30000UL);
    if(p==0)p=pulseIn(OUT_PIN,HIGH,30000UL);
    if(p>0){acc+=1e6/(double)p;n++;}
  }
  if(n==0)return 0;
  return acc/n;
}

bool isAmbient(float rn,float gn,float bn,float clearHz){
  if(CLEAR_BASELINE<=0)return true;
  if(clearHz < CLEAR_BASELINE * CLEAR_FLOOR_FACTOR) return true;
  if(clearHz < CLEAR_ABS_MIN_HZ) return true;
  float maxc=max(rn,max(gn,bn));
  float minc=min(rn,min(gn,bn));
  if((maxc-minc)<NEUTRAL_THRESHOLD)return true;
  return false;
}

String classifyClosest(float rn,float gn,float bn,float &bestDist){
  String bestColor=COLORS[0].name;
  float best=9999.0;
  for(int i=0;i<6;i++){
    float dr=(rn-COLORS[i].r)*1.0;   
    float dg=(gn-COLORS[i].g)*1.3;   
    float db=(bn-COLORS[i].b)*0.9;   
    float d=sqrt(dr*dr+dg*dg+db*db);
    if(d<best){best=d; bestColor=COLORS[i].name;}
  }
  bestDist=best;
  return bestColor;
}

int colorToNumber(String color){
  for(int i=0;i<6;i++){
    if(COLORS[i].name==color)return COLORS[i].num;
  }
  return 0;
}

// === Buzzer feedback ===
void buzzColor(int times){
  for(int i=0;i<times;i++){
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
  }
}
