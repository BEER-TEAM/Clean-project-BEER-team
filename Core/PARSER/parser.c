#include "parser.h"
// Function to build a TLV structure
TLV createTLV(uint8_t tag, uint8_t length, const uint8_t *value) {
    TLV tlv;
    tlv.tag = tag;
    tlv.length = length;
    memcpy(tlv.value, value, length);  // Copy value to the TLV struct
    return tlv;
}

// Function to build a message
Message createMessage(uint8_t nodeId, TLV *tlvs, uint8_t numberOfTLVs) {
    Message msg;
    msg.prefix = prefix;
    msg.nodeId = nodeId;
    msg.numberOfTLVs = numberOfTLVs;

    for (uint8_t i = 0; i < numberOfTLVs; i++) {
        msg.tlv[i] = tlvs[i];
    }

    return msg;
}

void messageToBytes(Message msg, uint8_t* bytes,uint8_t* length) {
    uint8_t index = 0;
    bytes[index++] = msg.prefix;
    bytes[index++] = msg.nodeId;
    bytes[index++] = msg.numberOfTLVs;

    for (uint8_t i = 0; i < msg.numberOfTLVs; i++) {
        bytes[index++] = msg.tlv[i].tag;
        bytes[index++] = msg.tlv[i].length;
        for (uint8_t j = 0; j < msg.tlv[i].length; j++) {
            bytes[index++] = msg.tlv[i].value[j];
        }
    }
    *length = index;
}

Message bytesToMessage(const uint8_t *bytes)
{
    Message msg;
    uint8_t index = 0;
    msg.prefix = bytes[index++];
    msg.nodeId = bytes[index++];
    msg.numberOfTLVs = bytes[index++];

    for (uint8_t i = 0; i < msg.numberOfTLVs; i++) {
        msg.tlv[i].tag = bytes[index++];
        msg.tlv[i].length = bytes[index++];
        for (uint8_t j = 0; j < msg.tlv[i].length; j++) {
            msg.tlv[i].value[j] = bytes[index++];
        }
    }

    return msg;
}

uint8_t quantizeTemperature(float temperature)
{
    if (temperature > temperatureUpperLimit)
    {
        return 0xFF;
    }
    else if (temperature < temperatureLowerLimit)
    {
        return 0x00;
    }
    else
    {
        //return (uint8_t)((temperature - temperatureLowerLimit) / (temperatureUpperLimit - temperatureLowerLimit) * 255);
    	return (uint8_t)((temperature- temperatureLowerLimit)/temperatureQuant);
    }
}

float dequantizeTemperature(uint8_t quantizedTemperature)
{
    return (float) ((float)quantizedTemperature*temperatureQuant)+ temperatureLowerLimit;
}

uint8_t quantizeHumidity(float humidity)
{
    if (humidity > humidityUpperLimit)
    {
        return 0xFF;
    }
    else if (humidity < humidityLowerLimit)
    {
        return 0x00;
    }
    else
    {
        return (uint8_t)(humidity - humidityLowerLimit) / (humidityQuant);
    }
}

float dequantizeHumidity(uint8_t quantizedHumidity)
{
	float res = (float)quantizedHumidity *humidityQuant;
	if(res<=0)
	{
		return 0;
	}
	if(res>=100)
	{
		return 100;
	}

    return res;
}

uint8_t getNodeType(const Message* msg)
{
	//#define NODE_ID ((doorPanel_node_type << 5) | (nodeNumber & 0x1F))
	return msg->nodeId>>5;
}

// int main() {
//     //Example message oper door task
//     uint8_t doorTaskValue[] = {openDoor};
//     TLV doorTaskTLV = createTLV(doorTask, 1, doorTaskValue);
//     TLV tlvs2[] = {doorTaskTLV};
//     Message doorTaskMsg = createMessage(NODE_ID, tlvs2, 1);

//     uint8_t bytes[MESSAGE_LENGTH];
//     //actual length of the message - without zeros at the end
//     uint8_t length = 0;
//     messageToBytes(doorTaskMsg, bytes, &length);
//     //but in scenario we have to send 9 bytes - const length of the key




//     //EXAMPLE IDENTYFIYING NODE
//     uint8_t receivedBytes[MESSAGE_LENGTH] = {0xDE, 0x22, 0x01, 0x03, 0x01, 0xFF};
//     Message receivedMsg = bytesToMessage(receivedBytes);

//     if (receivedMsg.nodeId == NODE_ID) {
//         printf("This is our node\n");

//         for (uint8_t i = 0; i < receivedMsg.numberOfTLVs; i++)
//         {
//             if (receivedMsg.tlv[i].tag == doorTask)
//             {
//                 printf("Door task: %d\n", receivedMsg.tlv[i].value[0]);
//             }
//         }

//     } else {
//         printf("This is not our node\n");
//     }

//     return 0;
// }
