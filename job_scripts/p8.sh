#!/bin/bash
#SBATCH --job-name=mpi_hello
#SBATCH -N 1 #### max limit now is 1
#SBATCH --ntasks-per-node=16  #### max limit now is 4
#SBATCH --output=mpi_hello_%j.out
#SBATCH --error=mpi_hello_%j.err
#SBATCH --partition=cpu
#SBATCH --time=00:05:00


# Experiment parameters


P=8
D1=2
D2=4
T=10
SEED=1000

# M values
M_LIST=(262144 1048576)

# Number of repetitions
REPEAT=5


# Run experiments

for M in "${M_LIST[@]}"
do
    for ((i=1; i<=REPEAT; i++))
    do
        echo "Run $i | P=$P M=$M D1=$D1 D2=$D2 T=$T Seed=$SEED"
        mpirun -np $P ./execute $M $D1 $D2 $T $SEED
    done
done

