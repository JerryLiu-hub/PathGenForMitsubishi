/*
 * File Name :			PathFileGen.cpp
 * Create Date :		2021.07.21
 * Last Update Date: 	2021.07.21
 * File Version : 		0.1.0
 * Auther :				Javnson(javnson@zju.edu.cn)
 *
 * Update log :
 *  
 * Description : 本文件将已经生成的lines生成相应的 Basic V代码
 *
 * Classes :
 * Functions:
 * generate_running_code 生成运行代码
 * 
 *
 */

#include <iostream>
#include <opencv.hpp>
#include <Windows.h>
#include <WinUser.h>
#include <functional>
#include <utility>
#include <string>
#include <thread>
#include <mutex>

 /*
  * @brief 辅助功能函数，计算线条所在的实际位置坐标，完成坐标变换
  * @param 
  * @notes
  * @fixed
  *
  * @update 2021.07.21
  */
void calc_real_pos(__out cv::Point3f &start_point, __out cv::Point3f &end_point, // 返回点
	__in cv::Point3f rect[3], // 实际定位标度
	__in std::pair<cv::Point2f, cv::Point2f>& line // 等待变换的线条
)
{
	start_point.x = line.first.y * rect[2].x + (line.first.x - line.first.y) * rect[1].x + (1 - line.first.x) * rect[0].x;
	start_point.y = line.first.y * rect[2].y + (line.first.x - line.first.y) * rect[1].y + (1 - line.first.x) * rect[0].y;
	start_point.z = line.first.y * rect[2].z + (line.first.x - line.first.y) * rect[1].z + (1 - line.first.x) * rect[0].z;

	end_point.x = line.second.y * rect[2].x + (line.second.x - line.second.y) * rect[1].x + (1 - line.second.x) * rect[0].x;
	end_point.y = line.second.y * rect[2].y + (line.second.x - line.second.y) * rect[1].y + (1 - line.second.x) * rect[0].y;
	end_point.z = line.second.y * rect[2].z + (line.second.x - line.second.y) * rect[1].z + (1 - line.second.x) * rect[0].z;
	
	return;
}


 /*
  * @brief 核心功能函数，将生成的lines生成代码
  * @param 配置文件名，保存到的目标文件名，需要计算的线条
  * @notes 
  * @fixed
  * 
  * @update 2021.07.21
  */
const char* point_options = "PointOptions";
const char* error_message = "ERROR!";
int generate_running_code(const char *profile, const char *targetfile, 
	std::vector<std::pair<cv::Point2f, cv::Point2f>>& lines)
{
	cv::Point3f board[3];
		std::pair<cv::Point3f, cv::Point3f> rect;
	char buffer[60] = { 0 };
	GetPrivateProfileStringA(point_options, "x1", error_message, buffer, 60, profile);
	if (strcmp(buffer, error_message))
	{
		std::cout << "系统配置读取错误，请检查文件是否存在，以及文件格式。\n";
		return 1;
	}
	board[0].x = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "y1", error_message, buffer, 60, profile);
	board[0].y = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "z1", error_message, buffer, 60, profile);
	board[0].z = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "x2", error_message, buffer, 60, profile);
	board[1].x = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "y2", error_message, buffer, 60, profile);
	board[1].y = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "z2", error_message, buffer, 60, profile);
	board[1].z = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "x3", error_message, buffer, 60, profile);
	board[2].x = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "y3", error_message, buffer, 60, profile);
	board[2].y = std::stof(std::string(buffer));
	GetPrivateProfileStringA(point_options, "z3", error_message, buffer, 60, profile);
	board[2].z = std::stof(std::string(buffer));

	//if (!(rect.first.x&& rect.first.y&& rect.second.x&& rect.second.y)) // 如果出现了读取错误
	//{
	//	std::cout << "系统配置读取错误，请检查文件是否存在，以及文件格式。\n";
	//	return 1;
	//}
	std::vector< std::pair<cv::Point3f, cv::Point3f>> line_after_trans;
	std::pair<cv::Point3f, cv::Point3f> tmp_line_trans;
	for (auto& itor : lines)
	{
		calc_real_pos(tmp_line_trans.first, tmp_line_trans.second, board, itor);
		line_after_trans.push_back(tmp_line_trans);
	} // 完成坐标变换

	GetPrivateProfileIntA()
	// 生成坐标
	
	return 0;

}


#define PROFILE_NAME "E:\\_Projects\\PathFileGen\\DrawOptions.ini"
int main()
{
    std::cout << "Hello World!\n";

    std::vector<std::pair<cv::Point2f, cv::Point2f>> lines;
    std::pair<cv::Point2f, cv::Point2f> line;

    // 这里需要编一些点进去
    line.first.x = 0.1;
    line.first.y = 0.1;
    line.second.x = 0.5;
    line.second.y = 0.5;
    lines.push_back(line);

	line.first.x = 0.9;
	line.first.y = 0.9;
	line.second.x = 0.5;
	line.second.y = 0.5;
	lines.push_back(line);

	line.first.x = 0.5;
	line.first.y = 0.5;
	line.second.x = 0.1;
	line.second.y = 0.9;
	lines.push_back(line);

    // 当前有3条线
	generate_running_code(PROFILE_NAME, "target.txt", lines);



}

