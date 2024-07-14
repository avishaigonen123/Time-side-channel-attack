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
            # Send point and write response to file
            generatePoints(file, ser)
            

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if ser.is_open:
            ser.close()
            print("Serial port closed.")

# generate the points on the elliptic curve
def generateEllipticCurvePoints(a, b, n):
    res = []
    for x in range(n):
        for y in range(n):
            if((y*y) % n == (x**3 + x*a + b) % n):
                res.append((x,y))
    return res

def generatePoints(file, ser):
    print("Generating points...")
    curve_points = generateEllipticCurvePoints(2, 3, 97) # curve has all points on the curve
    
    print("Sending points...")
    for point in curve_points: # iterate over all points
        t = 0
        # send each point 5 times, and take average calculation time
        for _ in range(5): 
            t += sendPoint(point, ser) 
        t /= 5
        file.write(f"{point[0]} {point[1]} {t}\n") # write to the file the results
        print(f"Sent point: {point}")


if __name__ == '__main__':
    main()