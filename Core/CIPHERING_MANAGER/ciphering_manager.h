/*
 * ciphering_manager.h
 *
 *  Created on: Sep 22, 2024
 *      Author: gangm
 */

#ifndef CIPHERING_MANAGER_CIPHERING_MANAGER_H_
#define CIPHERING_MANAGER_CIPHERING_MANAGER_H_

#include "stdint.h"
#include "cmox_crypto.h"

#define CIPHERING_HMAC_LEN 16
#define CIPHERING_CHUNK_SIZE  48u


static const cmox_cbc_handle_t Cbc_Ctx;

typedef enum
{
	CIPHERING_OK =0,
	CIPHERING_FAILED =1,
	CIPHERING_WRONG_AUTHENTICATION =2,
	CIPHERING_WRONG_DECRYPTION =3

}CipheringManagerStatus;

typedef enum
{
	ENCRYPT_THEN_HMAC_AES_NO_1 =1
}CipheringManagerMode;

typedef uint8_t msg_size_type;

CipheringManagerStatus ciphering_init();

CipheringManagerStatus stream_cipher_decipher(const uint8_t* data_to_cipher,uint32_t data_len,const uint8_t* key,uint32_t key_len, uint8_t* ciphered_buffer, uint32_t ciphered_buffer_len);

CipheringManagerStatus encrypt_and_hmac(const uint8_t* data_to_cipher,uint32_t data_len,const uint8_t* key,uint32_t key_len,
		uint8_t* ciphered_buffer, uint32_t ciphered_buffer_len,size_t * ciphered_size,
		const uint8_t *IV,uint32_t IV_len,
		const uint8_t *HMAC_key,uint32_t HMAC_key_len);


CipheringManagerStatus decrypt_and_hmac(const uint8_t* data_to_verify_and_decipher, uint32_t buffer_max_len,
						const uint8_t* key,uint32_t key_len,
						const uint8_t *IV,uint32_t IV_len,
						const uint8_t *HMAC_key,uint32_t HMAC_key_len,
						uint8_t * decrypted_data,uint32_t decrypted_data_buffer_len);

#endif /* CIPHERING_MANAGER_CIPHERING_MANAGER_H_ */
