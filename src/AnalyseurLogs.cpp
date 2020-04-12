/// Analyseur de statistiques grâce aux logs.
/// \author Misha Krieger-Raynauld, Adam Burhan, Jean-Sébastien Dulong-Grégoire
/// \date 2020-01-12
/// modifié 2020-03-11

#include "AnalyseurLogs.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include "Foncteurs.h"

/// Ajoute les lignes de log en ordre chronologique à partir d'un fichier de logs.
/// \param nomFichier               Le fichier à partir duquel lire les logs.
/// \param gestionnaireUtilisateurs Référence au gestionnaire des utilisateurs pour lier un utilisateur à un log.
/// \param gestionnaireFilms        Référence au gestionnaire des films pour pour lier un film à un log.
/// \return                         True si tout le chargement s'est effectué avec succès, false sinon.
bool AnalyseurLogs::chargerDepuisFichier(const std::string& nomFichier,
                                         GestionnaireUtilisateurs& gestionnaireUtilisateurs,
                                         GestionnaireFilms& gestionnaireFilms)
{
    std::ifstream fichier(nomFichier);
    if (fichier)
    {
        logs_.clear();
        vuesFilms_.clear();

        bool succesParsing = true;

        std::string ligne;
        while (std::getline(fichier, ligne))
        {
            std::istringstream stream(ligne);

            std::string timestamp;
            std::string idUtilisateur;
            std::string nomFilm;

            if (stream >> timestamp >> idUtilisateur >> std::quoted(nomFilm))
            {
                creerLigneLog(timestamp, idUtilisateur, nomFilm, gestionnaireUtilisateurs, gestionnaireFilms);
            }
            else
            {
                std::cerr << "Erreur AnalyseurLogs: la ligne " << ligne
                          << " n'a pas pu être interprétée correctement\n";
                succesParsing = false;
            }
        }
        return succesParsing;
    }
    std::cerr << "Erreur AnalyseurLogs: le fichier " << nomFichier << " n'a pas pu être ouvert\n";
    return false;
}

/// Création d'une ligne log nécessitant un timestamp, un utilisateur et un film.
/// \param timestamp                Représentant le moment où l'utilisateur a écouté le film.
/// \param idUtilisateur            Clé permettant l'accès à l'utilisateur dans le gestionnaireUtilisateur.
/// \param nomFilm                  Clé permettant l'accès au film dans le gestionnaireFilms.
/// \param gestionnaireUtilisateurs Objet dans lequel la clé de l'utilisateur sera utilisée.
/// \param gestionnaireFilms        Objet dans lequel la clé du film sera utilisée.
/// \return                         Un bool représentant si la ligne log à été créer avec succès.
bool AnalyseurLogs::creerLigneLog(const std::string& timestamp, const std::string& idUtilisateur, const std::string& nomFilm,
                       GestionnaireUtilisateurs& gestionnaireUtilisateurs, GestionnaireFilms& gestionnaireFilms)
{
    const Utilisateur* utilisateur = gestionnaireUtilisateurs.getUtilisateurParId(idUtilisateur);
    const Film* film = gestionnaireFilms.getFilmParNom(nomFilm);
    if (film != nullptr && utilisateur != nullptr)
    {
        ajouterLigneLog({timestamp, utilisateur, film});
        return true;
    }
    return false;
}

/// Methode qui ajoute une Ligne de log dans l'analyseur de logs d'une facon ordonnée.
/// \param ligneLog    La ligneLog que nous voulons ajouter.
void AnalyseurLogs::ajouterLigneLog(const LigneLog& ligneLog)
{
    auto it = std::upper_bound(logs_.begin(), logs_.end(), ligneLog, ComparateurLog()); 
    logs_.insert(it, ligneLog);
    vuesFilms_[ligneLog.film]++;
}

/// Trouve et retourne le nombre de vues pour le film passe en parametre.
/// \param film    Le film pour lequel nous voulons verifier son nombre de vues.
/// \return        Le nombre de vues du film.
int AnalyseurLogs::getNombreVuesFilm(const Film* film) const
{
    auto it = vuesFilms_.find(film);
    if (it != vuesFilms_.end())
    {
        return it->second;
    }
    return 0;
}

/// Trouve et retourne le film le plus populaire dans l'analyseur de logs.
/// \return    Un pointeur constant vers le film le plus populaire ou un nullptr si aucun film n'est dans l'analyseur de logs.
const Film* AnalyseurLogs::getFilmPlusPopulaire() const
{
    if (logs_.size() != 0)
    {
        return std::max_element(vuesFilms_.begin(), vuesFilms_.end(), ComparateurSecondElementPaire<const Film*, int>())->first;
    }
    return nullptr;
}

/// Trouve et retourne une liste de films les plus populaires. La taille de cette liste est determinee par un nombre passe en parametres.
/// \param nombre   La taille de la liste de films les plus populaires a retourner. (Si celle-ci ne depasse pas le nombre de films dans l'analyseur).
/// \return         Liste vers les films les plus populaires ainsi que le nombre de vues pour chacun d'entres eux. 
std::vector<std::pair<const Film*, int>> AnalyseurLogs::getNFilmsPlusPopulaires(std::size_t nombre) const
{
    std::vector<std::pair<const Film*, int>> filmsPlusPop (std::min(logs_.size(), nombre));
    std::partial_sort_copy(vuesFilms_.begin(), vuesFilms_.end(), filmsPlusPop.begin(), filmsPlusPop.end(),
                           [](const std::pair<const Film*, int>& pair1, const std::pair<const Film*, int>& pair2)
                           { return pair1.second > pair2.second; });
    filmsPlusPop.erase(std::remove_if(filmsPlusPop.begin(), filmsPlusPop.end(), [](auto pair){ return pair.second == 0; }), filmsPlusPop.end());
    return filmsPlusPop;
}

/// Trouve et retourne le nombre de films vus par un utilisateur.
/// \param utilisateur    L'utilisateur pour lequel nous voulons vérifier son nombre de vues.
/// \return               Le nombre de films qu'un utilisateur donne à vus.
int AnalyseurLogs::getNombreVuesPourUtilisateur(const Utilisateur* utilisateur) const
{
    auto lambda = [utilisateur](LigneLog ligneLog){ return utilisateur == ligneLog.utilisateur; };
    return static_cast<int>(std::count_if(logs_.begin(), logs_.end(), lambda));
}

/// Permet de listé tout les films qu'un utilisateur précis à visionné.
/// \param utilisateur    L'utilisateur dont l'on souhaite obtenir tout les films visionnés.
/// \return               Un vecteur de pointeurs constant vers les films visionnés par l'utilisateur.
std::vector<const Film*> AnalyseurLogs::getFilmsVusParUtilisateur(const Utilisateur* utilisateur) const
{
    std::unordered_set<const Film*> filmsVus;

    for (const auto& log : logs_)
    {
        if (log.utilisateur == utilisateur)
        {
            filmsVus.emplace(log.film);
        }
    }
    return std::vector<const Film*>(filmsVus.begin(), filmsVus.end());
}