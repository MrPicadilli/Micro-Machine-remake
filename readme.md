## Installation et execution
Requiert gkit2light. Pour l'installation: 
 - placer projet-mmachine dans le repertoire de gkit
 - se placer à la racine de gkit et executer ```premake4 gmake```
 - compiler le projet avec ```make test_terrain```
 - executer depuis la racine de gkit avec ```./projet-mmachine/bin/test_terrain```

## Utilisation 
Pour se déplacer, utilisez les flèches directionelles ainsi que zqsd. vous pouvez réinitialiser le jeu avec la touche ```R```. La touche ```C``` vous permet de changer le mode de caméra pour le mode "cool" qui n'est pas sans rappeler un plan drône digne des meilleurs blockbusters.

## Fonctionnalités
### Camera

la caméra simple est une caméra situé a la normal du point filmé qui représente le milieu du segment se situant de la voiture rouge a la voiture bleu. L'incidence qu'a la voiture en tête est représenté en multipliant sa position pendant l'estimation du milieu du segment de 150%. Pour test_player la voiture rouge a initialement plus d'incidence que la voiture bleu.
La caméra "cool" est une caméra drone qui va appliquer une rotation tout en visant le point visé(to) puis arrivé à une certaine distance en x et y la caméra se mettra a avancer dans la direction du vecteur (point_TO - camera_sansZ) dans test player on a mis la distance limite a 10 et la hauteur de la caméra reste a 5. pour le calcul du suivi on a fait:

camera = camera + normalize(point_TO - camera_sansZ) * (distance(camera_sansZ, point_TO) - hauteur) * (delta_time() / 1000);

le delta time perme de gerer automatiquement le framerate et meme si il a un peu de retard arrivé  un moment si on immobilise les voitures la camera va rattraper les voitures (on aimait bien l'effet)

La visibilité se fait en prenant les points des diagonales minimum et maximum de chaque voitures. Ensuite nous la remettons dans le repère du monde et appliquons la transformé de la vue de la caméra puis de sa projection a cette boite englobantes. Apres cela nous comparons les valeurs x et y pour attester de la victoire de la voiture rouge ou bleu en fonction de la visibilité des voiture restantes. Arrivé a la fin un rectangle au couleur du gagnant sera affiché.
Bonus: le petit tricks pour avoir une sorte de debut de cinematique de course est d'aller dans le mode drone de faire avancer les voitures en y jusqu'a se qu'elle ne soit plus dans le terrain et appuyer sur R pendant que les voitures sont encore en train d'avancer vous verrez que la camera va revenir sur ces pas depuis la direction ou vous étiez parti.
Je vous conseille de faire le jeu en mode drone sur le test_terrain (fun garantie).

### Obstacles
Nous avons implémenté des collisions par sphère englobante. Lorsque qu'un joueur en rencontre un autre, seul un des joueurs subit un contrecoup. 
A cela, nous avons ajouté plusieurs type d'obstacles: les flaques d'huile (en noir) perturbent les contrôles du joueur, les buissons (en vert) ralentissent et les caisses (en marron) stoppent le joueur.

Ces obstacles sont gérés de manière assez peu efficace: nous n'avons pas accès aux objets à l'origine de nos collisions, ce qui limite nos applications. De plus, la structure des colliders est loin d'être optimale et il aurait été bien plus intéressant de développer une interface qui nous permettrait de généraliser nos colliders pour tout type d'objet.

Enfin, nous avons ajouté de quoi faire apparaitre des obstacles sur notre terrain de manière aléatoire.

### Terrain
Nous avons implémenté de quoi lire un fichier texte pour placer des checkpoints sur un terrain. Ce fichier contient 2 premieres lignes qui sont les positions de départ des joueurs, puis la liste des checkpoints. Pour changer le terrain de jeu et les checkpoints, il faut changer le nom des fichiers lu dans test_terrain et terrain.cpp.

Nous avons un ensemble de méthodes capables de déterminer le pourcentage de terrain parcouru par le joueur en fonction de sa position, grâce auxquelles nous pouvons déterminer le joueur en tête.

Pour augmenter la précision de notre algorithme et limiter les erreurs dues à des terrains irréguliers (comme le notre), nous pouvons subdiviser les checkpoints pour augmenter la fréquence. Cependant, il n'est pas possible d'afficher les checkpoints, cela était possible mais un changement dans la structure du code a créé un problème pour l'affichage de ces checkpoints qui n'a pas été résolu. 

Nous avons tenté d'implémenter une logique de tours de circuits sans succès.

## Bugs
Les calculs de pourcentage de terrain sont imparfaits et de petits sauts de valeurs arrivent de manière sporadique. 
Pour passer en mode camera cool ou en mode drone le bouton "c" ne fonctionne pas toujour car on a fait la détection des touches du clavier dans le render donc il se peut que le bouton s'actionne plusieur fois, il faut avoir le doigté !