# Sous-Menu - Projet Modulaire SDL2

## Structure du Projet

```
sous_menu/
├── include/          # Headers (.h)
│   ├── app.h         # Structures principales et contexte global
│   ├── core.h        # Initialisation SDL et cleanup
│   ├── textures.h    # Gestion des ressources graphiques
│   ├── audio.h       # Gestion de l'audio
│   ├── scores.h      # Gestion du leaderboard
│   ├── ui.h          # Éléments d'interface utilisateur
│   └── game.h        # Boucle de jeu et événements
├── src/              # Fichiers source (.c)
│   ├── main.c        # Point d'entrée
│   ├── core.c        # Implémentation SDL init/cleanup
│   ├── textures.c    # Chargement des textures
│   ├── audio.c       # Implémentation audio
│   ├── scores.c      # Implémentation scores/leaderboard
│   ├── ui.c          # Implémentation UI
│   └── game.c        # Boucle de jeu
├── assets/           # Ressources (images, sons, police)
├── scores.txt        # Données de leaderboard
├── Makefile          # Compilation
└── README.md         # Ce fichier
```

## Modules

### `app.h` - Contexte Global
Définit les structures principales :
- `Window` : infos de fenêtre SDL
- `Textures` : toutes les textures du jeu
- `Audio` : ressources audio
- `Leaderboard` : données des scores
- `UIRects` : rectangles pour UI
- `App` : contexte principal regroupant tout

### `core.h/c` - Initialisation SDL
- `core_init()` : initialise SDL, SDL_image, SDL_ttf, SDL_mixer
- `core_cleanup()` : libère les ressources de base

### `textures.h/c` - Gestion Graphiques
- `textures_load()` : charge tous les PNG et crée les textures
- `textures_cleanup()` : libère les textures

### `audio.h/c` - Gestion Audio
- `audio_init()` : charge et lance la musique d'ambiance
- `audio_cleanup()` : libère les ressources audio

### `scores.h/c` - Leaderboard
- `scores_load()` : charge les scores depuis `scores.txt`
- `scores_render_leaderboard()` : affiche le leaderboard

### `ui.h/c` - Interface Utilisateur
- `ui_init()` : initialise les rectangles et textes UI
- `ui_update_hover()` : détecte les survols de souris
- `ui_render_main_button()` : dessine le bouton principal
- `ui_render_submenu()` : dessine le sous-menu

### `game.h/c` - Boucle de Jeu
- `game_handle_events()` : traite les événements SDL
- `game_render()` : affiche l'écran
- `game_run()` : boucle principale

## Compilation

```bash
make        # Compile le projet
make clean  # Nettoie les fichiers compilés
```

## Avantages de cette Structure

✓ **Séparation des responsabilités** : chaque module gère un aspect
✓ **Testabilité** : chaque module peut être testé indépendamment
✓ **Maintenabilité** : modifications isolées par module
✓ **Réutilisabilité** : modules peuvent être utilisés dans d'autres projets
✓ **Scalabilité** : facile d'ajouter de nouvelles fonctionnalités

## Utilisation

Pour ajouter une nouvelle fonctionnalité :
1. Créer un nouveau header dans `include/`
2. Créer l'implémentation dans `src/`
3. Inclure dans `app.h` si global
4. Ajouter au Makefile
5. Intégrer dans le module approprié

