import csv
import os
import statistics
import subprocess
import time
from collections import Counter
from dataclasses import dataclass
from math import log2
from pathlib import Path
from types import SimpleNamespace

import matplotlib.pyplot as plt
import requests

ROOT = Path(__file__).parent
DATASETS = ROOT / "datasets"
ARCHIVES = ROOT / "archives"
RESTORED = ROOT / "restored"

DATASETS.mkdir(exist_ok=True)
ARCHIVES.mkdir(exist_ok=True)
RESTORED.mkdir(exist_ok=True)

ARCHIVER = "../build-release/harch-cli"

RUNS = 10
RUNS_DELAY_SECONDS = 0.0

SIZES = [
    1,
    2,
    4,
    8,
    16,
    32,
    64,
    128,
    256,
    512,
    1024,
    2 * 1024,
    4 * 1024,
    8 * 1024,
    16 * 1024,
    32 * 1024,
    64 * 1024,
    128 * 1024,
    256 * 1024,
    512 * 1024,
    1024 * 1024,
    2 * 1024 * 1024,
    4 * 1024 * 1024,
    8 * 1024 * 1024,
    16 * 1024 * 1024,
    32 * 1024 * 1024,
    64 * 1024 * 1024,
    128 * 1024 * 1024,
]

DATA_TYPES = {
    "uniform": b"A",
    "periodic2b": b"AB",
    "periodic4b": b"ABCD",
    "periodic8b": b"ABCDEFGH",
    "periodic16b": b"ABCDEFGHIJKLMNOP",
    "periodic32b": b"ABCDEFGHIJKLMNOPabcdefghijklmnop",
    "random": b"",
}


REAL_TYPES = {
    "txt": "https://www.gutenberg.org/cache/epub/1342/pg1342.txt",
    "csv": "https://filesamples.com/samples/document/csv/sample4.csv",
    "json": "https://filesamples.com/samples/code/json/sample4.json",
    "bmp": "https://filesamples.com/samples/image/bmp/sample_640%C3%97426.bmp",
    "jpeg": "https://filesamples.com/samples/image/jpeg/sample_640%C3%97426.jpeg",
    "png": "https://filesamples.com/samples/image/png/sample_640%C3%97426.png",
    "wav": "https://filesamples.com/samples/audio/wav/sample3.wav",
    "mp3": "https://filesamples.com/samples/audio/mp3/sample3.mp3",
    "mp4": "https://filesamples.com/samples/video/mp4/sample_640x360.mp4",
    "zip": "https://samplefile.com/samples/download/archive/zip/accounting_system_exchange_fixture_pack.zip/",
    "7z": "https://samplefile.com/samples/download/archive/7z/7z_sample_file_1MB.7z/",
    "exe": "https://github.com/git-for-windows/git/releases/download/v2.45.1.windows.1/MinGit-2.45.1-64-bit.zip",
}


def generate_data_file(data_type: str, data_size: int):
    file_dir = DATASETS / data_type
    file_dir.mkdir(exist_ok=True)
    file_path = file_dir / f"{data_size}.bin"
    with open(file_path, "wb") as file:
        if data_type == "random":
            file.write(os.urandom(data_size))
        else:
            file.write(
                (DATA_TYPES[data_type] * (data_size // len(DATA_TYPES[data_type]) + 1))[
                    :data_size
                ]
            )


@dataclass
class BenchmarkResult:
    file_type: str = ""  # One of DATA_TYPES
    file_size: int = 0  # One of SIZES, bytes
    compressed_size: int = 0  # bytes
    compression_ratio: float = 0.0
    compression_time_mean: float = 0.0  # seconds
    compression_time_std: float = 0.0  # seconds
    decompression_time_mean: float = 0.0  # seconds
    decompression_time_std: float = 0.0  # seconds
    compression_speed: float = 0.0  # bytes per second
    decompression_speed: float = 0.0  # bytes per second
    compression_speed_std: float = 0.0  # bytes per second
    decompression_speed_std: float = 0.0  # bytes per second
    data_entropy: float = 0.0


def entropy(path: Path) -> float:
    data = path.read_bytes()
    counts = Counter(data)
    n = len(data)

    return -sum((count / n) * log2(count / n) for count in counts.values())


def measure_time(command: list[str]) -> float:
    result = subprocess.run(
        [
            "perf",
            "stat",
            "-x",
            ";",
            *command,
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
        check=True,
    )

    for line in result.stderr.splitlines():
        fields = line.split(";")

        if len(fields) < 3:
            continue

        metric = fields[2].strip()

        if metric.startswith("task-clock"):
            milliseconds = float(fields[0].replace(",", "."))

            return milliseconds / 1000.0

    raise RuntimeError(f"task-clock not found:\n{result.stderr}")


def benchmark(data_type: str, data_size: int) -> BenchmarkResult:
    result = BenchmarkResult(file_type=data_type, file_size=data_size)

    input_file = DATASETS / data_type / f"{data_size}.bin"

    archive_file = ARCHIVES / f"{data_type}_{data_size}.huff"
    restored_file = RESTORED / f"{data_type}_{data_size}.bin"

    compression_times = []
    decompression_times = []

    for _ in range(RUNS):
        archive_file.unlink(missing_ok=True)
        compression_times.append(
            measure_time(
                [ARCHIVER, "c", str(input_file), str(archive_file), "-m", "ram"]
            )
        )
        time.sleep(RUNS_DELAY_SECONDS)

    result.compressed_size = archive_file.stat().st_size

    for _ in range(RUNS):
        restored_file.unlink(missing_ok=True)
        decompression_times.append(
            measure_time(
                [ARCHIVER, "d", str(archive_file), str(restored_file), "-m", "ram"]
            )
        )
        time.sleep(RUNS_DELAY_SECONDS)

    if input_file.stat().st_size != restored_file.stat().st_size:
        raise RuntimeError(f"Verification failed for {input_file}")

    result.compression_ratio = result.compressed_size / result.file_size
    result.compression_time_mean = statistics.mean(compression_times)
    result.compression_time_std = (
        statistics.stdev(compression_times) if len(compression_times) > 1 else 0.0
    )
    result.decompression_time_mean = statistics.mean(decompression_times)
    result.decompression_time_std = (
        statistics.stdev(decompression_times) if len(decompression_times) > 1 else 0.0
    )
    result.compression_speed = result.file_size / result.compression_time_mean
    result.decompression_speed = result.compressed_size / result.decompression_time_mean

    if result.compression_time_mean > 0 and result.compression_time_std > 0:
        result.compression_speed_std = result.compression_speed * (
            result.compression_time_std / result.compression_time_mean
        )
    else:
        result.compression_speed_std = 0.0

    if result.decompression_time_mean > 0 and result.decompression_time_std > 0:
        result.decompression_speed_std = result.decompression_speed * (
            result.decompression_time_std / result.decompression_time_mean
        )
    else:
        result.decompression_speed_std = 0.0

    result.data_entropy = entropy(input_file)

    return result


def load_real_patterns():
    print("Loading patterns...")

    for name, url in REAL_TYPES.items():
        print(f"download {name}")

        response = requests.get(
            url,
            timeout=120,
            headers={
                "User-Agent": "huffman-benchmark/1.0",
            },
        )
        response.raise_for_status()

        data = response.content[: 3 * 1024 * 1024]

        if not data:
            raise RuntimeError(f"Downloaded empty file for {name}")

        DATA_TYPES[name] = data

        print(f"loaded {name}: {len(data):,} bytes".replace(",", " "))


# ROOT / "datasets" / f"{data_type}" / f"{data_size}.bin"
def generate_datasets():
    print("Generating datasets...")

    for data_type in DATA_TYPES:
        for size in SIZES:
            print(f"gener {data_type} + {size}")
            generate_data_file(data_type, size)


results: list[BenchmarkResult] = []


def run_benchmarks():
    print("Running benchmarks...")

    for data_type in DATA_TYPES:
        for size in SIZES:
            print(f"bench {data_type} + {size}")
            results.append(benchmark(data_type, size))


def save_results_to_csv():
    print("Saving results...")

    csv_path = ROOT / "benchmark_results.csv"
    fieldnames = [
        "file_type",
        "file_size",
        "compressed_size",
        "compression_ratio",
        "compression_time_mean",
        "compression_time_std",
        "decompression_time_mean",
        "decompression_time_std",
        "compression_speed",
        "compression_speed_std",
        "decompression_speed",
        "decompression_speed_std",
        "data_entropy",
    ]

    with open(csv_path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        for r in results:
            writer.writerow(
                {
                    "file_type": r.file_type,
                    "file_size": r.file_size,
                    "compressed_size": r.compressed_size,
                    "compression_ratio": r.compression_ratio,
                    "compression_time_mean": r.compression_time_mean,
                    "compression_time_std": r.compression_time_std,
                    "decompression_time_mean": r.decompression_time_mean,
                    "decompression_time_std": r.decompression_time_std,
                    "compression_speed": r.compression_speed,
                    "compression_speed_std": r.compression_speed_std,
                    "decompression_speed": r.decompression_speed,
                    "decompression_speed_std": r.decompression_speed_std,
                    "data_entropy": r.data_entropy,
                }
            )

    print(f"Results saved to {csv_path}")


def _plot_metric(
    by_type,
    plots_dir,
    x,
    y,
    xlabel,
    ylabel,
    title,
    fname,
    log_x,
    grid_kwargs,
    yerr=None,
    min_file_size=0,
):
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        filtered = [r for r in recs if r.file_size >= min_file_size]
        if not filtered:
            continue

        xs = [getattr(r, x) for r in filtered]
        ys = [getattr(r, y) for r in filtered]
        if yerr:
            errs = [getattr(r, yerr) for r in filtered]
            plt.errorbar(xs, ys, yerr=errs, marker="o", label=dtype, capsize=3)
        else:
            plt.plot(xs, ys, marker="o", label=dtype)

    if min_file_size > 0:
        note = f"Min file size ≥ {min_file_size:,} B".replace(",", " ")
        plt.text(
            0.99,
            0.01,
            note,
            transform=plt.gca().transAxes,
            fontsize=9,
            color="grey",
            ha="right",
            va="bottom",
            bbox=dict(boxstyle="round,pad=0.3", facecolor="white", alpha=0.7),
        )

    if log_x:
        plt.xscale("log")
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True, **grid_kwargs)
    plt.legend()
    plt.tight_layout()

    png_dir = plots_dir / "png"
    svg_dir = plots_dir / "svg"
    png_dir.mkdir(parents=True, exist_ok=True)
    svg_dir.mkdir(parents=True, exist_ok=True)

    plt.savefig(png_dir / f"{fname}.png")
    plt.savefig(svg_dir / f"{fname}.svg")
    plt.close()


def generate_plots():
    print("Generating plots...")

    plots_dir = ROOT / "plots"
    plots_dir.mkdir(exist_ok=True)

    csv_path = ROOT / "benchmark_results.csv"
    records = []
    with open(csv_path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rec = SimpleNamespace()
            for field, value in row.items():
                if field == "file_type":
                    setattr(rec, field, value)
                else:
                    try:
                        setattr(rec, field, float(value))
                    except (ValueError, TypeError):
                        setattr(rec, field, 0.0)
            records.append(rec)

    by_type = {}
    for r in records:
        by_type.setdefault(r.file_type, []).append(r)
    for recs in by_type.values():
        recs.sort(key=lambda x: x.file_size)

    specs = [
        {
            "x": "file_size",
            "y": "compression_ratio",
            "xlabel": "File size (bytes)",
            "ylabel": "Compression ratio (compressed/original)",
            "title": "Compression ratio vs file size",
            "fname": "compression_ratio",
            "log_x": True,
            "grid_kwargs": {"which": "both", "linestyle": "--", "linewidth": 0.5},
            "min_file_size": 512,
        },
        {
            "x": "file_size",
            "y": "compression_speed",
            "xlabel": "File size (bytes)",
            "ylabel": "Compression speed (bytes/s)",
            "title": "Compression speed vs file size",
            "fname": "compression_speed",
            "log_x": True,
            "grid_kwargs": {"which": "both", "linestyle": "--", "linewidth": 0.5},
            "yerr": "compression_speed_std",
        },
        {
            "x": "compressed_size",
            "y": "decompression_speed",
            "xlabel": "Compressed file size (bytes)",
            "ylabel": "Decompression speed (bytes/s)",
            "title": "Decompression speed vs file size",
            "fname": "decompression_speed",
            "log_x": True,
            "grid_kwargs": {"which": "both", "linestyle": "--", "linewidth": 0.5},
            "yerr": "decompression_speed_std",
        },
        {
            "x": "file_size",
            "y": "compression_time_mean",
            "xlabel": "File size (bytes)",
            "ylabel": "Compression time (s)",
            "title": "Compression time vs file size",
            "fname": "compression_time",
            "log_x": False,
            "grid_kwargs": {"which": "both", "linestyle": "--", "linewidth": 0.5},
            "yerr": "compression_time_std",
        },
        {
            "x": "file_size",
            "y": "decompression_time_mean",
            "xlabel": "File size (bytes)",
            "ylabel": "Decompression time (s)",
            "title": "Decompression time vs file size",
            "fname": "decompression_time",
            "log_x": False,
            "grid_kwargs": {"which": "both", "linestyle": "--", "linewidth": 0.5},
            "yerr": "decompression_time_std",
        },
        {
            "x": "data_entropy",
            "y": "compression_ratio",
            "xlabel": "Entropy (bits/byte)",
            "ylabel": "Compression ratio (compressed/original)",
            "title": "Compression ratio vs entropy",
            "fname": "compression_ratio_vs_entropy",
            "log_x": False,
            "grid_kwargs": {"linestyle": "--", "linewidth": 0.5},
            "min_file_size": 512,
        },
        {
            "x": "data_entropy",
            "y": "compression_speed",
            "xlabel": "Entropy (bits/byte)",
            "ylabel": "Compression speed (bytes/s)",
            "title": "Compression speed vs entropy",
            "fname": "compression_speed_vs_entropy",
            "log_x": False,
            "grid_kwargs": {"linestyle": "--", "linewidth": 0.5},
            "yerr": "compression_speed_std",
            "min_file_size": 512 * 1024,
        },
        {
            "x": "data_entropy",
            "y": "decompression_speed",
            "xlabel": "Entropy (bits/byte)",
            "ylabel": "Decompression speed (bytes/s)",
            "title": "Decompression speed vs entropy",
            "fname": "decompression_speed_vs_entropy",
            "log_x": False,
            "grid_kwargs": {"linestyle": "--", "linewidth": 0.5},
            "yerr": "decompression_speed_std",
            "min_file_size": 512 * 1024,
        },
    ]

    for spec in specs:
        _plot_metric(by_type, plots_dir, **spec)

    print(f"Plots saved to {plots_dir}")


def main():
    load_real_patterns()
    generate_datasets()
    run_benchmarks()
    save_results_to_csv()
    generate_plots()


if __name__ == "__main__":
    main()
