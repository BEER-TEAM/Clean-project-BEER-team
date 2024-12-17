/*
 * bme_wrapper.c
 *
 *  Created on: Sep 16, 2024
 *      Author: gangm
 */

//#ifndef BME280_BME_WRAPPER_C_
//#define BME280_BME_WRAPPER_C_

#include <bme_wrapper.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

I2C_HandleTypeDef *_hid_dev;

struct bme280_dev device;
struct bme280_data device_data;

int8_t I2C_comms_wrie (uint8_t dev_id, uint8_t reg_addr,
		uint8_t *data, uint16_t len)
{
	  int8_t *buf;
	  buf = malloc(len +1);
	  buf[0] = reg_addr;
	  memcpy(buf +1, data, len);

	  if(HAL_I2C_Master_Transmit(_hid_dev, (dev_id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK) return -1;

	  free(buf);
	  return 0;
}
int8_t I2C_comms_read (uint8_t dev_id, uint8_t reg_addr,
		uint8_t *data, uint16_t len)
{
	  if(HAL_I2C_Master_Transmit(_hid_dev, (dev_id << 1), &reg_addr, 1, 10) != HAL_OK) return -1;
	  if(HAL_I2C_Master_Receive(_hid_dev, (dev_id << 1) | 0x01, data, len, 10) != HAL_OK) return -1;

	  return 0;
}




uint8_t bme_init (I2C_HandleTypeDef *hid_dev)
{
	_hid_dev = hid_dev;
	device.dev_id = BME280_I2C_ADDR_PRIM;
	device.intf=BME280_I2C_INTF;
	device.delay_ms=HAL_Delay;
	device.write = I2C_comms_wrie;
	device.read = I2C_comms_read;
	uint8_t rslt = bme280_init(&device);
	if(rslt != BME280_OK)
	{
		for(;;){}
	}

	device.settings.osr_h = BME280_OVERSAMPLING_1X;
	device.settings.osr_p = BME280_OVERSAMPLING_16X;
	device.settings.osr_t = BME280_OVERSAMPLING_2X;
	device.settings.filter = BME280_FILTER_COEFF_16;
	rslt = bme280_set_sensor_settings(BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL, &device);

	return 0;

}

uint8_t bme_get_data (bme_results* res)
{
	uint8_t rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &device);

	HAL_Delay(40);
	rslt = bme280_get_sensor_data(BME280_ALL, &device_data, &device);

	res->temperature = device_data.temperature / 100.0;      /* °C  */
	res->humidity = device_data.humidity / 1024.0;           /* %   */
	res->pressure = device_data.pressure / 10000.0;          /* hPa */


	return rslt;
}

uint8_t bme_get_data_quantized (bme_results_quantized* res)
{
	bme_results rslt;
	uint8_t ret =  bme_get_data (&rslt);

	res->temperature= quantizeTemperature(rslt.temperature);
	res->humidity = quantizeHumidity(rslt.humidity);

	return ret;

}


//#endif /* BME280_BME_WRAPPER_C_ */
