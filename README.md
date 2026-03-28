# W-DUGS N-Body Simulation

This project simulates 2D Newtonian gravity with double precision and Euler integration.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run a simulation

```bash
./build/nbody_sim simulate brute_force 1000 500 25 simulation.csv
```

To reuse the same random initial conditions every time, generate them once and pass the file back into `simulate`:

```bash
./build/nbody_sim generate 100000 bodies_100k.csv 42
./build/nbody_sim simulate barnes_hut 100000 200 10 simulation_100k.csv bodies_100k.csv
```

This writes snapshots to `simulation.csv` with columns:

```text
step,body_id,x,y,vx,vy,mass
```

## Run a benchmark

```bash
./build/nbody_sim benchmark brute_force 10 benchmark.csv
./build/nbody_sim benchmark barnes_hut 10 benchmark_bh.csv
```

Benchmark output uses:

```text
n,algorithm,steps,time_seconds
```

## Notes

- `brute_force` computes all pairwise interactions in `O(n^2)`.
- `barnes_hut` uses a QuadTree approximation for larger particle counts.
- A softening term is included to avoid singularities and unstable accelerations.
- `animation.py` can animate `simulation.csv` on macOS with Python 3 and matplotlib.
