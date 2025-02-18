import matplotlib.pyplot as plt
import sys
import statistics

def plot_durations(player1, player2, filename):
    with open(filename, 'r') as file:
        data = file.read().strip()

    durations = [int(x) for x in data.split(';') if x.strip()]

    # Calcul moyenne et écart-type
    mean_duration = statistics.mean(durations)
    std_dev_duration = statistics.stdev(durations) if len(durations) > 1 else 0  # éviter erreur si une seule partie

    # stats
    print(f"Moyenne des durées : {mean_duration:.2f} tours")
    print(f"Écart-type : {std_dev_duration:.2f} tours")

    # Génération du plot
    plt.figure(figsize=(10, 6))
    plt.plot(durations, marker='o', linestyle='-', color='b', label='Durée des parties')
    plt.axhline(mean_duration, color='r', linestyle='--', label=f'Moyenne: {mean_duration:.2f}')
    
    # titres et labels
    plt.title(f'Durées des parties de Breakthrough : {player1} vs {player2}')
    plt.xlabel('Index de la partie')
    plt.ylabel('Nombre de tours')
    plt.legend()
    plt.grid(True)

    plt.show()

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python script.py <Joueur1> <Joueur2> <Nom_du_fichier>")
        sys.exit(1)

    player1 = sys.argv[1]
    player2 = sys.argv[2]
    filename = sys.argv[3]

    plot_durations(player1, player2, filename)
