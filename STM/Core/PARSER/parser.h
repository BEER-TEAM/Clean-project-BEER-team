#ifndef BEER_PARSER_H
#define BEER_PARSER_H

#include <stdio.h>
#include <stdint.h>

static const uint8_t prefix = 0xDE;

//NODE TYPES
static const uint8_t fireAlarm_node_type = 0x01;
static const uint8_t tempHumid_node_type = 0x02;
static const uint8_t doorLock_node_type = 0x03;
static const uint8_t doorPanel_node_type = 0x04;

//NODE ID!!!!
static const uint8_t nodeNumber = 0x01;

#define NODE_ID ((doorPanel_node_type << 5) | (nodeNumber & 0x1F))// for lcd panel
//#define NODE_ID ((fireAlarm_node_type << 5) | (nodeNumber & 0x1F))// for smoke detector
//#define NODE_ID ((tempHumid_node_type << 5) | (nodeNumber & 0x1F))// for env sensor
//#define NODE_ID ((doorLock_node_type << 5) | (nodeNumber & 0x1F))// for door lock

//TAGS
static const uint8_t temperature_tag = 0x01;
static const uint8_t humidity_tag = 0x02;
static const uint8_t doorStatus_tag = 0x03;
static const uint8_t doorTask_tag = 0x04;
static const uint8_t smokeSensorStatus_tag = 0x05;
static const uint8_t sequential_number_tag = 0x06;

//static const MESSAGE VALUES
static const uint8_t openDoor_msg_value = 0xFF;
static const uint8_t closeDoor_msg_value = 0x00;
static const uint8_t thereIsNoSmoke_msg_value = 0x00;
static const uint8_t thereIsSmoke_msg_value = 0xFF;

typedef struct
{
    uint8_t tag;
    uint8_t length;
    uint8_t value[10];
}TLV;

typedef struct
{
    uint8_t prefix;
    uint8_t nodeId;
    uint8_t numberOfTLVs;
    TLV tlv[10];
}Message;

//static const MESSAGE LENGTH IN OUR SCENARIO IS 9
#define MESSAGE_LENGTH 9




// Function to build a TLV structure
TLV createTLV(uint8_t tag, uint8_t length, const uint8_t *value);

uint8_t getNodeType(const Message* msg);
// Function to build a message
Message createMessage(uint8_t nodeId, TLV *tlvs, uint8_t numberOfTLVs);

void messageToBytes(Message msg, uint8_t* bytes,uint8_t* length);

Message bytesToMessage(const uint8_t* bytes);


static const float temperatureLowerLimit = 20.0f;
static const float temperatureQuant = 0.1f;
static const float temperatureUpperLimit = 255*temperatureQuant+temperatureLowerLimit;

uint8_t quantizeTemperature(float temperature);
float dequantizeTemperature(uint8_t quantizedTemperature);

static const float humidityLowerLimit = 0.0;
static const float humidityUpperLimit = 100.0;
static const float humidityQuant = 0.5f;


uint8_t quantizeHumidity(float humidity);
float dequantizeHumidity(uint8_t quantizedHumidity);

#endif /* BEER_PARSER_H */
