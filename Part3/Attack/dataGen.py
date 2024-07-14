import time
import serial
import datetime


# just so we can manage files more easily
key = "0x89"
isItRegular = "S"
message = "Hello world!"
N = 1000 # number of messages to send

# Function to generate points and write to file
a = 2
b = 3
p = 193939

def timeStr():
    return datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

def sendMessage(message, ser):
    ser.write((f"{message}\n").encode())
    while ser.in_waiting == 0:
        time.sleep(0.000001)  # Small delay to avoid busy-waiting
    ans1 = ser.readline().decode()
    ans = ans1.strip()
    return ans

'''
def generateEllipticCurvePoints(a, b, n, lim):
    # Precompute y^2 % n
    y_squares = {((y*y) % n): y for y in range(n)}
    
    res = []
    for x in range(n):
        temp = (x**3 + x*a + b) % n
        if temp in y_squares:
            res.append((x, y_squares[temp]))
            if len(res) == lim:
                return res
    return res
'''

def signMessages(file, ser):
    print("Sending messages...")
    for _ in range(N):
        content = sendMessage(message, ser) 
        r, s, time = content.split(", ")
        file.write(f"{r}, {s}, {time}\n") # write to the file the results


def main():
    # Open a serial port
    try:
        ser = serial.Serial(
            port='COM3',  # Replace with your port name
            baudrate=115200,
            timeout=1
        )
        # Check if the serial port is open
        if ser.is_open:
            print("Serial port is open.")

        print("waiting for reboot...")
        time.sleep(0.5)
        print("done.")

        # Open file for writing
        with open(f"data/{isItRegular}_{a}_{b}_{p}_{key}_{timeStr()}.txt", "w") as file:
            file.write("r  |  s  |  time[ms]\n")
            # Send messages and write response to file
            signMessages(file, ser)
            

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if ser.is_open:
            ser.close()
            print("Serial port closed.")


if __name__ == '__main__':
    main()