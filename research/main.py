import csv
import os
import statistics
import subprocess
import time
from collections import Counter
from dataclasses import dataclass
from math import log2
from pathlib import Path

import matplotlib.pyplot as plt

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
    4,
    16,
    64,
    256,
    1024,
    4 * 1024,
    16 * 1024,
    64 * 1024,
    256 * 1024,
    1024 * 1024,
    4 * 1024 * 1024,
    16 * 1024 * 1024,
    64 * 1024 * 1024,
    # 256 * 1024 * 1024,
]

DATA_TYPES = [
    "uniform",
    "periodic2b",
    "periodic4b",
    "periodic8b",
    "periodic16b",
    "periodic32b",
    "random",
    "text",
]


def generate_data_file(data_type: str, data_size: int):
    pattern = b""
    match data_type:
        case "uniform":
            pattern = b"A"
        case "periodic2b":
            pattern = b"AB"
        case "periodic4b":
            pattern = b"ABCD"
        case "periodic8b":
            pattern = b"ABCDEFGH"
        case "periodic16b":
            pattern = b"ABCDEFGHIJKLMNOP"
        case "periodic32b":
            pattern = b"ABCDEFGHIJKLMNOPabcdefghijklmnop"
        case "text":
            pattern = (
                b"Console archiver based on the Huffman algorithm.\n"
                b"Works with files of any size.\n"
                b"Can compress either in RAM or after two passes of the file.\n"
            )
        case "random":
            pass
        case _:
            return

    file_dir = DATASETS / data_type
    file_dir.mkdir(exist_ok=True)
    file_path = file_dir / f"{data_size}.bin"
    with open(file_path, "wb") as file:
        if data_type == "random":
            file.write(os.urandom(data_size))
        else:
            file.write((pattern * (data_size // len(pattern) + 1))[:data_size])


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

    if input_file.read_bytes() != restored_file.read_bytes():
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
    result.decompression_speed = result.file_size / result.decompression_time_mean
    result.data_entropy = entropy(input_file)

    return result


# ROOT / "datasets" / f"{data_type}" / f"{data_size}.bin"
def generate_datasets():
    for data_type in DATA_TYPES:
        for size in SIZES:
            print(f"gener {data_type} + {size}")
            generate_data_file(data_type, size)


results: list[BenchmarkResult] = []


def run_benchmarks():
    for data_type in DATA_TYPES:
        for size in SIZES:
            print(f"bench {data_type} + {size}")
            results.append(benchmark(data_type, size))


def save_results_to_csv():
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
        "decompression_speed",
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
                    "decompression_speed": r.decompression_speed,
                    "data_entropy": r.data_entropy,
                }
            )

    print(f"Results saved to {csv_path}")


def generate_plots():
    plots_dir = ROOT / "plots"
    plots_dir.mkdir(exist_ok=True)

    by_type = {}
    for r in results:
        by_type.setdefault(r.file_type, []).append(r)

    for data_type in by_type:
        by_type[data_type].sort(key=lambda x: x.file_size)

    # compression_ratio.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        sizes = [r.file_size for r in recs]
        ratios = [r.compression_ratio for r in recs]
        plt.plot(sizes, ratios, marker="o", label=dtype)
    plt.xscale("log")
    plt.xlabel("File size (bytes)")
    plt.ylabel("Compression ratio (compressed/original)")
    plt.title("Compression ratio vs file size")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "compression_ratio.png")
    plt.savefig(plots_dir / "compression_ratio.svg")
    plt.close()

    # compression_speed.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        sizes = [r.file_size for r in recs]
        speeds = [r.compression_speed for r in recs]
        plt.plot(sizes, speeds, marker="o", label=dtype)
    plt.xscale("log")
    plt.xlabel("File size (bytes)")
    plt.ylabel("Compression speed (bytes/s)")
    plt.title("Compression speed vs file size")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "compression_speed.png")
    plt.savefig(plots_dir / "compression_speed.svg")
    plt.close()

    # decompression_speed.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        sizes = [r.file_size for r in recs]
        speeds = [r.decompression_speed for r in recs]
        plt.plot(sizes, speeds, marker="o", label=dtype)
    plt.xscale("log")
    plt.xlabel("File size (bytes)")
    plt.ylabel("Decompression speed (bytes/s)")
    plt.title("Decompression speed vs file size")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "decompression_speed.png")
    plt.savefig(plots_dir / "decompression_speed.svg")
    plt.close()

    # compression_time.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        sizes = [r.file_size for r in recs]
        times = [r.compression_time_mean for r in recs]
        plt.plot(sizes, times, marker="o", label=dtype)
    plt.xscale("log")
    plt.xlabel("File size (bytes)")
    plt.ylabel("Compression time (s)")
    plt.title("Compression time vs file size")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "compression_time.png")
    plt.savefig(plots_dir / "compression_time.svg")
    plt.close()

    # decompression_time.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        sizes = [r.file_size for r in recs]
        times = [r.decompression_time_mean for r in recs]
        plt.plot(sizes, times, marker="o", label=dtype)
    plt.xscale("log")
    plt.xlabel("File size (bytes)")
    plt.ylabel("Decompression time (s)")
    plt.title("Decompression time vs file size")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "decompression_time.png")
    plt.savefig(plots_dir / "decompression_time.svg")
    plt.close()

    # compression_ratio_vs_entropy.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        entropy = [r.data_entropy for r in recs]
        ratios = [r.compression_ratio for r in recs]
        plt.plot(entropy, ratios, marker="o", label=dtype)
    plt.xlabel("Entropy (bits/byte)")
    plt.ylabel("Compression ratio (compressed/original)")
    plt.title("Compression ratio vs entropy")
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "compression_ratio_vs_entropy.png")
    plt.savefig(plots_dir / "compression_ratio_vs_entropy.svg")
    plt.close()

    # compression_speed_vs_entropy.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        entropy = [r.data_entropy for r in recs]
        speeds = [r.compression_speed for r in recs]
        plt.plot(entropy, speeds, marker="o", label=dtype)
    plt.xlabel("Entropy (bits/byte)")
    plt.ylabel("Compression speed (bytes/s)")
    plt.title("Compression speed vs entropy")
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "compression_speed_vs_entropy.png")
    plt.savefig(plots_dir / "compression_speed_vs_entropy.svg")
    plt.close()

    # decompression_speed_vs_entropy.png
    plt.figure(figsize=(10, 6))
    for dtype, recs in by_type.items():
        entropy = [r.data_entropy for r in recs]
        speeds = [r.decompression_speed for r in recs]
        plt.plot(entropy, speeds, marker="o", label=dtype)
    plt.xlabel("Entropy (bits/byte)")
    plt.ylabel("Decompression speed (bytes/s)")
    plt.title("Decompression speed vs entropy")
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.legend()
    plt.tight_layout()
    plt.savefig(plots_dir / "decompression_speed_vs_entropy.png")
    plt.savefig(plots_dir / "decompression_speed_vs_entropy.svg")
    plt.close()

    print(f"Plots saved to {plots_dir}")


def main():
    print("Generating datasets...")
    generate_datasets()

    print("Running benchmarks...")
    run_benchmarks()

    print("Saving results...")
    save_results_to_csv()

    print("Generating plots...")
    generate_plots()


if __name__ == "__main__":
    main()
