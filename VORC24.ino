// Khai báo các thư viện cần thiết 
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Adafruit_TCS34725.h>
#include <PS2X_lib.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
PS2X ps2x ;

// Định nghĩa các chân điều khiển 
#define PS2_DAT 12 // MISO  19
#define PS2_CMD 13 // MOSI  23
#define PS2_CLK 14 // SLK   18
#define PS2_SEL 15 // SS     5

#define NOR_SPEED 2048
#define MAX_SPEED 4095

int speed ;
// Các biến check trạng thái
bool checkCollector, checkReverse, ShootBlack, ShootWhite, OpenWhite, OpenBlack, checkSpin, offCollector, offShooter ;

uint16_t getColorTemp(){
  uint16_t r, g, b, c, colorTemp ;
  tcs.getRawData(&r, &g, &b, &c) ; // Lấy dữ liệu thô về 3 màu: Red, Blue, Green
  colorTemp = tcs.calculateColorTemperature(r, g, b) ; // Tính toán nhiệt độ màu dựa trên dữ liệu đã lấy ở trên
  return colorTemp ; // Trả về giá trị nhiệt độ màu 
}

void Move(int a, int b, int c, int d){ // Điều khiển động cơ di chuyển 
  pwm.setPWM(8, 0, a); // Truyền các giá trị xung pwm cho động cơ
  pwm.setPWM(9, 0, b);
  pwm.setPWM(10, 0, c);
  pwm.setPWM(11, 0, d);
}

void Collector(){ // Bật bộ thu
  pwm.setPWM(12, 0, 2730); // Động cơ hoạt động ở mức 75%
  pwm.setPWM(13, 0, 0) ;
}

void ReverseCollector(){ // Đảo ngược bộ thu
  pwm.setPWM(12, 0, 0);
  pwm.setPWM(13, 0, 2730); // Động cơ vẫn hoạt động ở mức 75% nhưng quay theo chiều ngược lại
}

void StopCollector(){ // Dừng động cơ, tắt bộ thu
  pwm.setPWM(12, 0, 0);
  pwm.setPWM(13, 0, 0) ;
}

void ShootWhiteBall(){ // Bắn bóng trắng
  pwm.setPWM(14, 0, 4095); // Động cơ hoạt động ở mức tối đa
  pwm.setPWM(15, 0, 0) ;
}

void ShootBlackBall(){ // Bắn bóng đen
  pwm.setPWM(14, 0, 2048) ; // Động cơ hoạt động ở mức 50%
  pwm.setPWM(15, 0, 0) ;
}

void StopShoot(){ // Tắt bộ bắn bóng
  pwm.setPWM(14, 0, 0) ;
  pwm.setPWM(15, 0, 0) ;
}

void setup() {
  Serial.begin(9600);
  Wire.begin(); 
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(50);
  Wire.setClock(400000);
 // Serial.print() ;
  // Kiểm tra kết nối với tay cầm 
  uint8_t err = -1;
  while (err != 0)
  {
    err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, true);
  }
  Move(0, 0, 0, 0) ; // Setup các giá trị ban đầu của động cơ di chuyển là 0
}

void loop() { 
  ps2x.read_gamepad(0, 0) ; // Đọc giá trị tay cầm 
  PS2Control() ; // Điều khiển robot
  delay(50) ;
}

void PS2Control(){
  Serial.print("ok");
  // Setup speed
  
  if(ps2x.ButtonPressed(PSB_L2)) speed = MAX_SPEED ; 
  if(ps2x.ButtonPressed(PSB_L1)) speed = NOR_SPEED ;
 
  // Movement
  int LY = ps2x.Analog(PSS_LY) -127 ; // Đọc tín hiệu của joystick
  int RX = ps2x.Analog(PSS_RX) -127 ;
 
  int a=0, b=0, c=0, d=0;
  if(LY > 0){ // Nếu đẩy joystick trái lên trên
    b = map(abs(LY), 0, 128, 0, speed) ; // Điều chỉnh giá trị xung của động cơ giúp robot tiến lên 
    c = map(abs(LY), 0, 128, 0, speed) ;
  }
  
  if(LY < 0){ // Nếu đẩy joystick trái xuống dưới 
    a = map(abs(LY), 0, 128, 0, speed ); // Điều chỉnh giá trị xung của động cơ giúp robot lùi lại 
    d = map(abs(LY), 0, 128, 0, speed) ;
  }
  if(RX > 0){ // Nếu đẩy joystick phải sang phải 
    a = map(abs(RX), 0, 128, 0, speed) ; // Điều chỉnh giá trị xung của động cơ giúp robot rẽ phải
    b = 0;
    c = map(abs(RX), 0, 128, 0, speed) ;
    d = 0; 
  }
  if(RX < 0){ // Nếu đẩy joystick phải sang trái
    b = map(abs(LY), 0, 128, 0, speed) ;  // Điều chỉnh giá trị xung của động cơ giúp robot rẽ trái
    a = 0; 
    d = map(abs(LY), 0, 128, 0, speed) ;
    c = 0 ;
  } 
  Move(a, b, c, d) ; // Điều khiển động cơ di chuyển 

  // Collector
  if(ps2x.ButtonPressed(PSB_L3)) offCollector = !offCollector ; // Ấn nút L3, thay đổi trạng thái có thể bật/tắt của bộ thu
  if(offCollector) StopCollector() ; 

  if(ps2x.ButtonPressed(PSB_R1)){ // Ấn nút R1
    checkCollector = !checkCollector ; // Chuyển trạng thái bộ thu
  }
  if(checkCollector && !offCollector){
    checkReverse = false ; // Tắt đảo ngược bộ thu
    Collector() ; // Bật bộ thu
  }
  if(ps2x.ButtonPressed(PSB_R2)){
    checkReverse = !checkReverse ; // Chuyển trạng thái đảo ngược bộ thu
  }
  if(checkReverse && !offCollector ){
    checkCollector = false ; // Tắt quay bộ thu theo chiều thuận
    ReverseCollector() ; // Đảo chiều bộ thu 
  }
  

  // Shooter 
  if(ps2x.ButtonPressed(PSB_R3)){ // Nếu ấn R3
    offShooter = !offShooter ; // Thay đổi trạng thái có thể bắn/tắt 
  }
  if(offShooter) StopShoot() ;

  if(ps2x.ButtonPressed(PSB_TRIANGLE)){ // Nếu ấn nút Tam giác
    ShootWhite = !ShootWhite ; // Chuyển trạng thái bắn bóng trắng
    if(ShootWhite){ 
      ShootBlack = false ; // Tắt bắn bóng đen
      ShootWhiteBall() ; 
    }
  }
  if(ps2x.ButtonPressed(PSB_SQUARE)){ // Nếu ấn nút Vuông
    ShootBlack = !ShootBlack ; // Chuyển trạng thái bắn bóng đen
    if(ShootBlack){
      ShootWhite = false ; // Tắt bắn bóng trắng
      ShootBlackBall() ;
    }
  }
  

  // Open door
  if(ps2x.ButtonPressed(PSB_PAD_LEFT)){ // Nếu ấn nút mũi tên bên trái
    OpenWhite = !OpenWhite ;// Chuyển trạng thái mở của khu chứa bóng trắng
    if(OpenWhite){
      pwm.setPWM(2, 0, 205) ; // Open White Door
    } else pwm.setPWM(2, 0, 410) ; // Close White Door
  }
  if(ps2x.ButtonPressed(PSB_PAD_RIGHT)){ // Nếu ấn nút mũi tên bên phải 
    OpenBlack = !OpenBlack ; // Chuyển trạng thái mở cửa khu chứa bóng đen
    if(OpenBlack){
      pwm.setPWM(4, 0, 410) ; //Open Black Door
    } else pwm.setPWM(4, 0, 205) ; // Close Black Door
  }

  // Spin dua bong vo khu loc
  if(ps2x.ButtonPressed(PSB_CROSS)){ // Nếu ấn nút X
    checkSpin = !checkSpin ; // Thay đổi trạng thái bộ quay
    if(checkSpin){
      pwm.setPWM(6, 0, 150) ; // Spin
    }else pwm.setPWM(6, 0, 0) ; // Stop Spin 
  } 

  uint16_t R, G, B, C, colorTemp;
  tcs.getRawData(&R, &G, &B, &C); // Lấy dữ liệu thô về 3 màu: Red, Blue, Green
  colorTemp = tcs.calculateColorTemperature(R, G, B);  // Tính toán nhiệt độ màu dựa trên dữ liệu đã lấy ở trên
  if(colorTemp > 10000 && colorTemp < 11000 && R + G + B < 1500) pwm.setPWM(5, 0, 495); // Kiểm tra có phải bóng đen hay không
  else if(colorTemp > 11000 && R + G + B > 3000) pwm.setPWM(5, 0, 150); // Kiểm tra bóng trắng
  else pwm.setPWM(5, 0, 0) ; // không có bóng -> động cơ dừng
}