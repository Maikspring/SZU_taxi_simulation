#pragma once

#include "CommonUnit.h"

class PathDecisionMaker {
public:
	PathDecisionMaker(
		RoadCondition *_pRoadConnection,
		TaxiPathExperience *_pTaxiPathExp) {
		if (_pRoadConnection == NULL || _pTaxiPathExp == NULL)
			throw "cannot set RoadConnection or TaxiPathExperience NULL to PathDecisionMaker";
		p_road_condition_ = _pRoadConnection;
		p_taxi_exp = _pTaxiPathExp;
	}
	~PathDecisionMaker() {

	}

	// IMPROVEMENT: 根据时空分布选择初始随机位置 或 预先设定好初始位置
	void GetRondomRoad(GoingRoad &going_road,
						int nCurTime); // 依据给定的时间，按照出租车分布，随机分配到路段中
	void GetCruisingPath(std::list<GoingRoad> &going_road_list,
						GoingRoad current_gr,
						int nCurTime);
	void GetOccupiedPath(std::list<GoingRoad> &going_road_list,
						ODTuple &od_tuple,
						GoingRoad &current_gr,
						int nCurTime);

private:
	// 路网拓扑网络
	RoadCondition* p_road_condition_;
	// 路网行驶经验
	TaxiPathExperience* p_taxi_exp;
};


class PassengerFinder {
public:
	PassengerFinder(PassengersDistribution *_pPsngrDis) {
		if (_pPsngrDis == NULL)
			throw "ERROR: cannot set PassengerDistribution NULL to PassengerFinder";
		p_passenger_dis_ = _pPsngrDis;
	}
	~PassengerFinder(){

	}

	// if pick up a passenger, return true; otherwise return false
	bool FindPassenger(int road_id, int current_time, ODTuple &od_tuple);

private:
	// 路段尺度乘客OD元组发生概率时态分布
	PassengersDistribution *p_passenger_dis_;
};

class ChargingDecisionMaker {
public:
	ChargingDecisionMaker() {

	}
	~ChargingDecisionMaker()
	{

	}


private:
	// 充电桩空间分布

	//EVTaxi的属性进行判断信息

};