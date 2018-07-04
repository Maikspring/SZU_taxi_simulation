#pragma once

// #include <cstdlib>
#include <vector>
#include <fstream>
#include <iomanip>

#include "SimulationUnit.h"

class SimulationControler;


class SimulationControler {
public:
	SimulationControler();
	~SimulationControler();

	/*	
	** 设定参数->初始化->更新迭代->输出最后的统计文件->结束
	*/

	/////////////////////////////////////////////

	//////////////////参数设定///////////////////
	void setOrdTaxiLogStrm(const char* sLogFileName);		// 设置输出普通出租车log文件
	void setEVTaxiLogStrm(const char* sLogFileName);		// 设置输出电动出租车log文件
	void setChargingStationsLogStrm(const char* sLogFileName);// 设置输出充电站log文件
	// TODO ... declare all methods

	void set_start_time(int sec_since_start_of_day);
	void set_time_inv_length(int time_inv_len);
	void set_road_condition_time_inv_num(int time_inv_num);

	void LoadRoadConnetion(const char* filename);
	void LoadCruisingPassingTime(const char* filename);
	void LoadODTuples(const char* filename);
	void LoadPickupProbabilites(const char* filename);

	////////////////////////////////////////////


	/////////////////迭代更新方法////////////////
	void initialization();						// 全局初始化
	SimulationControler& toNextTime();			// 模拟到下一时间间隔, 同时输出log
	SimulationControler& toNextNTime(int n);	// 模拟到下N次时间间隔, 同时输出log
	void outputLog();							// 输出所有Agent的情况

	void OutputFullStatisticsOrdTaxi(const char* output_filiename); // 输出Taxi所有统计值
	void OutputFullStatisticsEVTaxi(const char* output_filiename); // 输出EVTaxi所有统计值
	void OutputFullStatisticsChargingStations(const char* output_filiename); // 输出ChargingStation所有统计值
	///////////////////////////////////////////

// 参数设定与初始化
public:
	// initialization of TaxiPathMaker and TaxiPassengerFeeder
	void addOrdTaxies(int n);
	void addEVTaxies(int n, double BatteryComsumingRate);
	void addCharingStations(int n);

public:
	// Unfinished
	void outputOrdTaxiLog();
	void outputEVTaxiLog();
	void outputChargingStationLog();

// 模拟更新
public:
	// Unfinished
	void updateTime();
	void updateAllAgents();
	void updateOrdTaxies();
	void updateEVTaxies();
	void updateChargingStations();

private:
	// Unfinished
	void __InitializeOrdTaxies();
	void __InitializeEVTaxies();
	void __InitializeChargingStations();

// 所控制的Agent
private:
	// Units --> Taxi EVTaxi ChargingStation
	std::vector<Taxi> OrdTaxies;		// size of Taxies is nOrdTaxies
	std::vector<EVTaxi> EVTaxies;	// size of EVTaxies is nEVTaxies
	std::vector<ChargingStation> ChargingStations; // size of ChargingStations is nChargingStation

// 固定设施情况与驾驶经验
private:
	RoadCondition RoadCdtn;
	TaxiPathExperience PathExp;
	PassengersDistribution PsngrDis;

// 模拟的控制参数
private:
	int time_inv_len__;    // 模拟所采用的时间间隔(s)
	int start_time__;	  // 模拟的假定起始时间(seconds since the start of day)

	int nOrdTaxies;	// 普通出租车总数量
	int nEVTaxies;	// 电动出租车总数量
	int nChargingStations; // 充电站总数量

// 更新参数
private:
	int time_duration__;	// 总共模拟了多长时间 = nTimeInv * nSimulationTimes
	int simulation_times__;  // 总共模拟了多少次
	int current_time__;

// log 文件输出流
private:
	std::ofstream *str_ord_taxi_log__;	// 普通出租车log文件流
	std::ofstream *str_ev_taxi_log__;		// 电动出租车log文件流
	std::ofstream *str_chg_station_log__;		// 充电站log文件流
};