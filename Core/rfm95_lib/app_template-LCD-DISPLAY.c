/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Ping-Pong implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include <string.h>
#include "board.h"
#include "radio.h"
#include "app_template.h"

#include "bme_wrapper.h"
#include "main.h"

#include "parser.h"
#include "LCD.h"
#include "ciphering_manager.h"


  const uint8_t aes_key[] =
{
    0xd3, 0x83, 0xd5, 0xf3, 0x20, 0xea, 0x74, 0xec, 0x3d, 0x35, 0xfa, 0xf2, 0x26, 0x63, 0x31, 0xbd,
    0xa0, 0x21, 0x48, 0x48, 0xc1, 0x88, 0x00, 0xbc, 0xc2, 0x26, 0xc9, 0x12, 0x6d, 0x39, 0x1f, 0xdc
};

  const uint8_t hmac_key[] =
  {
      0x84, 0x45, 0xd7, 0xcd, 0x68, 0x59, 0xb8, 0x28, 0xde, 0xfb, 0xaa, 0xac, 0x41, 0x9b, 0xc6, 0xe0,
      0x1e, 0x72, 0x77, 0x17, 0xbc, 0xfc, 0x54, 0xbc, 0x63, 0x0a, 0x11, 0x7c, 0x8a, 0xaf, 0x15, 0xb6
  };

  const uint8_t IV[] =
  {
	  0x44, 0x9f, 0x1e, 0xbe, 0xbc, 0xe6, 0xf1, 0x67, 0x07, 0x49, 0x0a, 0x9f, 0x40, 0x8a, 0xe4, 0x80,
  };

																						  	
#define RF_FREQUENCY                                868000000 // Hz
#define TX_OUTPUT_POWER                             0         // dBm


#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false



#define FSK_FDEV                                    25e3      // Hz
#define FSK_DATARATE                                50e3      // bps
#define FSK_BANDWIDTH                               50e3      // Hz
#define FSK_AFC_BANDWIDTH                           83.333e3  // Hz
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   false

#define TIMEOUT_WATCHDOG_CTR						60*1000

typedef enum
{
    LOWPOWER,
    RX,
    RX_TIMEOUT,
    RX_ERROR,
    TX,
    TX_TIMEOUT,
}States_t;

#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 65

States_t State = LOWPOWER;

volatile int8_t RssiValue = 0;
volatile int8_t SnrValue = 0;


uint16_t BufferSize = BUFFER_SIZE;
uint8_t Buffer[BUFFER_SIZE];

typedef struct {
	int rxdone;
	int rxtimeout;
	int rxerror;
	int txdone;
	int txtimeout;
} trx_events_cnt_t;

trx_events_cnt_t trx_events_cnt;

int rx_cnt = 0;
int txdone_cnt = 0;


/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );



void init_radio()
{
	OPERATING_MODE oper_mode = oper_mode_global;
   BoardInitMcu( );
	BoardInitPeriph( );

	// Radio initialization
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;

	Radio.Init( &RadioEvents );

	Radio.SetChannel( RF_FREQUENCY );

if(oper_mode==AES_HMAC_LORA || oper_mode==WEAK_STREAM_LORA || oper_mode==LACK_SECURITY_LORA)
{



	Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
								   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
								   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
								   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

	    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
	                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
	                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
	                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

	}

	else if(oper_mode==AES_HMAC_FSK || oper_mode==WEAK_STREAM_FSK || oper_mode==LACK_SECURITY_FSK)
	{

	    Radio.SetTxConfig(  MODEM_FSK,						/* Radio modem to be used [0: FSK, 1: LoRa] */
	    					TX_OUTPUT_POWER,				/* Sets the output power [dBm] */
							FSK_FDEV,						/* Sets the frequency deviation (FSK only) [Hz] */
							0,								/* Sets the bandwidth (LoRa only); 0 for FSK */
	                        FSK_DATARATE, 					/* Sets the Datarate. FSK: 600..300000 bits/s */
							0,								/* Sets the coding rate (LoRa only) FSK: N/A ( set to 0 ) */
	                        FSK_PREAMBLE_LENGTH,			/* Sets the preamble length. FSK: Number of bytes */
							FSK_FIX_LENGTH_PAYLOAD_ON,		/* Fixed length packets [0: variable, 1: fixed] */
							true,							/* Enables disables the CRC [0: OFF, 1: ON] */
							0,								/* Enables disables the intra-packet frequency hopping. FSK: N/A ( set to 0 ) */
							0,								/* Number of symbols bewteen each hop. FSK: N/A ( set to 0 ) */
							0,								/* Inverts IQ signals (LoRa only). FSK: N/A ( set to 0 ) */
							3000							/* Transmission timeout [ms] */
		);

	    Radio.SetRxConfig(  MODEM_FSK,						/* Radio modem to be used [0: FSK, 1: LoRa] */
	    					FSK_BANDWIDTH,					/* Sets the bandwidth. FSK: >= 2600 and <= 250000 Hz. (CAUTION: This is "single side bandwidth") */
							FSK_DATARATE,					/* Sets the Datarate. FSK: 600..300000 bits/s */
							0,								/* Sets the coding rate (LoRa only) FSK: N/A ( set to 0 ) */
							FSK_AFC_BANDWIDTH,				/* Sets the AFC Bandwidth (FSK only). FSK: >= 2600 and <= 250000 Hz */
							FSK_PREAMBLE_LENGTH,			/* Sets the Preamble length. FSK: Number of bytes */
							0,								/* Sets the RxSingle timeout value (LoRa only). FSK: N/A ( set to 0 ) */
							FSK_FIX_LENGTH_PAYLOAD_ON,		/* Fixed length packets [0: variable, 1: fixed] */
							0,								/* Sets payload length when fixed lenght is used. */
							true,							/* Enables/Disables the CRC [0: OFF, 1: ON] */
	                        0,								/* Enables disables the intra-packet frequency hopping. FSK: N/A ( set to 0 ) */
							0,								/* Number of symbols bewteen each hop. FSK: N/A ( set to 0 ) */
							false,							/* Inverts IQ signals (LoRa only). FSK: N/A ( set to 0 ) */
							true							/* Sets the reception in continuous mode. [false: single mode, true: continuous mode] */
		);

	}

	else
	{
		HardFault_Handler();
	}

}

/**
 * Main application entry point.
 */
void app_main( OPERATING_MODE oper_mode )
{
    // Target board initialisation

	oper_mode_global=oper_mode;

	init_radio();
    

if(oper_mode==AES_HMAC_FSK || oper_mode==AES_HMAC_LORA ||oper_mode==AES_HMAC_OOK)
{
	CipheringManagerStatus status = ciphering_init();

	if(CIPHERING_OK == status)
	{
		uint16_t siema =0;
		uint16_t siema2 =0;

	}
}


while(1)
	{
	rx_loop(oper_mode);
	}

    while(1)
    {
    	printf("Infinite loop. This should never happen!\r\n");
    }
}


void tx_loop(OPERATING_MODE oper_mode)
{
	uint8_t txtab[256];
	bme_results_quantized results;



	uint8_t txt[] = "abc";
	uint8_t txt2[] = "sent\n";
	while(1)
	{

		bme_get_data_quantized(&results);

		TLV temperatureTLV = createTLV(temperature_tag, 1, &results.temperature);
		TLV humidityTLV = createTLV(humidity_tag, 1, &results.temperature);

		TLV tlvs2[2] = {temperatureTLV,humidityTLV};
		Message environmentkMsg = createMessage(NODE_ID, tlvs2, 2);

		uint8_t bytes[32];

		memset(bytes,0,sizeof(bytes));

		//actual length of the message - without zeros at the end
		uint8_t length = 0;
		messageToBytes(environmentkMsg, bytes, &length);
		//but in scenario we have to send 9 bytes - const length of the key

		uint8_t ciphered_buffer[256];

		memset(ciphered_buffer,0,sizeof(ciphered_buffer));

		size_t ciphered_size;

		size_t data_to_cipher_size;

		if(length<16)
		{
			data_to_cipher_size=16;
		}
		else
		{
			data_to_cipher_size=length;
		}


		uint8_t res = encrypt_and_hmac(bytes, data_to_cipher_size, aes_key, sizeof(aes_key), ciphered_buffer, sizeof(ciphered_buffer),&ciphered_size, IV, sizeof(IV),  hmac_key, sizeof(hmac_key));

		Radio.Send( ciphered_buffer, ciphered_size );

		  for(int i =0; i<=ciphered_size;i++)
		  {
			  printf("0X%x, ",ciphered_buffer[i]);
		  }
		  printf("\n\r");

		    if(res ==CIPHERING_OK)
		    {
		    	printf("Plaintext: ");
				  for(int i =0; i<=length;i++)
				  {
					  printf("0X%x, ",bytes[i]);
				  }
				  printf("\n\r");

		    }


		HAL_Delay(100);
	}

}


void rx_loop(OPERATING_MODE oper_mode)
{
	char buf[50];
	int loop_cnt = 0;

	printf("\r\n\r\nRX loop start\r\n");
	int time_on_air;
	int payload_size = BUFFER_SIZE;
	time_on_air = Radio.TimeOnAir(MODEM_FSK, payload_size);
	printf("Time on air: %d us for payload_size: %d bytes\r\n", time_on_air, payload_size);

	Radio.Rx(0);

	struct lcd_disp disp;
	disp.I2Chandler = &hi2c3;
	disp.addr = (0x27 << 1);
	disp.bl = 1;
	lcd_init(&disp);
	lcd_clear(&disp);

	sprintf((char *)disp.line_1, "               ");
	sprintf((char *)disp.line_2, "               ");
	sprintf((char *)disp.line_3, "               ");
	sprintf((char *)disp.line_4, "               ");

	uint8_t previous_hmac_part =0x00;

	uint8_t previous_packet[BUFFER_SIZE];

	memset(previous_packet,0,sizeof(previous_packet));

	uint32_t prev_rxdone =0;
	uint32_t previoustime=HAL_GetTick();

	uint32_t sequential_number_temp_sensor = 0;
	uint32_t allow_disp_temp_sensor = 1;

	uint32_t sequential_number_smoke_detector = 0;
	uint32_t allow_disp_smoke_detector = 1;

	while(1)
	{
	    DelayMs(25);

		snprintf(buf, sizeof(buf), "%d %d %d %d %d ", RssiValue, trx_events_cnt.rxdone, trx_events_cnt.rxerror, trx_events_cnt.rxtimeout, loop_cnt);

		if (State == RX_TIMEOUT)
		{
			Radio.Rx(0);
			State = RX;
		}
		printf("%s\r\n", buf);

		if (State == RX_ERROR)
		{
			init_radio();
		    Radio.Rx(0);
			State = RX;
		}

		if(trx_events_cnt.rxdone!=prev_rxdone)
		{
			previoustime=HAL_GetTick();
			prev_rxdone = trx_events_cnt.rxdone;
		}
		if(HAL_GetTick()-previoustime > TIMEOUT_WATCHDOG_CTR)
		{
			init_radio();
		    Radio.Rx(0);
			State = RX;
			previoustime = HAL_GetTick();
		}

		printf("STATE: %d\n\r",State);


		float temp;
		float humid;

		//z buffera robimy decipher
		uint8_t decrypted_data[256];


		uint8_t buffer_position_diff_len =16;

		if(memcmp(previous_packet,Buffer,buffer_position_diff_len))
		{
			memcpy(previous_packet,Buffer,buffer_position_diff_len);

			CipheringManagerStatus status;


			status = decrypt_and_hmac(Buffer, sizeof(Buffer),
									aes_key,sizeof(aes_key),
									IV,sizeof(IV),
									hmac_key,sizeof(hmac_key),
									decrypted_data, sizeof(decrypted_data));




			if(status==CIPHERING_OK)
			{
				Message rx_msg = bytesToMessage(decrypted_data);
				if(tempHumid_node_type==getNodeType(&rx_msg))
				{
					for(int i =0 ; i<rx_msg.numberOfTLVs;i++)
					{
						if((sequential_number_tag==rx_msg.tlv[i].tag) && (oper_mode==AES_HMAC_FSK || oper_mode==AES_HMAC_LORA ||oper_mode==AES_HMAC_OOK) )
						{
							uint32_t temp_value;
							memcpy(&temp_value, rx_msg.tlv[i].value, rx_msg.tlv[i].length);
							if(sequential_number_temp_sensor<temp_value)
							{
								sequential_number_temp_sensor=temp_value;
								allow_disp_temp_sensor = 1;
								printf("\t\t\t\t seq_num: %d\r\n",sequential_number_temp_sensor);
							}
							else
							{
								allow_disp_temp_sensor=0;
								break;
							}
						}
						else if(temperature_tag==rx_msg.tlv[i].tag)
						{
							temp = dequantizeTemperature(rx_msg.tlv[i].value[0]);
							sprintf((char *)disp.line_1, "T=%4.2f",temp);
						}
						else if(humidity_tag==rx_msg.tlv[i].tag)
						{
							humid = dequantizeHumidity(rx_msg.tlv[i].value[0]);
							sprintf((char *)disp.line_2, "H=%4.2f",humid);
						}


					}
					if(allow_disp_temp_sensor)
					{
						lcd_display(&disp);
					}

				}
				else if(fireAlarm_node_type==getNodeType(&rx_msg))
					{
						for(int i =0 ; i<rx_msg.numberOfTLVs;i++)
						{
							if((sequential_number_tag==rx_msg.tlv[i].tag) && (oper_mode==AES_HMAC_FSK || oper_mode==AES_HMAC_LORA ||oper_mode==AES_HMAC_OOK) )
							{
								uint32_t temp_value;
								memcpy(&temp_value, rx_msg.tlv[i].value, rx_msg.tlv[i].length);
								if(sequential_number_smoke_detector<temp_value)
								{
									sequential_number_smoke_detector=temp_value;
									allow_disp_smoke_detector = 1;
								}
								else
								{
									allow_disp_smoke_detector=0;
									break;
								}
							}
							else if(smokeSensorStatus_tag==rx_msg.tlv[i].tag)
							{
								if(thereIsSmoke_msg_value==rx_msg.tlv[i].value[0])
								{
									sprintf((char *)disp.line_3, "SMK = DET");
								}
								else
								{
									sprintf((char *)disp.line_3, "SMK = NOT DET");
								}
							}

						}
						if(allow_disp_smoke_detector)
						{
							lcd_display(&disp);
						}
					}

			}

		}

		loop_cnt++;
	}

}

void OnTxDone( void )
{
    Radio.Sleep( );
    State = TX;
    trx_events_cnt.txdone++;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    BufferSize = size;
    memcpy( Buffer, payload, BufferSize );
    RssiValue = rssi;
    SnrValue = snr;
    State = RX;
    trx_events_cnt.rxdone++;
    //Radio.Rx(0);
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    State = TX_TIMEOUT;
    trx_events_cnt.txtimeout++;
}

void OnRxTimeout( void )
{
    State = RX_TIMEOUT;
    trx_events_cnt.rxtimeout++;
}

void OnRxError( void )
{
    State = RX_ERROR;
    trx_events_cnt.rxerror++;
	init_radio();
    Radio.Rx(0);
}
