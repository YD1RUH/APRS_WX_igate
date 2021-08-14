import serial   # install dari "python -m pip3 install pyserial"
import aprslib  # install dari "python -m pip3 install aprslib"
import os       # sudah tersedia di python

os.system("cls")    # membersihkan layar utama
AIS1 = aprslib.IS("YD1RUH-4", host="YOUR SERVER IP", passwd="20156", port=14580) # ID ke server pertama
AIS2 = aprslib.IS("YD1RUH-5", passwd="20156", port=14580) # ID ke server kedua
AIS1.connect() # connect ke server pertama
AIS2.connect() # connect ke server kedua
ser = serial.Serial('COM11') # membaca port serial contoh COM3, COM4, COM14 tergantung yang terdeteksi pada komputer
ser.flushInput()

while True:
    ser_bytes = ser.readline()
    decoded_bytes = (ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
    paket = str(decoded_bytes) # mengubah output baca serial COM menjadi string
    AIS1.sendall(paket) # mengirimkan informasi ke server pertama
    AIS2.sendall(paket) # mengirimkan informasi ke server kedua
    print(paket) # menampilkan paket yang dikirimkan hasil dari membaca port COM

# jika program error, saat mencoba terhubung, itu karena aprs.fi
# terkadang untuk terhubung ke aprs.fi harus beberapa kali mengulang
# biasanya mengulan bisa hingga 4 - 5 kali
