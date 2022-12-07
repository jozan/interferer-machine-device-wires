#ifndef __STATE_H_
#define __STATE_H_

#include <string>
#include "spark_wiring_tcpclient.h"
#include "spark_wiring_usbserial.h"

class State
{
public:
	State();

	std::string serializeState(int state[10]);
};
#endif /* __STATE_H_ */
