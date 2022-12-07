#include <string>
#include "State.h"
#include "Particle.h"

State::State()
{
}

std::string State::serializeState(int state[10])
{
	Serial.println("State: creating serialized state");
	std::string d;

	d.append("[");

	for (int i = 0; i < 10; i += 2)
	{
		if (i != 0)
			d.append(",");

		d.append(std::to_string(i / 2));
		d.append(",");
		d.append(std::to_string(state[i + 1]));
	}

	d.append("]");

	Serial.print("State: state:");
	Serial.println(d.c_str());

	return d;
}
