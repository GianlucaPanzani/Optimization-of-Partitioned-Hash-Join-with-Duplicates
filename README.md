# Optimization of Partitioned Hash Join with Duplicates

## How to Run

1. Run the full grid search defined in `runners/grid_config.sh` (for a single run) with:

```bash
cd src
bash run_grid_search.sh
```

Then open and execute `statistics.ipynb` to inspect the corresponding statistics and plots.

2. Run the benchmark over the grid of parameters defined in `runners/grid_config_2.sh` for <n_runs> times, with:

```bash
cd src
bash benchmark.sh <n_runs> <output_file.csv>
```

Then open and execute `benchmark_statistics.ipynb` to visualize the related stability statistics.
