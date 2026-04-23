# 🎮 Lunatics v2 - Nouvelles Fonctionnalités

## ✅ Features Implémentées

### 1. **Système Multi-Ennemies & Powerups**
- **EntityManager**: Gère jusqu'à 15 entités simultanément (10 ennemies + 5 powerups)
- **Ennemies par level**:
  - **Level 1**: 3 ennemies (50 HP chacun)
  - **Level 2**: 4 ennemies plus forts (75 HP chacun)
- **Powerups**:
  - 🟢 **Health Bonus**: +20 HP au joueur
  - 🟡 **Score Bonus**: +50 points

### 2. **Déplacement Aléatoire (2 Trajectoires)**
```
Trajectoire 0: Oscillation gauche-droite fixe
- L'ennemi oscille entre x=200 et x=900
- Donne un comportement prévisible

Trajectoire 1: Mouvement fluide aléatoire
- Changements de direction aléatoires (5% chance)
- Limites: x=100 à x=1000
- Plus imprévisible et dynamique
```

### 3. **Animation Ennemie**
- **Move Left** (frames 0-9): Marche vers la gauche
- **Move Right** (frames 10-19): Marche vers la droite  
- **Attack Left** (frames 20-27): Attaque à gauche
- **Attack Right** (frames 28-35): Attaque à droite
- **Wait Left** (frames 36-50): Immobile regardant à gauche
- **Wait Right** (frames 51-65): Immobile regardant à droite

### 4. **Collision Bounding Box (1 fonction, 2 utilisations)**
```c
int collision_BB(SDL_Rect pos1, SDL_Rect pos2)
```

**Utilisation 1**: Joueur ↔ Ennemis
- Perte de 5 HP par collision
- Affichage de la position de collision

**Utilisation 2**: Joueur ↔ Powerups
- Récupération automatique
- Application du bonus (santé ou score)
- L'entité powerup disparaît après contact

### 5. **Gestion de Santé & Affichage Graphique**

#### États de Santé
| État | Couleur | Description |
|------|---------|-------------|
| **ALIVE** | 🟢 Vert | 100% → 50% HP |
| **HURT** | 🟠 Orange | 50% → 0% HP |
| **NEUTRALIZED** | ⚫ Gris | 0 HP (ennemi inactif) |

#### Barre de Vie
- Positionnée au-dessus de chaque ennemi
- Dimensions: 40px largeur × 5px hauteur
- Bordure blanche pour contraste
- Mise à jour en temps réel

## 🎮 Contrôles de Jeu

| Touche | Action |
|--------|--------|
| ← / → | Déplacer le joueur |
| ↑ / ↓ | Sauter/Descendre |
| **L** | Changer de Level |
| **ESC** | Quitter |

## 📊 Structures Principales

### Entity
```c
typedef struct {
    SDL_Surface *sprite;
    SDL_Rect rect;
    int health;
    int max_health;
    HealthState state;      // ALIVE, HURT, NEUTRALIZED
    EntityType type;        // ENEMY ou POWERUP
    GameLevel level;        // LEVEL1 ou LEVEL2
    int trajectory;         // 0 ou 1 (2 patterns)
    int active;            // 1 = visible, 0 = inactif
} Entity;
```

### EntityManager
```c
typedef struct {
    Entity entities[MAX_ENEMIES + MAX_POWERUPS];
    int count;
    int enemy_count;
    int powerup_count;
} EntityManager;
```

## 🛠️ Compilation

```bash
# Clean et build
make clean
make

# Ou directement avec make
make all

# Lancer le jeu
make run
# Ou: ./lunatics
```

## 📝 Fonctions Clés

### Initialisation
```c
initialize_entity_manager(&manager);
add_enemy(&manager, x, y, health, level);
add_powerup(&manager, x, y, bonus_type);
```

### Update & Affichage
```c
update_all_entities(&manager, screen, heropos);
display_all_entities(&manager, screen);
display_all_health_bars(&manager, screen);
```

### Collision
```c
if (collision_BB(heropos, entity.rect)) {
    // Collision détectée!
}
```

### Gestion Santé
```c
take_damage(&entity, damage_amount);
// Résultat: ALIVE → HURT → NEUTRALIZED
```

## 🎯 Prochaines Améliorations Possibles

- [ ] Images distinctes pour les powerups
- [ ] Sons pour les collisions et pickups
- [ ] Système de score avancé
- [ ] Niveaux supplémentaires
- [ ] Boss finaux
- [ ] Effets visuels (particules, flash)
- [ ] Système de vagues d'ennemies
- [ ] Migration vers SDL 2.0

## 📄 Notes Techniques

- **FPS**: ~60 (SDL_Delay 16ms)
- **Résolution**: 1280×720
- **Format binaire**: ELF 64-bit (Linux/WSL)
- **Dépendances SDL**: SDL 1.2, SDL_image, SDL_ttf, SDL_mixer

---

**Version**: 2.0  
**Date**: Avril 2026  
**Plateforme**: Linux/WSL
