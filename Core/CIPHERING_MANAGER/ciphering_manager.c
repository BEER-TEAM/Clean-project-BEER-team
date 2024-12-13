/*
 * ciphering_manager.c
 *
 *  Created on: Sep 22, 2024
 *      Author: gangm
 */
#include "ciphering_manager.h"


CipheringManagerStatus ciphering_init()
{
	if (cmox_initialize(NULL) != CMOX_INIT_SUCCESS)
	{
	  return CIPHERING_FAILED;
	}
	return CIPHERING_OK;
}



CipheringManagerStatus stream_cipher_decipher(const uint8_t* data_to_cipher, uint32_t data_len, const uint8_t* key,
						uint32_t key_len, uint8_t* ciphered_buffer, uint32_t ciphered_buffer_len)
{
	if(data_len>ciphered_buffer)
	{
		return CIPHERING_FAILED;
	}
	else if(data_len>ciphered_buffer_len)
	{
		return CIPHERING_FAILED;
	}


	for (uint32_t i=0; i<data_len;i++)
	{
		ciphered_buffer[i]=data_to_cipher[i] ^ key[i];
	}
	return CIPHERING_OK;
}


CipheringManagerStatus encrypt_and_hmac(const uint8_t* data_to_cipher,uint32_t data_len,const uint8_t* key,uint32_t key_len,
		uint8_t* ciphered_buffer, uint32_t ciphered_buffer_len,size_t * ciphered_size,
		const uint8_t *IV,uint32_t IV_len,
		const uint8_t *HMAC_key,uint32_t HMAC_key_len)
{

	  cmox_cipher_retval_t retval;
	  /* General cipher context */

	  msg_size_type fianl_msg_len_as_header=0;



	  retval = cmox_cipher_encrypt(CMOX_AES_CBC_ENC_ALGO,                  /* Use AES CBC algorithm */
								  data_to_cipher, data_len,           /* Plaintext to encrypt */
								  key, key_len,                       /* AES key to use */
								  IV, IV_len,                         /* Initialization vector */
								  ciphered_buffer+sizeof(fianl_msg_len_as_header), ciphered_size);   /* Data buffer to receive generated ciphertext */

//do zaszyfrowanej wiadomości dodajemy hmaca

	  fianl_msg_len_as_header+=(*ciphered_size);

	  if(CMOX_CIPHER_SUCCESS!=retval)
	  {
		  return CIPHERING_FAILED;
	  }

	  uint8_t Computed_Tag[CIPHERING_HMAC_LEN];
	  size_t computed_size;

	  cmox_mac_retval_t retval2;
	  retval2 = cmox_mac_compute(CMOX_HMAC_SHA256_ALGO,     /* Use HMAC SHA256 algorithm */
			  ciphered_buffer+sizeof(fianl_msg_len_as_header), *ciphered_size,  /* Message to authenticate */
			  	  HMAC_key, HMAC_key_len,          /* HMAC Key to use */
			  	  NULL, 0,                   /* Custom data */
				  ciphered_buffer+sizeof(fianl_msg_len_as_header)+*ciphered_size,              /* Data buffer to receive generated authnetication tag */
	                             sizeof(Computed_Tag),      /* Expected authentication tag size */
	                             &computed_size);           /* Generated tag size */

	  if(CMOX_MAC_SUCCESS!=retval2)
	  {
		  return CIPHERING_FAILED;
	  }

	  fianl_msg_len_as_header+=computed_size;
	  //concatenate and produce final PDU

	  ciphered_buffer[0]=fianl_msg_len_as_header;

	  *ciphered_size=fianl_msg_len_as_header;


	  return CIPHERING_OK;
}



//const uint8_t* data_to_cipher,uint32_t data_len,
//		uint8_t* ciphered_buffer, uint32_t ciphered_buffer_len,size_t * ciphered_size,
//		const uint8_t *IV,uint32_t IV_len,
//		const uint8_t *HMAC_key,uint32_t HMAC_key_len


CipheringManagerStatus decrypt_and_hmac(const uint8_t* data_to_verify_and_decipher, uint32_t buffer_max_len,
						const uint8_t* key,uint32_t key_len,
						const uint8_t *IV,uint32_t IV_len,
						const uint8_t *HMAC_key,uint32_t HMAC_key_len,
						uint8_t * decrypted_data,uint32_t decrypted_data_buffer_len)
{

	msg_size_type size = *(msg_size_type*)data_to_verify_and_decipher;

	if((uint32_t)size>buffer_max_len)
	{
		return CIPHERING_FAILED;
	}

	cmox_mac_retval_t retval = cmox_mac_verify(CMOX_HMAC_SHA256_ALGO,     /* Use HMAC SHA256 algorithm */
			data_to_verify_and_decipher+sizeof(size), size-CIPHERING_HMAC_LEN,  /* Message to authenticate */
			HMAC_key, HMAC_key_len,          /* HMAC Key to use */
	                         NULL, 0,                   /* Custom data */
							 data_to_verify_and_decipher+sizeof(size)+size-CIPHERING_HMAC_LEN,              /* Authentication tag */
							 CIPHERING_HMAC_LEN);     /* tag size */
	if(CMOX_MAC_AUTH_SUCCESS!=retval)
	{
		return CIPHERING_WRONG_AUTHENTICATION;
	}

	cmox_cipher_retval_t retval2 = cmox_cipher_decrypt(CMOX_AES_CBC_DEC_ALGO,
											 data_to_verify_and_decipher+sizeof(size),
											 size-CIPHERING_HMAC_LEN,
											 key, key_len,                       /* AES key to use */
											 IV, IV_len,                         /* Initialization vector */
											 decrypted_data,
	                                         NULL);
	if(CMOX_CIPHER_SUCCESS!=retval2)
	{
		return CIPHERING_WRONG_DECRYPTION;
	}

	return CIPHERING_OK;
}


