#!/usr/bin/env python3
"""Animate simulation CSV output on macOS/Linux with matplotlib."""

import argparse
import csv
import os
import sys
import tempfile
from collections import defaultdict


def configure_cache_dir():
    cache_dir = os.path.join(tempfile.gettempdir(), "nbody_matplotlib_cache")
    os.makedirs(cache_dir, exist_ok=True)
    os.environ.setdefault("MPLCONFIGDIR", cache_dir)
    os.environ.setdefault("XDG_CACHE_HOME", cache_dir)


def configure_backend():
    try:
        import matplotlib

        for backend in ("MacOSX", "TkAgg"):
            try:
                matplotlib.use(backend)
                return matplotlib
            except Exception:
                continue

        return matplotlib
    except ImportError as error:
        print(
            "matplotlib is required for animation.\n"
            "Install it with: python3 -m pip install matplotlib",
            file=sys.stderr,
        )
        raise SystemExit(1) from error


configure_cache_dir()
matplotlib = configure_backend()
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.animation import FuncAnimation  # noqa: E402


def parse_args():
    parser = argparse.ArgumentParser(description="Animate an N-body simulation CSV.")
    parser.add_argument("csv_path", help="Path to simulation CSV output")
    parser.add_argument(
        "--max-points",
        type=int,
        default=20000,
        help="Maximum number of bodies to draw per frame for responsiveness",
    )
    parser.add_argument(
        "--interval",
        type=int,
        default=50,
        help="Delay between frames in milliseconds",
    )
    parser.add_argument(
        "--size",
        type=float,
        default=4.0,
        help="Marker size for each body",
    )
    return parser.parse_args()


def load_frames(csv_path, max_points):
    frames = defaultdict(list)
    all_x = []
    all_y = []

    with open(csv_path, newline="", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)
        required = {"step", "body_id", "x", "y", "vx", "vy", "mass"}
        if not required.issubset(reader.fieldnames or set()):
            missing = sorted(required.difference(reader.fieldnames or set()))
            raise ValueError(f"CSV is missing required columns: {', '.join(missing)}")

        for row in reader:
            step = int(row["step"])
            if len(frames[step]) >= max_points:
                continue

            x = float(row["x"])
            y = float(row["y"])
            mass = float(row["mass"])
            frames[step].append((x, y, mass))
            all_x.append(x)
            all_y.append(y)

    ordered_steps = sorted(frames.keys())
    if not ordered_steps:
        raise ValueError("No frame data found in CSV")

    return ordered_steps, frames, all_x, all_y


def main():
    args = parse_args()
    steps, frames, all_x, all_y = load_frames(args.csv_path, args.max_points)

    min_x = min(all_x)
    max_x = max(all_x)
    min_y = min(all_y)
    max_y = max(all_y)
    padding_x = max(1.0, (max_x - min_x) * 0.05)
    padding_y = max(1.0, (max_y - min_y) * 0.05)

    figure, axis = plt.subplots(figsize=(8, 8))
    figure.canvas.manager.set_window_title("N-Body Simulation")
    axis.set_title("N-Body Simulation")
    axis.set_xlabel("x")
    axis.set_ylabel("y")
    axis.set_xlim(min_x - padding_x, max_x + padding_x)
    axis.set_ylim(min_y - padding_y, max_y + padding_y)
    axis.set_aspect("equal", adjustable="box")

    scatter = axis.scatter([], [], s=[], alpha=0.7, c=[], cmap="plasma")
    label = axis.text(0.02, 0.98, "", transform=axis.transAxes, va="top")

    def update(frame_index):
        step = steps[frame_index]
        points = frames[step]
        offsets = [(point[0], point[1]) for point in points]
        masses = [point[2] for point in points]
        sizes = [max(args.size, args.size * mass * 0.35) for mass in masses]

        scatter.set_offsets(offsets)
        scatter.set_sizes(sizes)
        scatter.set_array(masses)
        label.set_text(f"step={step}  shown={len(points)}")
        return scatter, label

    FuncAnimation(
        figure,
        update,
        frames=len(steps),
        interval=args.interval,
        blit=False,
        repeat=True,
    )
    plt.show()


if __name__ == "__main__":
    main()
