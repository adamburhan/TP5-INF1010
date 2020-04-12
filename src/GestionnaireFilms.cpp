/// Gestionnaire de films.
/// \author Misha Krieger-Raynauld, Adam Burhan, Jean-Sébastien Dulong-Grégoire
/// \date 2020-01-12
/// modifié 2020-03-11

#include "GestionnaireFilms.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "Foncteurs.h"
#include "RawPointerBackInserter.h"


/// Constructeur par copie.
/// \param other    Le gestionnaire de films à partir duquel copier la classe.
GestionnaireFilms::GestionnaireFilms(const GestionnaireFilms& other)
{
    films_.reserve(other.films_.size());
    filtreNomFilms_.reserve(other.filtreNomFilms_.size());
    filtreGenreFilms_.reserve(other.filtreGenreFilms_.size());
    filtrePaysFilms_.reserve(other.filtrePaysFilms_.size());

    for (const auto& film : other.films_)
    {
        ajouterFilm(*film);
    }
}

/// Opérateur d'assignation par copie utilisant le copy-and-swap idiom.
/// \param other    Le gestionnaire de films à partir duquel copier la classe.
/// \return         Référence à l'objet actuel.
GestionnaireFilms& GestionnaireFilms::operator=(GestionnaireFilms other)
{
    std::swap(films_, other.films_);
    std::swap(filtreNomFilms_, other.filtreNomFilms_);
    std::swap(filtreGenreFilms_, other.filtreGenreFilms_);
    std::swap(filtrePaysFilms_, other.filtrePaysFilms_);
    return *this;
}

/// Affiche les informations des films gérés par le gestionnaire de films à la sortie du stream donné.
/// \param outputStream         Le stream auquel écrire les informations des films.
/// \param gestionnaireFilms    Le gestionnaire de films à afficher au stream.
/// \return                     Une référence au stream.
std::ostream& operator<<(std::ostream& outputStream, const GestionnaireFilms& gestionnaireFilms)
{
    outputStream << "Le gestionnaire de films contient "  << gestionnaireFilms.getNombreFilms() << " films.\n"
                 << "Affichage par catégories:\n";

    for (auto& [key, liste] : gestionnaireFilms.filtreGenreFilms_)
    {
        Film::Genre genre = key;
        std::vector<const Film*> listeFilms = liste;
        outputStream << "Genre: " << getGenreString(genre) << " (" << listeFilms.size() << " films):\n";
        for (std::size_t i = 0; i < listeFilms.size(); i++)
        {
            outputStream << '\t' << *listeFilms[i] << '\n';
        }
    }
    return outputStream;
}

/// Ajoute les films à partir d'un fichier de description des films.
/// \param nomFichier   Le fichier à partir duquel lire les informations des films.
/// \return             True si tout le chargement s'est effectué avec succès, false sinon.
bool GestionnaireFilms::chargerDepuisFichier(const std::string& nomFichier)
{
    std::ifstream fichier(nomFichier);
    if (fichier)
    {
        films_.clear();
        filtreNomFilms_.clear();
        filtreGenreFilms_.clear();
        filtrePaysFilms_.clear();

        bool succesParsing = true;

        std::string ligne;
        while (std::getline(fichier, ligne))
        {
            std::istringstream stream(ligne);

            std::string nom;
            int genre;
            int pays;
            std::string realisateur;
            int annee;

            if (stream >> std::quoted(nom) >> genre >> pays >> std::quoted(realisateur) >> annee)
            {
                ajouterFilm(Film{nom, static_cast<Film::Genre>(genre), static_cast<Pays>(pays), realisateur, annee});
            }
            else
            {
                std::cerr << "Erreur GestionnaireFilms: la ligne " << ligne
                          << " n'a pas pu être interprétée correctement\n";
                succesParsing = false;
            }
        }
        return succesParsing;
    }
    std::cerr << "Erreur GestionnaireFilms: le fichier " << nomFichier << " n'a pas pu être ouvert\n";
    return false;
}

/// Ajoute le film passé en paramètre au vecteur de films du gestionnaire de film.
/// \param film    Film qui doit être ajouté aux vecteurs s'il existe.
/// \return        Un bool qui représente si l'ajout du film à bien été effectué.
bool GestionnaireFilms::ajouterFilm(const Film& film)
{
    if (getFilmParNom(film.nom) == nullptr)
    {
        films_.push_back(std::make_unique<Film>(film));
        const Film* ptr = films_.back().get();
        filtreNomFilms_.emplace(film.nom, ptr);
        filtreGenreFilms_[film.genre].push_back(ptr);
        filtrePaysFilms_[film.pays].push_back(ptr);
        return true;
    }
    return false;
}

/// Supprime un film du gestionnaire a partir de son nom.
/// \param nomFilm    Le nom du film a supprimer.
/// \return           Un bool representant si l'operation a ete faite avec succes.
bool GestionnaireFilms::supprimerFilm(const std::string& nomFilm)
{
    auto it = std::find_if(films_.begin(), films_.end(), 
        [nomFilm](std::unique_ptr<Film>& film) { return film->nom == nomFilm; });
    if (it != films_.end())
    {
        filtreNomFilms_.erase((*it)->nom);

        std::vector<const Film*>& filmsGenre = filtreGenreFilms_[(*it)->genre];
        auto removeGenre = std::remove(filmsGenre.begin(), filmsGenre.end(), it->get());
        filmsGenre.erase(removeGenre);

        std::vector<const Film*>& filmsPays = filtrePaysFilms_[(*it)->pays];
        auto removePays = std::remove(filmsPays.begin(), filmsPays.end(), it->get());
        filmsPays.erase(removePays);

        films_.erase(it);
        return true;
    }
    return false;
}

/// Retourne le nombre de films dans le gestionnaire.
/// \return    Size_t qui represente le nombre de films qui sont actuellement dans le gestionnaire.
std::size_t GestionnaireFilms::getNombreFilms() const
{
    return films_.size();
}

/// Trouve et retourne un pointeur constant vers un film à l'aide de son nom.
/// \param nom    Le nom du film a chercher.
/// \return       Un pointeur constant vers le film ayant ce nom ou un nullptr si celui-ci n'existe pas.
const Film* GestionnaireFilms::getFilmParNom(const std::string& nom) const
{
    auto it = filtreNomFilms_.find(nom);
    if (it != filtreNomFilms_.end())
    {
        return it->second;
    }
    return nullptr;
}
/// Trouve et retourne le vecteur contenant tout les films ayant le genre passé en paramètre.
/// \param genre    clé permettant l'accès au vecteurs de films associé au genre.
/// \return         Une copie du vecteur de pointeurs constant de Film ayant ce genre.
std::vector<const Film*> GestionnaireFilms::getFilmsParGenre(Film::Genre genre) const
{
    auto it = filtreGenreFilms_.find(genre);
    if (it != filtreGenreFilms_.end())
    {
        return it->second;
    }
    return std::vector<const Film*>();
}

/// Retourne une copie de la liste des films associés a un pays donné.
/// \param pays    Le pays donné que nous voulons associé à une liste de films.
/// \return        Vector qui contient la liste de films associes au pays donne.
std::vector<const Film*> GestionnaireFilms::getFilmsParPays(Pays pays) const
{
    auto it = filtrePaysFilms_.find(pays);
    if (it != filtrePaysFilms_.end())
    {
        return (it->second);
    }
    return std::vector<const Film*>();
}

/// Trouve et retourne un vecteur des films qui ont été réalisés entre les années passées en paramètres.
/// \param anneDebut    Borne inférieure de l'intervalle de recherche.
/// \param anneeFin     Borne supérieure de l'intervalle de recherche.
/// \return             Le vecteur contenant tout les films ayant une date de création compris dans l'intervalle.
std::vector<const Film*> GestionnaireFilms::getFilmsEntreAnnees(int anneeDebut, int anneeFin)
{
    std::vector<const Film*> filmsTrouves;
    std::copy_if(films_.begin(), films_.end(), RawPointerBackInserter(filmsTrouves), 
        EstDansIntervalleDatesFilm(anneeDebut, anneeFin));
    return filmsTrouves;
}