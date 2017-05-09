
#include <ctime>
#define ONE_MINUTE 60

////////////////////////////// timer borrador ejemplo //////////////////////////////

bool timer_borrador_ejemplo ()
{
	clock_t tInicial;
	tInicial = clock();

	bool ret = false;
	while (!ret) //Esto seria el while gigante. Habria que ver bien. Lo mejor seria que funcione el timer de boost
	{			//probar poll_one
		if ((clock() - tInicial) > ONE_MINUTE * CLOCKS_PER_SEC)
		{
			//hacer lo que queramos
			ret = true;
			break;
		}
	}
	return ret;
}



/////// timer de boost que no compila por los bind
///////////////clase
class TimeoutEventSource : public genericEventSource
{
public:

	TimeoutEventSource() :t(ioForTimer, boost::posix_time::minutes(1)), timeout(false) {} //constructor, setea el timer
																						  //TimeoutEventSource();
	bool isThereEvent();
	void startTimer();
	void stopTimer();
	genericEvent* insertEvent();

	boost::function<void(const boost::system::error_code&, boost::asio::deadline_timer*)> HANDLER123;

private:
	boost::asio::io_service ioForTimer;
	boost::asio::deadline_timer t;
	bool timeout;
	void setTimeout(const boost::system::error_code& );
	void handler(const boost::system::error_code&, boost::asio::deadline_timer* t); //prototipo del handler
};

class SoftwareEventSource : public genericEventSource
{
public:
	SoftwareEventSource();
	~SoftwareEventSource();
	bool isThereEvent();
	//loadSoftwareEvent(ev); //TODO: agregar una funcion para cargar eventos de software
};




////////////////////////////////////////////////////////

/*****  TIMEOUTS EVENT SOURCE  *****/

//NO TOCAR LOS DE TIMEOUT, CREO QUE YA ESTAN LISTOS

bool TimeoutEventSource::isThereEvent()
{
	ioForTimer.poll();			//Si se cumple el tiempo del timer, se ejecuta el handler correspondiente.

	bool ret = timeout;			//Para que luego devuelva true si se cumplio timeout y false en caso contrario.

	if (timeout)
	{
		evCode = TIMEOUT;
		//timeout = false;		//vuelvo a poner en false para que espere al siguiente timeout.
		startTimer();			//Se reinicia el timer.
	}
	else
	{
		evCode = NO_EV;
	}
	return ret;					//Se devuelve si hubo timeout o no.
}

void TimeoutEventSource::setTimeout(const boost::system::error_code& /*e*/) //VER DE USARLA
{
	//timeout = true;			//Set timeout modifica una variable de control que indica si ocurrio un timeout
}

//constructor de prueba para el bind
/*
TimeoutEventSource::TimeoutEventSource()
{
HANDLER123=(boost::bind(&TimeoutEventSource::handler, this,
boost::asio::placeholders::error, &t));
}
*/



void TimeoutEventSource::startTimer()
{
	timeout = false;			//Variable de control indicando que no ocurrio un timeout.

								//boost::asio::deadline_timer t(ioForTimer, boost::posix_time::seconds(60)); 

	boost::function<void(const boost::system::error_code&, boost::asio::deadline_timer*)> HANDLER123( //le saque lo segundo
		boost::bind(&TimeoutEventSource::handler, this,
			boost::asio::placeholders::error, &t)); //VER

	t.async_wait(boost::bind(HANDLER123, &t));

	//t.async_wait(boost::bind(HANDLER123,boost::asio::placeholders::error, &t));
}

void TimeoutEventSource::stopTimer()
{
	//timer.cancel();	//Se cancela el timer. No anda esto. ESTOY PROBANDO CON OTRAS
}


//////////ver
void TimeoutEventSource::handler(const boost::system::error_code&, boost::asio::deadline_timer* t)  //PRUEBA TIMER (le agrego 2 params para que repita)
{
	timeout = true;				//Se indica que ocurrió un timeout.
}
/////////////

genericEvent * TimeoutEventSource::insertEvent()
{
	genericEvent * ret;
	//Hago un switch solo para mantener la estructura del metodo en las otras clases
	switch (evCode)
	{
	case TIMEOUT:
		ret = (genericEvent *) new EV_Timeout;
		break;
	default:
		break;
	}
	return ret;
}

