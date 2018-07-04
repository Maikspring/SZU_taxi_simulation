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

	// IMPROVEMENT: ����ʱ�շֲ�ѡ���ʼ���λ�� �� Ԥ���趨�ó�ʼλ��
	void GetRondomRoad(GoingRoad &going_road,
						int nCurTime); // ���ݸ�����ʱ�䣬���ճ��⳵�ֲ���������䵽·����
	void GetCruisingPath(std::list<GoingRoad> &going_road_list,
						GoingRoad current_gr,
						int nCurTime);
	void GetOccupiedPath(std::list<GoingRoad> &going_road_list,
						ODTuple &od_tuple,
						GoingRoad &current_gr,
						int nCurTime);

private:
	// ·����������
	RoadCondition* p_road_condition_;
	// ·����ʻ����
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
	// ·�γ߶ȳ˿�ODԪ�鷢������ʱ̬�ֲ�
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
	// ���׮�ռ�ֲ�

	//EVTaxi�����Խ����ж���Ϣ

};