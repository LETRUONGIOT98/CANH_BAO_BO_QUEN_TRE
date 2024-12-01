#include <SoftwareSerial.h>  // Thư viện cho giao tiếp UART (A7680C)

#define PIR_PIN 2           // Chân cảm biến PIR HC-SR501
#define RADAR_PIN 3         // Chân cảm biến radar HLK-LD2410
#define SW420_PIN 4         // Chân cảm biến rung SW420
#define BUZZER_PIN 5        // Chân còi Buzzer thụ động
#define A7680C_TX 6         // Chân TX của module A7680C (kết nối với RX của A7680C)
#define A7680C_RX 7         // Chân RX của module A7680C (kết nối với TX của A7680C)

SoftwareSerial a7680c(A7680C_RX, A7680C_TX);  // Khai báo giao tiếp UART với A7680C

bool carRunning = false;
bool personDetected = false;
String response = "";

void setup() {
  pinMode(PIR_PIN, INPUT_PULLUP);       // Cảm biến PIR
  pinMode(RADAR_PIN, INPUT_PULLUP);     // Cảm biến radar
  pinMode(SW420_PIN, INPUT_PULLUP);     // Cảm biến rung
  pinMode(BUZZER_PIN, OUTPUT);   // Còi buzzer

  Serial.begin(9600);            // Giao tiếp với Serial Monitor
  a7680c.begin(115200);            // Khởi động giao tiếp với module A7680C

  delay(1000);                  // Chờ 30 giây để hệ thống ổn định
  //sendSMS();
}

void loop() {
  carRunning = digitalRead(SW420_PIN);  // Đọc trạng thái của cảm biến rung

  if (!carRunning) {  // Nếu xe đã dừng hoặc tắt máy
    int pirState = digitalRead(PIR_PIN);      // Đọc cảm biến PIR
    int radarState = digitalRead(RADAR_PIN);  // Đọc cảm biến radar

    if (pirState == HIGH || radarState == HIGH) {
      // Phát hiện có người trong xe
      personDetected = true;
      activateBuzzer();
      //sendSMS();
    }
  }
  delay(1000);  // Đợi 1 giây trước khi kiểm tra lại
}

void activateBuzzer() {
  tone(BUZZER_PIN, 1000);  // Phát âm thanh tần số 1000Hz
  delay(60000);            // Kéo dài 1 phút
  noTone(BUZZER_PIN);      // Tắt âm thanh
}

void sendSMS() {
  a7680c.println("AT");                    // Kiểm tra kết nối A7680C
  delay(1000);
  a7680c.println("AT+CMGF=1");             // Chọn chế độ nhắn tin văn bản
  delay(1000);
  a7680c.println("AT+CMGS=\"0862888879\""); // Số điện thoại nhận tin nhắn 0862888879
  delay(1000);
  a7680c.print("Phat hien: Con nguoi trong xe"); // Nội dung tin nhắn
  delay(100);
  a7680c.write(26);                        // Gửi tin nhắn (CTRL+Z)
  Serial.println("DA NHAN TIN");
  // Chờ phản hồi từ người lái xe
  while (a7680c.available()) {
    response += char(a7680c.read());
  }

  // Kiểm tra nếu nhận được phản hồi "1"
  if (response.indexOf("1") != -1) {
    noTone(BUZZER_PIN);  // Ngừng còi buzzer
  }

  response = "";  // Xóa phản hồi để chuẩn bị cho lần kiểm tra tiếp theo
}
