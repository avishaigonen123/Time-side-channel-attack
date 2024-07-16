import time
import serial
import datetime


####
# TODO: look at this file, remove unneccasry staff, change func name, etc
####

# just so we can manage files more easily
key = "0xBA"
message = "Hello world!"
# TODO: remove this, this shouldn't exist
hash = 1234
N = 1000 # number of messages to send

# Function to generate points and write to file
a = 2
b = 3
p = 193939

def timeStr():
    return datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

def sendMessage(message, ser):
<<<<<<< HEAD
    ser.write((f"S").encode())
=======
    ser.write(("S").encode())
    time.sleep(0.001)
>>>>>>> a4cf2c1a625dcf3d0309c9e191bf883ca3fc59a4
    while ser.in_waiting == 0:
        time.sleep(0.000001)  # Small delay to avoid busy-waiting
    ans = ser.readline().decode()
    return ans

def signMessages(file, ser):
    print("Sending messages...")
    for _ in range(N):
        content = sendMessage(message, ser) 
<<<<<<< HEAD
        r, s, time = content.split(" ")
=======
        r, s, time = content.strip().split(" ")
        print(f"{r} {s} {time}")
>>>>>>> a4cf2c1a625dcf3d0309c9e191bf883ca3fc59a4
        file.write(f"{r} {s} {time}\n") # write to the file the results


def main():
    # Open a serial port
    try:
        ser = serial.Serial(
            port='COM6',  # Replace with your port name
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
        with open(f"data/{a}_{b}_{p}_{key}.txt", "w") as file:
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