EXECUTABLE="./fg_player"

# Vérifier si l'exécutable existe
if [ ! -f "$EXECUTABLE" ]; then
    echo "Erreur : L'exécutable $EXECUTABLE est introuvable."
    exit 1
fi

{
    echo "verbose ON"
    echo "newgame 10 6"
    echo "showboard"
    echo "genmove" 
    echo "showboard"
    echo "genmove"
    echo "showboard"
} | "$EXECUTABLE"

echo "fin du test"