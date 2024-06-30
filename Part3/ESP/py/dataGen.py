import time
import serial
import datetime

def timeStr():
    return datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

def sendPoint(point, ser):
    ser.write((f"{point[0]} {point[1]}\n").encode())
    while ser.in_waiting == 0:
        time.sleep(0.00001)  # Small delay to avoid busy-waiting
    ans1 = ser.readline().decode()
    ans = ans1.strip()
    return int(ans)

def main():
    # Open a serial port
    try:
        ser = serial.Serial(
            port='COM5',  # Replace with your port name
            baudrate=115200,
            timeout=1
        )
        print("waiting for reboot...")
        time.sleep(0.5)
        print("done.")
        # Check if the serial port is open
        if ser.is_open:
            print("Serial port is open.")

        # Open file for writing
        with open(f"data/data_{timeStr()}.txt", "w") as file:
            file.write("X cord  |  Y cord  |  time[ms]\n")
            generatePoints(file, ser)
            # Send point and write response to file
            

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if ser.is_open:
            ser.close()
            print("Serial port closed.")
def generateEllipticCurve(a, b, n):
    res = []
    for x in range(n):
        for y in range(n):
            if((y*y) % n == (x**3 + x*a + b) % n):
                res.append((x,y))
    return res

def generatePoints(file, ser):
    curve = generateEllipticCurve(2, 3, 97)
    for i in range(len(curve)):
        point = curve[i]
        t = 0
        for _ in range(5):
            t += sendPoint(point, ser)
        t /= 5
        file.write(f"{point[0]} {point[1]} {t}\n")
        print(f"Sent point: {point}")

main()