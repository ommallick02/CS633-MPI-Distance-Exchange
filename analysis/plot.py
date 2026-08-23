import matplotlib.pyplot as plt
import numpy as np

# =========================
# Load Data From File
# =========================

data = {}

with open("timing_data.txt", "r") as f:
    next(f)  # skip header line
    for line in f:
        if line.strip() == "":
            continue
        P, M, T = line.split()
        P = int(P)
        M = int(M)
        T = float(T)

        if P not in data:
            data[P] = {}
        if M not in data[P]:
            data[P][M] = []

        data[P][M].append(T)

# =========================
# Prepare Data for Single Plot
# =========================

box_data = []
labels = []

for P in sorted(data):
    box_data.append(data[P][262144])
    labels.append(f"P={P}\nM=262144")

    box_data.append(data[P][1048576])
    labels.append(f"P={P}\nM=1048576")

positions = list(range(1, len(box_data) + 1))

# =========================
# Plot
# =========================

plt.figure(figsize=(12, 12))

# Boxplots
plt.boxplot(
    box_data,
    positions=positions,
    widths=0.5,
    showmeans=True
)

# Scatter points with jitter
for i, vals in enumerate(box_data):
    x = np.random.normal(positions[i], 0.05, size=len(vals))
    plt.scatter(x, vals, alpha=0.7, zorder=3)

# Mean line connecting all
means = [np.mean(vals) for vals in box_data]
plt.plot(positions, means, marker='o', linestyle='-', linewidth=2)

# Axis formatting
plt.xticks(positions, labels)
plt.ylabel("Execution Time (seconds)")
plt.xlabel("Process & Problem Size")

# Y ticks every 0.05 sec
ymin = min(min(v) for v in box_data)
ymax = max(max(v) for v in box_data)
plt.yticks(np.arange(0, ymax + 0.05, 0.05))

plt.grid(axis="y", linestyle="--", alpha=0.6)
plt.title("Execution Time Comparison ")

plt.tight_layout()
plt.savefig("plot.jpeg", dpi=300)
plt.show()