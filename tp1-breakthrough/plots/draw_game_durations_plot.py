import matplotlib.pyplot as plt
import sys
import statistics
import re

# Récupère les noms des deux joueurs depuis le fichier de log resume.txt
def get_player_names(resume_file):
    player1 = None
    player2 = None

    try:
        with open(resume_file, 'r') as f:
            data = f.read()

            matches = re.findall(r'./players/([a-zA-Z0-9_]+) is \d+', data)

        if len(matches) >= 2:
            player1 = matches[0]  # premier joueur
            player2 = matches[1]  # deuxième joueur
    except FileNotFoundError:
        print(f"Le fichier {resume_file} est introuvable")
        return

    return player1, player2

# Récupère les temps de partie depuis les fichiers logs log1.txt (joueur 1 commence) et log2.txt (joueur 2 commence)
def get_game_durations():
    durations = []

    for log_file in ['./new_stats/log1.txt', './new_stats/log2.txt']:
        try:
            with open(log_file, 'r') as file:
                data = file.read()

            # cherche les occurrences de la chaîne nb_turn: <nombre> dans le fichier
            matches = re.findall(r'nb_turn: (\d+)', data)

            for match in matches:
                print(match)
                durations.append(int(match))

        except FileNotFoundError:
            print(f"Le fichier {log_file} est introuvable.")
            continue
    
    return durations

def create_game_durations_plot(player1_name, player2_name, durations):
    # calcul moyenne et écart-type
    mean_duration = statistics.mean(durations)
    std_dev_duration = statistics.stdev(durations) if len(durations) > 1 else 0

    print(f"Moyenne des longeurs de partie : {mean_duration:.2f} tours")
    print(f"Écart-type : {std_dev_duration:.2f} tours")

    plt.figure(figsize=(10, 6))

    mid_point = len(durations) // 2

    # Première moitié ou le joueur 1 commence en 1er
    plt.plot(range(mid_point), durations[:mid_point], marker='o', linestyle='-', color='b', label=f'{player1} commence')
    
    # Deuxième moitié ou le joueur 2 commence en 1er
    plt.plot(range(mid_point, len(durations)), durations[mid_point:], marker='o', linestyle='-', color='g', label=f'{player2} commence')

    # Ligne representant la moyenne
    plt.axhline(mean_duration, color='r', linestyle='--', label=f'Moyenne: {mean_duration:.2f}')
    
    # Zone représentant l'écart-type autour de la moyenne
    plt.axhline(mean_duration + std_dev_duration, color='orange', linestyle='-.', label=f'Moyenne + Écart-type: {mean_duration + std_dev_duration:.2f}')
    plt.axhline(mean_duration - std_dev_duration, color='orange', linestyle='-.', label=f'Moyenne - Écart-type: {mean_duration - std_dev_duration:.2f}')
    plt.fill_between(range(len(durations)), mean_duration - std_dev_duration, mean_duration + std_dev_duration, color='orange', alpha=0.3)

    plt.title(f'Durées des parties (Breakthrough 6x10) : {player1_name} vs {player2_name}')
    plt.xlabel('Index de la partie')
    plt.ylabel('Nombre de tours')
    plt.legend()
    plt.grid(True)

    plt.show()

if __name__ == "__main__":
    player1, player2 = get_player_names("./new_stats/resume.txt")

    if not player1 or not player2:
        print("Erreur de parsing.....")
        sys.exit(1)

    print(f"Joueur 1: {player1}")
    print(f"Joueur 2: {player2}")

    durations = get_game_durations()

    create_game_durations_plot(player1, player2, durations)