from enum import Enum

# Communication Flags
FLAGS_ACK = 0x80
FLAGS_REQ_ACK = 0x40
BROADCAST_ADDRESS = 255

# Register Addresses
REG_00_FIFO = 0x00
REG_01_OP_MODE = 0x01
REG_06_FRF_MSB = 0x06
REG_07_FRF_MID = 0x07
REG_08_FRF_LSB = 0x08
REG_0E_FIFO_TX_BASE_ADDR = 0x0e
REG_0F_FIFO_RX_BASE_ADDR = 0x0f
REG_10_FIFO_RX_CURRENT_ADDR = 0x10
REG_12_IRQ_FLAGS = 0x12
REG_13_RX_NB_BYTES = 0x13
REG_1D_MODEM_CONFIG1 = 0x1d
REG_1E_MODEM_CONFIG2 = 0x1e
REG_19_PKT_SNR_VALUE = 0x19
REG_1A_PKT_RSSI_VALUE = 0x1a
REG_20_PREAMBLE_MSB = 0x20
REG_21_PREAMBLE_LSB = 0x21
REG_22_PAYLOAD_LENGTH = 0x22
REG_26_MODEM_CONFIG3 = 0x26
REG_4D_PA_DAC = 0x4d
REG_40_DIO_MAPPING1 = 0x40
REG_41_DIO_MAPPING2 = 0x41
REG_0D_FIFO_ADDR_PTR = 0x0d

# PA Configurations
PA_DAC_ENABLE = 0x07
PA_DAC_DISABLE = 0x04
PA_SELECT = 0x80

# IRQ Masks and Flags
CAD_DETECTED_MASK = 0x01
RX_DONE = 0x40
TX_DONE = 0x08
CAD_DONE = 0x04
CAD_DETECTED = 0x01
RX_TIMEOUT = 0x80

# Operation Modes
LONG_RANGE_MODE_OFF = 0x00
MODE_SLEEP = 0x00
MODE_STDBY = 0x01
MODE_TX = 0x03
MODE_RXCONTINUOUS = 0x05
MODE_CAD = 0x07

# PA Config Register
REG_09_PA_CONFIG = 0x09

# Frequency Settings
FXOSC = 32000000.0
FSTEP = (FXOSC / 524288)

# FSK Specific Settings
RF_OPMODE_MASK = 0xF8
RFLR_OPMODE_LONGRANGEMODE_MASK = 0x7F

REG_FDEVMSB = 0x04
REG_FDEVLSB = 0x05
REG_BITRATEMSB = 0x02
REG_BITRATELSB = 0x03
REG_PREAMBLEMSB = 0x25
REG_PREAMBLELSB = 0x26
REG_PACKETCONFIG1 = 0x30
RF_PACKETCONFIG1_CRC_MASK = 0xEF
RF_PACKETCONFIG1_PACKETFORMAT_MASK = 0x7F
RF_PACKETCONFIG1_PACKETFORMAT_FIXED = 0x00
RF_PACKETCONFIG1_PACKETFORMAT_VARIABLE = 0x80

# FSK Parameters
FSK_POWER = 0
FSK_FDEV = 25000
FSK_BANDWIDTH = 50000
FSK_DATARATE = 50000
FSK_PREAMBLE_LENGTH = 5
FSK_FIX_LENGTH_PAYLOAD_ON = 0
FSK_CRC_ON = 1
FSK_IqInverted = 0
FSK_TX_TIMEOUT = 3000

# Frequency Calculation Constants
FREQ_STEP = 61.03515625
XTAL_FREQ = 32000000

# FSK Bandwidth and AFC Configurations
FSK_AFC_BANDWIDTH = 83333
FSK_AFC_BANDWIDTH_REG_VALUE = 0x06
FSK_BANDWIDTH_REG_VALUE = 0x07

# Register Configurations
REG_RXBW = 0x12
REG_AFCBW = 0x13
REG_PAYLOADLENGTH = 0x32

# DIO Mapping Configurations
RF_DIOMAPPING1_DIO0_MASK = 0x3f
RF_DIOMAPPING1_DIO1_MASK = 0xCF
RF_DIOMAPPING1_DIO2_MASK = 0xF3
RF_DIOMAPPING1_DIO1_01 = 0x10

RF_DIOMAPPING2_DIO4_MASK = 0x3F
RF_DIOMAPPING2_MAP_MASK = 0xFE

# FSK Initial Values for Registers
REG_FSK_LNA_INIT_VAL = 0x23
REG_FSK_RXCONFIG_INIT_VAL = 0x1E
REG_FSK_RSSICONFIG_INIT_VAL = 0xD2
REG_FSK_AFCFEI_INIT_VAL = 0x01
REG_FSK_PREAMBLEDETECT_INIT_VAL = 0xAA
REG_FSK_OSC_INIT_VAL = 0x07
REG_FSK_SYNCCONFIG_INIT_VAL = 0x12
REG_FSK_SYNCVALUE1_INIT_VAL = 0xC1
REG_FSK_SYNCVALUE2_INIT_VAL = 0x94
REG_FSK_SYNCVALUE3_INIT_VAL = 0xC1
REG_FSK_PACKETCONFIG1_INIT_VAL = 0xD8
REG_FSK_FIFOTHRESH_INIT_VAL = 0x8F
REG_FSK_IMAGECAL_INIT_VAL = 0x02
REG_FSK_DIOMAPPING1_INIT_VAL = 0x00
REG_FSK_DIOMAPPING2_INIT_VAL = 0x30

# FSK Registers
REG_FSK_LNA = 0x0C
REG_FSK_RXCONFIG = 0x0D
REG_FSK_RSSICONFIG = 0x0E
REG_FSK_AFCFEI = 0x1A
REG_FSK_PREAMBLEDETECT = 0x1F
REG_FSK_OSC = 0x24
REG_FSK_SYNCCONFIG = 0x27
REG_FSK_SYNCVALUE1 = 0x28
REG_FSK_SYNCVALUE2 = 0x29
REG_FSK_SYNCVALUE3 = 0x2A
REG_FSK_PACKETCONFIG1 = 0x30
REG_FSK_FIFOTHRESH = 0x35
REG_FSK_IMAGECAL = 0x3B
REG_FSK_DIOMAPPING1 = 0x40
REG_FSK_DIOMAPPING2 = 0x41

# RX Configuration Registers and Masks
REG_RXCONFIG = 0x0D
RF_RXCONFIG_AFCAUTO_ON = 0x10
RF_RXCONFIG_AGCAUTO_ON = 0x08
RF_RXCONFIG_RXTRIGER_PREAMBLEDETECT = 0x06

REG_FSK_RSSIVALUE = 0x11
RF_RXCONFIG_RESTARTRXWITHOUTPLLLOCK = 0x40