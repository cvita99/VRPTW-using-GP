import matplotlib.pyplot as plt
import re

# Replace with your actual file name
filename = 'serial1.txt'

# Lists to store iteration and fitness values
iterations = []
fitness_values = []

# Regex pattern to extract iteration and fitness
pattern = re.compile(r"iteration (\d+): best sol so far: ([\d.]+)")

# Read and parse the file
with open(filename, 'r') as file:
    for line in file:
        match = pattern.search(line)
        if match:
            iteration = int(match.group(1))
            fitness = float(match.group(2))
            iterations.append(iteration)
            fitness_values.append(fitness)

# Plotting the convergence graph
plt.figure(figsize=(12, 6))
plt.plot(iterations, fitness_values, marker='o', linestyle='-', color='darkgreen')
plt.title('Convergence of Best Syntax Tree Fitness Over Iterations')
plt.xlabel('Iteration')
plt.ylabel('Best Fitness (Lower is Better)')
plt.grid(True)
plt.tight_layout()
plt.show()
