#include "DecisionMaker.h"

void PathDecisionMaker::GetRondomRoad(GoingRoad &going_road, int nCurTime)
{
	// IMPROVEMENT: 根据出租车时空分布选取随机点 或 依据预先设定好的初始位置
	going_road = p_road_condition_->GetRandomGoingRoad();
	going_road.passing_time = p_road_condition_->GetRoadCruisingPassingTime(going_road.road.id, nCurTime);
}

void PathDecisionMaker::GetCruisingPath(std::list<GoingRoad> &going_road_list, GoingRoad current_gr, int nCurTime)
{
	if (!p_taxi_exp->GetCruisingPath(current_gr, going_road_list, nCurTime))	// if missing data, choose a connected road randomly
	{
		GoingRoad gr = p_road_condition_->GetRandomNextRoad(current_gr);
		gr.passing_time = p_road_condition_->GetRoadCruisingPassingTime(gr.road.id, nCurTime);
		going_road_list.push_back(gr);
	}
}

void PathDecisionMaker::GetOccupiedPath(std::list<GoingRoad> &going_road_list, ODTuple &od_tuple, GoingRoad &current_gr, int nCurTime)
{
	// IMPROVEMENT:
// 	if (p_taxi_exp->GetOccupiedPath(current_gr, od_tuple, going_road_list, nCurTime) != true)
// 	{
// 		throw "ERROR in PathDecisionMaker: Cannot find corresponding path to given od tuple";
// 	}

	GoingRoad gr_end;
	gr_end.passing_time = od_tuple.time_duration;

	Road path_road = p_road_condition_->get_road(od_tuple.road_id_d);
	path_road.length = od_tuple.path_length;

	gr_end.road = path_road;

	going_road_list.push_back(current_gr);
	going_road_list.push_back(gr_end);
}

bool PassengerFinder::FindPassenger(int road_id, int current_time, ODTuple &od_tuple)
{
	return p_passenger_dis_->FindPassenger(road_id, current_time, od_tuple);
}
