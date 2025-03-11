/*
 * config.h
 *
 *  Created on: Mar 3, 2025
 *      Author: arigu
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#define SD_CARD_LOGGING
//#define SWO_LOGGING

#if defined(SD_CARD_LOGGING) && defined(SWO_LOGGING)
  #error Only one can be defined
#elif !defined(SD_CARD_LOGGING) && !defined(SWO_LOGGING)
  #error Define a Logging interface
#endif

#endif /* INC_CONFIG_H_ */
