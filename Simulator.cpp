#include "Simulator.h"

using namespace std;

SimulationControler::SimulationControler()
{
	srand((unsigned)time(NULL));

	str_ord_taxi_log__ = NULL;
	str_ev_taxi_log__ = NULL;
	str_chg_station_log__ = NULL;

	nOrdTaxies = 0;
	nEVTaxies = 0;
	nChargingStations = 0;

	time_duration__ = 0;
	simulation_times__ = 0;

	time_inv_len__ = 0;
	current_time__ = 0;
}

SimulationControler::~SimulationControler()
{
	if (str_ord_taxi_log__ != NULL)
	{
		str_ord_taxi_log__->close();
		delete str_ord_taxi_log__;
	}

	if (str_ev_taxi_log__ != NULL)
	{
		str_ev_taxi_log__->close();
		delete str_ev_taxi_log__;
	}

	if (str_chg_station_log__ != NULL)
	{
		str_chg_station_log__->close();
		delete str_chg_station_log__;
	}

	OrdTaxies.clear();
	EVTaxies.clear();
	ChargingStations.clear();
}

SimulationControler& SimulationControler::toNextTime()
{
	updateAllAgents();
	updateTime();
	outputLog();

	return *this;
}

SimulationControler& SimulationControler::toNextNTime(int n)
{
	if (n <= 0)
		throw "ERROR: simulator cannot simulate a scenario after in invalid time";

	for (int i = 0; i < n; i++)
	{
		toNextTime();
	}
	
	return *this;
}

void SimulationControler::initialization()
{
	time_duration__ = 0;
	simulation_times__ = 0;
	current_time__ = start_time__;

	// TODO: initialize all taxies' locations
	__InitializeOrdTaxies();
	__InitializeEVTaxies();
	__InitializeChargingStations();
}

void SimulationControler::updateTime()
{
	current_time__ += time_inv_len__;
	current_time__ %= DAY_LENGTH;
	simulation_times__++;
	time_duration__ += time_inv_len__;
}


void SimulationControler::updateAllAgents()
{
	updateOrdTaxies();
	updateEVTaxies();
	updateChargingStations();
}

void SimulationControler::updateOrdTaxies()
{
	std::vector<Taxi>::iterator _taxiIter = OrdTaxies.begin();
	std::vector<Taxi>::iterator _taxiIterEnd = OrdTaxies.end();
	while (_taxiIter != _taxiIterEnd)
	{
		//cout << _taxiIter->get_taxi_id() << ' ';
		_taxiIter->ActInGivenTimeInv(time_inv_len__, current_time__);
		_taxiIter++;
	}
}

// UNFINISHED
void SimulationControler::updateEVTaxies()
{

}

// UNFINISHED
void SimulationControler::updateChargingStations()
{

}

void SimulationControler::__InitializeOrdTaxies()
{
	std::vector<Taxi>::iterator _taxiIter = OrdTaxies.begin();
	std::vector<Taxi>::iterator _taxiIterEnd = OrdTaxies.end();
	while (_taxiIter != _taxiIterEnd)
	{
		_taxiIter->Initialization(current_time__);
		_taxiIter++;
	}
}


// UNFINISHED
void SimulationControler::__InitializeEVTaxies()
{

}

// UNFINISHED
void SimulationControler::__InitializeChargingStations()
{

}

void SimulationControler::setOrdTaxiLogStrm(const char* sLogFileName)
{
	if (str_ord_taxi_log__ != NULL)
	{
		str_ord_taxi_log__->close();
		delete str_ord_taxi_log__;
		str_ord_taxi_log__ = NULL;
	}

	str_ord_taxi_log__ = new std::ofstream(sLogFileName, std::ios::out);
	if (str_ord_taxi_log__->fail())
	{
		str_ord_taxi_log__->clear();
		delete str_ord_taxi_log__;
		throw "FAIL: Open Log file of Ordinary taxi failed!";
	}

	*str_ord_taxi_log__ << "Time, TaxiID, RoadId, isOccupied, isStop\n";
}

void SimulationControler::setEVTaxiLogStrm(const char* sLogFileName)
{
	if (str_ev_taxi_log__ != NULL)
	{
		str_ev_taxi_log__->close();
		delete str_ev_taxi_log__;
		str_ev_taxi_log__ = NULL;
	}

	str_ev_taxi_log__ = new std::ofstream(sLogFileName, std::ios::out);
	if (str_ev_taxi_log__->fail())
	{
		str_ev_taxi_log__->clear();
		delete str_ev_taxi_log__;
		throw "FAIL: Open Log file of EVTaxi failed!";
	}

	*str_ev_taxi_log__ << "Time, TaxiID, RoadId, isOccupied, isStop, isCharging, isWaiting, BatteryPecentage(%), BatteryConsumingRate(%/km)\n";
}

void SimulationControler::setChargingStationsLogStrm(const char* sLogFileName)
{
	if (str_chg_station_log__ != NULL)
	{
		str_chg_station_log__->close();
		delete str_chg_station_log__;
		str_chg_station_log__ = NULL;
	}

	str_chg_station_log__ = new std::ofstream(sLogFileName, std::ios::out);
	if (str_chg_station_log__->fail())
	{
		str_chg_station_log__->clear();
		delete str_chg_station_log__;
		throw "FAIL: Open Log file of charging stations failed!";
	}

	*str_chg_station_log__ << "Time, ChargingStationId, RoadId(fixed), plogNum, chargingTaxiNum, WaitingTaxiNum\n";
}


void SimulationControler::set_start_time(int sec_since_start_of_day)
{
	if (sec_since_start_of_day > 24 * 60 * 60 || sec_since_start_of_day < 0)
		throw "ERROR in Simulator: start time is invalid";
	start_time__ = sec_since_start_of_day;
	current_time__ = start_time__;
}

void SimulationControler::set_time_inv_length(int time_inv_len)
{
	if (time_inv_len < 0 || time_inv_len > 100)
		throw "ERROR in Simulator: time_inv_num is invalid or too large for simulation";
	time_inv_len__ = time_inv_len;
}

void SimulationControler::set_road_condition_time_inv_num(int time_inv_num)
{
	RoadCdtn.set_time_inv_num(time_inv_num);
}

void SimulationControler::LoadRoadConnetion(const char* filename)
{
	RoadCdtn.LoadRoadConnectionFromFile(filename);
}

void SimulationControler::LoadCruisingPassingTime(const char* filename)
{
	RoadCdtn.LoadRoadCruinsingPassingTimeFromFile(filename);
}

void SimulationControler::LoadODTuples(const char* filename)
{
	PsngrDis.LoadODTuplesFromFile(filename);
}

void SimulationControler::LoadPickupProbabilites(const char* filename)
{
	PsngrDis.LoadProDisFromFile(filename);
}

void SimulationControler::addOrdTaxies(int n)
{
	for (int i = 0; i < n; i++) 
	{
		OrdTaxies.push_back(Taxi(nOrdTaxies ++, &RoadCdtn, &PathExp, &PsngrDis));
	}
}

void SimulationControler::addEVTaxies(int n, double BattaryConsumingRate)
{
	for (int i = 0; i < n; i++)
		EVTaxies.push_back(EVTaxi(nEVTaxies ++, BattaryConsumingRate, &RoadCdtn, &PathExp, &PsngrDis));
}

void SimulationControler::addCharingStations(int n)
{
	for (int i = 0; i < n; i++)
		ChargingStations.push_back(ChargingStation(nChargingStations ++));
}


void SimulationControler::outputOrdTaxiLog()
{
	vector<Taxi>::iterator iter_taxi_vec = OrdTaxies.begin();
	vector<Taxi>::iterator iter_taxi_vec_end = OrdTaxies.end();
	
	while (iter_taxi_vec != iter_taxi_vec_end)
	{
		*str_ord_taxi_log__
			<< current_time__ << ","
			<< iter_taxi_vec->get_taxi_id() << ","
			<< iter_taxi_vec->get_road_id() << ","
			<< (iter_taxi_vec->get_is_occupied() ? 1 : 0) << ","
			<< (iter_taxi_vec->get_is_stop() ? 1 : 0) << "\n";

		iter_taxi_vec++;
	}
}

// UNFINISHED
void SimulationControler::outputEVTaxiLog()
{

}

// UNFINISHED
void SimulationControler::outputChargingStationLog()
{

}

void SimulationControler::outputLog()
{
	outputOrdTaxiLog();
	outputEVTaxiLog();
	outputChargingStationLog();
}

void SimulationControler::OutputFullStatisticsOrdTaxi(const char* output_filiename)
{
	ofstream out_str(output_filiename);
	if (out_str.fail() == true)
		throw "ERROR in SimulationControler: cannot open log file for outputting ordinary taxies statistics";

	// header
	out_str << "taxi_id, cruising_length, occupied_length, cruising_time, occupied_time, passenger_num\n";

	vector<Taxi>::iterator iter_taxi_vec = OrdTaxies.begin();
	vector<Taxi>::iterator iter_taxi_vec_end = OrdTaxies.end();

	while (iter_taxi_vec != iter_taxi_vec_end)
	{
		double cruising_len, occupied_len, cruising_time, occupied_time;
		int passenger_num;

		iter_taxi_vec->get_all_statistics(cruising_len, occupied_len, cruising_time, occupied_time, passenger_num);

		out_str
			<< iter_taxi_vec->get_taxi_id() << ","
			<< setprecision(12) << cruising_len << ","
			<< setprecision(12) << occupied_len << ","
			<< setprecision(12) << cruising_time << ","
			<< setprecision(12) << occupied_time << ","
			<< passenger_num << "\n";


		iter_taxi_vec++;
	}

	out_str.close();
}
