#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
任务一图表：数据已硬编码（来自一次实测 summary.csv），无需读 CSV。
在 Windows 上：pip install matplotlib 后运行 python plot_task1.py
图片保存目录：与本脚本同级的 figures/task1/
"""

from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

# -----------------------------------------------------------------------------
# 输出目录（脚本所在目录下的 figures/task1）
# -----------------------------------------------------------------------------
OUTPUT_DIR = Path(__file__).resolve().parent / "figures" / "task1"

# True：标题/图例用中文（建议在 Windows 且已安装「微软雅黑」等字体时使用）
USE_CHINESE_LABELS = False

if USE_CHINESE_LABELS:
    plt.rcParams["font.sans-serif"] = ["Microsoft YaHei", "SimHei", "Arial Unicode MS", "DejaVu Sans"]
    plt.rcParams["axes.unicode_minus"] = False

# -----------------------------------------------------------------------------
# 硬编码数据（单位：QPS；benchmark：顺序写 / 随机写 / 随机读）
# 来源示例（可自行改成你的最终数据）：
# write_buffer_size: 4194304=4MB, 16777216=16MB, 67108864=64MB
# block_size: 4096, 16384, 65536（字节）
# bloom_filter: readrandom 对比 Bloom 关 vs 10 bits/key
# -----------------------------------------------------------------------------

# write_buffer_size（MemTable）：横轴标签用 MB
WB_LABELS_EN = ["4 MB", "16 MB", "64 MB"]
WB_LABELS_ZH = ["4 MB", "16 MB", "64 MB"]
WB_QPS_EN = {
    "fillseq": [223413.76, 166889.19, 181719.06],
    "fillrandom": [12386.66, 16018.45, 25726.78],
    "readrandom": [301477.24, 146284.38, 134662.00],
}
WB_QPS_ZH = {
    "顺序写 fillseq": [223413.76, 166889.19, 181719.06],
    "随机写 fillrandom": [12386.66, 16018.45, 25726.78],
    "随机读 readrandom": [301477.24, 146284.38, 134662.00],
}

# block_size：横轴用 KB 便于阅读
BS_LABELS_EN = ["4 KB", "16 KB", "64 KB"]
BS_LABELS_ZH = ["4 KB", "16 KB", "64 KB"]
BS_QPS_EN = {
    "fillseq": [96070.71, 93170.60, 90171.33],
    "fillrandom": [8808.79, 8581.55, 9110.87],
    "readrandom": [207425.85, 193986.42, 174428.75],
}
BS_QPS_ZH = {
    "顺序写 fillseq": [96070.71, 93170.60, 90171.33],
    "随机写 fillrandom": [8808.79, 8581.55, 9110.87],
    "随机读 readrandom": [207425.85, 193986.42, 174428.75],
}

# Bloom：随机读 QPS（作业重点）
BLOOM_READ_LABELS_EN = ["Bloom off", "Bloom 10 bits/key"]
BLOOM_READ_LABELS_ZH = ["Bloom 关闭", "Bloom 10 bits/key"]
BLOOM_READ_QPS = [217817.47, 214454.21]

# Bloom：随机写 QPS（加载阶段参考，可选单独小图）
BLOOM_WRITE_LABELS_EN = ["Bloom off", "Bloom 10 bits/key"]
BLOOM_WRITE_LABELS_ZH = ["Bloom 关闭", "Bloom 10 bits/key"]
BLOOM_WRITE_QPS = [9159.44, 8375.07]


def grouped_bar_qps(labels: list, series: dict, title: str, ylabel: str, filename: str):
    """分组柱状图：labels 为横轴类别，series 为 {图例名: [每个类别一根柱子的 QPS]}"""
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    x = np.arange(len(labels))
    names = list(series.keys())
    n = len(names)
    width = 0.8 / max(n, 1)

    fig, ax = plt.subplots(figsize=(10, 5.8))
    for i, name in enumerate(names):
        offset = width * (i - (n - 1) / 2)
        bars = ax.bar(x + offset, series[name], width, label=name)
        for bar, v in zip(bars, series[name]):
            ax.text(
                bar.get_x() + bar.get_width() / 2,
                bar.get_height(),
                f"{v:,.0f}",
                ha="center",
                va="bottom",
                fontsize=8,
                clip_on=False,
            )

    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.legend()
    ax.grid(axis="y", linestyle="--", alpha=0.35)
    ax.margins(y=0.18)

    fig.tight_layout()
    path = OUTPUT_DIR / filename
    fig.savefig(path, dpi=150)
    plt.close(fig)
    print("saved:", path)


def simple_bar_qps(labels: list, values: list, title: str, ylabel: str, filename: str):
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)
    fig, ax = plt.subplots(figsize=(7, 4))
    x = np.arange(len(labels))
    ax.bar(x, values, color=["#4477AA", "#EE6677"])
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(axis="y", linestyle="--", alpha=0.35)
    for i, v in enumerate(values):
        ax.text(i, v, f"{v:,.0f}", ha="center", va="bottom", fontsize=9)
    fig.tight_layout()
    path = OUTPUT_DIR / filename
    fig.savefig(path, dpi=150)
    plt.close(fig)
    print("saved:", path)


def main():
    if USE_CHINESE_LABELS:
        wb_labels, wb_series = WB_LABELS_ZH, WB_QPS_ZH
        bs_labels, bs_series = BS_LABELS_ZH, BS_QPS_ZH
        bloom_read_lbl = BLOOM_READ_LABELS_ZH
        bloom_write_lbl = BLOOM_WRITE_LABELS_ZH
        titles = (
            "任务一：write_buffer_size（MemTable）与 QPS",
            "任务一：block_size（SSTable block）与 QPS",
            "任务一：Bloom Filter 与 readrandom QPS",
            "任务一：Bloom Filter 与 fillrandom QPS（参考）",
        )
        ylabel = "QPS（越高越好）"
    else:
        wb_labels, wb_series = WB_LABELS_EN, WB_QPS_EN
        bs_labels, bs_series = BS_LABELS_EN, BS_QPS_EN
        bloom_read_lbl = BLOOM_READ_LABELS_EN
        bloom_write_lbl = BLOOM_WRITE_LABELS_EN
        titles = (
            "Task 1: write_buffer_size vs throughput (QPS)",
            "Task 1: block_size vs throughput (QPS)",
            "Task 1: Bloom filter vs readrandom QPS",
            "Task 1: Bloom filter vs fillrandom QPS (reference)",
        )
        ylabel = "QPS (higher is better)"

    grouped_bar_qps(
        wb_labels,
        wb_series,
        title=titles[0],
        ylabel=ylabel,
        filename="task1_write_buffer_qps.png",
    )
    grouped_bar_qps(
        bs_labels,
        bs_series,
        title=titles[1],
        ylabel=ylabel,
        filename="task1_block_size_qps.png",
    )
    simple_bar_qps(
        bloom_read_lbl,
        BLOOM_READ_QPS,
        title=titles[2],
        ylabel=ylabel,
        filename="task1_bloom_readrandom_qps.png",
    )
    simple_bar_qps(
        bloom_write_lbl,
        BLOOM_WRITE_QPS,
        title=titles[3],
        ylabel=ylabel,
        filename="task1_bloom_fillrandom_qps.png",
    )

    print("\n全部图片目录:", OUTPUT_DIR.resolve())


if __name__ == "__main__":
    main()
