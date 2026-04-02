Exemple SDL2 (C) — Sous-menu

Prerequis:
- SDL2 et SDL2_image installés sur votre système
  (sur Debian/Ubuntu: `sudo apt install libsdl2-dev libsdl2-image-dev`)

Placez vos images dans le dossier `assets/`:
- `assets/background.png` : image de fond
- `assets/button.png` : icône/bouton cliquable
- `assets/sousmenu.png` : image du sous-menu affichée en overlay

Compilation:

```sh
cd /home/anas/sous_menu
make
```

Execution:

```sh
./submenu
```

Comportement:
- Le programme affiche la `background.png` et un bouton en haut à gauche.
- Cliquer sur le bouton bascule l'affichage de `sousmenu.png` centré.

Si vous souhaitez que le bouton ait une autre position, modifiez `btn_rect` dans `main.c`.
