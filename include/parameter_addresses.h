/*
 * parameter_addresses — таблица адресов (VP) параметров устройства.
 * Содержит адреса угла станины, расстояния до упора и системных переменных.
 */

#ifndef PARAMETER_ADDRESSES_H
#define PARAMETER_ADDRESSES_H

#include <stdint.h>

namespace param_addr {

const uint16_t TABLE_ANGLE      = 0x5002;
const uint16_t DISTANCE_TO_STOP = 0x5004;
const uint16_t SPEED_RPM        = 0x6000;
const uint16_t REVS_PER_DEGREE  = 0x6002;

}

#endif
