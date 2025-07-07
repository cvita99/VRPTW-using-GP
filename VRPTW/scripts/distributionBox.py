import matplotlib.pyplot as plt
import seaborn as sns

test_fitness_1 = [83871.1, 85489.3, 82896.6, 106390.0, 82479.2, 87133.4, 82141.3, 86021.9, 87965.1, 91435.5]
test_fitness_2 = [108627, 110755, 109780, 101517, 109758, 104933, 102048, 112902, 107587, 108352]
test_fitness_3 = [103589, 105337, 110213, 103869, 120086, 111829, 105067, 112905, 118739, 112862]

data = test_fitness_1 + test_fitness_2 + test_fitness_3
labels = ['Serijski'] * len(test_fitness_1) + ['Paralelni dubina 5'] * len(test_fitness_2) + ['Paralelni dubina 6'] * len(test_fitness_3)

sns.set(style="whitegrid")

plt.figure(figsize=(8, 5))
sns.boxplot(x=labels, y=data, palette="pastel")

plt.title("Usporedba distribucija")
plt.ylabel("Funkcija dobrote na testnom skupu")
plt.show()

