# RFID Project

Komponen-komponen

1. ESP32
2. RC522
3. OLED Screen
4. Buzzer
5. Solenoid pintu
6. Touch button

## Catatan

- Ga nyambung ke internet
- (Future Implementation) Data RFID tersimpan di SD Card

## Logika Alat

1.  (INIT) Alat nyala, nampilin logo robotic buat booting

### Orang datang dari luar

1. Masuk standby
2. Kalo nempel, terus dia berhasil buka pintu, buzzer bunyi 2 kali, oled tampil berhasil (akses diterima)
3. kalo nempel, terus dia gagal, buzzer bunyi lama, oled tampil gagal
4. loop terus

### Orang dari dalam ke luar

1. Touch sensor di tekan
2. Pintu terbuka, delay 3 detik
3. Pintu tertutup
