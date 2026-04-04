# Optimization of Partitioned Hash Join with Duplicates

## How to Run

1. Run the full grid search over all values of `N`, `P`, and all hash functions:

```bash
cd src
bash run_grid_search.sh
```

Then open and execute `statistics.ipynb` to inspect the corresponding statistics and plots.

2. Run the stability benchmark with fixed `N` and fixed hash function (varying only `P`):

```bash
cd src
bash stability_benchmark.sh <n_runs> <output_file.csv>
```

Then open and execute `stable_statistics.ipynb` to visualize the related stability statistics.
