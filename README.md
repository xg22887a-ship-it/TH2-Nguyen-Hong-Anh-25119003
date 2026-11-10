# TH2 - ESP32: thời gian và deep sleep

## Bài 1 - So sánh cách tạo nhịp thời gian

- `vd1_dung_delay.cpp`: quan sát độ lệch nhịp của LED, ADC và heartbeat khi CPU bận. Kết luận: `delay()` chặn vòng lặp; tránh chờ lâu khi cần nhiều tác vụ đúng nhịp.
- `vd2_non_blocking.cpp`: quan sát các tác vụ chạy theo mốc `millis()` mà không chặn vòng lặp. Kết luận: cách này cho nhiều tác vụ hoạt động đồng thời mà không phải dừng chương trình để chờ.
- `vd3_timer_phan_cung.cpp`: quan sát bộ đếm mili giây được cập nhật bằng ngắt timer phần cứng. Kết luận: ISR nên làm việc ngắn; xử lý chính vẫn chạy trong `loop()`.

## Bài 2 - Deep sleep và nguyên nhân thức dậy

- `vd1_bien_giu_bien_mat.cpp`: `soLanThuc` đặt trong vùng RTC tăng qua mỗi lần thức dậy; `demThuong` trở về 0 sau deep sleep. Kết luận: deep sleep khởi động lại chương trình, chỉ dữ liệu ở vùng RTC được giữ.
- `vd2_nguyen_nhan_thuc.cpp`: ESP32 thức bằng timer sau 10 giây hoặc sớm hơn khi GPIO4 bị kéo xuống GND; LED GPIO2 nhấp nháy khi CPU đang chạy. Kết luận: sau deep sleep, `setup()` chạy lại và nguyên nhân thức dậy có thể được đọc bằng `esp_sleep_get_wakeup_cause()`.
