from data_parser import DataParser
from influxdb_handle import InfluxDBHandler
from encryption_handle import EncryptionHandler, EncryptionMode
from radio_handle import RadioMode, RadioHandler
import sys

class DataHandler:
    nodeIdSequentialNumber = {}

    def __init__(self, trx_mode, encryption_mode, tx_callback=None):
        self.trx_mode = trx_mode
        self.data_parser = DataParser()
        self.tx_callback = tx_callback

        if trx_mode is None:
            print("Invalid radio mode. Please choose 'fsk', 'ook' or 'lora'.")
            sys.exit(1)

        if encryption_mode is None:
            print("Invalid encryption mode. Please choose 'aes', 'xor' or 'raw'.")
            sys.exit(1)

        self.encryption_handler = EncryptionHandler(mode=encryption_mode)

        # Initialize the InfluxDB handler and encryption handler
        try:
            self.influxdb_handler = InfluxDBHandler(
                host="localhost",
                port=8086,
                username="3dn@mjHHHN3mAX@Jsweetly",
                password="9Wg1RLJfj1S6HvEBzmYmjYwYXiRwBUwA",
                database="telemetry"
            )
        except Exception as e:
            print(f"An error occurred while initializing InfluxDB: {e}")
            sys.exit(1)

    def cleanup(self):
        self.influxdb_handler.close()

    def handle_data(self, raw_data):
        decrypted_data = self.encryption_handler.decrypt(raw_data)

        if len(decrypted_data) == 0:
            return

        nodeId, parsed_data = self.data_parser.parse_rx_data(decrypted_data)

        if self.encryption_handler.get_encryption_mode() == EncryptionMode.AES:
            if nodeId not in self.nodeIdSequentialNumber:
                self.nodeIdSequentialNumber[nodeId] = parsed_data['sequence_number']
            else:
                if self.nodeIdSequentialNumber[nodeId] > parsed_data['sequence_number']:
                    return
                else:
                    self.nodeIdSequentialNumber[nodeId] = parsed_data['sequence_number']

        self.influxdb_handler.write_data('environment', parsed_data)

        # Check if there's an alarm trigger message in parsed_data dictionary
        if 'smoke_sensor_status' not in parsed_data:
            return
        
        # Send an alarm, and open the door.
        if parsed_data['smoke_sensor_status'] == 'detected':
            response_code = 0xFF  # Alarm trigger code
            response_message = self.data_parser.parse_tx_data(response_code, DataParser.doorTask) # Create response message
            encrypted_response = self.encryption_handler.encrypt(response_message) # Encrypt the response
            self.tx_callback(encrypted_response) # Send the encrypted response
