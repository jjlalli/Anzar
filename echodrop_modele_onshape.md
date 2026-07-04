# EchoDrop — modèle CAO pas à pas (Onshape, gratuit, dans le navigateur)

But : un vrai modèle mécanique paramétrique (boîtier + clip) qu'on peut éditer,
exporter en STEP (pour SolidWorks / Fusion) ou en STL (pour impression 3D).
Temps : ~30 min. Onshape marche sur Mac, dans le navigateur, sans installation.
Fusion 360 suit exactement la même logique (Sketch → Extrude → Shell → Fillet).

Toutes les cotes sont en millimètres. Diamètre du tuyau visé : **25 mm**
(si le vrai tuyau fait 20 ou 32 mm, remplace la valeur partout où c'est marqué "tuyau").

## 0. Préparer
1. Créer un compte gratuit sur onshape.com (usage personnel/éducation).
2. Create > Document, nom : "EchoDrop".
3. (Recommandé) Onglet Variables : créer `#pipe = 25 mm`, `#wall = 2.4 mm`.
   Tu pourras tout ajuster en changeant un seul chiffre.

## 1. Le boîtier
1. Sketch sur le plan Top. Dessiner un rectangle **centré** 46 × 18 mm.
2. Congés (Sketch fillet) de 5 mm aux 4 coins. Fermer le sketch.
3. Extrude 26 mm (New, solide). → un bloc plein arrondi.
4. Shell : sélectionner la **face du dessous**, épaisseur 2.4 mm.
   → boîtier creux, ouvert en dessous pour l'électronique.
5. Fillet 3 mm sur les arêtes du haut et les verticales (aspect premium arrondi).
6. Trou LED : Sketch sur la face du dessus, un cercle Ø 4.4 mm à ~9 mm d'un petit côté.
   Extrude → Remove, profondeur 4 mm.

## 2. Le clip (collier en C qui se clipse sur le tuyau)
1. Sketch sur le plan Front. Deux cercles concentriques :
   intérieur Ø 25 mm (= tuyau), extérieur Ø 29.8 mm (= tuyau + 2×wall).
2. Toujours dans le sketch, tracer une **fente** ouverte en bas : deux lignes
   verticales espacées de ~14 mm qui coupent l'anneau depuis le bas.
   La région à garder = l'anneau (entre les deux cercles) **moins** la fente basse.
3. Extrude cette région, symmetric, longueur 22 mm. → le collier en C.
4. Placer le collier sous le boîtier, axes alignés. Boolean Union boîtier + collier
   (ou les laisser comme deux parts si tu préfères un assemblage).

## 3. Finitions et export
1. (Option) Logo : Sketch "EchoDrop" en texte sur la façade, Extrude Remove 0.4 mm.
2. Clic droit sur la Part > Export :
   - **STEP** → pour ouvrir/éditer dans SolidWorks, Fusion, FreeCAD.
   - **STL** → pour impression 3D ou pour un rendu dans Blender.

## Astuce crédibilité (pour le stand)
Montrer le modèle CAO qui tourne à l'écran + la vue éclatée dit "ingénieure"
bien plus qu'une belle image. Garde le fichier Onshape ouvert sur le portable.
