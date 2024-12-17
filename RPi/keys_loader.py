import hashlib
from cryptography.fernet import Fernet
import os
from dotenv import load_dotenv
import base64
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
class KeysLoader:
    def __init__(self):
        self.key_aes = None
        self.IV = None
        self.key_hmac = None
        self.key_stream = None

        load_dotenv()

        encrypted_aes_key = os.getenv('1')
        encrypted_IV = os.getenv('2')
        encrypted_hmac_key = os.getenv('3')
        encrypted_stream_key = os.getenv('4')

        password = input("Give password: ")

        self.aes_key = self.decrypt_data(encrypted_aes_key, password)
        self.IV = self.decrypt_data(encrypted_IV, password)
        self.hmac_key = self.decrypt_data(encrypted_hmac_key, password)
        self.stream_key = self.decrypt_data(encrypted_stream_key, password)

    def decrypt_data(self, encrypted_data, password):
        try:
            fernet = Fernet(password)
            decrypted_data = fernet.decrypt(encrypted_data.encode())
            return decrypted_data  # Zwracamy dane w formacie bytes
        except Exception:
            return b''
        
    def get_aes_key(self, suposed_len):
        if suposed_len == len(self.aes_key):
            return self.aes_key
        else:
            return self.random_bytes(suposed_len)
        
    def get_IV(self, suposed_len):
        if suposed_len == len(self.IV):
            return self.IV
        else:
            return self.random_bytes(suposed_len)
        
    def get_hmac_key(self, suposed_len):
        if suposed_len == len(self.hmac_key):
            return self.hmac_key
        else:
            return self.random_bytes(suposed_len)
        
    def get_stream_key(self, suposed_len):
        if suposed_len == len(self.stream_key):
            return self.stream_key
        else:
            return self.random_bytes(suposed_len)
        
    def random_bytes(self, suposed_len):
        return os.urandom(suposed_len)
