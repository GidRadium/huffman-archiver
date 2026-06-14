import os
import statistics
import subprocess
import time
from collections import Counter
from dataclasses import dataclass
from math import log2
from pathlib import Path

ROOT = Path(__file__).parent
DATASETS = ROOT / "datasets"
ARCHIVES = ROOT / "archives"
RESTORED = ROOT / "restored"

DATASETS.mkdir(exist_ok=True)
ARCHIVES.mkdir(exist_ok=True)
RESTORED.mkdir(exist_ok=True)

ARCHIVER = "../build-release/harch-cli"

RUNS = 10
RUNS_DELAY_SECONDS = 0.2

SIZES = [
    64 * 1024,
    256 * 1024,
    1024 * 1024,
    4 * 1024 * 1024,
    16 * 1024 * 1024,
    32 * 1024 * 1024,
    64 * 1024 * 1024,
    128 * 1024 * 1024,
    256 * 1024 * 1024,
]

DATA_TYPES = [
    "uniform",
    "periodic2b",
    "periodic4b",
    "periodic8b",
    "periodic16b",
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
        case "periodic8b":
            pattern = b"ABCDEFGHIJKLMNOP"
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
            "/usr/bin/time",
            "-f",
            "%U %S",
            *command,
        ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
        check=True,
    )

    user_time, system_time = map(float, result.stderr.strip().split())

    return user_time + system_time


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
            measure_time([ARCHIVER, "c", str(input_file), str(archive_file)])
        )
        time.sleep(RUNS_DELAY_SECONDS)

    result.compressed_size = archive_file.stat().st_size

    for _ in range(RUNS):
        restored_file.unlink(missing_ok=True)
        decompression_times.append(
            measure_time([ARCHIVER, "d", str(archive_file), str(restored_file)])
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
            generate_data_file(data_type, size)


results: list[BenchmarkResult] = []


def run_benchmarks():
    for data_type in DATA_TYPES:
        for size in SIZES:
            results.append(benchmark(data_type, size))


def main():
    pass


if __name__ == "__main__":
    main()
