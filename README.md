# VORC24 - Vankilde

Dự án điều khiển Robot thi đấu (sử dụng ESP32) hỗ trợ thu hồi, phân loại và bắn bóng.

## 🛠️ Thiết bị chính
* **ESP32**: Vi điều khiển trung tâm.
* **Tay cầm PS2**: Điều khiển di chuyển và kích hoạt cơ cấu.
* **PCA9685**: Mạch driver mở rộng điều khiển động cơ và servo.
* **TCS34725**: Cảm biến màu sắc dùng để phân loại bóng đen/trắng.

## 🎮 Điều khiển cơ bản
* **Di chuyển**: Joystick Trái (Tiến/Lùi), Joystick Phải (Rẽ).
* **Tốc độ**: L1 (Thường), L2 (Tối đa).
* **Thu bóng (Collector)**: L3 (Bật/Tắt), R1 (Quay thuận), R2 (Quay ngược).
* **Bắn bóng (Shooter)**: R3 (Bật/Tắt), Triangle (Bắn bóng trắng), Square (Bắn bóng đen).
* **Đóng/Mở cửa**: Pad Left (Cửa trắng), Pad Right (Cửa đen).
* **Trục xoay (Spin)**: Cross (X).

## 🧠 Phân loại bóng tự động
Cảm biến màu sắc TCS34725 tự động nhận diện bóng dựa trên màu sắc và độ sáng để điều khiển servo phân loại (kênh 5) đưa bóng vào đúng ngăn chứa.
