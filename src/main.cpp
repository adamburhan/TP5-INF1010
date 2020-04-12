/// Fonction main.
/// \author Misha Krieger-Raynauld, Adam Burhan, Jean-Sébastien Dulong-Grégoire
/// \date 2020-01-12
/// modifié 2020-03-11

#include <iostream>
#include "AnalyseurLogs.h"
#include "GestionnaireFilms.h"
#include "GestionnaireUtilisateurs.h"
#include "Tests.h"
#include "WindowsUnicodeConsole.h"

int main()
{
    // Change le code page de la console Windows en UTF-8 si l'OS est Windows
    initializeConsole();

    Tests::testAll();

    // Écrivez le code pour le bonus ici

    //affichage gestionnaire d'utilisateurs
    GestionnaireUtilisateurs gestUtilisateurs;
    gestUtilisateurs.chargerDepuisFichier("utilisateurs.txt");
    std::cout << "\n" << gestUtilisateurs << "\n";

    GestionnaireFilms gestFilms;
    gestFilms.chargerDepuisFichier("films.txt");
    std::vector<const Film*> filmsAventure = gestFilms.getFilmsParGenre(Film::Genre::Aventure);
    std::cout << "Films d'aventure:\n";
    for (const auto& film : filmsAventure)
    {
        std::cout << "\t" << *film << "\n";
    }

    std::vector<const Film*> filmsEntre_1960a1961 = gestFilms.getFilmsEntreAnnees(1960, 1961);
    std::cout << "\nFilms produits de 1960 à 1961:\n";
    for (const auto& film : filmsEntre_1960a1961)
    {
        std::cout << "\t" << *film << "\n";
    }

    AnalyseurLogs analyseurLogs;
    analyseurLogs.chargerDepuisFichier("logs.txt", gestUtilisateurs, gestFilms);
    const Film* filmPlusPop = analyseurLogs.getFilmPlusPopulaire();
    std::cout << "\nFilm le plus populaire (" << analyseurLogs.getNombreVuesFilm(filmPlusPop) << " vues): " << *filmPlusPop << "\n\n";
    std::cout << "5 films les plus populaires:\n";
    for (const auto& film : analyseurLogs.getNFilmsPlusPopulaires(5))
    {
        std::cout << "\t" << *film.first << " (" << film.second << " vues)\n";
    }

    std::cout << "\nNombre de films vus par l'utilisateur karasik@msn.com: " << analyseurLogs.getNombreVuesPourUtilisateur(gestUtilisateurs.getUtilisateurParId("karasik@msn.com")) << "\n\n";
}
