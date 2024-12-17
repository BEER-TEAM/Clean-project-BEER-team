# Main File

import sys
from radio_handle import RadioMode, RadioHandler
from encryption_handle import EncryptionMode
from data_handler import DataHandler
import time

def main():
    if len(sys.argv) != 3:
        print("Usage: python main.py <mode> <encryption_mode>")
        print("Available modes: fsk, ook, lora")
        print("Available encryption modes: aes, xor, raw")
        sys.exit(1)

    RADIO_MODE = RadioMode.FSK

    ENCRYPTION_MODE = EncryptionMode.AES
    
    lastReceivedData = time.time()
    # Callback function to handle the received data
    def data_callback(raw_data, rssi=None, index=None):
        lastReceivedData = time.time()
        data_handler.handle_data(raw_data)

    # Initialize the DataHandler with the radio mode, encryption mode, and send callback
    data_handler = DataHandler(RADIO_MODE, ENCRYPTION_MODE)

    # Initialize the RadioHandler with the mode and callback function
    radio_handler = RadioHandler(RADIO_MODE, data_callback)

    try:
        while True:
            if lastReceivedData is not None and time.time() - lastReceivedData >= 30:
                lastReceivedData = None
                # print("No data received for 30 seconds.")
                radio_handler = RadioHandler(RADIO_MODE, data_callback)
            pass  # Main loop keeps running; callback handles received packets
    except KeyboardInterrupt:
        print(f"{RADIO_MODE} reception stopped.")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        radio_handler.cleanup()  # Clean up GPIO and close SPI
        data_handler.cleanup() # Close InfluxDB connection

if __name__ == "__main__":
    main()
