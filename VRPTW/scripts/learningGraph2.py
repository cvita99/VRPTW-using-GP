import re
import matplotlib.pyplot as plt
from collections import defaultdict
import sys


LOG_FILE = sys.argv[1]  

# === READ LOG FILE ===
with open(LOG_FILE, "r") as f:
    raw_text = f.read()

# === PARSE LOG CONTENT ===
run_blocks = raw_text.strip().split("################################")
iteration_data = defaultdict(lambda: defaultdict(list))

pattern = re.compile(
    r"\[TRAINING\]: iteration (\d+): best sol so far: (\d+)\s+\[TEST\]: iteration \1: best sol so far: (\d+)"
)

for block in run_blocks:
    for match in pattern.findall(block):
        iteration = int(match[0])
        train_fit = int(match[1])
        test_fit = int(match[2])
        iteration_data[iteration]["train"].append(train_fit)
        iteration_data[iteration]["test"].append(test_fit)

# === COMPUTE MEANS ===
sorted_iterations = sorted(iteration_data.keys())
mean_train = [sum(iteration_data[it]["train"]) / len(iteration_data[it]["train"]) for it in sorted_iterations]
mean_test = [sum(iteration_data[it]["test"]) / len(iteration_data[it]["test"]) for it in sorted_iterations]

# === ADD MANUAL 5000 ITERATION ===
if 5000 not in iteration_data:
    last_train = mean_train[-1]
    last_test = mean_test[-1]
    sorted_iterations.append(5000)
    mean_train.append(last_train)
    mean_test.append(last_test)

# === PLOT ===
plt.figure(figsize=(10, 6))
plt.plot(sorted_iterations, mean_train, label="Mean Training Fitness", marker="o")
plt.plot(sorted_iterations, mean_test, label="Mean Test Fitness", marker="s")
plt.xticks(fontsize=12)  
plt.yticks(fontsize=12) 
plt.title("Heuristike maksimalne dubine 6", fontsize=18) 

plt.xlabel("Iteracija", fontsize=18)
plt.ylabel("Funkcija dobrote", fontsize=18)

plt.grid(True)
plt.tight_layout()
plt.show()
