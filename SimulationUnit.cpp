#include "SimulationUnit.h"

Taxi::~Taxi()
{
	GoingRoadList.clear();
}

// SHOULD BE CHECKED
void Taxi::ActInGivenTimeInv(int nTimeLength, int nCurTime)
{
//  	if (TimeOffset > .1)
//  		throw "ERROR: a time offset of a taxi is invalid";
	
	if (GoingRoadList.empty())
		throw "ERROR: GoingRoadList of a taxi is empty";

	TimeOffset += nTimeLength;
	GoingRoad _GR = GoingRoadList.front();
	while (_GR.passing_time - TimeOffset < -0.01)
	{
		TimeOffset -= _GR.passing_time;
		GoingRoadList.pop_front();
		
		// update statistics
		if (!isOccupied)
		{
			CruisingLength += _GR.road.length;
			CruisingTime += _GR.passing_time;
		}
		else
		{
			OccupiedLength += _GR.road.length;
			OccupiedTime += _GR.passing_time;
		}
		
		// if the taxi is occupied and GoingRoadList is finished, 
		// which indicates the passenger was delivered to his/her destination,
		// isOccupied should turn to FALSE
		// in codes below, taxi would not pick-up another passenger in the destination road
		if (GoingRoadList.empty())
		{
			isOccupied = false;
			_ChoosePath(nCurTime);
		}

		// find passenger and update GoingRoadList
		if (!isOccupied)
			_FindAndPickPsngr(nCurTime);		
		
		_GR = GoingRoadList.front();
		RoadID = _GR.road.id;
		current_going_road_ = _GR;
	}

}

void Taxi::Initialization(int nCurTime)
{
	GoingRoadList.clear();
	GoingRoad tempGR;
	PathDecMkr.GetRondomRoad(tempGR, nCurTime);
	GoingRoadList.push_back(tempGR);
	current_going_road_ = tempGR;
	RoadID = tempGR.road.id;
}

void Taxi::_ChoosePath(int nCurTime)
{
	PathDecMkr.GetCruisingPath(GoingRoadList, current_going_road_, nCurTime);
}

void Taxi::_FindAndPickPsngr(int nCurTime)
{
	ODTuple od_tuple;
	if (PsngrFdr.FindPassenger(RoadID, nCurTime, od_tuple))
	{
		PassengerNum++;
		isOccupied = true;

		GoingRoadList.clear();

		PathDecMkr.GetOccupiedPath(GoingRoadList, od_tuple, current_going_road_, nCurTime);
	}
}

int Taxi::get_taxi_id()
{
	return TaxiID;
}

int Taxi::get_road_id()
{
	return RoadID;
}

bool Taxi::get_is_occupied()
{
	return isOccupied;
}

bool Taxi::get_is_stop()
{
	return isStop;
}

void Taxi::get_all_statistics(double &cruising_len, double &occupied_len, double &cruising_time, double &occupied_time, int &passenger_num)
{
	cruising_len = CruisingLength;
	occupied_len = OccupiedLength;
	cruising_time = CruisingTime;
	occupied_time = OccupiedTime;
	passenger_num = PassengerNum;
}

