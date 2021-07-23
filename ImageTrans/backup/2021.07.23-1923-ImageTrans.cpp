/*
 * File Name :			ImageTrans.cpp
 * Create Date :		2021.07.10
 * Last Update Date: 	2021.07.21
 * File Version : 		0.1.10
 * Auther :				Javnson(javnson@zju.edu.cn)
 *
 * Update log :
 *  - 2021.07.10 完成图像你和算法
 * Description : 本文件负责生成一个轨迹绘制算法
 *
 * Classes :
 * Functions:
 * show_pincture 显示绘制好的函数
 * gamma_trans 为图像做gamma变换
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



 // using namespace cv;
 // using namespace std;
 //using namespace std::placeholders;
// using namespace std::cout;
// using namespace std::endl;

// 显示窗口最大大小
constexpr int show_picture_window_max_width = 512 * 2;
constexpr int show_picture_window_max_height
= show_picture_window_max_width / 16 * 9; // 16:9

/*
 * @brief 限制图像最大输出大小的显示函数
 * @param 需要显示的图像，窗口标题，窗口的最大限度
 * @notes 默认的窗口大小通过constexpr定义给出
 * @fixed
 *
 * @update 2021.07.10
 */
void show_picture(cv::InputArray source, const char* window_title,
	const int max_width = show_picture_window_max_width, const int max_height = show_picture_window_max_height)
{
	cv::Mat input = source.getMat();
	cv::Size pic_size = input.size();

	if (pic_size.height == 0 || pic_size.width == 0) {
		throw(input);
	}
	else if (pic_size.height > max_height || pic_size.width > max_width) {
		float scale = static_cast<float>(max_height) / static_cast<float>(pic_size.height) > max_width / pic_size.width ?
			static_cast<float>(max_width) / pic_size.width : static_cast<float>(max_height) / pic_size.height; // 计算图片缩放比

		assert(scale != 0);

		cv::Size new_size(static_cast<int>(static_cast<float>(pic_size.width) * scale),
			static_cast<int>(static_cast<float>(pic_size.height) * scale));
		cv::Mat resize_picture;
		cv::resize(input, resize_picture, new_size);
		cv::imshow(window_title, resize_picture);
		std::cout << "[INFO] the picture has been scaled to "
			<< static_cast<int>(static_cast<float>(pic_size.width) * scale) << "*"
			<< static_cast<int>(static_cast<float>(pic_size.height) * scale)
			<< " Scale: " << scale << std::endl;
	}
	else {
		cv::imshow(window_title, input);
	}

	return;
}

constexpr float default_gamma = 0.5;
/*
 * @brief 对输入的灰度图做gamma变换
 * @param 输入和输出的图像
 * @notes 输出的图像会被一律转换成float类型的元素，之后有必要可以写这个函数的加速版本
 * @fixed
 *
 * @update 2021.07.10
 */
void gamma_trans(cv::InputArray input, cv::OutputArray transform_result,
	float gamma = default_gamma)
{
	cv::Mat image = input.getMat();
	cv::Mat& result = transform_result.getMatRef();
	result.create(image.size(), CV_32F);

	cv::normalize(image, image, 1.0, 0, cv::NORM_MINMAX);
	cv::pow(image, gamma, result);

	return;
}


/*
 * @brief 插值算法，给定任意坐标可以计算图像的像素灰度
 * @param 给定极限位置，传入灰度图，传入待求点坐标
 * @notes 参考二元插值公式进行计算
 * @fixed
 * 条件出错可能导致绘图越界 FIXED
 * 条件语句失效，没有正常起到限幅的作用，发现问题是xy和width和height的对应关系不正确
 * @update 2021.07.11
 */
float coordinate_mapping_pic2std(
	//const float bmp_corrdinate_x, const float bmp_coordinate_y,
	//const float src_corrdinate_x, const float src_corrdinate_y,
	//cv::Mat gray_map,
	cv::InputArray input,
	const float x, const float y)
{
	assert(x < 1 && x > 0); // domiain of defination
	assert(y < 1 && y > 0);

	cv::Mat gray_map = input.getMat();

	int index_x1 = gray_map.size().height - 1;
	int index_x2 = gray_map.size().height - 1;
	int index_y1 = gray_map.size().width - 1;
	int index_y2 = gray_map.size().width - 1;

	index_x1 = index_x1 > static_cast<int>(x * gray_map.size().height) ?
		static_cast<int>(x * gray_map.size().height) : index_x1;
	index_x2 = index_x2 > static_cast<int>(x * gray_map.size().height + 1) ?
		static_cast<int>(x * gray_map.size().height + 1) : index_x2;
	index_y1 = index_y1 > static_cast<int>(y * gray_map.size().width) ?
		static_cast<int>(y * gray_map.size().width) : index_y1;
	index_y2 = index_y2 > static_cast<int>(y * gray_map.size().width + 1) ?
		static_cast<int>(y * gray_map.size().width + 1) : index_y2;

	//index_x1 = index_x1 >= (gray_map.size().width - 2) ? (gray_map.size().width - 1) : index_x1;
	//int index_x2 = index_x1 >= (gray_map.size().width - 2) ? (gray_map.size().width - 1) : index_x1 + 1;
	//int index_y1 = static_cast<int>(y * gray_map.size().width);
	//index_y1 = index_y1 >= (gray_map.size().height - 2) ? (gray_map.size().height - 1) : index_y1;
	//int index_y2 = index_y1 >= (gray_map.size().height - 2) ? (gray_map.size().height - 1) : index_y1 + 1;

	// 以下两句市场出现不正常工作的情况
	//index_x2 = index_x2 >= (gray_map.size().width - 1) ? (gray_map.size().width - 1) : index_x2;
	//index_y2 = index_y2 >= (gray_map.size().height - 1) ? (gray_map.size().height - 1) : index_y2;

	float result_x1y1 = gray_map.at<float>(index_x1, index_y1);
	float result_x2y1 = gray_map.at<float>(index_x2, index_y1);
	float result_x1y2 = gray_map.at<float>(index_x1, index_y2);
	float result_x2y2 = gray_map.at<float>(index_x2, index_y2);

	float result
		= result_x1y1 * (x * gray_map.size().width - index_x1) * (y * gray_map.size().height - index_y1)
		+ result_x2y1 * (index_x2 - x * gray_map.size().width) * (y * gray_map.size().height - index_y1)
		+ result_x1y2 * (x * gray_map.size().width - index_x1) * (index_y2 - y * gray_map.size().height)
		+ result_x2y2 * (index_x2 - x * gray_map.size().width) * (index_y2 - y * gray_map.size().height);
	result /= (x * gray_map.size().width - index_x1) * (y * gray_map.size().height - index_y1)
		+ (index_x2 - x * gray_map.size().width) * (y * gray_map.size().height - index_y1)
		+ (x * gray_map.size().width - index_x1) * (index_y2 - y * gray_map.size().height)
		+ (index_x2 - x * gray_map.size().width) * (index_y2 - y * gray_map.size().height);


	return result;
}

constexpr float parper_coordinate_x = 1920.0;
constexpr float parper_coordinate_y = 1080.0;
/*
 * @brief 反解位置算法
 * @param 待求点坐标和球的点坐标
 * @notes 使用引用实现函数值返回
 * @fixed
 *
 * @update 2021.07.11
 */
void coordinate_mapping_std2scr_basic(const float x, const float y,
	float& scr_x, float& scr_y,
	const float src_corrdinate_x, const float src_corrdinate_y)
{
	scr_x = x * src_corrdinate_x;
	scr_y = y * src_corrdinate_y;

	return;
}

constexpr float drawstep = 0.001f; // 绘制步长
constexpr float min_line_length = drawstep * 5; // 最少10步
enum line_type {
	UPDOWN = 0, LEFTRIGHT, FORWARDSLASH, BACKSLASH
};
/*
 * @brief 求解直线填充直线端点信息
 * @param 传入待求解点的集合（作为返回值），传入待分解的图像矩阵item,传入分解线的类型,传入分解过程判定点存在性的阈值
 * @notes 正斜杠的方向为左上到右下↘，反斜杠的方向为左下到右上↗
 * @fixed
 * 2021.07.12 FIXED 新增轨迹计数功能，方便调试
 * 输出轨迹错误（出现汇聚现象，应该是没有更新绘图起始点或者重点导致的） FIXED
 * 第一类线条始终没有参与计算，完全没有输出 FIXED
 * 不满足16:9比例的图像将会激发异常，需要根据图像尺寸进行修正
 * @update 2021.07.12
 */
void LineSolver(std::vector<std::pair<cv::Point2f, cv::Point2f>>& lines, cv::InputArray item,
	const line_type type, const float threshold, const float mininum_line_length,
	const float sin_theta, const float cos_theta)
{
	cv::Mat plot = item.getMat();

	std::pair<cv::Point2f, cv::Point2f> tmp_line;

	int checkpoint1 = 0; // pointer has enter the range of line.
	float shift;
	float x, y;

	unsigned int num_new_lines = 0;
	static unsigned int num_all_new_lines = 0;

	float distance = 0.0f;
	float one_condition_distance = 0.0f;
	static float all_distance = 0.0f;

	std::cout << "测试显示字符串：" << item.getMat().at<float>(1079, 1919) << std::endl;
	switch (type)
	{
	case UPDOWN:
		for (shift = drawstep; shift < 1; shift += drawstep) {
			checkpoint1 = 0;
			for (y = drawstep; y < 1; y += drawstep) {
				if (coordinate_mapping_pic2std(item.getMat(), y, shift) > threshold) {
					if (!checkpoint1) { // record the entry point
						tmp_line.first.x = shift;
						tmp_line.first.y = y;
					}
					checkpoint1 = 1;
				}
				else {
					if (checkpoint1) { // record the end point
						tmp_line.second.x = shift;
						tmp_line.second.y = y;
						distance = fabsf(tmp_line.second.y - tmp_line.first.y);
						if (distance >= mininum_line_length) { // 线长足够，记录
							lines.push_back(tmp_line);
							num_new_lines += 1;
							num_all_new_lines += 1;
							one_condition_distance += distance;
							all_distance += distance;
						}
					}
					checkpoint1 = 0;
				}
			}
			// 进入到这里说明一行的处理刚刚完成，此时有可能checkpoint1并没有归零，此时直接进入下一行将会出错
			// 在这里恢复初始条件
			if (checkpoint1) { // record the end point
				tmp_line.second.x = shift;
				tmp_line.second.y = y;
				distance = fabsf(tmp_line.second.y - tmp_line.first.y);
				if (distance >= mininum_line_length) { // 线长足够，记录
					lines.push_back(tmp_line);
					num_new_lines += 1;
					num_all_new_lines += 1;
					one_condition_distance += distance;
					all_distance += distance;
				}
			}
		}
		break;
	case LEFTRIGHT:
		for (shift = drawstep; shift < 1; shift += drawstep) {
			checkpoint1 = 0;
			for (x = drawstep; x < 1; x += drawstep) {
				if (coordinate_mapping_pic2std(item.getMat(), shift, x) > threshold) {
					if (!checkpoint1) {
						tmp_line.first.x = x;
						tmp_line.first.y = shift;
					}
					checkpoint1 = 1;
				}
				else {
					if (checkpoint1) {
						tmp_line.second.x = x;
						tmp_line.second.y = shift;
						distance = fabs(tmp_line.second.x - tmp_line.first.x);
						if (distance >= mininum_line_length) {
							lines.push_back(tmp_line);
							num_new_lines += 1;
							num_all_new_lines += 1;
							one_condition_distance += distance;
							all_distance += distance;
						}
					}
					checkpoint1 = 0;
				}
			}
			if (checkpoint1) {
				tmp_line.second.x = x;
				tmp_line.second.y = shift;
				distance = fabs(tmp_line.second.x - tmp_line.first.x);
				if (distance >= mininum_line_length) {
					lines.push_back(tmp_line);
					num_new_lines += 1;
					num_all_new_lines += 1;
					one_condition_distance += distance;
					all_distance += distance;
				}
			}
		}
		break;
	case FORWARDSLASH: // 正交斜线使用两轮遍历法进行绘制
		x = 0, y = 0;
		for (shift = drawstep; shift < 1; shift += drawstep) { // 行遍历
			checkpoint1 = 0;
			for (x = shift, y = drawstep; (x < 1) && (x > 0) && (y < 1) && (y > 0); x += cos_theta * drawstep, y += sin_theta * drawstep) {
				if (coordinate_mapping_pic2std(item.getMat(), y, x) > threshold) {
					if (!checkpoint1) {
						tmp_line.first.x = x;
						tmp_line.first.y = y;
					}
					checkpoint1 = 1;
				}
				else {
					if (checkpoint1) {
						tmp_line.second.x = x;
						tmp_line.second.y = y;
						distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
							+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
						if (distance >= mininum_line_length) {
							lines.push_back(tmp_line);
							num_new_lines += 1;
							num_all_new_lines += 1;
							one_condition_distance += distance;
							all_distance += distance;
						}
					}
					checkpoint1 = 0;
				}
			}
			if (checkpoint1) {
				tmp_line.second.x = x;
				tmp_line.second.y = y;
				distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
					+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
				if (distance >= mininum_line_length) {
					lines.push_back(tmp_line);
					num_new_lines += 1;
					num_all_new_lines += 1;
					one_condition_distance += distance;
					all_distance += distance;
				}
			}
		}
		for (shift = drawstep; shift < 1; shift += drawstep) { // 列遍历
			checkpoint1 = 0;
			for (y = shift, x = drawstep; (x < 1) && (x > 0) && (y < 1) && (y > 0); x += cos_theta * drawstep, y += sin_theta * drawstep) {
				if (coordinate_mapping_pic2std(item.getMat(), y, x) > threshold) {
					if (!checkpoint1) {
						tmp_line.first.x = x;
						tmp_line.first.y = y;
					}
					checkpoint1 = 1;
				}
				else {
					if (checkpoint1) {
						tmp_line.second.x = x;
						tmp_line.second.y = y;
						distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
							+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
						if (distance >= mininum_line_length) {
							lines.push_back(tmp_line);
							num_new_lines += 1;
							num_all_new_lines += 1;
							one_condition_distance += distance;
							all_distance += distance;
						}
						checkpoint1 = 0;
					}
				}
			}
			if (checkpoint1) {
				tmp_line.second.x = x;
				tmp_line.second.y = y;
				distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
					+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
				if (distance >= mininum_line_length) {
					lines.push_back(tmp_line);
					num_new_lines += 1;
					num_all_new_lines += 1;
					one_condition_distance += distance;
					all_distance += distance;
				}
			}
		}
		break;
	case BACKSLASH:
		x = 0, y = 0;
		for (shift = drawstep; shift < 1; shift += drawstep) { // 行遍历
			checkpoint1 = 0;
			for (x = shift, y = drawstep; (x < 1) && (x > 0) && (y < 1) && (y > 0); x -= cos_theta * drawstep, y += sin_theta * drawstep) {
				if (coordinate_mapping_pic2std(item.getMat(), y, x) > threshold) {
					if (!checkpoint1) {
						tmp_line.first.x = x;
						tmp_line.first.y = y;
					}
					checkpoint1 = 1;
				}
				else {
					if (checkpoint1) {
						tmp_line.second.x = x;
						tmp_line.second.y = y;
						distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
							+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
						if (distance >= mininum_line_length) {
							lines.push_back(tmp_line);
							num_new_lines += 1;
							num_all_new_lines += 1;
							one_condition_distance += distance;
							all_distance += distance;
						}
					}
					checkpoint1 = 0;
				}

			}
			if (checkpoint1) {
				tmp_line.second.x = x;
				tmp_line.second.y = y;
				distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
					+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
				if (distance >= mininum_line_length) {
					lines.push_back(tmp_line);
					num_new_lines += 1;
					num_all_new_lines += 1;
					one_condition_distance += distance;
					all_distance += distance;
				}
			}
		}
		for (shift = drawstep; shift < 1; shift += drawstep) { // 列遍历
			checkpoint1 = 0;
			for (y = shift, x = drawstep; (x < 1) && (x > 0) && (y < 1) && (y > 0); x -= cos_theta * drawstep, y += sin_theta * drawstep) {
				if (coordinate_mapping_pic2std(item.getMat(), y, x) > threshold) {
					if (!checkpoint1) {
						tmp_line.first.x = x;
						tmp_line.first.y = y;
					}
					checkpoint1 = 1;
				}
				else {
					if (checkpoint1) {
						tmp_line.second.x = x;
						tmp_line.second.y = y;
						distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
							+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
						if (distance >= mininum_line_length) {
							lines.push_back(tmp_line);
							num_new_lines += 1;
							num_all_new_lines += 1;
							one_condition_distance += distance;
							all_distance += distance;
						}
						checkpoint1 = 0;
					}
				}
			}
			if (checkpoint1) {
				tmp_line.second.x = x;
				tmp_line.second.y = y;
				distance = sqrt((tmp_line.second.x - tmp_line.first.x) * (tmp_line.second.x - tmp_line.first.x)
					+ (tmp_line.second.y - tmp_line.first.y) * (tmp_line.second.y - tmp_line.first.y));
				if (distance >= mininum_line_length) {
					lines.push_back(tmp_line);
					num_new_lines += 1;
					num_all_new_lines += 1;
					one_condition_distance += distance;
					all_distance += distance;
				}
			}
		}
		break;
	}
	std::cout << "[INFO] 当前总共直线线数：" << num_all_new_lines << "," << "本轮循环新增：" << num_new_lines << std::endl;
	std::cout << "[INF] 当前轨迹总长度：" << all_distance << "," << "本轮循环新增：" << one_condition_distance << std::endl;
	return;


}

/*
 * @brief 计算PI函数的值，借助constexpr函数
 * @param NULL
 * @notes
 * @fixed
 *
 * @update 2021.07.11
 */
constexpr float pi()
{
	return 3.141592f;
}

/*
 * @brief 将当前的DC句柄转换成位图
 * @param 等待转换的DC句柄，位图的宽度和高度
 * @notes 默认的窗口大小通过constexpr定义给出
 * @fixed
 *
 * @update 2021.07.10
 */
HBITMAP GetSrcBit(HDC hDC, DWORD BitWidth, DWORD BitHeight)
{
	HDC hBufDC;
	HBITMAP hBitmap, hBitTemp;
	hBufDC = CreateCompatibleDC(hDC); // 创建设备上下文(HDC)

	hBitmap = CreateCompatibleBitmap(hDC, BitWidth, BitHeight); // 创建HBITMAP
	hBitTemp = (HBITMAP)SelectObject(hBufDC, hBitmap);
	StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight, hDC, 0, 0, BitWidth, BitHeight, SRCCOPY); // 得到位图缓冲区
	hBitmap = (HBITMAP)SelectObject(hBufDC, hBitTemp); // 得到最终的位图信息

	DeleteObject(hBitTemp);
	DeleteDC(hBufDC);
	return hBitmap;
}

/*
 * @brief 限制图像最大输出大小的显示函数
 * @param 需要显示的图像，窗口标题，窗口的最大限度
 * @notes 默认的窗口大小通过constexpr定义给出
 * @fixed
 * 参数不匹配（不能传入正常传入文件名参数） FIXED
 * 函数重构（出现不能够正常保存的问题） FIXED
 *
 * @update 2021.07.10
 */

BOOL SaveBmp(HBITMAP hBitmap, LPCWSTR fileName)
{
	HDC   hDC;
	//当前分辨率下每象素所占字节数     
	int   iBits;
	//位图中每象素所占字节数     
	WORD  wBitCount;
	//定义调色板大小，位图中像素字节大小，位图文件大小，写入文件字节数  
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//位图属性结构       
	BITMAP   Bitmap;
	//位图文件头结构     
	BITMAPFILEHEADER   bmfHdr;
	//位图信息头结构       
	BITMAPINFOHEADER   bi;
	//指向位图信息头结构         
	LPBITMAPINFOHEADER lpbi;
	//定义文件，分配内存句柄，调色板句柄       
	HANDLE  fh, hDib, hPal, hOldPal = NULL;

	//计算位图文件每个像素所占字节数       
	hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)wBitCount = 1;
	else if (iBits <= 4) wBitCount = 4;
	else if (iBits <= 8) wBitCount = 8;
	else wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

	//   为位图内容分配内存       
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	//   处理调色板         
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = ::GetDC(NULL);
		//hDC   =   m_pDc->GetSafeHdc();     
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	//   获取该调色板下新的像素值       
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	//   恢复调色板         
	if (hOldPal)
	{
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	//   创建位图文件         
	fh = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
	{
		GlobalUnlock(hDib);
		GlobalFree(hDib);
		return FALSE;
	}

	//   设置位图文件头       
	bmfHdr.bfType = 0x4D42;   //   ;BM;  
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	//   写入位图文件头  
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//   写入位图文件其余内容  
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);

	//   清除  
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return TRUE;
}
//————————————————
//版权声明：本文为CSDN博主「xiao2macf」的原创文章，遵循CC 4.0 BY - SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https ://blog.csdn.net/xxm524/article/details/82082585
//BOOL SaveBmp(HDC hDc, HBITMAP hBitmap, LPCWSTR FileName)
//{
//	//		调色板大小，		位图中像素字节大小 ，	位图文件大小 ， 写入文件字节数 
//	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
//	//	文件，分配内存句柄，调色板句柄 
//	HANDLE fh, hDib, hPal, hOldPal = NULL;
//
//	
//	BITMAPFILEHEADER bmfHdr; // 位图文件头结构
//	
//	int iBits; // 当前分辨率下每象素所占字节数
//	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES); // 计算位图文件每个像素所占字节数 
//
//	WORD wBitCount; // 位图中每象素所占字节数
//	if (iBits <= 1) wBitCount = 1;
//	else if (iBits <= 4) wBitCount = 4;
//	else if (iBits <= 8) wBitCount = 8;
//	else wBitCount = 24;
//	
//	BITMAPINFOHEADER bi; // 位图信息头结构 
//	BITMAP Bitmap; // 位图属性结构 
//	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
//	bi.biSize = sizeof(BITMAPINFOHEADER);
//	bi.biWidth = Bitmap.bmWidth;
//	bi.biHeight = Bitmap.bmHeight;
//	bi.biPlanes = 1;
//	bi.biBitCount = wBitCount;
//	bi.biCompression = BI_RGB;
//	bi.biSizeImage = 0;
//	bi.biXPelsPerMeter = 0;
//	bi.biYPelsPerMeter = 0;
//	bi.biClrImportant = 0;
//	bi.biClrUsed = 0;
//	dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
//
//	//为位图内容分配内存
//	LPBITMAPINFOHEADER lpbi = nullptr;
//	hDib = GlobalAlloc(GHND, static_cast<UINT>(dwBmBitsSize + dwPaletteSize) + sizeof(BITMAPINFOHEADER));
//	if(!hDib) {
//		std::cout << "[Error] 位图存储空间错误\n";
//		return FALSE;
//	}
//	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
//	if (!lpbi) {
//		std::cout << "[Error] Global Lock错误。\n";
//		return FALSE;
//	}
//	*lpbi = bi;
//
//	// 处理调色板 
//	hPal = GetStockObject(DEFAULT_PALETTE);
//	if (hPal)
//	{
//		hDC = ::GetDC(NULL);
//		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
//		RealizePalette(hDC);
//	}
//	// 获取该调色板下新的像素值 
//	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
//		+ dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
//	//恢复调色板 
//	if (hOldPal)
//	{
//		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
//		RealizePalette(hDC);
//		::ReleaseDC(NULL, hDC);
//	}
//	//创建位图文件 
//	fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
//		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
//	if (fh == INVALID_HANDLE_VALUE) return FALSE;
//
//	// 设置位图文件头 
//	LPBITMAPINFOHEADER lpbi; // 指向位图信息头结构 
//	bmfHdr.bfType = 0x4D42; // "BM" 
//	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
//	bmfHdr.bfSize = dwDIBSize;
//	bmfHdr.bfReserved1 = 0;
//	bmfHdr.bfReserved2 = 0;
//	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
//
//	// 写入位图文件头
//	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
//	// 写入位图文件其余内容 
//	   // WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL); //清除 
//	GlobalUnlock(hDib);
//	GlobalFree(hDib);
//	CloseHandle(fh);
//	return TRUE;
//}


constexpr LPCWSTR picture_name = TEXT("DrawResult.bmp");
/*
 * @brief 将线条绘制出
 * @param 传入设备上下文句柄，传入线条集合，传入窗口的大小
 * @notes
 * @fixed
 * 绘制结果为一片黑，完全显示不出来，通过修改默认画刷和默认的笔来实现
 * @update 2021.07.12
 */
void DrawLinePicture(HDC hdc, std::vector<std::pair<cv::Point2f, cv::Point2f>>& lines,
	int window_size_x, int window_size_y)
{
	HDC mdc = CreateCompatibleDC(hdc); // 创建兼容DC画板
	HBITMAP bmp = CreateCompatibleBitmap(hdc, window_size_x, window_size_y);
	SelectObject(mdc, bmp);

	HBRUSH hBrush;
	hBrush = (HBRUSH)GetStockObject(DC_BRUSH);
	Rectangle(mdc, 0, 0, window_size_x, window_size_y);

	HPEN hPen;
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	SelectObject(mdc, hPen);
	// todo 在这里完成绘制工作

	// 缓冲交换
	//BitBlt(hdc, 0, 0, window_size_x, window_size_y, mdc, 0, 0, SRCCOPY);
	float x = 0.0f, y = 0.0f;
	for (auto line_inf : lines) {
		coordinate_mapping_std2scr_basic(line_inf.first.x, line_inf.first.y, x, y,
			parper_coordinate_x, parper_coordinate_y);
		MoveToEx(mdc, static_cast<int>(x), static_cast<int>(y), nullptr);
		coordinate_mapping_std2scr_basic(line_inf.second.x, line_inf.second.y, x, y,
			parper_coordinate_x, parper_coordinate_y);
		LineTo(mdc, static_cast<int>(x), static_cast<int>(y));
	}

	HBITMAP bitmap = GetSrcBit(mdc,
		static_cast<DWORD>(parper_coordinate_x), static_cast<DWORD>(parper_coordinate_y));
	if (!SaveBmp(bitmap, picture_name)) {
		std::cout << "[ERROR] Can't Save file\n";
	}
	else {
		std::cout << "[INFO] 保存绘制结果。\n";
	}
	// 释放DC对象
	DeleteDC(mdc);

	return;
}


//std::vector<std::pair<cv::Point2f, cv::Point2f>> lines;

int main(int argc, char* argv[])
{
	std::cout << "Hello World!\n";
	
	cv::Mat input = cv::imread("demopic.jpg", CV_LOAD_IMAGE_ANYCOLOR);
	if (input.empty()) {
		// 没有读取到图像信息
		std::cout << "Error: can't read image information.\n";
		return 1;
	}
	std::cout << "[INFO] Read Pic info.\n";

#ifdef _DEBUG
	show_picture(input, "sourcepic");
	std::cout << "[DBG] Show source picture.\n";
#endif

	cv::Size pic_size = input.size();
	std::cout << "[INFO] picture size: " << pic_size.width << " * " << pic_size.height << std::endl;

	cv::Mat gray_map;
	gray_map.create(input.size(), CV_32F);
	for (int i = 0; i < gray_map.rows; i++) {
		for (int j = 0; j < gray_map.cols; j++) {
			gray_map.at<float>(i, j) = cv::sqrt(static_cast<float>(
				input.at<cv::Vec3b>(i, j).operator()(0) * input.at<cv::Vec3b>(i, j).operator()(0) +
				input.at<cv::Vec3b>(i, j).operator()(1) * input.at<cv::Vec3b>(i, j).operator()(1) +
				input.at<cv::Vec3b>(i, j).operator()(2) * input.at<cv::Vec3b>(i, j).operator()(2)));
		}
	}

	// 建立一个映射将图片区域映射到[0,1]*[0,1]区域内，通过bind绑定参数实现坐标变换定向
	auto coordinate_mapping_std2scr = std::bind(coordinate_mapping_std2scr_basic,
		std::placeholders::_1, std::placeholders::_2,
		std::placeholders::_3, std::placeholders::_4,
		parper_coordinate_x, parper_coordinate_y);

	// 对图像做gamma变换
	const size_t gamma_level = 4;
	const float gamma_level_param[gamma_level] = { 0.5, 2.0, 8.0, 32.0 };
	cv::Mat gamma_result_level[gamma_level];
	cv::Mat gamma_binarization[gamma_level];
	cv::GaussianBlur(gray_map, gray_map, cv::Size(11, 11), 2, 5);
	constexpr float binarization_threshold = 0.999f;
	std::vector<std::pair<cv::Point2f, cv::Point2f>> lines; // 生成的轨迹
	HDC hdc = CreateDC(TEXT("DISPLAY"), nullptr, nullptr, nullptr);
	for (int i = 0; i < gamma_level; i++) {
		// gamma transformation
		gamma_trans(gray_map, gamma_result_level[i], gamma_level_param[i]);

		gamma_binarization[i].create(gamma_result_level[i].size(), CV_8UC1);
		// 对于gamma变换的结果进行二值化
		for (int k = 0; k < gamma_result_level[i].rows; k++) {
			for (int j = 0; j < gamma_result_level[i].cols; j++) {
				gamma_binarization[i].at<unsigned char>(k, j) =
					gamma_result_level[i].at<float>(k, j) > binarization_threshold ?
					1 : 0;
			}
		}

#ifndef SAVEPIC
		char index[2] = { static_cast<char>(i) + '0',0 };
		std::string gamma_result_filename = "./gamma_result_level";
		gamma_result_filename.append(index);
		gamma_result_filename.append(".jpg");
		cv::Mat write_buffer;
		write_buffer.create(gamma_result_level[i].size(), CV_8UC1);
		for (int k = 0; k < gamma_result_level[i].rows; k++) {
			for (int j = 0; j < gamma_result_level[i].cols; j++) {
				write_buffer.at<unsigned char>(k, j) =
					static_cast<unsigned char>(gamma_result_level[i].at<float>(k, j) * 256);
			}
		}

		cv::imwrite(gamma_result_filename, write_buffer);
		std::string binarization_result_filename = "./gamma_binarization";
		binarization_result_filename.append(index);
		binarization_result_filename.append(".jpg");
		for (int k = 0; k < gamma_result_level[i].rows; k++) {
			for (int j = 0; j < gamma_result_level[i].cols; j++) {
				write_buffer.at<unsigned char>(k, j) = gamma_binarization[i].at<unsigned char>(k, j) * 128;
			}
		}
		cv::imwrite(binarization_result_filename, write_buffer);

		show_picture(gamma_result_level[i], "gamma result");
		show_picture(write_buffer, "gamma_binarization");
		cv::waitKey();
		std::cout << "[DBG] Show gamma picture.\n";
#endif

		// 生成直线轨迹，四种不同的gamma对应不同的直线方向，以实现深浅不同的阴影
		LineSolver(lines, gamma_result_level[i], static_cast<line_type>(i),
			binarization_threshold, min_line_length, sin(pi() / 4), cos(pi() / 4));

		// 生成曲线轨迹


	}

	std::cout << "position(0.1,0.1): " << coordinate_mapping_pic2std(gamma_result_level[0], 0.5, 0.5) << std::endl;

	// 创建测试窗口
	//g_lines = lines; // 这里为窗口显示提供足够的信息
	//std::vector<std::thread> threads;
	//threads.emplace_back(window_create); // 创建窗口

	//for (auto& thread : threads) {
	//	thread.detach(); // 忽略线程的终止条件
	//}

	DrawLinePicture(hdc, lines,
		static_cast<int>(parper_coordinate_x), static_cast<int>(parper_coordinate_y));

	cv::Mat result_bmp = cv::imread("DrawResult.bmp");
	show_picture(result_bmp, "result");

	cv::waitKey();

	DeleteDC(hdc);
	return 0;
}



// 窗口类名称
constexpr LPCWSTR window_class_name = L"MyImageTransformResultWindow";
constexpr LPCWSTR window_name = L"Transformation result";
/*
 * @brief 主窗口回调函数
 * @param 传入回调函数激发条件
 * @notes
 * @fixed
 *
 * @update 2021.07.12
 */
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*
 * @brief 注册窗口类
 * @param 传入当前窗口的实例句柄
 * @notes
 * @fixed
 *
 * @update 2021.07.12
 */
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = window_class_name;
	wcex.hIconSm = NULL;

	return RegisterClassExW(&wcex);
}

constexpr int window_size_x = 1920;
constexpr int window_size_y = 1080;

/*
 * @brief 创建主窗口
 * @param 传入当前实例和当前显示的条件
 * @notes
 * @fixed
 *
 * @update 2021.07.12
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//hInst = hInstance; // 将实例句柄存储在全局变量中

	HWND hWnd = CreateWindowW(window_class_name, window_name, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, window_size_x, window_size_y, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		throw hWnd;
		return FALSE;
	}

	// 计算绘图轨迹
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	//DarwLinePicture(hdc, g_lines, window_size_x, window_size_y);

	EndPaint(hWnd, &ps);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

/*
 * @brief 创建主窗口
 * @param NULL
 * @notes 需要使用一个新的线程运行这个函数
 * @fixed
 *
 * @update 2021.07.12
 */
int window_create()
{
	HINSTANCE hInstance = GetModuleHandle(0);

	MyRegisterClass(hInstance);
	if (!InitInstance(hInstance, SW_SHOW)) {
		throw hInstance;
		//return FALSE;
	}

	MSG msg;

	// 主消息循环
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//return (int)msg.wParam;
}




// 最小二乘法
#include <iostream>
#include <cmath>
using namespace std;
//x[n]       存放给定数据点的X坐标。
//y[n]       存放给定数据点的Y坐标。
//n          给定数据点的个数。
//a[m]       返回m - 1次拟合多项式的系数。
//m          拟合多项式的项数。要求m<=min(n,20)。
//dt[3]      分别返回误差平方和，误差绝对值之和与误差绝对值的最大值。
void pir1(double x[], double y[], int n, double a[], int m, double dt[])
{
	int i, j, k;
	double p, c, g, q, d1, d2, s[20], t[20], b[20];
	for (i = 0; i <= m - 1; i++) a[i] = 0.0;
	if (m > n) m = n;
	if (m > 20) m = 20;
	b[0] = 1.0; d1 = 1.0 * n; p = 0.0; c = 0.0;
	for (i = 0; i <= n - 1; i++)
	{
		p = p + x[i]; c = c + y[i];
	}
	c = c / d1; p = p / d1;
	a[0] = c * b[0];
	if (m > 1)
	{
		t[1] = 1.0; t[0] = -p;
		d2 = 0.0; c = 0.0; g = 0.0;
		for (i = 0; i <= n - 1; i++)
		{
			q = x[i] - p; d2 = d2 + q * q;
			c = c + y[i] * q;
			g = g + x[i] * q * q;
		}
		c = c / d2; p = g / d2; q = d2 / d1;
		d1 = d2;
		a[1] = c * t[1]; a[0] = c * t[0] + a[0];
	}
	for (j = 2; j <= m - 1; j++)
	{
		s[j] = t[j - 1];
		s[j - 1] = -p * t[j - 1] + t[j - 2];
		if (j >= 3)
			for (k = j - 2; k >= 1; k--)
				s[k] = -p * t[k] + t[k - 1] - q * b[k];
		s[0] = -p * t[0] - q * b[0];
		d2 = 0.0; c = 0.0; g = 0.0;
		for (i = 0; i <= n - 1; i++)
		{
			q = s[j];
			for (k = j - 1; k >= 0; k--)  q = q * x[i] + s[k];
			d2 = d2 + q * q; c = c + y[i] * q;
			g = g + x[i] * q * q;
		}
		c = c / d2; p = g / d2; q = d2 / d1;
		d1 = d2;
		a[j] = c * s[j]; t[j] = s[j];
		for (k = j - 1; k >= 0; k--)
		{
			a[k] = c * s[k] + a[k];
			b[k] = t[k]; t[k] = s[k];
		}
	}
	dt[0] = 0.0; dt[1] = 0.0; dt[2] = 0.0;
	for (i = 0; i <= n - 1; i++)
	{
		q = a[m - 1];
		for (k = m - 2; k >= 0; k--) q = a[k] + q * x[i];
		p = q - y[i];
		if (fabs(p) > dt[2]) dt[2] = fabs(p);
		dt[0] = dt[0] + p * p;
		dt[1] = dt[1] + fabs(p);
	}
	return;
}

/*
//最小二乘曲线拟合例
  #include <iostream>
  #include <cmath>
//  #include "最小二乘曲线拟合.cpp"
  using namespace std;
  int main()
  {
	  int i;
	  double x[20],y[20],a[6],dt[3];
	  for (i=0; i<=19; i++)
	  {
		  x[i]=0.1*i; y[i]=x[i]-exp(-x[i]);
	  }
	  pir1(x,y,20,a,6,dt);
	  cout <<"拟合多项式系数: " <<endl;
	  for (i=0; i<=5; i++)
		cout <<"a(" <<i <<") = " <<a[i] <<endl;
	  cout <<"误差平方和 = " <<dt[0] <<endl;
	  cout <<"误差绝对值和 = " <<dt[1] <<endl;
	  cout <<"误差绝对值最大值 = " <<dt[2] <<endl;
	  return 0;
  }
*/

// 光滑插值
//x[n]       存放给定的n个结点值。
//y[n]       存放给定n个结点上的函数值。
//n          给定的结点个数。
//t　　　　　指定的插值点。
//s[4]　　   返回插值点所在子区间上的三次多项式系数。
  //函数返回指定插值点处的函数近似值。
double akima(double x[], double y[], int n, double t, double s[4])
{
	int k, m, j;
	double u[5], p, q;
	s[0] = 0.0; s[1] = 0.0; s[2] = 0.0; s[3] = 0.0;
	if (n < 1) return(0.0);
	if (n == 1) { s[0] = y[0];  return(y[0]); }
	if (n == 2)
	{
		s[0] = y[0]; s[1] = (y[1] - y[0]) / (x[1] - x[0]);
		p = (y[0] * (t - x[1]) - y[1] * (t - x[0])) / (x[0] - x[1]);
		return(p);
	}
	if (t <= x[1]) k = 0;          //确定插值点t所在的子区间
	else if (t >= x[n - 1]) k = n - 2;
	else
	{
		k = 1; m = n - 1;
		while (((k - m) != 1) && ((k - m) != -1))
		{
			j = (k + m) / 2;
			if (t <= x[j]) m = j;
			else k = j;
		}
	}
	u[2] = (y[k + 1] - y[k]) / (x[k + 1] - x[k]);
	if (n == 3)
	{
		if (k == 0)
		{
			u[3] = (y[2] - y[1]) / (x[2] - x[1]);
			u[4] = 2.0 * u[3] - u[2];
			u[1] = 2.0 * u[2] - u[3];
			u[0] = 2.0 * u[1] - u[2];
		}
		else
		{
			u[1] = (y[1] - y[0]) / (x[1] - x[0]);
			u[0] = 2.0 * u[1] - u[2];
			u[3] = 2.0 * u[2] - u[1];
			u[4] = 2.0 * u[3] - u[2];
		}
	}
	else
	{
		if (k <= 1)
		{
			u[3] = (y[k + 2] - y[k + 1]) / (x[k + 2] - x[k + 1]);
			if (k == 1)
			{
				u[1] = (y[1] - y[0]) / (x[1] - x[0]);
				u[0] = 2.0 * u[1] - u[2];
				if (n == 4) u[4] = 2.0 * u[3] - u[2];
				else u[4] = (y[4] - y[3]) / (x[4] - x[3]);
			}
			else
			{
				u[1] = 2.0 * u[2] - u[3];
				u[0] = 2.0 * u[1] - u[2];
				u[4] = (y[3] - y[2]) / (x[3] - x[2]);
			}
		}
		else if (k >= (n - 3))
		{
			u[1] = (y[k] - y[k - 1]) / (x[k] - x[k - 1]);
			if (k == (n - 3))
			{
				u[3] = (y[n - 1] - y[n - 2]) / (x[n - 1] - x[n - 2]);
				u[4] = 2.0 * u[3] - u[2];
				if (n == 4) u[0] = 2.0 * u[1] - u[2];
				else u[0] = (y[k - 1] - y[k - 2]) / (x[k - 1] - x[k - 2]);
			}
			else
			{
				u[3] = 2.0 * u[2] - u[1];
				u[4] = 2.0 * u[3] - u[2];
				u[0] = (y[k - 1] - y[k - 2]) / (x[k - 1] - x[k - 2]);
			}
		}
		else
		{
			u[1] = (y[k] - y[k - 1]) / (x[k] - x[k - 1]);
			u[0] = (y[k - 1] - y[k - 2]) / (x[k - 1] - x[k - 2]);
			u[3] = (y[k + 2] - y[k + 1]) / (x[k + 2] - x[k + 1]);
			u[4] = (y[k + 3] - y[k + 2]) / (x[k + 3] - x[k + 2]);
		}
	}
	s[0] = fabs(u[3] - u[2]);
	s[1] = fabs(u[0] - u[1]);
	if ((s[0] + 1.0 == 1.0) && (s[1] + 1.0 == 1.0))
		p = (u[1] + u[2]) / 2.0;
	else p = (s[0] * u[1] + s[1] * u[2]) / (s[0] + s[1]);
	s[0] = fabs(u[3] - u[4]);
	s[1] = fabs(u[2] - u[1]);
	if ((s[0] + 1.0 == 1.0) && (s[1] + 1.0 == 1.0))
		q = (u[2] + u[3]) / 2.0;
	else q = (s[0] * u[2] + s[1] * u[3]) / (s[0] + s[1]);
	s[0] = y[k];
	s[1] = p;
	s[3] = x[k + 1] - x[k];
	s[2] = (3.0 * u[2] - 2.0 * p - q) / s[3];
	s[3] = (q + p - 2.0 * u[2]) / (s[3] * s[3]);
	p = t - x[k];
	p = s[0] + s[1] * p + s[2] * p * p + s[3] * p * p * p;
	return(p);
}
/*
//光滑插值例
  #include <cmath>
  #include <iostream>
//  #include "光滑插值.cpp"
  using namespace std;
  int main()
  {
	  int i, n;
	  double t, z;
	  double x[11]={-1.0,-0.95,-0.75,-0.55,-0.3,0.0,
						 0.2,0.45,0.6,0.8,1.0};
	  double y[11]={0.0384615,0.0424403,0.06639,0.116788,
		   0.307692,1.0,0.5,0.164948,0.1,0.0588236,0.0384615};
	  double s[4];
	  n=11;
	  for (i=0; i<=10; i++)
	  {
		  t = x[i];
		  z = akima(x,y,n,t,s);
		  cout <<"t = " <<t <<"     z = f(t) = " <<z <<endl;
		  cout <<"s0 = " <<s[0] <<endl;
		  cout <<"s1 = " <<s[1] <<endl;
		  cout <<"s2 = " <<s[2] <<endl;
		  cout <<"s3 = " <<s[3] <<endl;
	  }

	  t=-0.85; z = akima(x,y,n,t,s);
		  cout <<"t = " <<t <<"     z = f(t) = " <<z <<endl;
		  cout <<"s0 = " <<s[0] <<endl;
		  cout <<"s1 = " <<s[1] <<endl;
		  cout <<"s2 = " <<s[2] <<endl;
		  cout <<"s3 = " <<s[3] <<endl;
	  t=0.15; z = akima(x,y,n,t,s);
		  cout <<"t = " <<t <<"     z = f(t) = " <<z <<endl;
		  cout <<"s0 = " <<s[0] <<endl;
		  cout <<"s1 = " <<s[1] <<endl;
		  cout <<"s2 = " <<s[2] <<endl;
		  cout <<"s3 = " <<s[3] <<endl;
	  return 0;
  }
*/

// 三次样条插值
//n            给定结点个数。
//x[n]         存放给定n个结点值。
//y[n]         存放给定n个结点上的函数值。当flag=3时，要求y[0]=y[n-1],dy[0]=dy[n-1],ddy[0]=ddy[n-1]。
//dy[n]        返回给定n个结点上的一阶导数值。当flag=1时，要求dy[0]与dy[n-1]给定。
//ddy[n]       返回给定n个结点上的二阶导数值。当flag=2时，要求ddy[0]与ddy[n-1]给定。
//m            指定插值点个数。
//t[m]         存放定m个插值点值。
//z[m]         返回指定m个插值点上的函数值。
//dz[m]        返回指定m个插值点上的一阶导数值。
//ddz[m]       返回指定m个插值点上的二阶导数值。
//flag         边界条件类型。
  //函数返回积分值。
double splin(int n, double x[], double y[], double dy[], double ddy[],
	int m, double t[], double z[], double dz[], double ddz[], int flag)
{
	int i, j;
	double h0, h1, alpha, beta, g, y0, y1, u, * s;
	s = new double[n];
	//计算给定n个结点上的一阶导数值
	if (flag == 1)              //第一种边界类型
	{
		s[0] = dy[0]; dy[0] = 0.0;
		h0 = x[1] - x[0];
		for (j = 1; j <= n - 2; j++)
		{
			h1 = x[j + 1] - x[j];
			alpha = h0 / (h0 + h1);
			beta = (1.0 - alpha) * (y[j] - y[j - 1]) / h0;
			beta = 3.0 * (beta + alpha * (y[j + 1] - y[j]) / h1);
			dy[j] = -alpha / (2.0 + (1.0 - alpha) * dy[j - 1]);
			s[j] = (beta - (1.0 - alpha) * s[j - 1]);
			s[j] = s[j] / (2.0 + (1.0 - alpha) * dy[j - 1]);
			h0 = h1;
		}
		for (j = n - 2; j >= 0; j--)
			dy[j] = dy[j] * dy[j + 1] + s[j];
	}
	else if (flag == 2)        //第二种边界类型
	{
		dy[0] = -0.5;
		h0 = x[1] - x[0];
		s[0] = 3.0 * (y[1] - y[0]) / (2.0 * h0) - ddy[0] * h0 / 4.0;
		for (j = 1; j <= n - 2; j++)
		{
			h1 = x[j + 1] - x[j];
			alpha = h0 / (h0 + h1);
			beta = (1.0 - alpha) * (y[j] - y[j - 1]) / h0;
			beta = 3.0 * (beta + alpha * (y[j + 1] - y[j]) / h1);
			dy[j] = -alpha / (2.0 + (1.0 - alpha) * dy[j - 1]);
			s[j] = (beta - (1.0 - alpha) * s[j - 1]);
			s[j] = s[j] / (2.0 + (1.0 - alpha) * dy[j - 1]);
			h0 = h1;
		}
		dy[n - 1] = (3.0 * (y[n - 1] - y[n - 2]) / h1 + ddy[n - 1] * h1 /
			2.0 - s[n - 2]) / (2.0 + dy[n - 2]);
		for (j = n - 2; j >= 0; j--)
			dy[j] = dy[j] * dy[j + 1] + s[j];
	}
	else if (flag == 3)       //第三种边界类型
	{
		h0 = x[n - 1] - x[n - 2];
		y0 = y[n - 1] - y[n - 2];
		dy[0] = 0.0; ddy[0] = 0.0; ddy[n - 1] = 0.0;
		s[0] = 1.0; s[n - 1] = 1.0;
		for (j = 1; j <= n - 1; j++)
		{
			h1 = h0; y1 = y0;
			h0 = x[j] - x[j - 1];
			y0 = y[j] - y[j - 1];
			alpha = h1 / (h1 + h0);
			beta = 3.0 * ((1.0 - alpha) * y1 / h1 + alpha * y0 / h0);
			if (j < n - 1)
			{
				u = 2.0 + (1.0 - alpha) * dy[j - 1];
				dy[j] = -alpha / u;
				s[j] = (alpha - 1.0) * s[j - 1] / u;
				ddy[j] = (beta - (1.0 - alpha) * ddy[j - 1]) / u;
			}
		}
		for (j = n - 2; j >= 1; j--)
		{
			s[j] = dy[j] * s[j + 1] + s[j];
			ddy[j] = dy[j] * ddy[j + 1] + ddy[j];
		}
		dy[n - 2] = (beta - alpha * ddy[1] - (1.0 - alpha) * ddy[n - 2]) /
			(alpha * s[1] + (1.0 - alpha) * s[n - 2] + 2.0);
		for (j = 2; j <= n - 1; j++)
			dy[j - 2] = s[j - 1] * dy[n - 2] + ddy[j - 1];
		dy[n - 1] = dy[0];
	}
	else
	{
		cout << "没有这种边界类型！" << endl;
		delete[] s; return(0.0);
	}
	//计算n个给定结点上的二阶导数值
	for (j = 0; j <= n - 2; j++) s[j] = x[j + 1] - x[j];
	for (j = 0; j <= n - 2; j++)
	{
		h1 = s[j] * s[j];
		ddy[j] = 6.0 * (y[j + 1] - y[j]) / h1 - 2.0 * (2.0 * dy[j] + dy[j + 1]) / s[j];
	}
	h1 = s[n - 2] * s[n - 2];
	ddy[n - 1] = 6. * (y[n - 2] - y[n - 1]) / h1 + 2. * (2. * dy[n - 1] + dy[n - 2]) / s[n - 2];
	//计算插值区间上的积分
	g = 0.0;
	for (i = 0; i <= n - 2; i++)
	{
		h1 = 0.5 * s[i] * (y[i] + y[i + 1]);
		h1 = h1 - s[i] * s[i] * s[i] * (ddy[i] + ddy[i + 1]) / 24.0;
		g = g + h1;
	}
	//计算m个指定插值点处的函数值，一阶导数值以及二阶导数值
	for (j = 0; j <= m - 1; j++)
	{
		if (t[j] >= x[n - 1]) i = n - 2;
		else
		{
			i = 0;
			while (t[j] > x[i + 1]) i = i + 1;
		}
		h1 = (x[i + 1] - t[j]) / s[i];
		h0 = h1 * h1;
		z[j] = (3.0 * h0 - 2.0 * h0 * h1) * y[i];
		z[j] = z[j] + s[i] * (h0 - h0 * h1) * dy[i];
		dz[j] = 6.0 * (h0 - h1) * y[i] / s[i];
		dz[j] = dz[j] + (3.0 * h0 - 2.0 * h1) * dy[i];
		ddz[j] = (6.0 - 12.0 * h1) * y[i] / (s[i] * s[i]);
		ddz[j] = ddz[j] + (2.0 - 6.0 * h1) * dy[i] / s[i];
		h1 = (t[j] - x[i]) / s[i];
		h0 = h1 * h1;
		z[j] = z[j] + (3.0 * h0 - 2.0 * h0 * h1) * y[i + 1];
		z[j] = z[j] - s[i] * (h0 - h0 * h1) * dy[i + 1];
		dz[j] = dz[j] - 6.0 * (h0 - h1) * y[i + 1] / s[i];
		dz[j] = dz[j] + (3.0 * h0 - 2.0 * h1) * dy[i + 1];
		ddz[j] = ddz[j] + (6.0 - 12.0 * h1) * y[i + 1] / (s[i] * s[i]);
		ddz[j] = ddz[j] - (2.0 - 6.0 * h1) * dy[i + 1] / s[i];
	}
	delete[] s;
	return(g);
}

/*
//第一种边界条件例
  #include <cmath>
  #include <iostream>
  #include <iomanip>
//  #include "三次样条函数插值微商与积分.cpp"
  using namespace std;
  int main()
  {
	  int n,m,i;
	  double s;
	  double dy[12],ddy[12],z[8],dz[8],ddz[8];
	  double x[12]={0.52,8.0,17.95,28.65,50.65,104.6,
					   156.6,260.7,364.4,468.0,507.0,520.0};
	  double y[12]={5.28794,13.84,20.2,24.9,31.1,36.5,
						 36.6,31.0,20.9,7.8,1.5,0.2};
	  double t[8]={4.0,14.0,30.0,60.0,130.0,230.0,
						450.0,515.0};
	  dy[0]=1.86548; dy[11]=-0.046115;
	  n=12; m=8;
	  s = splin(n, x, y, dy, ddy, m, t, z, dz, ddz, 1);
	  cout <<setw(15) <<"x[i]" <<setw(15) <<"y[i]" <<setw(15)
		   <<"dy[i]" <<setw(15) <<"ddy[i]" <<endl;
	  for (i=0;i<=11;i++)
		  cout <<setw(15) <<x[i] <<setw(15) <<y[i] <<setw(15)
			   <<dy[i] <<setw(15) <<ddy[i] <<endl;
	  cout <<"s = " <<s <<endl;
	  cout <<setw(15) <<"t[i]" <<setw(15) <<"z[i]" <<setw(15)
		   <<"dz[i]" <<setw(15) <<"ddz[i]" <<endl;
	  for (i=0;i<=7;i++)
		  cout <<setw(15) <<t[i] <<setw(15) <<z[i] <<setw(15)
			   <<dz[i] <<setw(15) <<ddz[i] <<endl;
	  return 0;
  }
*/
/*
//第二种边界条件例
  #include <cmath>
  #include <iostream>
  #include <iomanip>
//  #include "三次样条函数插值微商与积分.cpp"
  using namespace std;
  int main()
  {
	  int n,m,i;
	  double s;
	  double dy[12],ddy[12],z[8],dz[8],ddz[8];
	  double x[12]={0.52,8.0,17.95,28.65,50.65,104.6,
					   156.6,260.7,364.4,468.0,507.0,520.0};
	  double y[12]={5.28794,13.84,20.2,24.9,31.1,36.5,
						 36.6,31.0,20.9,7.8,1.5,0.2};
	  double t[8]={4.0,14.0,30.0,60.0,130.0,230.0,
						450.0,515.0};
	  ddy[0]=-0.279319; ddy[11]=0.011156;
	  n=12; m=8;
	  s = splin(n, x, y, dy, ddy, m, t, z, dz, ddz, 2);
	  cout <<setw(15) <<"x[i]" <<setw(15) <<"y[i]" <<setw(15)
		   <<"dy[i]" <<setw(15) <<"ddy[i]" <<endl;
	  for (i=0;i<=11;i++)
		  cout <<setw(15) <<x[i] <<setw(15) <<y[i] <<setw(15)
			   <<dy[i] <<setw(15) <<ddy[i] <<endl;
	  cout <<"s = " <<s <<endl;
	  cout <<setw(15) <<"t[i]" <<setw(15) <<"z[i]" <<setw(15)
		   <<"dz[i]" <<setw(15) <<"ddz[i]" <<endl;
	  for (i=0;i<=7;i++)
		  cout <<setw(15) <<t[i] <<setw(15) <<z[i] <<setw(15)
			   <<dz[i] <<setw(15) <<ddz[i] <<endl;
	  return 0;
  }
*/
/*
//第三种边界条件例
  #include <cmath>
  #include <iostream>
  #include <iomanip>
//  #include "三次样条函数插值微商与积分.cpp"
  using namespace std;
  int main()
  {
	  int n,m,i,j;
	  double u,s;
	  double x[37],y[37],dy[37],ddy[37];
	  double t[36],z[36],dz[36],ddz[36];
	  for (i=0;i<=36;i++)
	  {
		  x[i]=i*6.2831852/36.0; y[i]=sin(x[i]);
	  }
	  for (i=0;i<=35;i++)  t[i]=(0.5+i)*6.2831852/36.0;
	  n=37; m=36;
	  s = splin(n, x, y, dy, ddy, m, t, z, dz, ddz, 3);
	  cout <<setw(15) <<"x[i]" <<setw(15) <<"y[i]=sin(x)" <<setw(15)
		   <<"dy[i]" <<setw(15) <<"ddy[i]" <<endl;
	  cout <<setw(15) <<x[0] <<setw(15) <<y[0] <<setw(15)
		   <<dy[0] <<setw(15) <<ddy[0] <<endl;
	  for (i=0;i<=35;i++)
	  {
		  u=t[i]*36.0/0.62831852;       //弧度化为度
		  cout <<setw(15) <<u <<setw(15) <<z[i] <<setw(15)
			   <<dz[i] <<setw(15) <<ddz[i] <<endl;
		  u=x[i+1]*36.0/0.62831852;     //弧度化为度
		  j=i+1;
		  cout <<setw(15) <<u <<setw(15) <<y[j] <<setw(15)
			   <<dy[j] <<setw(15) <<ddy[j] <<endl;
	  }
	  cout <<"s = " <<s <<endl;
	  return 0;
  }
*/

// 五点三次平滑
//n          等距观测点数。
//y[n]       存放n个等距观测点上的观测数据。
//yy[n]      返回n个等距观测点上的平滑结果。
void kspt(int n, double y[], double yy[])
{
	int i;
	if (n < 5)
	{
		for (i = 0; i <= n - 1; i++) yy[i] = y[i];
	}
	else
	{
		yy[0] = 69.0 * y[0] + 4.0 * y[1] - 6.0 * y[2] + 4.0 * y[3] - y[4];
		yy[0] = yy[0] / 70.0;
		yy[1] = 2.0 * y[0] + 27.0 * y[1] + 12.0 * y[2] - 8.0 * y[3];
		yy[1] = (yy[1] + 2.0 * y[4]) / 35.0;
		for (i = 2; i <= n - 3; i++)
		{
			yy[i] = -3.0 * y[i - 2] + 12.0 * y[i - 1] + 17.0 * y[i];
			yy[i] = (yy[i] + 12.0 * y[i + 1] - 3.0 * y[i + 2]) / 35.0;
		}
		yy[n - 2] = 2.0 * y[n - 5] - 8.0 * y[n - 4] + 12.0 * y[n - 3];
		yy[n - 2] = (yy[n - 2] + 27.0 * y[n - 2] + 2.0 * y[n - 1]) / 35.0;
		yy[n - 1] = -y[n - 5] + 4.0 * y[n - 4] - 6.0 * y[n - 3];
		yy[n - 1] = (yy[n - 1] + 4.0 * y[n - 2] + 69.0 * y[n - 1]) / 70.0;
	}
	return;
}
/*
//五点三次平滑例
  #include <cmath>
  #include <iostream>
  #include <iomanip>
//  #include "五点三次平滑.cpp"
  using namespace std;
  int main()
  {
	  int i;
	  double y[9]={54.0,145.0,227.0,359.0,401.0,
						342.0,259.0,112.0,65.0};
	  double yy[9];
	  kspt(9,y,yy);
	  for (i=0; i<=8; i++)
		  cout <<"y(" <<i <<")=" <<setw(6) <<y[i]
			   <<"          yy(" <<i <<")=" <<setw(10) <<yy[i] <<endl;
	  return 0;
  }
*/

// 最佳一致逼近的里米兹方法
//a         区间左端点值。
//b         区间右端点值。
//p[n]      返回n - 1次最佳一致逼近多项式的系数。
//n         n - 1次最佳一致逼近多项式的项数。
//eps       控制精度要求。
//f         计算f(x)函数值的函数名。
  //函数返回偏差绝对值。
double remz(double a, double b, double p[], int n, double eps, double (*f)(double))
{
	int i, j, k, m;
	double x[21], g[21], pp[21], d, t, u, s, xx, x0, h, yy;
	if (n > 20) n = 20;
	m = n + 1; d = 1.0e+35;
	for (k = 0; k <= n; k++)      //初始点集
	{
		t = cos((n - k) * 3.1415926 / (1.0 * n));
		x[k] = (b + a + (b - a) * t) / 2.0;
	}
	while (1 == 1)
	{
		u = 1.0;
		for (i = 0; i <= m - 1; i++)
		{
			pp[i] = (*f)(x[i]);  g[i] = -u; u = -u;
		}
		for (j = 0; j <= n - 1; j++)
		{
			k = m; s = pp[k - 1]; xx = g[k - 1];
			for (i = j; i <= n - 1; i++)
			{
				t = pp[n - i + j - 1]; x0 = g[n - i + j - 1];
				pp[k - 1] = (s - t) / (x[k - 1] - x[m - i - 2]);
				g[k - 1] = (xx - x0) / (x[k - 1] - x[m - i - 2]);
				k = n - i + j; s = t; xx = x0;
			}
		}
		u = -pp[m - 1] / g[m - 1];
		for (i = 0; i <= m - 1; i++) pp[i] = pp[i] + g[i] * u;
		for (j = 1; j <= n - 1; j++)
		{
			k = n - j; h = x[k - 1]; s = pp[k - 1];
			for (i = m - j; i <= n; i++)
			{
				t = pp[i - 1]; pp[k - 1] = s - h * t;  s = t; k = i;
			}
		}
		pp[m - 1] = fabs(u); u = pp[m - 1];
		if (fabs(u - d) <= eps)
		{
			for (i = 0; i < n; i++)  p[i] = pp[i];
			return(u);
		}
		d = u; h = 0.1 * (b - a) / (1.0 * n);
		xx = a; x0 = a;
		while (x0 <= b)
		{
			s = (*f)(x0); t = pp[n - 1];
			for (i = n - 2; i >= 0; i--)  t = t * x0 + pp[i];
			s = fabs(s - t);
			if (s > u) { u = s; xx = x0; }
			x0 = x0 + h;
		}
		s = (*f)(xx); t = pp[n - 1];
		for (i = n - 2; i >= 0; i--) t = t * xx + pp[i];
		yy = s - t; i = 1; j = n + 1;
		while ((j - i) != 1)
		{
			k = (i + j) / 2;
			if (xx < x[k - 1]) j = k;
			else i = k;
		}
		if (xx < x[0])
		{
			s = (*f)(x[0]); t = pp[n - 1];
			for (k = n - 2; k >= 0; k--) t = t * x[0] + pp[k];
			s = s - t;
			if (s * yy > 0.0) x[0] = xx;
			else
			{
				for (k = n - 1; k >= 0; k--)  x[k + 1] = x[k];
				x[0] = xx;
			}
		}
		else
		{
			if (xx > x[n])
			{
				s = (*f)(x[n]); t = pp[n - 1];
				for (k = n - 2; k >= 0; k--)  t = t * x[n] + pp[k];
				s = s - t;
				if (s * yy > 0.0) x[n] = xx;
				else
				{
					for (k = 0; k <= n - 1; k++) x[k] = x[k + 1];
					x[n] = xx;
				}
			}
			else
			{
				i = i - 1; j = j - 1;
				s = (*f)(x[i]); t = pp[n - 1];
				for (k = n - 2; k >= 0; k--) t = t * x[i] + pp[k];
				s = s - t;
				if (s * yy > 0.0) x[i] = xx;
				else x[j] = xx;
			}
		}
	}
}

/*
//Remez算法例
  #include <cmath>
  #include <iostream>
//  #include "Remez算法.cpp"
  using namespace std;
  int main()
  {
	  int i;
	  double a,b,eps,p[4], u;
	  double remzf(double);
	  a=-1.0; b=1.0; eps=1.0e-10;
	  u = remz(a,b,p,4,eps,remzf);
	  cout <<"最佳一致逼近多项式系数 :" <<endl;
	  for (i=0; i<=3; i++)
		 cout <<"p(" <<i <<") = " <<p[i] <<endl;
	  cout <<"偏差绝对值 = " <<u <<endl;
	  return 0;
  }

  double remzf(double x)
  {
	  return(exp(x));
  }
*/



