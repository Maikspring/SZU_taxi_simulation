#pragma once

#include <list>

#include "DecisionMaker.h"

// Agent类
class Taxi;
class EVTaxi;
class ChargingStation;

// 出租车行为决策控制类
class ChargingDecisionMaker;
class PathDecisionMaker;
class PassengerFinder;


/******************************************************
** 所有的行驶统计值，从开始模拟起算，不会清零
** 
******************************************************/
class Taxi {
public:
	Taxi(int _TaxiID,
		RoadCondition *_pRoadConnection,
		TaxiPathExperience *_pTaxiPathExp,
		PassengersDistribution *_pPsngrDis
		)
		:PathDecMkr(_pRoadConnection, _pTaxiPathExp)
		,PsngrFdr(_pPsngrDis)
	{
		TaxiID = _TaxiID;
		
		TimeOffset = 0;
		RoadID = -1;	// 由PathDecisionMaker随机分配初始路径
		isOccupied = false;
		isStop = false;	// 是否停止运行由PathDecisionMaker决定

		CruisingLength = 0;
		OccupiedLength = 0;
		CruisingTime = 0;
		PassengerNum = 0;
	}
	~Taxi();


	void Initialization(int nCurTime);	// 初始化出租车位置 
	void ActInGivenTimeInv(int nTimeLength, int nCurTime);	// 在给定的假定的当前时间下，一个时间长度路段变换和状态等变换，在EVTaxi中重载
	void rest();	// 停止运行，在EvTaxi中重载

	int get_taxi_id();
	int get_road_id();
	bool get_is_occupied();
	bool get_is_stop();
	void get_all_statistics(double &cruising_len,
							double &occupied_len,
							double &cruising_time,
							double &occupied_time,
							int    &passenger_num);


protected:
	void _ChoosePath(int nCurTime);	// 预设行驶路径, 在EVTaxi中重载
	void _FindAndPickPsngr(int nCurTime);	// 一旦pick up 则将预设的Cruising Path清空
	

// 属性
protected:
	int TaxiID;
	int RoadID;  // 当前所在的路段id
	GoingRoad current_going_road_;
	bool isOccupied; //是否载客
	bool isStop;	// 是否停止运行

	float TimeOffset;	// 判断在GoingRoadList的front element上度过了多久

	PathDecisionMaker PathDecMkr;	// 行驶路径选择器
	PassengerFinder PsngrFdr;	// 是否载客状态转换器

	std::list<GoingRoad> GoingRoadList;		// 规划好的行驶路线
	
// 行驶统计值
private:
	double CruisingLength;	// 空驶总行驶里程
	double OccupiedLength;	// 载客总行驶里程
	double CruisingTime;	// 空驶行驶总时长
	double OccupiedTime;	// 载客行驶总时长
	int    PassengerNum;     // 总载客数量
};

class EVTaxi : public Taxi {
public:
	EVTaxi(
		int _TaxiID, 
		double _BatteryConsumingRate, 
		RoadCondition *_pRoadConnection,
		TaxiPathExperience *_pTaxiPathExp,
		PassengersDistribution *_pPsngrDis)
		:Taxi(_TaxiID, _pRoadConnection, _pTaxiPathExp, _pPsngrDis)
	{
		BatteryConsumingRate = _BatteryConsumingRate;
	}
	~EVTaxi() {}

	void Initialization(int nCurTime); // 初始化位置，电量等
	void ActInGivenTimeInv(int nTimeLength, int nCurTime);	// 在给定的假定的当前时间下，一个时间长度路段变换和状态等变换，在EVTaxi中重载
	void rest();	// 停止运行

protected:
	void _ChoosePath(int nCurTime);	// 预设行驶路径, 在EVTaxi中重载
	void _FindAndPickPsngr(int nCurTime);	// 一旦pick up 则将预设的Cruising Path清空


// 属性
private:
	double BatteryPecentage; // 剩余电量百分比
	ChargingDecisionMaker ChargingDecMakr;	// 充电行为决策发生器

	bool isCharging;
	bool isWaiting;

	int charging_station_id__;  // be -1, if it is not charging or waiting in a charging station

private:
	double BatteryConsumingRate; // 耗电速度(百分比/公里数)

// 行驶统计值
private:
	double EnergyConsumed;  // 消耗总电量
	double TimeInCharging;	// 充电的总耗费时长
	double TimeInWaiting;	// 在充电桩等待的总时长
	double LengthToCS;	    // 准备充电到充电桩的总耗费里程
	double TimeToCS;        // 准备充电到充电桩的总耗费时间
	int ChargingTimesNum;   // 总的充电次数
};

class ChargingStation {
public:
	ChargingStation(int _StationID) {
		StationID = _StationID;
	}
	~ChargingStation(){}

// 固定属性
private:
	int StationID;
	int PlugNum; // 充电槽个数
	int RoadID;  // 要从哪一条路径进入
	
// 社会车辆充电情况时态分布情况

// 当前车辆充电的情况
private:
	int EVTaxiWaitingNum;	// 
	int EVTaxiChargingNum;

private:
	std::list<EVTaxi> ChargingList;	// 充电队列
	std::list<EVTaxi> WaitingList;	// 排队等待队列
};

