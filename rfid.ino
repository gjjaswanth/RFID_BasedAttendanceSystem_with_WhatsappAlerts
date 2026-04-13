#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SPI.h>
#include <MFRC522.h>
#include <base64.h>
#include <vector>
#include <algorithm>

// ================= RFID =================
#define SS_PIN D2
#define RST_PIN D1
#define BUZZER_PIN D0

MFRC522 mfrc522(SS_PIN, RST_PIN);

// ================= WIFI =================
const char* ssid = "XXXX";
const char* password = "XXXX";

// ================= GOOGLE =================
const char* host = "script.google.com";
String GAS_ID = "AKfycbz0W86cbx3Y4deDnXu97nzanVJWiJOzyQXMNCglKhLessbRJ5sG5UKCbXkhkbPBEUk";

// ================= TWILIO =================
String accountSID = "SID";
String authToken  = "TOKEN";
String fromNumber = "whatsapp:+14155238886";

// ================= TEACHER =================
String teacherUID = "f78860b2";

// ================= TIMING (UNCHANGED AS YOU SAID) =================
const unsigned long CLASS_TIME = 30000;
const unsigned long ONTIME_LIMIT = 5000;
const unsigned long ROUND2_TIME = 5000;
const unsigned long SCAN_DELAY = 3000;

// ================= STATE =================
bool classRunning = false;
bool round2Started = false;
bool round2Running = false;

unsigned long classStart = 0;
unsigned long round2Start = 0;
unsigned long round2End = 0;
unsigned long lastScan = 0;

// ================= STUDENTS =================
struct Student {
  String uid;
  String name;
  String parent;
};

Student students[] = {
  {"675b93b2", "sowmya", "whatsapp:+91XXXXX"},
  {"b7c864b2", "canteen", "whatsapp:+91XXXX"}
};

const int N = sizeof(students)/sizeof(students[0]);

// ================= RECORD =================
struct Record {
  String uid;
  bool r1=false, r2=false;
  bool r1Late=false, r2Late=false;
};

std::vector<Record> rec;

// ================= HELPERS =================
int findStudent(String uid){
  for(int i=0;i<N;i++) if(students[i].uid==uid) return i;
  return -1;
}

Record& getRec(String uid){
  for(auto &r:rec) if(r.uid==uid) return r;
  rec.push_back({uid});
  return rec.back();
}

String getName(String uid){
  int i=findStudent(uid);
  return i>=0?students[i].name:"Unknown";
}

String getParent(String uid){
  int i=findStudent(uid);
  return i>=0?students[i].parent:"whatsapp:+91XXXXXXXXXX";
}

// ================= IO =================
void beep(int t){
  digitalWrite(BUZZER_PIN,HIGH);
  delay(t);
  digitalWrite(BUZZER_PIN,LOW);
}

// ================= GOOGLE =================
void sendToGoogle(String uid,String name,String status){
  WiFiClientSecure c; 
  c.setInsecure();
  if(!c.connect(host,443)) return;

  String url="/macros/s/"+GAS_ID+"/exec?uid="+uid+"&name="+name+"&status="+status;

  c.print(String("GET ")+url+" HTTP/1.1\r\nHost: "+host+"\r\n\r\n");
}

// ================= URL ENCODE =================
String urlEncode(String msg) {
  msg.replace(" ", "%20");
  msg.replace(":", "%3A");
  return msg;
}

// ================= WHATSAPP =================
void sendWhatsApp(String msg,String to){

  WiFiClientSecure client;
  client.setInsecure();

  if(!client.connect("api.twilio.com",443)){
    Serial.println("❌ Twilio connection failed");
    return;
  }

  String url="/2010-04-01/Accounts/"+accountSID+"/Messages.json";

  String toEnc = to;
  toEnc.replace("+", "%2B");

  String fromEnc = fromNumber;
  fromEnc.replace("+", "%2B");

  String body = urlEncode(msg);

  String postData = "To=" + toEnc +
                    "&From=" + fromEnc +
                    "&Body=" + body;

  String auth = accountSID + ":" + authToken;
  String enc  = base64::encode(auth);

  Serial.println("📤 Sending WhatsApp...");
  Serial.println(postData);

  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: api.twilio.com\r\n" +
               "Authorization: Basic " + enc + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: " + postData.length() + "\r\n\r\n" +
               postData);

  delay(1000);

  while(client.available()){
    Serial.write(client.read());
  }
}

// ================= RESET =================
void resetAll(){
  classRunning=false;
  round2Started=false;
  round2Running=false;
  rec.clear();
  Serial.println("System Reset");
}

// ================= FINAL EVALUATION =================
void evaluate(){

  Serial.println("FINAL PROCESSING");

  for(int i=0;i<N;i++){

    String uid=students[i].uid;
    String name=students[i].name;
    String parent=students[i].parent;

    bool r1=false,r2=false,r1Late=false,r2Late=false;

    for(auto &r:rec){
      if(r.uid==uid){
        r1=r.r1; r2=r.r2;
        r1Late=r.r1Late;
        r2Late=r.r2Late;
      }
    }

    if(!round2Started){

      if(r1){
        if(r1Late){
          sendWhatsApp("ALERT: "+name+" was LATE today.", parent);
          sendToGoogle(uid,name,"LATE");
        }else{
          sendToGoogle(uid,name,"ON_TIME");
        }
      }else{
        sendWhatsApp("ALERT: "+name+" was ABSENT today.", parent);
        sendToGoogle(uid,name,"ABSENT");
      }
    }

    else{

      if(r1 && r2){

        if(r1Late || r2Late){
          sendWhatsApp("ALERT: "+name+" was LATE in attendance.", parent);
          sendToGoogle(uid,name,"LATE");
        }else{
          sendToGoogle(uid,name,"ON_TIME");
        }
      }

      else if((r1 && !r2) || (!r1 && r2)){
        sendWhatsApp("ALERT: "+name+" is suspected of PROXY attendance.", parent);
        sendToGoogle(uid,name,"PROXY");
      }

      else{
        sendWhatsApp("ALERT: "+name+" was ABSENT today.", parent);
        sendToGoogle(uid,name,"ABSENT");
      }
    }
  }

  resetAll();
}

// ================= SETUP =================
void setup(){
  Serial.begin(115200);
  pinMode(BUZZER_PIN,OUTPUT);

  SPI.begin();
  mfrc522.PCD_Init();

  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED) delay(500);

  Serial.println("Ready");
}

// ================= LOOP =================
void loop(){

  unsigned long now=millis();

  if(classRunning && now-classStart>=CLASS_TIME){
    evaluate();
  }

  if(round2Running && now>=round2End){
    round2Running=false;
    Serial.println("Round2 End");
  }

  if(!mfrc522.PICC_IsNewCardPresent()) return;
  if(!mfrc522.PICC_ReadCardSerial()) return;

  String uid="";
  for(byte i=0;i<mfrc522.uid.size;i++){
    if(mfrc522.uid.uidByte[i]<0x10) uid+="0";
    uid+=String(mfrc522.uid.uidByte[i],HEX);
  }
  uid.toLowerCase();

  if(now-lastScan<SCAN_DELAY) return;
  lastScan=now;

  beep(100);
  Serial.println(uid);

  // ===== TEACHER =====
  if(uid==teacherUID){

    if(!classRunning){
      classRunning=true;
      classStart=now;
      Serial.println("Round1 Start");
    }
    else if(!round2Started){
      round2Started=true;
      round2Running=true;
      round2Start=now;
      round2End=now+ROUND2_TIME;
      Serial.println("Round2 Start");
    }

    return;
  }

  if(!classRunning) return;
  if(findStudent(uid)<0) return;

  Record &r=getRec(uid);

  // 🔥 FIXED LOGIC HERE
  if(!round2Started){
    if(!r.r1){
      r.r1=true;
      r.r1Late=(now-classStart)>ONTIME_LIMIT;
      Serial.println("Marked in Round1");
    }
  }
  else{
    if(!r.r2){
      r.r2=true;
      r.r2Late=(now-round2Start)>ONTIME_LIMIT;
      Serial.println("Marked in Round2");
    }
  }

  mfrc522.PICC_HaltA();
}
