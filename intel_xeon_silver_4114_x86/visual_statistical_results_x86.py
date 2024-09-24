#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import pandas as pd # csv library
import matplotlib
matplotlib.use('Agg')  # 使用无图形界面的后端
import matplotlib.pyplot as plt # plot library
import numpy as np # array library

# 移位平均算法
window_size = 6
# 移位平均算法类
def moving_average(data, window_size):
    moving_averages = []
    window = []

    for i, val in enumerate(data):
        window.append(val)

        if len(window) > window_size:
            window.pop(0)  # 删除最旧的数据点

        moving_avg = sum(window) / len(window)
        moving_averages.append(moving_avg)

    return moving_averages

# 设置文件路径app and config
appConfig = 'redis-ctrlableAutoNuma'
out_put_path='/home/liujianguo/01-auto-numa-test-20240117/11-redis-test-20240411/01-redis-t1-numactl-64G-MRR55percent-20240412/'
file_path = out_put_path + 'ctrlableAutoNumastatisticalResult.csv'

# 读取 CSV 文件
data = pd.read_csv(file_path)

# 假设 CSV 文件中有两列数据，你可以根据需要选择特定的列来绘制图像
x = np.linspace(1, data.shape[0], data.shape[0], dtype=int)  # 替换为你想要作为 x 轴的列名
# y = data.iloc[:,1].values  # 替换为你想要作为 y 轴的列名

# IMC图像绘制
# IMC总体情况绘制
node0_IMC_CAS_COUNT_RD_WR = data['node0/uncore_imc_0/CAS_COUNT/RD'].values + data['node0/uncore_imc_0/CAS_COUNT/WR'].values + \
    data['node0/uncore_imc_1/CAS_COUNT/RD'].values + data['node0/uncore_imc_1/CAS_COUNT/WR'].values + \
        data['node0/uncore_imc_2/CAS_COUNT/RD'].values + data['node0/uncore_imc_2/CAS_COUNT/WR'].values + \
            data['node0/uncore_imc_3/CAS_COUNT/RD'].values + data['node0/uncore_imc_3/CAS_COUNT/WR'].values + \
                data['node0/uncore_imc_4/CAS_COUNT/RD'].values + data['node0/uncore_imc_4/CAS_COUNT/WR'].values + \
                    data['node0/uncore_imc_5/CAS_COUNT/RD'].values + data['node0/uncore_imc_5/CAS_COUNT/WR'].values

node1_IMC_CAS_COUNT_RD_WR = data['node1/uncore_imc_0/CAS_COUNT/RD'].values + data['node1/uncore_imc_0/CAS_COUNT/WR'].values + \
    data['node1/uncore_imc_1/CAS_COUNT/RD'].values + data['node1/uncore_imc_1/CAS_COUNT/WR'].values + \
        data['node1/uncore_imc_2/CAS_COUNT/RD'].values + data['node1/uncore_imc_2/CAS_COUNT/WR'].values + \
            data['node1/uncore_imc_3/CAS_COUNT/RD'].values + data['node1/uncore_imc_3/CAS_COUNT/WR'].values + \
                data['node1/uncore_imc_4/CAS_COUNT/RD'].values + data['node1/uncore_imc_4/CAS_COUNT/WR'].values + \
                    data['node1/uncore_imc_5/CAS_COUNT/RD'].values + data['node1/uncore_imc_5/CAS_COUNT/WR'].values


# 平滑数据
ma_node0_IMC_CAS_COUNT_RD_WR = moving_average(node0_IMC_CAS_COUNT_RD_WR, window_size)
ma_node1_IMC_CAS_COUNT_RD_WR = moving_average(node1_IMC_CAS_COUNT_RD_WR, window_size)

# 创建图像
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）
plt.plot(x, ma_node0_IMC_CAS_COUNT_RD_WR, linestyle='-', label='node0_IMC_CAS_COUNT_RD_WR')
plt.plot(x, ma_node1_IMC_CAS_COUNT_RD_WR, linestyle='-', label='node1_IMC_CAS_COUNT_RD_WR')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('IMC_CAS_COUNT_RD_WR(couters per second)')  # 设置 y 轴标签
plt.title(appConfig + ' - IMC_CAS_COUNT_RD_WR #17')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例

# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(out_put_path + appConfig + '-IMC_CAS_COUNT_RD_WR.png')  # 替换为你想要保存的文件名和格式

# 关闭图像
plt.close()

# 创建第二张图
# IMC 读写比例
plt.figure(2)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）
node0_IMC_CAS_COUNT_RD = data['node0/uncore_imc_0/CAS_COUNT/RD'].values + \
    data['node0/uncore_imc_1/CAS_COUNT/RD'].values + \
        data['node0/uncore_imc_2/CAS_COUNT/RD'].values + \
            data['node0/uncore_imc_3/CAS_COUNT/RD'].values + \
                data['node0/uncore_imc_4/CAS_COUNT/RD'].values + \
                    data['node0/uncore_imc_5/CAS_COUNT/RD'].values

node1_IMC_CAS_COUNT_RD = data['node1/uncore_imc_0/CAS_COUNT/RD'].values + \
    data['node1/uncore_imc_1/CAS_COUNT/RD'].values + \
        data['node1/uncore_imc_2/CAS_COUNT/RD'].values + \
            data['node1/uncore_imc_3/CAS_COUNT/RD'].values + \
                data['node1/uncore_imc_4/CAS_COUNT/RD'].values + \
                    data['node1/uncore_imc_5/CAS_COUNT/RD'].values


node0_IMC_CAS_RD_PERCENT = (node0_IMC_CAS_COUNT_RD / list(map(float, node0_IMC_CAS_COUNT_RD_WR))) * 100
node1_IMC_CAS_RD_PERCENT = (node1_IMC_CAS_COUNT_RD / list(map(float, node1_IMC_CAS_COUNT_RD_WR))) * 100

# 平滑数据
ma_node0_IMC_CAS_RD_PERCENT = moving_average(node0_IMC_CAS_RD_PERCENT, window_size)
ma_node1_IMC_CAS_RD_PERCENT = moving_average(node1_IMC_CAS_RD_PERCENT, window_size)

# 绘制图像
plt.plot(x, ma_node0_IMC_CAS_RD_PERCENT, linestyle='-', label='node0_IMC_CAS_RD_PERCENT')
plt.plot(x, ma_node1_IMC_CAS_RD_PERCENT, linestyle='-', label='node1_IMC_CAS_RD_PERCENT')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('percent(%)')  # 设置 y 轴标签
plt.title(appConfig + ' - IMC_CAS_RD_PERCENT #17')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(out_put_path + appConfig + '-IMC_CAS_RD_PERCENT.png')  # 替换为你想要保存的文件名和格式

# 关闭图像
plt.close()

# UPI统计绘制
# 创建第三张图
plt.figure(3)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

node0_UPI_TxL_FLITS = data['node0/uncore_upi_0/TxL_FLITS'].values + data['node0/uncore_upi_1/TxL_FLITS'].values
node0_UPI_RxL_FLITS = data['node0/uncore_upi_0/RxL_FLITS'].values + data['node0/uncore_upi_1/RxL_FLITS'].values
node1_UPI_TxL_FLITS = data['node1/uncore_upi_0/TxL_FLITS'].values + data['node1/uncore_upi_1/TxL_FLITS'].values
node1_UPI_RxL_FLITS = data['node1/uncore_upi_0/RxL_FLITS'].values + data['node1/uncore_upi_1/RxL_FLITS'].values

# 平滑数据
ma_node0_UPI_TxL_FLITS = moving_average(node0_UPI_TxL_FLITS, window_size)
ma_node0_UPI_RxL_FLITS = moving_average(node0_UPI_RxL_FLITS, window_size)
ma_node1_UPI_TxL_FLITS = moving_average(node1_UPI_TxL_FLITS, window_size)
ma_node1_UPI_RxL_FLITS = moving_average(node1_UPI_RxL_FLITS, window_size)

plt.plot(x, ma_node0_UPI_TxL_FLITS, linestyle='-', label='ma_node0_UPI_TxL_FLITS')
plt.plot(x, ma_node0_UPI_RxL_FLITS, linestyle='-', label='ma_node0_UPI_RxL_FLITS')
plt.plot(x, ma_node1_UPI_TxL_FLITS, linestyle='-', label='ma_node1_UPI_TxL_FLITS')
plt.plot(x, ma_node1_UPI_RxL_FLITS, linestyle='-', label='ma_node1_UPI_RxL_FLITS')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('counts per second')  # 设置 y 轴标签
plt.title(appConfig + ' - UPI TxL/RxL_FLITS #3')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(out_put_path + appConfig + '-UPI_TxL_RxL_FLITS.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# CHA LLC_LOOKUP
# 创建第五张图
plt.figure(5)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

# node 0
node0_CHA_LLC_LOOKUP_ANY = data['node0/uncore_cha_0/LLC_LOOKUP/ANY'].values + \
    data['node0/uncore_cha_1/LLC_LOOKUP/ANY'].values + \
        data['node0/uncore_cha_2/LLC_LOOKUP/ANY'].values + \
            data['node0/uncore_cha_3/LLC_LOOKUP/ANY'].values + \
                data['node0/uncore_cha_4/LLC_LOOKUP/ANY'].values + \
                    data['node0/uncore_cha_5/LLC_LOOKUP/ANY'].values + \
                        data['node0/uncore_cha_6/LLC_LOOKUP/ANY'].values + \
                            data['node0/uncore_cha_7/LLC_LOOKUP/ANY'].values + \
                                data['node0/uncore_cha_9/LLC_LOOKUP/ANY'].values

node0_CHA_LLC_LOOKUP_ANY_HIT = data['node0/uncore_cha_0/LLC_LOOKUP/ANY/HIT'].values + \
    data['node0/uncore_cha_1/LLC_LOOKUP/ANY/HIT'].values + \
        data['node0/uncore_cha_2/LLC_LOOKUP/ANY/HIT'].values + \
            data['node0/uncore_cha_3/LLC_LOOKUP/ANY/HIT'].values + \
                data['node0/uncore_cha_4/LLC_LOOKUP/ANY/HIT'].values + \
                    data['node0/uncore_cha_5/LLC_LOOKUP/ANY/HIT'].values + \
                        data['node0/uncore_cha_6/LLC_LOOKUP/ANY/HIT'].values + \
                            data['node0/uncore_cha_7/LLC_LOOKUP/ANY/HIT'].values + \
                                data['node0/uncore_cha_8/LLC_LOOKUP/ANY/HIT'].values + \
                                    data['node0/uncore_cha_9/LLC_LOOKUP/ANY/HIT'].values

node0_CHA_LLC_LOOKUP_ANY_MISS = data['node0/uncore_cha_0/LLC_LOOKUP/ANY/MISS'].values + \
    data['node0/uncore_cha_1/LLC_LOOKUP/ANY/MISS'].values + \
        data['node0/uncore_cha_2/LLC_LOOKUP/ANY/MISS'].values + \
            data['node0/uncore_cha_3/LLC_LOOKUP/ANY/MISS'].values + \
                data['node0/uncore_cha_4/LLC_LOOKUP/ANY/MISS'].values + \
                    data['node0/uncore_cha_5/LLC_LOOKUP/ANY/MISS'].values + \
                        data['node0/uncore_cha_6/LLC_LOOKUP/ANY/MISS'].values + \
                            data['node0/uncore_cha_7/LLC_LOOKUP/ANY/MISS'].values + \
                                data['node0/uncore_cha_8/LLC_LOOKUP/ANY/MISS'].values + \
                                    data['node0/uncore_cha_9/LLC_LOOKUP/ANY/MISS'].values

node0_CHA_LLC_LOOKUP_DATA_READ_WRITE = data['node0/uncore_cha_0/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_0/LLC_LOOKUP/WRITE'].values + \
    data['node0/uncore_cha_1/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_1/LLC_LOOKUP/WRITE'].values + \
        data['node0/uncore_cha_2/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_2/LLC_LOOKUP/WRITE'].values + \
            data['node0/uncore_cha_3/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_3/LLC_LOOKUP/WRITE'].values + \
                data['node0/uncore_cha_4/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_4/LLC_LOOKUP/WRITE'].values + \
                    data['node0/uncore_cha_5/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_5/LLC_LOOKUP/WRITE'].values + \
                        data['node0/uncore_cha_6/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_6/LLC_LOOKUP/WRITE'].values + \
                            data['node0/uncore_cha_7/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_7/LLC_LOOKUP/WRITE'].values + \
                                data['node0/uncore_cha_8/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_8/LLC_LOOKUP/WRITE'].values + \
                                    data['node0/uncore_cha_9/LLC_LOOKUP/DATA_READ'].values + data['node0/uncore_cha_9/LLC_LOOKUP/WRITE'].values

node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT = data['node0/uncore_cha_0/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_0/LLC_LOOKUP/WRITE/HIT'].values + \
    data['node0/uncore_cha_1/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_1/LLC_LOOKUP/WRITE/HIT'].values + \
        data['node0/uncore_cha_2/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_2/LLC_LOOKUP/WRITE/HIT'].values + \
            data['node0/uncore_cha_3/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_3/LLC_LOOKUP/WRITE/HIT'].values + \
                data['node0/uncore_cha_4/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_4/LLC_LOOKUP/WRITE/HIT'].values + \
                    data['node0/uncore_cha_5/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_5/LLC_LOOKUP/WRITE/HIT'].values + \
                        data['node0/uncore_cha_6/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_6/LLC_LOOKUP/WRITE/HIT'].values + \
                            data['node0/uncore_cha_7/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_7/LLC_LOOKUP/WRITE/HIT'].values + \
                                data['node0/uncore_cha_8/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_8/LLC_LOOKUP/WRITE/HIT'].values + \
                                    data['node0/uncore_cha_9/LLC_LOOKUP/DATA_READ/HIT'].values + data['node0/uncore_cha_9/LLC_LOOKUP/WRITE/HIT'].values

node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_MISS = data['node0/uncore_cha_0/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_0/LLC_LOOKUP/WRITE/MISS'].values + \
    data['node0/uncore_cha_1/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_1/LLC_LOOKUP/WRITE/MISS'].values + \
        data['node0/uncore_cha_2/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_2/LLC_LOOKUP/WRITE/MISS'].values + \
            data['node0/uncore_cha_3/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_3/LLC_LOOKUP/WRITE/MISS'].values + \
                data['node0/uncore_cha_4/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_4/LLC_LOOKUP/WRITE/MISS'].values + \
                    data['node0/uncore_cha_5/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_5/LLC_LOOKUP/WRITE/MISS'].values + \
                        data['node0/uncore_cha_6/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_6/LLC_LOOKUP/WRITE/MISS'].values + \
                            data['node0/uncore_cha_7/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_7/LLC_LOOKUP/WRITE/MISS'].values + \
                                data['node0/uncore_cha_8/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_8/LLC_LOOKUP/WRITE/MISS'].values + \
                                    data['node0/uncore_cha_9/LLC_LOOKUP/DATA_READ/MISS'].values + data['node0/uncore_cha_9/LLC_LOOKUP/WRITE/MISS'].values

node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS = node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT + node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_MISS
# node 1
node1_CHA_LLC_LOOKUP_ANY = data['node1/uncore_cha_0/LLC_LOOKUP/ANY'].values + \
    data['node1/uncore_cha_1/LLC_LOOKUP/ANY'].values + \
        data['node1/uncore_cha_2/LLC_LOOKUP/ANY'].values + \
            data['node1/uncore_cha_3/LLC_LOOKUP/ANY'].values + \
                data['node1/uncore_cha_4/LLC_LOOKUP/ANY'].values + \
                    data['node1/uncore_cha_5/LLC_LOOKUP/ANY'].values + \
                        data['node1/uncore_cha_6/LLC_LOOKUP/ANY'].values + \
                            data['node1/uncore_cha_7/LLC_LOOKUP/ANY'].values + \
                                data['node1/uncore_cha_9/LLC_LOOKUP/ANY'].values

node1_CHA_LLC_LOOKUP_ANY_HIT = data['node1/uncore_cha_0/LLC_LOOKUP/ANY/HIT'].values + \
    data['node1/uncore_cha_1/LLC_LOOKUP/ANY/HIT'].values + \
        data['node1/uncore_cha_2/LLC_LOOKUP/ANY/HIT'].values + \
            data['node1/uncore_cha_3/LLC_LOOKUP/ANY/HIT'].values + \
                data['node1/uncore_cha_4/LLC_LOOKUP/ANY/HIT'].values + \
                    data['node1/uncore_cha_5/LLC_LOOKUP/ANY/HIT'].values + \
                        data['node1/uncore_cha_6/LLC_LOOKUP/ANY/HIT'].values + \
                            data['node1/uncore_cha_7/LLC_LOOKUP/ANY/HIT'].values + \
                                data['node1/uncore_cha_8/LLC_LOOKUP/ANY/HIT'].values + \
                                    data['node1/uncore_cha_9/LLC_LOOKUP/ANY/HIT'].values

node1_CHA_LLC_LOOKUP_ANY_MISS = data['node1/uncore_cha_0/LLC_LOOKUP/ANY/MISS'].values + \
    data['node1/uncore_cha_1/LLC_LOOKUP/ANY/MISS'].values + \
        data['node1/uncore_cha_2/LLC_LOOKUP/ANY/MISS'].values + \
            data['node1/uncore_cha_3/LLC_LOOKUP/ANY/MISS'].values + \
                data['node1/uncore_cha_4/LLC_LOOKUP/ANY/MISS'].values + \
                    data['node1/uncore_cha_5/LLC_LOOKUP/ANY/MISS'].values + \
                        data['node1/uncore_cha_6/LLC_LOOKUP/ANY/MISS'].values + \
                            data['node1/uncore_cha_7/LLC_LOOKUP/ANY/MISS'].values + \
                                data['node1/uncore_cha_8/LLC_LOOKUP/ANY/MISS'].values + \
                                    data['node1/uncore_cha_9/LLC_LOOKUP/ANY/MISS'].values

node1_CHA_LLC_LOOKUP_DATA_READ_WRITE = data['node1/uncore_cha_0/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_0/LLC_LOOKUP/WRITE'].values + \
    data['node1/uncore_cha_1/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_1/LLC_LOOKUP/WRITE'].values + \
        data['node1/uncore_cha_2/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_2/LLC_LOOKUP/WRITE'].values + \
            data['node1/uncore_cha_3/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_3/LLC_LOOKUP/WRITE'].values + \
                data['node1/uncore_cha_4/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_4/LLC_LOOKUP/WRITE'].values + \
                    data['node1/uncore_cha_5/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_5/LLC_LOOKUP/WRITE'].values + \
                        data['node1/uncore_cha_6/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_6/LLC_LOOKUP/WRITE'].values + \
                            data['node1/uncore_cha_7/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_7/LLC_LOOKUP/WRITE'].values + \
                                data['node1/uncore_cha_8/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_8/LLC_LOOKUP/WRITE'].values + \
                                    data['node1/uncore_cha_9/LLC_LOOKUP/DATA_READ'].values + data['node1/uncore_cha_9/LLC_LOOKUP/WRITE'].values

node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT = data['node1/uncore_cha_0/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_0/LLC_LOOKUP/WRITE/HIT'].values + \
    data['node1/uncore_cha_1/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_1/LLC_LOOKUP/WRITE/HIT'].values + \
        data['node1/uncore_cha_2/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_2/LLC_LOOKUP/WRITE/HIT'].values + \
            data['node1/uncore_cha_3/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_3/LLC_LOOKUP/WRITE/HIT'].values + \
                data['node1/uncore_cha_4/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_4/LLC_LOOKUP/WRITE/HIT'].values + \
                    data['node1/uncore_cha_5/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_5/LLC_LOOKUP/WRITE/HIT'].values + \
                        data['node1/uncore_cha_6/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_6/LLC_LOOKUP/WRITE/HIT'].values + \
                            data['node1/uncore_cha_7/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_7/LLC_LOOKUP/WRITE/HIT'].values + \
                                data['node1/uncore_cha_8/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_8/LLC_LOOKUP/WRITE/HIT'].values + \
                                    data['node1/uncore_cha_9/LLC_LOOKUP/DATA_READ/HIT'].values + data['node1/uncore_cha_9/LLC_LOOKUP/WRITE/HIT'].values

node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_MISS = data['node1/uncore_cha_0/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_0/LLC_LOOKUP/WRITE/MISS'].values + \
    data['node1/uncore_cha_1/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_1/LLC_LOOKUP/WRITE/MISS'].values + \
        data['node1/uncore_cha_2/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_2/LLC_LOOKUP/WRITE/MISS'].values + \
            data['node1/uncore_cha_3/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_3/LLC_LOOKUP/WRITE/MISS'].values + \
                data['node1/uncore_cha_4/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_4/LLC_LOOKUP/WRITE/MISS'].values + \
                    data['node1/uncore_cha_5/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_5/LLC_LOOKUP/WRITE/MISS'].values + \
                        data['node1/uncore_cha_6/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_6/LLC_LOOKUP/WRITE/MISS'].values + \
                            data['node1/uncore_cha_7/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_7/LLC_LOOKUP/WRITE/MISS'].values + \
                                data['node1/uncore_cha_8/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_8/LLC_LOOKUP/WRITE/MISS'].values + \
                                    data['node1/uncore_cha_9/LLC_LOOKUP/DATA_READ/MISS'].values + data['node1/uncore_cha_9/LLC_LOOKUP/WRITE/MISS'].values

node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS = node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT + node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_MISS
# 总体命中率，单位%
node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT = (node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT / list(map(float, (node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT + node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_MISS)))) * 100
node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT = (node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT / list(map(float, (node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT + node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_MISS)))) * 100
node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2 = (node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT / list(map(float, (node0_CHA_LLC_LOOKUP_DATA_READ_WRITE)))) * 100
node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2 = (node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT / list(map(float, (node1_CHA_LLC_LOOKUP_DATA_READ_WRITE)))) * 100
node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT = (node0_CHA_LLC_LOOKUP_ANY_HIT / list(map(float, (node0_CHA_LLC_LOOKUP_ANY_HIT + node0_CHA_LLC_LOOKUP_ANY_MISS)))) * 100
node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT = (node1_CHA_LLC_LOOKUP_ANY_HIT / list(map(float, (node1_CHA_LLC_LOOKUP_ANY_HIT + node1_CHA_LLC_LOOKUP_ANY_MISS)))) * 100
node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2 = (node0_CHA_LLC_LOOKUP_ANY_HIT / list(map(float, (node0_CHA_LLC_LOOKUP_ANY)))) * 100
node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2 = (node1_CHA_LLC_LOOKUP_ANY_HIT / list(map(float, (node1_CHA_LLC_LOOKUP_ANY)))) * 100
# 平滑数据
ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT = moving_average(node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT, window_size)
ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT = moving_average(node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT, window_size)
ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2 = moving_average(node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2, window_size)
ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2 = moving_average(node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2, window_size)
ma_node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT = moving_average(node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT, window_size)
ma_node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT = moving_average(node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT, window_size)
ma_node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2 = moving_average(node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2, window_size)
ma_node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2 = moving_average(node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2, window_size)

# 绘制图像
plt.plot(x, ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT, linestyle='-', label='node0')
# plt.plot(x, ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2, linestyle='-', label='node0-v2')
# plt.plot(x, ma_node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT, linestyle='-', label='node0-ANY')
# plt.plot(x, ma_node0_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2, linestyle='-', label='node0-ANY-v2')
plt.plot(x, ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT, linestyle='-', label='node1')
# plt.plot(x, ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT_2, linestyle='-', label='node1-v2')
# plt.plot(x, ma_node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT, linestyle='-', label='node1-ANY')
# plt.plot(x, ma_node1_CHA_LLC_LOOKUP_ANY_HIT_PERCENT_2, linestyle='-', label='node1-ANY-v2')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('percent(%)')  # 设置 y 轴标签
plt.title(appConfig + ' - CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT #17')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(out_put_path + appConfig + '-CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_PERCENT.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# CHA REQUESTS
# 创建第六张图
plt.figure(6)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

# node 0 
node0_CHA_REQUESTS_READS_WRITES_LOCAL = data['node0/uncore_cha_0/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_0/REQUESTS/WRITES_LOCAL'].values + \
    data['node0/uncore_cha_1/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_1/REQUESTS/WRITES_LOCAL'].values + \
        data['node0/uncore_cha_2/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_2/REQUESTS/WRITES_LOCAL'].values + \
            data['node0/uncore_cha_3/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_3/REQUESTS/WRITES_LOCAL'].values + \
                data['node0/uncore_cha_4/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_4/REQUESTS/WRITES_LOCAL'].values + \
                    data['node0/uncore_cha_5/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_5/REQUESTS/WRITES_LOCAL'].values + \
                        data['node0/uncore_cha_6/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_6/REQUESTS/WRITES_LOCAL'].values + \
                            data['node0/uncore_cha_7/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_7/REQUESTS/WRITES_LOCAL'].values + \
                                data['node0/uncore_cha_8/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_8/REQUESTS/WRITES_LOCAL'].values + \
                                    data['node0/uncore_cha_9/REQUESTS/READS_LOCAL'].values + data['node0/uncore_cha_9/REQUESTS/WRITES_LOCAL'].values

node0_CHA_REQUESTS_READS_WRITES_REMOTE = data['node0/uncore_cha_0/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_0/REQUESTS/WRITES_REMOTE'].values + \
    data['node0/uncore_cha_1/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_1/REQUESTS/WRITES_REMOTE'].values + \
        data['node0/uncore_cha_2/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_2/REQUESTS/WRITES_REMOTE'].values + \
            data['node0/uncore_cha_3/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_3/REQUESTS/WRITES_REMOTE'].values + \
                data['node0/uncore_cha_4/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_4/REQUESTS/WRITES_REMOTE'].values + \
                    data['node0/uncore_cha_5/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_5/REQUESTS/WRITES_REMOTE'].values + \
                        data['node0/uncore_cha_6/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_6/REQUESTS/WRITES_REMOTE'].values + \
                            data['node0/uncore_cha_7/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_7/REQUESTS/WRITES_REMOTE'].values + \
                                data['node0/uncore_cha_8/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_8/REQUESTS/WRITES_REMOTE'].values + \
                                    data['node0/uncore_cha_9/REQUESTS/READS_REMOTE'].values + data['node0/uncore_cha_9/REQUESTS/WRITES_REMOTE'].values

# node 1
node1_CHA_REQUESTS_READS_WRITES_LOCAL = data['node1/uncore_cha_0/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_0/REQUESTS/WRITES_LOCAL'].values + \
    data['node1/uncore_cha_1/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_1/REQUESTS/WRITES_LOCAL'].values + \
        data['node1/uncore_cha_2/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_2/REQUESTS/WRITES_LOCAL'].values + \
            data['node1/uncore_cha_3/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_3/REQUESTS/WRITES_LOCAL'].values + \
                data['node1/uncore_cha_4/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_4/REQUESTS/WRITES_LOCAL'].values + \
                    data['node1/uncore_cha_5/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_5/REQUESTS/WRITES_LOCAL'].values + \
                        data['node1/uncore_cha_6/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_6/REQUESTS/WRITES_LOCAL'].values + \
                            data['node1/uncore_cha_7/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_7/REQUESTS/WRITES_LOCAL'].values + \
                                data['node1/uncore_cha_8/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_8/REQUESTS/WRITES_LOCAL'].values + \
                                    data['node1/uncore_cha_9/REQUESTS/READS_LOCAL'].values + data['node1/uncore_cha_9/REQUESTS/WRITES_LOCAL'].values

node1_CHA_REQUESTS_READS_WRITES_REMOTE = data['node1/uncore_cha_0/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_0/REQUESTS/WRITES_REMOTE'].values + \
    data['node1/uncore_cha_1/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_1/REQUESTS/WRITES_REMOTE'].values + \
        data['node1/uncore_cha_2/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_2/REQUESTS/WRITES_REMOTE'].values + \
            data['node1/uncore_cha_3/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_3/REQUESTS/WRITES_REMOTE'].values + \
                data['node1/uncore_cha_4/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_4/REQUESTS/WRITES_REMOTE'].values + \
                    data['node1/uncore_cha_5/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_5/REQUESTS/WRITES_REMOTE'].values + \
                        data['node1/uncore_cha_6/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_6/REQUESTS/WRITES_REMOTE'].values + \
                            data['node1/uncore_cha_7/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_7/REQUESTS/WRITES_REMOTE'].values + \
                                data['node1/uncore_cha_8/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_8/REQUESTS/WRITES_REMOTE'].values + \
                                    data['node1/uncore_cha_9/REQUESTS/READS_REMOTE'].values + data['node1/uncore_cha_9/REQUESTS/WRITES_REMOTE'].values

node0_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT = (node0_CHA_REQUESTS_READS_WRITES_REMOTE / list(map(float, (node0_CHA_REQUESTS_READS_WRITES_LOCAL + node0_CHA_REQUESTS_READS_WRITES_REMOTE)))) * 100
node1_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT = (node1_CHA_REQUESTS_READS_WRITES_REMOTE / list(map(float, (node1_CHA_REQUESTS_READS_WRITES_LOCAL + node1_CHA_REQUESTS_READS_WRITES_REMOTE)))) * 100

# 平滑数据
ma_node0_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT = moving_average(node0_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT, window_size)
ma_node1_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT = moving_average(node1_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT, window_size)

# 绘制图像
plt.plot(x, ma_node0_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT, linestyle='-', label='node0-remote-percent')
plt.plot(x, ma_node1_CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT, linestyle='-', label='node1-remote-percent')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('percent(%)')  # 设置 y 轴标签
plt.title(appConfig + ' - CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT #17')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(out_put_path + appConfig + '-CHA_REQUESTS_READS_WRITES_REMOTE_PERCENT.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# CHA victim_num
# 创建第六张图
plt.figure(6)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

# 平滑数据
ma_node0_CHA_LLC_LOOKUP_ANY = moving_average(node0_CHA_LLC_LOOKUP_ANY, window_size)
ma_node1_CHA_LLC_LOOKUP_ANY = moving_average(node1_CHA_LLC_LOOKUP_ANY, window_size)
ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE = moving_average(node0_CHA_LLC_LOOKUP_DATA_READ_WRITE, window_size)
ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS = moving_average(node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS, window_size)
ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE = moving_average(node1_CHA_LLC_LOOKUP_DATA_READ_WRITE, window_size)
ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS = moving_average(node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS, window_size)

# 绘制图像
plt.plot(x, ma_node0_CHA_LLC_LOOKUP_ANY, linestyle='-', label='node0-ANY')
# plt.plot(x, ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE, linestyle='-', label='node0')
# plt.plot(x, ma_node0_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS, linestyle='-', label='node0-HIT_MISS')
plt.plot(x, ma_node1_CHA_LLC_LOOKUP_ANY, linestyle='-', label='node1-ANY')
# plt.plot(x, ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE, linestyle='-', label='node1')
# plt.plot(x, ma_node1_CHA_LLC_LOOKUP_DATA_READ_WRITE_HIT_MISS, linestyle='-', label='node1-HIT_MISS')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('conts per second')  # 设置 y 轴标签
plt.title(appConfig + ' - CHA_LLC_LOOKUP_DATA_READ_WRITE #17')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(out_put_path + appConfig + '-CHA_LLC_LOOKUP_DATA_READ_WRITE.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()
