#include "eventGenerator.h"

eventGenerator::eventGenerator(usefulInfo* _I) :buffer(16)
{
	I = _I;
}

void eventGenerator::generateEvent()
{
	if (I->userSrc->isThereEvent())
	{
		buffer.push_back(I->userSrc->insertEvent());
	}
	if (I->softwareSrc->isThereEvent())
	{
		buffer.push_back(I->softwareSrc->insertEvent());
	}

	if (I->networkSrc->isThereEvent())
	{
		buffer.push_back(I->networkSrc->insertEvent());
	}

	if (I->timeoutSrc->isThereEvent())
	{
		buffer.push_back(I->timeoutSrc->insertEvent());
	}


}
genericEvent * eventGenerator::getNextEvent()
{
	genericEvent * ret;
	if (buffer.size() != 0)
	{
		ret = buffer.front();	//carga en ret el primer elemento del buffer
		buffer.pop_front();	//elimina el ultimo elemento del buffer
	}
	else
	{
		ret = nullptr;	//Si no hay eventos en el buffer devuelve nullptr
	}
	return ret;
}
