#pragma once

#include <map>
#include <list>
#include <vector>
#include <ctime>
#include <iostream>

#include "csv_reader.h"

#define DAY_LENGTH 86400

class RoadCondition;

// 以下两个类的时间切片是各自规定的，可以不同
class TaxiPathExperience;
class PassengersDistribution;

struct GoingRoad;
struct Road;
struct ODTuple;

struct Road {
	Road() {
		id = -1;
		node_start = -1;
		node_end = -1;
		is_bi_direct = true;
		length = 0;
	}


	int id;

	int node_start;  // node of the start of road
	int node_end;    // node of the end of road
	bool is_bi_direct;		    // is the road bi-directional
	float length;
};

struct GoingRoad {
	GoingRoad(Road road, bool drtnl, float pas_time){
		this->road = road;
		directional = drtnl;
		passing_time = pas_time;
	}

	GoingRoad() {
		directional = false;
		passing_time = 0;
	}

	Road road;

	bool directional;		// true denotes travel from node_start to node_end, false inversed
	float passing_time;	// 走完该路段所需的时间
};


struct ODTuple {
	int road_id_o;
	int road_id_d;

	int occur_time;	// seconds since the start of day(00:00)

	float path_length;
	int time_duration;	// in second
};

GoingRoad GetRandomRoadInVector(std::vector<Road>* p_road_vec, int node, int exclude_road_id);

class RoadCondition {
public:
	RoadCondition();
	~RoadCondition() {}

 	bool CheckRoadsConnection(std::list<int> &going_road_id_list, int list_size);	// 判断由road id 构成的将要行驶的路径，是否联通
	GoingRoad GetRandomNextRoad(GoingRoad going_road);	// 随机获取road_id 对应Road的下一条路径，且不允许在bi-directional road 中掉头（用exclude_road_id排除）
	GoingRoad GetRandomGoingRoad();

	//WARNING: this method has an O(n^2) computation complexity (n is the number of roads)
	void GetShortestPath(std::list<GoingRoad> &going_road_list, int road_id_start, int road_id_end);

	float GetRoadCruisingPassingTime(int road_id, int current_time);
	float GetRoadOccupiedPassingTime(int road_id, int current_time);
	int GetWhichTimeInv(int current_time);	// 获得当前时间所属的时间切片下标

	Road get_road(int road_id);

// Load data from files
// set paras
public:
	void set_time_inv_num(int time_inv_num);
	void LoadRoadConnectionFromFile(const char* road_connection_filename);
	void LoadRoadCruinsingPassingTimeFromFile(const char* road_passing_time_filename);
	void LoadRoadOccupiedPassingTimeFromFile(const char* road_passing_time_filename);

private:
	void __CheckNodesToRoadMap();

private:
	// 路段连接关系
	std::map<int, Road> id_to_road_map_;	// road_id 对应的 Road
	std::map<int, std::vector<Road>> node_to_road_map_; // node_id 对应的 Road集合

	// 用GDAL支持的图网络

	// 路段上的平均行驶时间，分为time_inv_num_个时间切片
	std::vector<std::map<int, float>> road_cruising_passing_time_vec_;
	std::vector<std::map<int, float>> road_occupied_passing_time_vec_;
	int time_inv_num_; // 总共将全天划分为多少个时间切片存储概率
	int time_inv_len_; // 一个时间切片的时间长度 = 24*60*60/time_inv_num
};

// 包含Cruising 和 Occupied 情况下的时空行驶路径选择
// 包含 Taxi 运行和停止运行时空分布情况
class TaxiPathExperience {
public:
	TaxiPathExperience();
	~TaxiPathExperience();

	// TODO: methods .....
//	int GetWhichTimeInv(int current_time);
//	void loadTaxiExpFromFile(const char* sTaxiExpFileName);
	
	// return true, if succeed; otherwise return false
	// false indicates missing data
	bool GetCruisingPath(GoingRoad &current_going_road, std::list<GoingRoad> &going_road_list, int current_time); 
	bool GetOccupiedPath(GoingRoad &cueent_going_road, ODTuple od_tuple, std::list<GoingRoad> &going_road_list, int current_time);



private:
	int time_inv_num_;	// 总共将全天划分为多少个时间切片存储，每个切片中的驾驶经验具有时空异质性
	int time_inv_len_;

	std::vector<std::map<int, std::vector<std::vector<int>>>> cruising_path_list_;
};

class PassengersDistribution {
public:
	PassengersDistribution();
	PassengersDistribution(const char* pro_filename);
	~PassengersDistribution();

	bool FindPassenger(int road_id, int current_time, ODTuple &od_tuple); // 在路段在当前时间路过载客的概率下，获取乘客出行OD元组，return true if succeed, otherwise false

private:
	// 载客概率
	float GetProbabilityOfOccupiedInTimeOfRoad(int road_id, int current_time);	// 获取路段在当前时间路过时，其载客概率为多少
	int GetWhichTimeInvInPro(int current_time);	// 获得当前时间所属的时间切片下标
	int GetWhichTimeInvInOD(int current_time);

	// OD元组
	std::vector<ODTuple>* GetAllODTuple(int road_id, int time_inv_ind);	// 获取以road_id为o路段的，且在第time_inv_ind个时间切片下的所有OD元组

public:
	// set time interval number before load data
	void set_time_inv_num_pro(int time_inv_num);
	void set_time_inv_num_od(int time_inv_num);

	void LoadODTuplesFromFile(const char* od_tuples_filename);
	void LoadProDisFromFile(const char* pro_dis_filename);

private:
	// 建议是time_inv_num_pro_ = time_inv_num_od_
	std::vector<std::map<int, float>> probability_distribution_time_map_;
	int time_inv_num_pro_; // 总共将全天划分为多少个时间切片存储概率
	int time_inv_len_pro_; // 一个时间切片的时间长度 = 24*60*60/time_inv_num


	std::map<int, std::vector<std::vector<ODTuple>>> o_road_to_od_tuples_map_;	// 只要路段有上客记录，对应value vector的size为time_inv_od_num_
	int time_inv_num_od_;
	int time_inv_len_od_;
};