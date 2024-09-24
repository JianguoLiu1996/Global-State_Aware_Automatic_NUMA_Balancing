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

# app and config
appConfig = 'gups-enableAutoNuma'

# 读取 CSV 文件
file_path = '/home/jianguoliu/Global-State_Aware_Automatic_NUMA_Balancing/kunpeng920_huawei_arm/enableAutoNumastatisticalResult.csv'
data = pd.read_csv(file_path)

# 假设 CSV 文件中有两列数据，你可以根据需要选择特定的列来绘制图像
x = np.linspace(1, data.shape[0], data.shape[0], dtype=int)  # 替换为你想要作为 x 轴的列名
# y = data.iloc[:,1].values  # 替换为你想要作为 y 轴的列名

# DDRC图像绘制
# DDRC总体情况绘制
node0_flux_wrcmd = data['hisi_sccl1_ddrc0/flux_wcmd/'].values + data['hisi_sccl1_ddrc0/flux_rcmd/'].values + \
    data['hisi_sccl1_ddrc1/flux_wcmd/'].values + data['hisi_sccl1_ddrc1/flux_rcmd/'].values + \
        data['hisi_sccl1_ddrc2/flux_wcmd/'].values + data['hisi_sccl1_ddrc2/flux_rcmd/'].values + \
            data['hisi_sccl1_ddrc3/flux_wcmd/'].values + data['hisi_sccl1_ddrc3/flux_rcmd/'].values

node1_flux_wrcmd = data['hisi_sccl3_ddrc0/flux_wcmd/'].values + data['hisi_sccl3_ddrc0/flux_rcmd/'].values + \
    data['hisi_sccl3_ddrc1/flux_wcmd/'].values + data['hisi_sccl3_ddrc1/flux_rcmd/'].values + \
        data['hisi_sccl3_ddrc2/flux_wcmd/'].values + data['hisi_sccl3_ddrc2/flux_rcmd/'].values + \
            data['hisi_sccl3_ddrc3/flux_wcmd/'].values + data['hisi_sccl3_ddrc3/flux_rcmd/'].values

node2_flux_wrcmd = data['hisi_sccl5_ddrc0/flux_wcmd/'].values + data['hisi_sccl5_ddrc0/flux_rcmd/'].values + \
    data['hisi_sccl5_ddrc1/flux_wcmd/'].values + data['hisi_sccl5_ddrc1/flux_rcmd/'].values + \
        data['hisi_sccl5_ddrc2/flux_wcmd/'].values + data['hisi_sccl5_ddrc2/flux_rcmd/'].values + \
            data['hisi_sccl5_ddrc3/flux_wcmd/'].values + data['hisi_sccl5_ddrc3/flux_rcmd/'].values

node3_flux_wrcmd = data['hisi_sccl7_ddrc0/flux_wcmd/'].values + data['hisi_sccl7_ddrc0/flux_rcmd/'].values + \
    data['hisi_sccl7_ddrc1/flux_wcmd/'].values + data['hisi_sccl7_ddrc1/flux_rcmd/'].values + \
        data['hisi_sccl7_ddrc2/flux_wcmd/'].values + data['hisi_sccl7_ddrc2/flux_rcmd/'].values + \
            data['hisi_sccl7_ddrc3/flux_wcmd/'].values + data['hisi_sccl7_ddrc3/flux_rcmd/'].values

# 平滑数据
ma_node0_flux_wrcmd = moving_average(node0_flux_wrcmd, window_size)
ma_node1_flux_wrcmd = moving_average(node1_flux_wrcmd, window_size)
ma_node2_flux_wrcmd = moving_average(node2_flux_wrcmd, window_size)
ma_node3_flux_wrcmd = moving_average(node3_flux_wrcmd, window_size)

# 创建图像
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）
plt.plot(x, ma_node0_flux_wrcmd, linestyle='-', label='node0_flux_wrcmd')
plt.plot(x, ma_node1_flux_wrcmd, linestyle='-', label='node1_flux_wrcmd')
plt.plot(x, ma_node2_flux_wrcmd, linestyle='-', label='node2_flux_wrcmd')
plt.plot(x, ma_node3_flux_wrcmd, linestyle='-', label='node3_flux_wrcmd')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('DDRC wr_cmd(couters per second)')  # 设置 y 轴标签
plt.title(appConfig + ' - DDRC statistic #3')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例

# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-DDRC-Statistic.png')  # 替换为你想要保存的文件名和格式

# 关闭图像
plt.close()

# 创建第二张图
# DDRC 读写比例
plt.figure(2)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）
node0_flux_rcmd = data['hisi_sccl1_ddrc0/flux_rcmd/'].values + data['hisi_sccl1_ddrc1/flux_rcmd/'].values + \
    data['hisi_sccl1_ddrc2/flux_rcmd/'].values + data['hisi_sccl1_ddrc3/flux_rcmd/'].values
node1_flux_rcmd = data['hisi_sccl3_ddrc0/flux_rcmd/'].values + data['hisi_sccl3_ddrc1/flux_rcmd/'].values + \
    data['hisi_sccl3_ddrc2/flux_rcmd/'].values + data['hisi_sccl3_ddrc3/flux_rcmd/'].values
node2_flux_rcmd = data['hisi_sccl5_ddrc0/flux_rcmd/'].values + data['hisi_sccl5_ddrc1/flux_rcmd/'].values + \
    data['hisi_sccl5_ddrc2/flux_rcmd/'].values + data['hisi_sccl5_ddrc3/flux_rcmd/'].values
node3_flux_rcmd = data['hisi_sccl7_ddrc0/flux_rcmd/'].values + data['hisi_sccl7_ddrc1/flux_rcmd/'].values + \
    data['hisi_sccl7_ddrc2/flux_rcmd/'].values + data['hisi_sccl7_ddrc3/flux_rcmd/'].values
node0_flux_r_percent = (node0_flux_rcmd / list(map(float, node0_flux_wrcmd))) * 100
node1_flux_r_percent = (node1_flux_rcmd / list(map(float, node1_flux_wrcmd))) * 100
node2_flux_r_percent = (node2_flux_rcmd / list(map(float, node2_flux_wrcmd))) * 100
node3_flux_r_percent = (node3_flux_rcmd / list(map(float, node3_flux_wrcmd))) * 100

# 平滑数据
ma_node0_flux_r_percent = moving_average(node0_flux_r_percent, window_size)
ma_node1_flux_r_percent = moving_average(node1_flux_r_percent, window_size)
ma_node2_flux_r_percent = moving_average(node2_flux_r_percent, window_size)
ma_node3_flux_r_percent = moving_average(node3_flux_r_percent, window_size)

plt.plot(x, ma_node0_flux_r_percent, linestyle='-', label='node0_flux_r_percent')
plt.plot(x, ma_node1_flux_r_percent, linestyle='-', label='node1_flux_r_percent')
plt.plot(x, ma_node2_flux_r_percent, linestyle='-', label='node2_flux_r_percent')
plt.plot(x, ma_node3_flux_r_percent, linestyle='-', label='node3_flux_r_percent')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('percent(%)')  # 设置 y 轴标签
plt.title(appConfig + ' - DDRC statistic - read percent #3')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-DDRC-Statistic-read-percent.png')  # 替换为你想要保存的文件名和格式

# 关闭图像
plt.close()

# HHA统计绘制
# 创建第三张图
plt.figure(3)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

node0_hha_rx_ops_num = data['hisi_sccl1_hha2/rx_ops_num/'].values + data['hisi_sccl1_hha3/rx_ops_num/'].values
node1_hha_rx_ops_num = data['hisi_sccl3_hha0/rx_ops_num/'].values + data['hisi_sccl3_hha1/rx_ops_num/'].values
node2_hha_rx_ops_num = data['hisi_sccl5_hha6/rx_ops_num/'].values + data['hisi_sccl5_hha7/rx_ops_num/'].values
node3_hha_rx_ops_num = data['hisi_sccl7_hha4/rx_ops_num/'].values + data['hisi_sccl7_hha5/rx_ops_num/'].values

# 平滑数据
ma_node0_hha_rx_ops_num = moving_average(node0_hha_rx_ops_num, window_size)
ma_node1_hha_rx_ops_num = moving_average(node1_hha_rx_ops_num, window_size)
ma_node2_hha_rx_ops_num = moving_average(node2_hha_rx_ops_num, window_size)
ma_node3_hha_rx_ops_num = moving_average(node3_hha_rx_ops_num, window_size)

plt.plot(x, ma_node0_hha_rx_ops_num, linestyle='-', label='node0_hha_rx_ops_num')
plt.plot(x, ma_node1_hha_rx_ops_num, linestyle='-', label='node1_hha_rx_ops_num')
plt.plot(x, ma_node2_hha_rx_ops_num, linestyle='-', label='node2_hha_rx_ops_num')
plt.plot(x, ma_node3_hha_rx_ops_num, linestyle='-', label='node3_hha_rx_ops_num')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('counts per second')  # 设置 y 轴标签
plt.title(appConfig + ' - HHA statistic - rx_ops_num #3')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-HHA-Statistic-rx_ops_num.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# HHA远程访问比例
# 创建第四张图
plt.figure(4)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

node0_rx_out = data['hisi_sccl1_hha2/rx_outer/'].values + data['hisi_sccl1_hha3/rx_outer/'].values + \
    data['hisi_sccl1_hha2/rx_sccl/'].values + data['hisi_sccl1_hha3/rx_sccl/']
node1_rx_out = data['hisi_sccl3_hha0/rx_outer/'].values + data['hisi_sccl3_hha1/rx_outer/'].values + \
    data['hisi_sccl3_hha0/rx_sccl/'].values + data['hisi_sccl3_hha1/rx_sccl/']
node2_rx_out = data['hisi_sccl5_hha6/rx_outer/'].values + data['hisi_sccl5_hha7/rx_outer/'].values + \
    data['hisi_sccl5_hha6/rx_sccl/'].values + data['hisi_sccl5_hha7/rx_sccl/']
node3_rx_out = data['hisi_sccl7_hha4/rx_outer/'].values + data['hisi_sccl7_hha5/rx_outer/'].values + \
    data['hisi_sccl7_hha4/rx_sccl/'].values + data['hisi_sccl7_hha5/rx_sccl/']
node0_rx_out_percent = (node0_rx_out / list(map(float, node0_hha_rx_ops_num))) * 100
node1_rx_out_percent = (node1_rx_out / list(map(float, node1_hha_rx_ops_num))) * 100
node2_rx_out_percent = (node2_rx_out / list(map(float, node2_hha_rx_ops_num))) * 100
node3_rx_out_percent = (node3_rx_out / list(map(float, node3_hha_rx_ops_num))) * 100

# 平滑数据
ma_node0_rx_out_percent = moving_average(node0_rx_out_percent, window_size)
ma_node1_rx_out_percent = moving_average(node1_rx_out_percent, window_size)
ma_node2_rx_out_percent = moving_average(node2_rx_out_percent, window_size)
ma_node3_rx_out_percent = moving_average(node3_rx_out_percent, window_size)

plt.plot(x, ma_node0_rx_out_percent, linestyle='-', label='node0_rx_out_percent')
plt.plot(x, ma_node1_rx_out_percent, linestyle='-', label='node1_rx_out_percent')
plt.plot(x, ma_node2_rx_out_percent, linestyle='-', label='node2_rx_out_percent')
plt.plot(x, ma_node3_rx_out_percent, linestyle='-', label='node3_rx_out_percent')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('rx_out percent(%)')  # 设置 y 轴标签
plt.title(appConfig + ' - HHA statistic - rx_out percent #3')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-HHA-Statistic-rx_out_percent.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# L3C hit percent
# 创建第五张图
plt.figure(5)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

# sccl1_l3c
sccl1_l3c_rd_cpipe = data['hisi_sccl1_l3c8/rd_cpipe'].values + \
    data['hisi_sccl1_l3c9/rd_cpipe'].values + \
        data['hisi_sccl1_l3c10/rd_cpipe'].values + \
            data['hisi_sccl1_l3c11/rd_cpipe'].values + \
                data['hisi_sccl1_l3c12/rd_cpipe'].values + \
                    data['hisi_sccl1_l3c13/rd_cpipe'].values
sccl1_l3c_wr_cpipe = data['hisi_sccl1_l3c8/wr_cpipe'].values + \
    data['hisi_sccl1_l3c9/wr_cpipe'].values + \
        data['hisi_sccl1_l3c10/wr_cpipe'].values + \
            data['hisi_sccl1_l3c11/wr_cpipe'].values + \
                data['hisi_sccl1_l3c12/wr_cpipe'].values + \
                    data['hisi_sccl1_l3c13/wr_cpipe'].values
sccl1_l3c_rd_hit_cpipe = data['hisi_sccl1_l3c8/rd_hit_cpipe'].values + \
    data['hisi_sccl1_l3c9/rd_hit_cpipe'].values + \
        data['hisi_sccl1_l3c10/rd_hit_cpipe'].values + \
            data['hisi_sccl1_l3c11/rd_hit_cpipe'].values + \
                data['hisi_sccl1_l3c12/rd_hit_cpipe'].values + \
                    data['hisi_sccl1_l3c13/rd_hit_cpipe'].values
sccl1_l3c_wr_hit_cpipe = data['hisi_sccl1_l3c8/wr_hit_cpipe'].values + \
    data['hisi_sccl1_l3c9/wr_hit_cpipe'].values + \
        data['hisi_sccl1_l3c10/wr_hit_cpipe'].values + \
            data['hisi_sccl1_l3c11/wr_hit_cpipe'].values + \
                data['hisi_sccl1_l3c12/wr_hit_cpipe'].values + \
                    data['hisi_sccl1_l3c13/wr_hit_cpipe'].values
sccl1_l3c_victim_num = data['hisi_sccl1_l3c8/victim_num'].values + \
    data['hisi_sccl1_l3c9/victim_num'].values + \
        data['hisi_sccl1_l3c10/victim_num'].values + \
            data['hisi_sccl1_l3c11/victim_num'].values + \
                data['hisi_sccl1_l3c12/victim_num'].values + \
                    data['hisi_sccl1_l3c13/victim_num'].values
# sccl3_l3c
sccl3_l3c_rd_cpipe = data['hisi_sccl3_l3c0/rd_cpipe'].values + \
    data['hisi_sccl3_l3c1/rd_cpipe'].values + \
        data['hisi_sccl3_l3c2/rd_cpipe'].values + \
            data['hisi_sccl3_l3c3/rd_cpipe'].values + \
                data['hisi_sccl3_l3c4/rd_cpipe'].values + \
                    data['hisi_sccl3_l3c5/rd_cpipe'].values
sccl3_l3c_wr_cpipe = data['hisi_sccl3_l3c0/wr_cpipe'].values + \
    data['hisi_sccl3_l3c1/wr_cpipe'].values + \
        data['hisi_sccl3_l3c2/wr_cpipe'].values + \
            data['hisi_sccl3_l3c3/wr_cpipe'].values + \
                data['hisi_sccl3_l3c4/wr_cpipe'].values + \
                    data['hisi_sccl3_l3c5/wr_cpipe'].values
sccl3_l3c_rd_hit_cpipe = data['hisi_sccl3_l3c0/rd_hit_cpipe'].values + \
    data['hisi_sccl3_l3c1/rd_hit_cpipe'].values + \
        data['hisi_sccl3_l3c2/rd_hit_cpipe'].values + \
            data['hisi_sccl3_l3c3/rd_hit_cpipe'].values + \
                data['hisi_sccl3_l3c4/rd_hit_cpipe'].values + \
                    data['hisi_sccl3_l3c5/rd_hit_cpipe'].values
sccl3_l3c_wr_hit_cpipe = data['hisi_sccl3_l3c0/wr_hit_cpipe'].values + \
    data['hisi_sccl3_l3c1/wr_hit_cpipe'].values + \
        data['hisi_sccl3_l3c2/wr_hit_cpipe'].values + \
            data['hisi_sccl3_l3c3/wr_hit_cpipe'].values + \
                data['hisi_sccl3_l3c4/wr_hit_cpipe'].values + \
                    data['hisi_sccl3_l3c5/wr_hit_cpipe'].values
sccl3_l3c_victim_num = data['hisi_sccl3_l3c0/victim_num'].values + \
    data['hisi_sccl3_l3c1/victim_num'].values + \
        data['hisi_sccl3_l3c2/victim_num'].values + \
            data['hisi_sccl3_l3c3/victim_num'].values + \
                data['hisi_sccl3_l3c4/victim_num'].values + \
                    data['hisi_sccl3_l3c5/victim_num'].values
# sccl5_l3c
sccl5_l3c_rd_cpipe = data['hisi_sccl5_l3c24/rd_cpipe'].values + \
    data['hisi_sccl5_l3c25/rd_cpipe'].values + \
        data['hisi_sccl5_l3c26/rd_cpipe'].values + \
            data['hisi_sccl5_l3c27/rd_cpipe'].values + \
                data['hisi_sccl5_l3c28/rd_cpipe'].values + \
                    data['hisi_sccl5_l3c29/rd_cpipe'].values
sccl5_l3c_wr_cpipe = data['hisi_sccl5_l3c24/wr_cpipe'].values + \
    data['hisi_sccl5_l3c25/wr_cpipe'].values + \
        data['hisi_sccl5_l3c26/wr_cpipe'].values + \
            data['hisi_sccl5_l3c27/wr_cpipe'].values + \
                data['hisi_sccl5_l3c28/wr_cpipe'].values + \
                    data['hisi_sccl5_l3c29/wr_cpipe'].values
sccl5_l3c_rd_hit_cpipe = data['hisi_sccl5_l3c24/rd_hit_cpipe'].values + \
    data['hisi_sccl5_l3c25/rd_hit_cpipe'].values + \
        data['hisi_sccl5_l3c26/rd_hit_cpipe'].values + \
            data['hisi_sccl5_l3c27/rd_hit_cpipe'].values + \
                data['hisi_sccl5_l3c28/rd_hit_cpipe'].values + \
                    data['hisi_sccl5_l3c29/rd_hit_cpipe'].values
sccl5_l3c_wr_hit_cpipe = data['hisi_sccl5_l3c24/wr_hit_cpipe'].values + \
    data['hisi_sccl5_l3c25/wr_hit_cpipe'].values + \
        data['hisi_sccl5_l3c26/wr_hit_cpipe'].values + \
            data['hisi_sccl5_l3c27/wr_hit_cpipe'].values + \
                data['hisi_sccl5_l3c28/wr_hit_cpipe'].values + \
                    data['hisi_sccl5_l3c29/wr_hit_cpipe'].values
sccl5_l3c_victim_num = data['hisi_sccl5_l3c24/victim_num'].values + \
    data['hisi_sccl5_l3c25/victim_num'].values + \
        data['hisi_sccl5_l3c26/victim_num'].values + \
            data['hisi_sccl5_l3c27/victim_num'].values + \
                data['hisi_sccl5_l3c28/victim_num'].values + \
                    data['hisi_sccl5_l3c29/victim_num'].values
# sccl7_l3c
sccl7_l3c_rd_cpipe = data['hisi_sccl7_l3c16/rd_cpipe'].values + \
    data['hisi_sccl7_l3c17/rd_cpipe'].values + \
        data['hisi_sccl7_l3c18/rd_cpipe'].values + \
            data['hisi_sccl7_l3c19/rd_cpipe'].values + \
                data['hisi_sccl7_l3c20/rd_cpipe'].values + \
                    data['hisi_sccl7_l3c21/rd_cpipe'].values
sccl7_l3c_wr_cpipe = data['hisi_sccl7_l3c16/wr_cpipe'].values + \
    data['hisi_sccl7_l3c17/wr_cpipe'].values + \
        data['hisi_sccl7_l3c18/wr_cpipe'].values + \
            data['hisi_sccl7_l3c19/wr_cpipe'].values + \
                data['hisi_sccl7_l3c20/wr_cpipe'].values + \
                    data['hisi_sccl7_l3c21/wr_cpipe'].values
sccl7_l3c_rd_hit_cpipe = data['hisi_sccl7_l3c16/rd_hit_cpipe'].values + \
    data['hisi_sccl7_l3c17/rd_hit_cpipe'].values + \
        data['hisi_sccl7_l3c18/rd_hit_cpipe'].values + \
            data['hisi_sccl7_l3c19/rd_hit_cpipe'].values + \
                data['hisi_sccl7_l3c20/rd_hit_cpipe'].values + \
                    data['hisi_sccl7_l3c21/rd_hit_cpipe'].values
sccl7_l3c_wr_hit_cpipe = data['hisi_sccl7_l3c16/wr_hit_cpipe'].values + \
    data['hisi_sccl7_l3c17/wr_hit_cpipe'].values + \
        data['hisi_sccl7_l3c18/wr_hit_cpipe'].values + \
            data['hisi_sccl7_l3c19/wr_hit_cpipe'].values + \
                data['hisi_sccl7_l3c20/wr_hit_cpipe'].values + \
                    data['hisi_sccl7_l3c21/wr_hit_cpipe'].values
sccl7_l3c_victim_num = data['hisi_sccl7_l3c16/victim_num'].values + \
    data['hisi_sccl7_l3c17/victim_num'].values + \
        data['hisi_sccl7_l3c18/victim_num'].values + \
            data['hisi_sccl7_l3c19/victim_num'].values + \
                data['hisi_sccl7_l3c20/victim_num'].values + \
                    data['hisi_sccl7_l3c21/victim_num'].values

# 总体命中率，单位%
node0_wr_hit_percent = ((sccl1_l3c_rd_hit_cpipe + sccl1_l3c_wr_hit_cpipe) / list(map(float, (sccl1_l3c_rd_cpipe + sccl1_l3c_wr_cpipe)))) * 100
node1_wr_hit_percent = ((sccl3_l3c_rd_hit_cpipe + sccl3_l3c_wr_hit_cpipe) / list(map(float, (sccl3_l3c_rd_cpipe + sccl3_l3c_wr_cpipe)))) * 100
node2_wr_hit_percent = ((sccl5_l3c_rd_hit_cpipe + sccl5_l3c_wr_hit_cpipe) / list(map(float, (sccl5_l3c_rd_cpipe + sccl5_l3c_wr_cpipe)))) * 100
node3_wr_hit_percent = ((sccl7_l3c_rd_hit_cpipe + sccl7_l3c_wr_hit_cpipe) / list(map(float, (sccl7_l3c_rd_cpipe + sccl7_l3c_wr_cpipe)))) * 100

# 平滑数据
ma_node0_wr_hit_percent = moving_average(node0_wr_hit_percent, window_size)
ma_node1_wr_hit_percent = moving_average(node1_wr_hit_percent, window_size)
ma_node2_wr_hit_percent = moving_average(node2_wr_hit_percent, window_size)
ma_node3_wr_hit_percent = moving_average(node3_wr_hit_percent, window_size)

plt.plot(x, ma_node0_wr_hit_percent, linestyle='-', label='node0')
plt.plot(x, ma_node1_wr_hit_percent, linestyle='-', label='node1')
plt.plot(x, ma_node2_wr_hit_percent, linestyle='-', label='node2')
plt.plot(x, ma_node3_wr_hit_percent, linestyle='-', label='node3')

plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('read hit percent(%)')  # 设置 y 轴标签
plt.title(appConfig + ' - L3C statistic - read hit percent #3')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-L3C-Statistic-read_hit_percent.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# L3C victim_num
# 创建第六张图
plt.figure(6)
plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）

# 平滑数据
ma_sccl1_l3c_victim_num = moving_average(sccl1_l3c_victim_num, window_size)
ma_sccl3_l3c_victim_num = moving_average(sccl3_l3c_victim_num, window_size)
ma_sccl5_l3c_victim_num = moving_average(sccl5_l3c_victim_num, window_size)
ma_sccl7_l3c_victim_num = moving_average(sccl7_l3c_victim_num, window_size)


plt.plot(x, ma_sccl1_l3c_victim_num, linestyle='-', label='node0')
plt.plot(x, ma_sccl3_l3c_victim_num, linestyle='-', label='node1')
plt.plot(x, ma_sccl5_l3c_victim_num, linestyle='-', label='node2')
plt.plot(x, ma_sccl7_l3c_victim_num, linestyle='-', label='node3')
plt.xlabel('time(s)')  # 设置 x 轴标签
plt.ylabel('victim_num(conts per second)')  # 设置 y 轴标签
plt.title(appConfig + ' - L3C statistic - victim_num #3')  # 设置图像标题
plt.grid(True)  # 添加网格线（可选）
plt.legend() # 添加图例
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-L3C-Statistic-victim_num.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# L3C 每个节点的情况
# 创建第七张图
plt.figure(7)
# plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）
fig, axs = plt.subplots(2, 2, figsize=(20, 20))  # 创建 2x2 的子图

# node 0
node0_L3C0_rd_hit_percent = data['hisi_sccl1_l3c8/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl1_l3c8/rd_cpipe'].values)) * 100
node0_L3C1_rd_hit_percent = data['hisi_sccl1_l3c9/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl1_l3c9/rd_cpipe'].values)) * 100
node0_L3C2_rd_hit_percent = data['hisi_sccl1_l3c10/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl1_l3c10/rd_cpipe'].values)) * 100
node0_L3C3_rd_hit_percent = data['hisi_sccl1_l3c11/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl1_l3c11/rd_cpipe'].values)) * 100
node0_L3C4_rd_hit_percent = data['hisi_sccl1_l3c12/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl1_l3c12/rd_cpipe'].values)) * 100
node0_L3C5_rd_hit_percent = data['hisi_sccl1_l3c13/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl1_l3c13/rd_cpipe'].values)) * 100
node0_L3C0_victim_num = data['hisi_sccl1_l3c8/victim_num'].values
node0_L3C1_victim_num = data['hisi_sccl1_l3c9/victim_num'].values
node0_L3C2_victim_num = data['hisi_sccl1_l3c10/victim_num'].values
node0_L3C3_victim_num = data['hisi_sccl1_l3c11/victim_num'].values
node0_L3C4_victim_num = data['hisi_sccl1_l3c12/victim_num'].values
node0_L3C5_victim_num = data['hisi_sccl1_l3c13/victim_num'].values
# node 1
node1_L3C0_rd_hit_percent = data['hisi_sccl3_l3c0/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl3_l3c0/rd_cpipe'].values)) * 100
node1_L3C1_rd_hit_percent = data['hisi_sccl3_l3c1/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl3_l3c1/rd_cpipe'].values)) * 100
node1_L3C2_rd_hit_percent = data['hisi_sccl3_l3c2/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl3_l3c2/rd_cpipe'].values)) * 100
node1_L3C3_rd_hit_percent = data['hisi_sccl3_l3c3/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl3_l3c3/rd_cpipe'].values)) * 100
node1_L3C4_rd_hit_percent = data['hisi_sccl3_l3c4/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl3_l3c4/rd_cpipe'].values)) * 100
node1_L3C5_rd_hit_percent = data['hisi_sccl3_l3c5/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl3_l3c5/rd_cpipe'].values)) * 100
node1_L3C0_victim_num = data['hisi_sccl3_l3c0/victim_num'].values
node1_L3C1_victim_num = data['hisi_sccl3_l3c1/victim_num'].values
node1_L3C2_victim_num = data['hisi_sccl3_l3c2/victim_num'].values
node1_L3C3_victim_num = data['hisi_sccl3_l3c3/victim_num'].values
node1_L3C4_victim_num = data['hisi_sccl3_l3c4/victim_num'].values
node1_L3C5_victim_num = data['hisi_sccl3_l3c5/victim_num'].values
# node 2
node2_L3C0_rd_hit_percent = data['hisi_sccl5_l3c24/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl5_l3c24/rd_cpipe'].values)) * 100
node2_L3C1_rd_hit_percent = data['hisi_sccl5_l3c25/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl5_l3c25/rd_cpipe'].values)) * 100
node2_L3C2_rd_hit_percent = data['hisi_sccl5_l3c26/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl5_l3c26/rd_cpipe'].values)) * 100
node2_L3C3_rd_hit_percent = data['hisi_sccl5_l3c27/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl5_l3c27/rd_cpipe'].values)) * 100
node2_L3C4_rd_hit_percent = data['hisi_sccl5_l3c28/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl5_l3c28/rd_cpipe'].values)) * 100
node2_L3C5_rd_hit_percent = data['hisi_sccl5_l3c29/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl5_l3c29/rd_cpipe'].values)) * 100
node2_L3C0_victim_num = data['hisi_sccl5_l3c24/victim_num'].values
node2_L3C1_victim_num = data['hisi_sccl5_l3c25/victim_num'].values
node2_L3C2_victim_num = data['hisi_sccl5_l3c26/victim_num'].values
node2_L3C3_victim_num = data['hisi_sccl5_l3c27/victim_num'].values
node2_L3C4_victim_num = data['hisi_sccl5_l3c28/victim_num'].values
node2_L3C5_victim_num = data['hisi_sccl5_l3c29/victim_num'].values
# node 3
node3_L3C0_rd_hit_percent = data['hisi_sccl7_l3c16/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl7_l3c16/rd_cpipe'].values)) * 100
node3_L3C1_rd_hit_percent = data['hisi_sccl7_l3c17/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl7_l3c17/rd_cpipe'].values)) * 100
node3_L3C2_rd_hit_percent = data['hisi_sccl7_l3c18/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl7_l3c18/rd_cpipe'].values)) * 100
node3_L3C3_rd_hit_percent = data['hisi_sccl7_l3c19/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl7_l3c19/rd_cpipe'].values)) * 100
node3_L3C4_rd_hit_percent = data['hisi_sccl7_l3c20/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl7_l3c20/rd_cpipe'].values)) * 100
node3_L3C5_rd_hit_percent = data['hisi_sccl7_l3c21/rd_hit_cpipe'].values / list(map(float, data['hisi_sccl7_l3c21/rd_cpipe'].values)) * 100
node3_L3C0_victim_num = data['hisi_sccl7_l3c16/victim_num'].values
node3_L3C1_victim_num = data['hisi_sccl7_l3c17/victim_num'].values
node3_L3C2_victim_num = data['hisi_sccl7_l3c18/victim_num'].values
node3_L3C3_victim_num = data['hisi_sccl7_l3c19/victim_num'].values
node3_L3C4_victim_num = data['hisi_sccl7_l3c20/victim_num'].values
node3_L3C5_victim_num = data['hisi_sccl7_l3c21/victim_num'].values

# 在各个子图中绘制不同的图形
axs[0, 0].set_title('Node 0 L3C')
axs[0, 0].plot(x, node0_L3C0_victim_num, linestyle='-', label='node0_L3C0')
axs[0, 0].plot(x, node0_L3C1_victim_num, linestyle='-', label='node0_L3C1')
axs[0, 0].plot(x, node0_L3C2_victim_num, linestyle='-', label='node0_L3C2')
axs[0, 0].plot(x, node0_L3C3_victim_num, linestyle='-', label='node0_L3C3')
axs[0, 0].plot(x, node0_L3C4_victim_num, linestyle='-', label='node0_L3C4')
axs[0, 0].plot(x, node0_L3C5_victim_num, linestyle='-', label='node0_L3C5')
axs[0, 0].set_xlabel('time(s)')  # 设置 x 轴标签
axs[0, 0].legend() # 添加图例
axs[0, 0].grid(True)  # 添加网格线（可选）
axs[0, 0].set_ylabel('victim_num(conts per second)')  # 设置 y 轴标签

axs[0, 1].set_title('Node 1 L3C')
axs[0, 1].plot(x, node1_L3C0_victim_num, linestyle='-', label='node1_L3C0')
axs[0, 1].plot(x, node1_L3C1_victim_num, linestyle='-', label='node1_L3C1')
axs[0, 1].plot(x, node1_L3C2_victim_num, linestyle='-', label='node1_L3C2')
axs[0, 1].plot(x, node1_L3C3_victim_num, linestyle='-', label='node1_L3C3')
axs[0, 1].plot(x, node1_L3C4_victim_num, linestyle='-', label='node1_L3C4')
axs[0, 1].plot(x, node1_L3C5_victim_num, linestyle='-', label='node1_L3C5')
axs[0, 1].set_xlabel('time(s)')  # 设置 x 轴标签
axs[0, 1].legend() # 添加图例
axs[0, 1].grid(True)  # 添加网格线（可选）
axs[0, 1].set_ylabel('victim_num(conts per second)')  # 设置 y 轴标签

axs[1, 0].set_title('Node 2 L3C')
axs[1, 0].plot(x, node2_L3C0_victim_num, linestyle='-', label='node2_L3C0')
axs[1, 0].plot(x, node2_L3C1_victim_num, linestyle='-', label='node2_L3C1')
axs[1, 0].plot(x, node2_L3C2_victim_num, linestyle='-', label='node2_L3C2')
axs[1, 0].plot(x, node2_L3C3_victim_num, linestyle='-', label='node2_L3C3')
axs[1, 0].plot(x, node2_L3C4_victim_num, linestyle='-', label='node2_L3C4')
axs[1, 0].plot(x, node2_L3C5_victim_num, linestyle='-', label='node2_L3C5')
axs[1, 0].set_xlabel('time(s)')  # 设置 x 轴标签
axs[1, 0].legend() # 添加图例
axs[1, 0].grid(True)  # 添加网格线（可选）
axs[1, 0].set_ylabel('victim_num(conts per second)')  # 设置 y 轴标签

axs[1, 1].set_title('Node 3 L3C')
axs[1, 1].plot(x, node3_L3C0_victim_num, linestyle='-', label='node3_L3C0')
axs[1, 1].plot(x, node3_L3C1_victim_num, linestyle='-', label='node3_L3C1')
axs[1, 1].plot(x, node3_L3C2_victim_num, linestyle='-', label='node3_L3C2')
axs[1, 1].plot(x, node3_L3C3_victim_num, linestyle='-', label='node3_L3C3')
axs[1, 1].plot(x, node3_L3C4_victim_num, linestyle='-', label='node3_L3C4')
axs[1, 1].plot(x, node3_L3C5_victim_num, linestyle='-', label='node3_L3C5')
axs[1, 1].set_xlabel('time(s)')  # 设置 x 轴标签
axs[1, 1].legend() # 添加图例
axs[1, 1].grid(True)  # 添加网格线（可选）
axs[1, 1].set_ylabel('victim_num(conts per second)')  # 设置 y 轴标签

# 调整子图之间的间距
plt.tight_layout()
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-L3C-Statistic-per_node_victim_num.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()

# L3C 每个节点的情况
# 创建第八张图
plt.figure(8)
# plt.figure(figsize=(8, 6))  # 设置图像尺寸（可选）
fig, axs = plt.subplots(2, 2, figsize=(20, 20))  # 创建 2x2 的子图

# 在各个子图中绘制不同的图形
axs[0, 0].set_title('Node 0 L3C')
axs[0, 0].plot(x, node0_L3C0_rd_hit_percent, linestyle='-', label='node0_L3C0')
axs[0, 0].plot(x, node0_L3C1_rd_hit_percent, linestyle='-', label='node0_L3C1')
axs[0, 0].plot(x, node0_L3C2_rd_hit_percent, linestyle='-', label='node0_L3C2')
axs[0, 0].plot(x, node0_L3C3_rd_hit_percent, linestyle='-', label='node0_L3C3')
axs[0, 0].plot(x, node0_L3C4_rd_hit_percent, linestyle='-', label='node0_L3C4')
axs[0, 0].plot(x, node0_L3C5_rd_hit_percent, linestyle='-', label='node0_L3C5')
axs[0, 0].set_xlabel('time(s)')  # 设置 x 轴标签
axs[0, 0].legend() # 添加图例
axs[0, 0].grid(True)  # 添加网格线（可选）
axs[0, 0].set_ylabel('read hit percent(%)')  # 设置 y 轴标签

axs[0, 1].set_title('Node 1 L3C')
axs[0, 1].plot(x, node1_L3C0_rd_hit_percent, linestyle='-', label='node1_L3C0')
axs[0, 1].plot(x, node1_L3C1_rd_hit_percent, linestyle='-', label='node1_L3C1')
axs[0, 1].plot(x, node1_L3C2_rd_hit_percent, linestyle='-', label='node1_L3C2')
axs[0, 1].plot(x, node1_L3C3_rd_hit_percent, linestyle='-', label='node1_L3C3')
axs[0, 1].plot(x, node1_L3C4_rd_hit_percent, linestyle='-', label='node1_L3C4')
axs[0, 1].plot(x, node1_L3C5_rd_hit_percent, linestyle='-', label='node1_L3C5')
axs[0, 1].set_xlabel('time(s)')  # 设置 x 轴标签
axs[0, 1].legend() # 添加图例
axs[0, 1].grid(True)  # 添加网格线（可选）
axs[0, 1].set_ylabel('read hit percent(%)')  # 设置 y 轴标签

axs[1, 0].set_title('Node 2 L3C')
axs[1, 0].plot(x, node1_L3C0_rd_hit_percent, linestyle='-', label='node2_L3C0')
axs[1, 0].plot(x, node1_L3C1_rd_hit_percent, linestyle='-', label='node2_L3C1')
axs[1, 0].plot(x, node1_L3C2_rd_hit_percent, linestyle='-', label='node2_L3C2')
axs[1, 0].plot(x, node1_L3C3_rd_hit_percent, linestyle='-', label='node2_L3C3')
axs[1, 0].plot(x, node1_L3C4_rd_hit_percent, linestyle='-', label='node2_L3C4')
axs[1, 0].plot(x, node1_L3C5_rd_hit_percent, linestyle='-', label='node2_L3C5')
axs[1, 0].set_xlabel('time(s)')  # 设置 x 轴标签
axs[1, 0].legend() # 添加图例
axs[1, 0].grid(True)  # 添加网格线（可选）
axs[1, 0].set_ylabel('read hit percent(%)')  # 设置 y 轴标签

axs[1, 1].set_title('Node 3 L3C')
axs[1, 1].plot(x, node1_L3C0_rd_hit_percent, linestyle='-', label='node3_L3C0')
axs[1, 1].plot(x, node1_L3C1_rd_hit_percent, linestyle='-', label='node3_L3C1')
axs[1, 1].plot(x, node1_L3C2_rd_hit_percent, linestyle='-', label='node3_L3C2')
axs[1, 1].plot(x, node1_L3C3_rd_hit_percent, linestyle='-', label='node3_L3C3')
axs[1, 1].plot(x, node1_L3C4_rd_hit_percent, linestyle='-', label='node3_L3C4')
axs[1, 1].plot(x, node1_L3C5_rd_hit_percent, linestyle='-', label='node3_L3C5')
axs[1, 1].set_xlabel('time(s)')  # 设置 x 轴标签
axs[1, 1].legend() # 添加图例
axs[1, 1].grid(True)  # 添加网格线（可选）
axs[1, 1].set_ylabel('read hit percent(%)')  # 设置 y 轴标签

# 调整子图之间的间距
plt.tight_layout()
# 保存图像为文件（例如 PNG、SVG、PDF 等格式）
plt.savefig(appConfig + '-L3C-Statistic-per_node_read_hit_percent.png')  # 替换为你想要保存的文件名和格式
# 关闭图像
plt.close()