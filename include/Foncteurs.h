/// Foncteurs.
/// \author Misha Krieger-Raynauld, Adam Burhan, Jean-Sébastien Dulong-Grégoire
/// \date 2020-03-11

#ifndef FONCTEURS_H
#define FONCTEURS_H

#include "LigneLog.h"

/// Foncteur qui verifie si un film a ete produit dans un intervalle de dates (date debut et date fin).
class EstDansIntervalleDatesFilm
{
public:
    /// Constructeur par paramètres/défaut du foncteur EstDansIntervalleDatesFilm.
    /// \param borneInferieure     la borne inférieure de l'intervalle à vérifier. 
    /// \param borneSuperieure     la borne supérieure de l'intervalle à vérifier.
    EstDansIntervalleDatesFilm(int borneInferieure = 0, int borneSuperieure = 0) 
        : borneInferieure_(borneInferieure)
        , borneSuperieure_(borneSuperieure)
    {
    }

    /// Opérateur permettant d'utiliser le foncteur sur une reference d'un unique_ptr de film.
    /// \param film    Référence constante d'un pointeur unique vers le film sur lequel nous voulons utiliser le foncteur.
    /// \return        Un bool nous indiquant si le film se situe entre les intervalles d'annees donnees.
    bool operator()(const std::unique_ptr<Film>& film)
    {
        return film->annee <= borneSuperieure_ && film->annee >= borneInferieure_;
    }

private:
    int borneInferieure_;
    int borneSuperieure_;
};

/// Foncteur qui compare les timestamp de deux lignes de logs afin d'etablir un ordre chronologique entre les deux.
class ComparateurLog 
{
public:
    /// Opérateur permettant d'utilser le foncteur sur deux lignes de logs.
    /// \param ligneLog1    Une des deux lignes logs à comparer.
    /// \param ligneLog2    Une des deux lignes logs à comparer.
    /// \return             Un bool représentant si le timestamp de l'objet LigneLog de gauche est plus petit que celui de droite.
    bool operator()(const LigneLog& ligneLog1, const LigneLog& ligneLog2)
    {
        return ligneLog1.timestamp < ligneLog2.timestamp;
    }
};

/// Foncteur qui compare les seconds elements de deux paires et qui retourne le resultat de cette comparaison.
/// \param T1   Le type du premier element de la paire.
/// \param T2   Le type du deuxieme element de la paire.
template <typename T1, typename T2> 
class ComparateurSecondElementPaire
{
public:
    /// Opérateur permettant d'utiliser le foncteur sur deux paires ayant deux membres génériques T1 et T2 chacunes.
    /// \param paire1   Une des deux paires d'éléments génériques de types T1 et T2. 
    /// \param paire2   Une des deux paires d'éléments génériques de types T1 et T2.
    /// \return         Un bool représentant si le deuxième élément de la paire de gauche est plus petite que celui de droite.
    bool operator()(const std::pair<T1, T2>& paire1, const std::pair<T1, T2>& paire2)
    {
        return paire1.second < paire2.second;
    }
};

#endif
