import struct
import json

# TLV Structure
class TLV:
    def __init__(self, tag, length, value):
        self.tag = tag
        self.length = length
        self.value = value
    
    def __str__(self):
        return f"TLV(Tag={self.tag}, Length={self.length}, Value={self.value[0]})"

# Message Structure
class Message:
    def __init__(self, nodeId, tlvs):
        self.prefix = DataParser.prefix
        self.nodeId = nodeId
        self.numberOfTLVs = len(tlvs)
        self.tlvs = tlvs
    
    def __str__(self):
        return f"Message(Node ID={self.nodeId}, TLVs={[str(tlv) for tlv in self.tlvs]})"




class DataParser:
    prefix = 0xDE
    temperature = 0x01
    humidity = 0x02
    doorStatus = 0x03
    doorTask = 0x04
    smokeSensorStatus = 0x05
    tempHumid = 0x02
    nodeNumber = 0x01  # node Temp and Humidity
    NODE_ID = (tempHumid << 5) | (nodeNumber & 0x1F)
    MESSAGE_LENGTH = 9
    sequenceNumber = 0x06

    # Const Message Values
    openDoor = 0xFF
    closeDoor = 0x00
    thereIsNoSmoke = 0x00
    thereIsSmoke = 0xFF

    def __init__(self):
        """Initialize the DataParser class."""
        print("DataParser initialized.")

    def create_tlv(self, tag, length, value):
        return TLV(tag, length, value)      
    
    def create_message(self, tlvs):
        return Message(self.NODE_ID, tlvs)

    def parse_rx_data(self, rx_data):
        """
        Parse the received data and return a dictionary of extracted values.
        :param rx_data: The received data to be parsed.
        :return: A dictionary of extracted values.
        """
        # Decode the decrypted data into a Message object
        message = self.bytes_to_message(rx_data)
        # Extract values from the message
        nodeId, extracted_values = self.extract_values_from_message(message)
        return nodeId, extracted_values
    
    def parse_tx_data(self, tx_data, type):
        """
        Parse the data to be transmitted and return a byte array.
        :param tx_data: The data to be transmitted.
        :param type: The type of data to be transmitted.
        :return: A byte array representing the message.
        """
        if type != self.doorTask:
            raise ValueError("Invalid data type for transmission. This node transmits only door tasks.")
        
        # Create TLVs for the door task
        door_task_tlv = self.create_tlv(self.doorTask, 1, self.encode_value(self.doorTask, tx_data))
        # Create a message with the door task TLV
        message = self.create_message([door_task_tlv])
        # Convert the message to a byte array as latin-1 string
        return bytes(self.message_to_bytes(message)).decode('latin-1')



    # TLV and Message Decoding from Byte Array
    def bytes_to_message(self, byte_array):
        """
        Decode the byte array into a Message object containing TLVs.
        :param byte_array: The decrypted byte array.
        :return: A Message object with TLVs.
        """
        
        # Check if byte_array is not string
        if not isinstance(byte_array, bytearray):
            byte_array = bytearray(byte_array, 'latin-1')

        if byte_array[0] != self.prefix:
            raise ValueError("Invalid prefix in message. Was: " + str(byte_array[0]))

        nodeId = byte_array[1]
        num_tlvs = byte_array[2]

        index = 3  # Start after prefix, nodeId, and num_tlvs
        tlvs = []

        for _ in range(num_tlvs):
            tag = byte_array[index]
            length = byte_array[index + 1]
            value = byte_array[index + 2 : index + 2 + length]  # Extract value
            tlvs.append(TLV(tag, length, value))
            index += 2 + length  # Move to the next TLV

        return Message(nodeId, tlvs)

    def message_to_bytes(self, msg):
        byte_array = bytearray()
        byte_array.append(msg.prefix)
        byte_array.append(msg.nodeId)
        byte_array.append(msg.numberOfTLVs)

        for tlv in msg.tlvs:
            byte_array.append(tlv.tag)
            byte_array.append(tlv.length)
            byte_array.extend(tlv.value)

        # Ensure the message has a fixed length (MESSAGE_LENGTH)
        while len(byte_array) < self.MESSAGE_LENGTH:
            byte_array.append(0)

        return byte_array

    # Helper Methods to Convert Between Tag Names and IDs
    def get_tag_name(self, tag):
        if tag == self.temperature:
            return "Temperature"
        elif tag == self.humidity:
            return "Humidity"
        elif tag == self.doorStatus:
            return "Door Status"
        elif tag == self.doorTask:
            return "Door Task"
        elif tag == self.smokeSensorStatus:
            return "Smoke Sensor Status"
        else:
            return "Unknown Tag"

    # Helper Methods to Encode and Decode Values
    def decode_value(self, tag, value):
        if tag == self.temperature:
            return self.dequantize_temperature(value[0])
        elif tag == self.humidity:
            return self.dequantize_humidity(value[0])
        elif tag == self.doorStatus:
            return "open" if value[0] == self.openDoor else "closed"
        elif tag == self.smokeSensorStatus:
            return 1 if value[0] == self.thereIsSmoke else 0
        elif tag == self.sequenceNumber:
            return int.from_bytes(value, byteorder='little')
        else:
            return value[0]

    def encode_value(self, tag, value):
        if tag == self.temperature:
            return [self.quantize_temperature(value)]
        elif tag == self.humidity:
            return [self.quantize_humidity(value)]
        elif tag == self.doorStatus:
            return [self.openDoor if value == "open" else self.closeDoor]
        elif tag == self.doorTask:
            return [value]
        elif tag == self.smokeSensorStatus:
            return [self.thereIsSmoke if value == 1 else self.thereIsNoSmoke]
        else:
            return [value]

    # Quantization Methods
    def quantize_temperature(self, temperature):
        temperature_quant = 0.1
        temperature_lower_limit = 20.0
        if temperature > 255 * temperature_quant + temperature_lower_limit:
            return 0xFF
        elif temperature < temperature_lower_limit:
            return 0x00
        else:
            return int((temperature - temperature_lower_limit) / temperature_quant)

    def dequantize_temperature(self, quantized_temperature):
        temperature_quant = 0.1
        temperature_lower_limit = 20.0
        return float(quantized_temperature) * temperature_quant + temperature_lower_limit

    def quantize_humidity(self, humidity):
        humidity_quant = 0.5
        if humidity > 100.0:
            return 0xFF
        elif humidity < 0.0:
            return 0x00
        else:
            return int(humidity / humidity_quant)

    def dequantize_humidity(self, quantized_humidity):
        humidity_quant = 0.5
        result = float(quantized_humidity) * humidity_quant
        return min(max(result, 0.0), 100.0)  # Bound between 0 and 100
    
    def extract_values_from_message(self, message):
        """
        Extract temperature and humidity from the Message object.
        :param message: The Message object containing TLVs.
        :return: A dictionary of extracted values.
        """
        extracted_values = {}

        for tlv in message.tlvs:
            if tlv.tag == self.temperature:
                extracted_values["temperature"] = self.decode_value(tlv.tag, tlv.value)
            elif tlv.tag == self.humidity:
                extracted_values["humidity"] = self.decode_value(tlv.tag, tlv.value)
            elif tlv.tag == self.doorStatus:
                extracted_values["door_status"] = self.decode_value(tlv.tag, tlv.value)
            elif tlv.tag == self.smokeSensorStatus:
                extracted_values["smoke_sensor_status"] = self.decode_value(tlv.tag, tlv.value)
            elif tlv.tag == self.sequenceNumber:
                extracted_values["sequence_number"] = self.decode_value(tlv.tag, tlv.value)
            # Add more elif conditions for other tags if needed

        return message.nodeId, extracted_values
