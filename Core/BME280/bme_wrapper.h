#ifndef BME280_BME_WRAPPER_H_
#define BME280_BME_WRAPPER_H_

#include "bme280.h"
#include "bme280_defs.h"
#include "i2c.h"
#include "parser.h"

typedef struct
{
	float temperature;
	float humidity;
	float pressure;
}bme_results;

typedef struct
{
	uint8_t temperature;
	uint8_t humidity;
}bme_results_quantized;

uint8_t bme_init (I2C_HandleTypeDef *hid_dev);

uint8_t bme_get_data (bme_results* res);

uint8_t bme_get_data_quantized (bme_results_quantized* res);



#endif /* BME280_BME_WRAPPER_H_ */
