#!/bin/bash
# Directory containing the benchmark executable and configurations
# 
# Note: If errors, you need to give permissions- do the following:
# cd ~/ece-562-project/tests/benchmark_dir
# "pwd" will return absolute path
# chmod +x [absolute path]

BENCHMARK_DIR="./tests/benchmark_dir"
CONFIG_DIR="./tests"
# Ensure the benchmark executable exists
if [ ! -f "$BENCHMARK_DIR/benchmark" ]; then
echo "Error: Benchmark executable not found in $BENCHMARK_DIR."
exit 1
fi
# Define an array of configuration files
configs=("ece-562-simple.cfg")
# configs=("ece-562-simple.cfg" "simple.cfg")

# Loop through configurations and run benchmark for each
for config in "${configs[@]}"; do
config_path="$CONFIG_DIR/$config"
if [ ! -f "$config_path" ]; then
echo "Warning: Configuration file $config_path not found. Skipping..."
continue
fi
echo "Running benchmark with configuration: $config"
"$BENCHMARK_DIR/benchmark" "$config_path"
done
# Summarize
echo "Benchmark completed."
