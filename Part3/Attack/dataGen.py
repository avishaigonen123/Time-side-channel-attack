import time
import serial
import datetime

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
    ser.write((f"S{N};").encode())

def signMessages(file, ser):
    print("Sending messages...")
    sendMessage(message, ser)
    numOfSigs = 0
    line = ""
    while True:
        c = ser.read(1)
        if c:
            line += c.decode()
            if line[-1:] == ";":
                break
        else:
            break
        if c.decode() == "S":
            numOfSigs+=1
            print(f"{numOfSigs} / {N}")
    sigDict = {}
    for sig in line[:-1].split("SIG")[1:]:
        r, s, time = sig[1:-1].split(",")
        if (r, s) not in sigDict:
            sigDict[(r, s)] = [time]
        else:
            sigDict[(r, s)] += [time]
    
    for rs, timeArray in sigDict.items():
        t = 0
        for time in timeArray:
            t += int(time)
        sigDict[rs] = t / len(timeArray)

    sorted_sigDict = dict(sorted(sigDict.items(), key=lambda item: item[1]))
    for rs, time in sorted_sigDict.items():
        file.write(f"{rs[0]} {rs[1]} {time:.3f}\n")

def getPubKey(file, ser):
    ser.write(("P").encode())
    time.sleep(0.001)
    line = ""
    while True:
        c = ser.read(1)
        if c:
            line += c.decode()
            if line[-1:] == ";":
                break
        else:
            break
    c = ser.read(1)
    x, y = line[4:-2].split(",")
    file.write(f"public_key: {x} {y}\n")



def main():
    # Open a serial port
    ser = None
    try:
        ser = serial.Serial(
            port='COM5',  # Replace with your port name
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
            getPubKey(file, ser)
            file.write("r  |  s  |  time[ms]\n")
            # Send messages and write response to file
            signMessages(file, ser)
            

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        if ser and ser.is_open:
            ser.close()
            print("Serial port closed.")


if __name__ == '__main__':
    main()