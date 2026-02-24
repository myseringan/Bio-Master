// ======================= Bio Master (ESP8266 + Blynk + Captive Portal) =======================
#define BLYNK_TEMPLATE_ID   "TMPL6-Qhz9qFz"
#define BLYNK_TEMPLATE_NAME "Bio Master"

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <DNSServer.h>
#include <BlynkSimpleEsp8266.h>

#include <LittleFS.h>
#include <ArduinoJson.h>

const byte DNS_PORT = 53;
const char *ap_ssid = "Bio Master";
const char *ap_pass = "samurai2023";

char auth[] = "b-4atMkevFd_1iP5NTRkZr1t_zIO4iS0";

DNSServer dnsServer;
ESP8266WebServer server(80);

SoftwareSerial arduinoSerial(D2, D3);

float val;
int rele;
int ldr;

struct WifiCfg {
  String staSsid;
  String staPass;
  String blynkToken;
} wc;

bool loadCfg() {
  if (!LittleFS.exists("/wifi.json")) return false;
  File f = LittleFS.open("/wifi.json", "r");
  if (!f) return false;
  StaticJsonDocument<256> doc;
  auto err = deserializeJson(doc, f);
  f.close();
  if (err) return false;
  wc.staSsid    = String((const char*)doc["ssid"]);
  wc.staPass    = String((const char*)doc["pass"]);
  wc.blynkToken = String((const char*)doc["token"]);
  return wc.staSsid.length() > 0;
}

bool saveCfg(const String& s, const String& p, const String& t) {
  StaticJsonDocument<256> doc;
  doc["ssid"]  = s;
  doc["pass"]  = p;
  doc["token"] = t;
  File f = LittleFS.open("/wifi.json", "w");
  if (!f) return false;
  serializeJson(doc, f);
  f.close();
  wc.staSsid = s; wc.staPass = p; wc.blynkToken = t;
  return true;
}

const char loginPage[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>WiFi Biomaster</title>
  <style>
  :root{--bg:#000;--txt:#fff;--accent:#28de32;--focus:#03f417;--glass:rgba(255,255,255,.10);--glass-border:rgba(255,255,255,.28)}
  body{margin:0;padding:0;font-family:sans-serif;color:var(--txt);background:var(--bg);min-height:100dvh;overflow:hidden}
  .login-box{width:80%;max-width:400px;margin:auto;position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);padding:40px;background:var(--glass);border:1px solid var(--glass-border);border-radius:10px;box-sizing:border-box;box-shadow:0 15px 25px rgba(0,0,0,.6);backdrop-filter:blur(18px)}
  .login-box h2{margin:0 0 30px;text-align:center}
  .user-box{position:relative;margin-bottom:30px}
  .user-box input{width:100%;padding:12px 10px;font-size:16px;color:var(--txt);background:rgba(255,255,255,.14);border:1px solid rgba(255,255,255,.22);border-radius:12px;outline:none;transition:.2s}
  .user-box input:focus{border-color:var(--focus);box-shadow:0 0 0 3px rgba(3,244,23,.22)}
  .user-box label{position:absolute;top:0;left:0;padding:10px 6px;font-size:16px;color:var(--txt);pointer-events:none;transition:.18s}
  .user-box input:focus~label,.user-box input:valid~label{top:-14px;left:6px;color:var(--focus);font-size:12px;background:rgba(255,255,255,.10);backdrop-filter:blur(10px)}
  input[type="submit"]{padding:12px 20px;color:var(--accent);font-size:16px;text-transform:uppercase;margin-top:40px;letter-spacing:4px;cursor:pointer;background:rgba(40,222,50,0.10);border:2px solid var(--accent);border-radius:10px}
  input[type="submit"]:hover{background:var(--accent);color:#fff}
  </style>
</head>
<body>
  <div class="login-box">
    <h2>WiFi Setup</h2>
    <form action="/save" method="post">
      <div class="user-box"><input type="text" name="ssid" required=""><label>SSID</label></div>
      <div class="user-box"><input type="password" name="password" required=""><label>Password</label></div>
      <input type="submit" value="Save">
    </form>
  </div>
</body>
</html>
)";

void handleRoot() { server.send_P(200, "text/html", loginPage); }

const char redirectAfterSavePage[] PROGMEM = 
"<!doctype html><meta charset='utf-8'>"
"<meta http-equiv='refresh' content='1;url=/result'>"
"<title>Redirect</title>"
"<body style='font-family:sans-serif;background:#000;color:#fff'>"
"<p style='text-align:center;margin-top:2rem'>Redirecting...</p></body>";

void handleSave() {
  String ssidValue = server.arg("ssid");
  String passwordValue = server.arg("password");
  String tokenValue = server.arg("token");
  if (ssidValue.length() && passwordValue.length()) saveCfg(ssidValue, passwordValue, tokenValue);
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssidValue.c_str(), passwordValue.c_str());
  server.send_P(200, "text/html; charset=utf-8", redirectAfterSavePage);
}

const char resultOkPage[] PROGMEM = R"(
<html><head><meta charset="UTF-8"><style>
body{margin:0;font-family:sans-serif;color:#fff;min-height:100dvh;display:grid;place-items:center;background:#000}
h1{text-align:center;background:rgba(255,255,255,.10);border:1px solid rgba(255,255,255,.28);padding:10px 16px;border-radius:14px}
.ok{font-size:150px;display:flex;justify-content:center;align-items:center;color:#fff;border-radius:24px;background:#2bff00;width:200px;height:200px;margin:auto;margin-top:20px}
</style></head><body><div><h1>Connected to WiFi</h1><div class="ok">&#10003;</div></div></body></html>
)";

const char resultErrPage[] PROGMEM = R"(
<html><head><meta charset="UTF-8"><style>
body{margin:0;font-family:sans-serif;color:#fff;min-height:100dvh;display:grid;place-items:center;background:#000}
h1{text-align:center;background:rgba(255,255,255,.10);border:1px solid rgba(255,255,255,.28);padding:10px 16px;border-radius:14px}
.err{font-size:150px;display:flex;justify-content:center;align-items:center;color:#fff;border-radius:24px;background:#f00;width:200px;height:200px;margin:auto;margin-top:20px}
</style></head><body><div><h1>Failed to connect. Check credentials.</h1><div class="err">&#10007;</div></div></body></html>
)";

void handleResult() {
  const uint32_t WAIT_MS = 8000;
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < WAIT_MS) { delay(100); yield(); }
  if (WiFi.status() == WL_CONNECTED) server.send_P(200, "text/html", resultOkPage);
  else server.send_P(200, "text/html", resultErrPage);
}

BLYNK_WRITE(V4) {
  int v = param.asInt();
  static int last = -9999;
  if (v == last) return;
  last = v;
  arduinoSerial.write('<'); arduinoSerial.write('R');
  arduinoSerial.print(v); arduinoSerial.write('>'); arduinoSerial.write('\n');
}

BLYNK_WRITE(V5) {
  int v = param.asInt();
  static int last = -9999;
  if (v == last) return;
  last = v;
  arduinoSerial.write('<'); arduinoSerial.write('L');
  arduinoSerial.print(v); arduinoSerial.write('>'); arduinoSerial.write('\n');
}

void pollUNO() {
  static char frame[24];
  static uint8_t idx = 0;
  static bool inFrame = false;

  while (arduinoSerial.available()) {
    char c = (char)arduinoSerial.read();
    if (!inFrame) { if (c == '<') { inFrame = true; idx = 0; } continue; }
    if (c == '>') {
      frame[idx] = '\0';
      inFrame = false;
      if (idx >= 2) {
        char type = frame[0];
        for (uint8_t i = 1; i < idx; i++) if (frame[i] == ',') frame[i] = '.';
        float v = atof(&frame[1]);
        switch (type) {
          case 'T': Blynk.virtualWrite(V1, v); break;
          case 'H': Blynk.virtualWrite(V2, v); break;
          case 'S': Blynk.virtualWrite(V3, v); break;
          case 'I': Blynk.virtualWrite(V6, v); break;
        }
      }
      idx = 0; continue;
    }
    if (c == '\n' || c == '\r') continue;
    if (idx < sizeof(frame) - 1) frame[idx++] = c;
    else { inFrame = false; idx = 0; }
  }
}

void setup() {
  Serial.begin(115200);
  arduinoSerial.begin(9600);
  LittleFS.begin();
  bool have = loadCfg();
  WiFi.persistent(false);
  WiFi.setSleep(false);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ap_ssid, ap_pass);
  WiFi.softAPConfig(WiFi.softAPIP(), WiFi.softAPIP(), IPAddress(255,255,255,0));
  if (have) WiFi.begin(wc.staSsid.c_str(), wc.staPass.c_str());

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(300);
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  server.on("/", HTTP_ANY, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/result", HTTP_GET, handleResult);

  auto cp_ok = [](){ server.send(200, "text/plain", "OK"); };
  auto cp_redir = [](){ server.send(200, "text/html", "<!doctype html><meta http-equiv='refresh' content='0;url=/'><a href='/'>Continue</a>"); };
  server.on("/generate_204", HTTP_ANY, cp_ok);
  server.on("/gen_204", HTTP_ANY, cp_ok);
  server.on("/hotspot-detect.html", HTTP_ANY, cp_redir);
  server.on("/ncsi.txt", HTTP_ANY, [](){ server.send(200, "text/plain", "Microsoft NCSI"); });
  server.on("/connecttest.txt", HTTP_ANY, [](){ server.send(200, "text/plain", "OK"); });
  server.on("/fwlink", HTTP_ANY, cp_redir);
  server.on("/wfibio.css", HTTP_GET, [](){ server.send(200, "text/css", ""); });
  server.onNotFound([](){ server.sendHeader("Location", "/", true); server.send(302, "text/plain", "Redirecting..."); });
  server.begin();

  if (wc.blynkToken.length() > 0) Blynk.config(wc.blynkToken.c_str());
  else Blynk.config(auth);
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  Blynk.run();
  pollUNO();
}
