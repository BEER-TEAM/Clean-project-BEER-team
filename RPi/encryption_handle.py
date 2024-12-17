from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend
from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes
import base64
from enum import Enum, auto
import hashlib
import hmac
from keys_loader import KeysLoader

# Define an Enum for the encryption modes
class EncryptionMode(Enum):
    AES = auto()  # AES encryption
    XOR = auto()  # XOR encryption
    RAW = auto()  # No encryption

class EncryptionHandler:
    def __init__(self, key=None, mode=EncryptionMode.AES):
        """
        Initialize the EncryptionHandler class with the specified encryption mode.

        :param key: Encryption key for AES or XOR (optional for AES; generated if not provided).
        :param mode: Encryption mode, either EncryptionMode.AES, EncryptionMode.XOR or EncryptionMode.RAW.
        """
        self.mode = mode
        if self.mode == EncryptionMode.AES:
            keysLoader = KeysLoader()
            self.key_aes_len = 32
            self.key_aes = keysLoader.get_aes_key(self.key_aes_len)
            self.IV_len = 16
            self.IV = keysLoader.get_IV(self.IV_len)
            self.key_hmac_len = 32
            self.key_hmac = keysLoader.get_hmac_key(self.key_hmac_len)
            self.expected_tag_len = 16

        elif self.mode == EncryptionMode.XOR:
            keysLoader = KeysLoader()
            self.key_stream_len = 98
            self.key_stream = keysLoader.get_stream_key(self.key_stream_len)
        elif self.mode == EncryptionMode.RAW:
            # No encryption is performed for RAW mode
            self.key = None
        else:
            raise ValueError("Invalid mode. Choose EncryptionMode.AES, EncryptionMode.XOR, or EncryptionMode.RAW.")

    def encrypt(self, message):
        """
        Encrypt the message using the selected encryption method.

        :param message: The message to be encrypted (string).
        :return: Encrypted data in bytes or original message if RAW mode.
        """
        if self.mode == EncryptionMode.AES:
            return self._encrypt_aes(message)
        elif self.mode == EncryptionMode.XOR:
            return self._encrypt_xor(message)
        elif self.mode == EncryptionMode.RAW:
            return message # No encryption, return original message

    def decrypt(self, encrypted_data):
        """
        Decrypt the message using the selected encryption method.

        :param encrypted_data: The encrypted data in bytes.
        :return: Decrypted message as a string or original message if RAW mode.
        """
        if self.mode == EncryptionMode.AES:
            return self._decrypt_aes(encrypted_data)
        elif self.mode == EncryptionMode.XOR:
            return self._decrypt_xor(encrypted_data)
        elif self.mode == EncryptionMode.RAW:
            return encrypted_data  # No decryption, return original message

    def mac_compute(self, input_data):
        algo = hashlib.sha256
        key = self.key_hmac
        # custom_data = b""
        # custom_data = custom_data.decode('utf-8')
        expected_tag_len = self.expected_tag_len

        data_to_hash = input_data #+ custom_data
        
        # Utworzenie obiektu HMAC z wybranym algorytmem
        hmac_obj = hmac.new(key, data_to_hash, algo)

        # Obliczanie tagu (MAC)
        computed_tag = hmac_obj.digest()

        # Truncation do oczekiwanej długości tagu
        if expected_tag_len > len(computed_tag):
            raise ValueError("")

        truncated_tag = computed_tag[:expected_tag_len]

        return truncated_tag, len(truncated_tag)

    def _encrypt_aes(self, message):
        """Encrypt a message using AES encryption."""
        cipher = AES.new(self.key, AES.MODE_EAX)
        ciphertext, tag = cipher.encrypt_and_digest(message.encode())
        encrypted_data = cipher.nonce + tag + ciphertext
        return base64.b64encode(encrypted_data)  # Return as base64 encoded string for easier handling

    def _decrypt_aes(self, encrypted_data):
        """Decrypt a message using AES encryption."""
        encrypted_data = bytearray(encrypted_data, 'latin-1')

        if len(encrypted_data) < 33:
            print("Invalid data length.")
            ret = b""
            return ret.decode('latin-1')

        result = encrypted_data[1:-16]
        last_16_bytes = encrypted_data[-16:]

        computed_tag, computed_tag_len = self.mac_compute(result)

        if computed_tag != last_16_bytes:
            # print("MAC verification failed.")
            ret = b""
            return ret.decode('latin-1')

        ciphered_data = result
        key = self.key_aes
        iv = self.IV

        cipher = Cipher(algorithms.AES(key), modes.CBC(iv), backend=default_backend())

        decryptor = cipher.decryptor()

        decrypted_data = decryptor.update(ciphered_data) + decryptor.finalize()
        # print("Decrypted data: ", decrypted_data)
        return decrypted_data.decode('latin-1')

    def _encrypt_xor(self, message):
        """Encrypt a message using XOR encryption with a fixed 9-byte key."""
        encrypted = bytes([b ^ self.key[i % len(self.key)] for i, b in enumerate(message.encode())])
        return base64.b64encode(encrypted)  # Return as base64 encoded string

    def _decrypt_xor(self, encrypted_data):
        """Decrypt a message using XOR encryption with a fixed 9-byte key."""
        encrypted_data = bytearray(encrypted_data, 'latin-1')
        decrypted = bytes([b ^ self.key_stream[i % len(self.key_stream)] for i, b in enumerate(encrypted_data)])
        return decrypted.decode('latin-1')
    
    def get_encryption_mode(self):
        return self.mode
