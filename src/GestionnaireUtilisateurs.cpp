/// Gestionnaire d'utilisateurs.
/// \author Misha Krieger-Raynauld, Adam Burhan, Jean-Sébastien Dulong-Grégoire
/// \date 2020-01-12
/// modifié 2020-03-11

#include "GestionnaireUtilisateurs.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

/// Affiche les informations des utilisateurs gérés par le gestionnaire d'utilisateurs à la sortie du stream donné.
/// \param outputStream         Le stream auquel écrire les informations des utilisateurs.
/// \param gestionnaireFilms    Le gestionnaire d'utilisateurs à afficher au stream.
/// \return                     Une référence au stream.
std::ostream& operator<<(std::ostream& outputStream, const GestionnaireUtilisateurs& gestionnaireUtilisateurs)
{
    outputStream << "Le gestionnaire d'utilisateurs contient "  << gestionnaireUtilisateurs.getNombreUtilisateurs()
                 << " utilisateurs:\n";
    for (const auto& [key, utilisateur] : gestionnaireUtilisateurs.utilisateurs_)
    {
        outputStream << '\t' << utilisateur << '\n';
    }
    return outputStream;
}

/// Ajoute les utilisateurs à partir d'un fichier de données d'utilisateurs.
/// \param nomFichier   Le fichier à partir duquel lire les informations des utilisateurs.
/// \return             True si tout le chargement s'est effectué avec succès, false sinon.
bool GestionnaireUtilisateurs::chargerDepuisFichier(const std::string& nomFichier)
{
    std::ifstream fichier(nomFichier);
    if (fichier)
    {
        utilisateurs_.clear();

        bool succesParsing = true;

        std::string ligne;
        while (std::getline(fichier, ligne))
        {
            std::istringstream stream(ligne);

            std::string id;
            std::string nom;
            int age;
            int pays;

            if (stream >> id >> std::quoted(nom) >> age >> pays)
            {
                ajouterUtilisateur(Utilisateur{id, nom, age, static_cast<Pays>(pays)});
            }
            else
            {
                std::cerr << "Erreur GestionnaireUtilisateurs: la ligne " << ligne
                          << " n'a pas pu être interprétée correctement\n";
                succesParsing = false;
            }
        }
        return succesParsing;
    }
    std::cerr << "Erreur GestionnaireUtilisateurs: le fichier " << nomFichier << " n'a pas pu être ouvert\n";
    return false;
}

/// Construit une pair et l'ajoute dans le gestionnaire d'utilisateurs.
/// \param utilisateur  L'objet de type Utilisateur a ajouter au gestionnaire.
/// \return             Un bool representant si l'ajout à été fait avec succès.
bool GestionnaireUtilisateurs::ajouterUtilisateur(const Utilisateur& utilisateur)
{
    return utilisateurs_.emplace(utilisateur.id, utilisateur).second;
}

/// Supprime un utilisateur du gestionnaire en utilisant son ID.
/// \param idUtilisateur    Id de l'utilisateur qui sert comme clé pour retrouver l'utilisateur.
/// \return                 Un bool représentant le nombre d'éléments supprimés soit 1 ou 0.
bool GestionnaireUtilisateurs::supprimerUtilisateur(const std::string& idUtilisateur)
{
    return utilisateurs_.erase(idUtilisateur);
}

/// "Getter" du nombre d'utilisateurs dans le gestionnaire.
/// \return     Un size_t representant le nombre d'utilisateurs dans le gestionnaire.
std::size_t GestionnaireUtilisateurs::getNombreUtilisateurs() const
{
    return utilisateurs_.size();
}

/// Trouve et retourne un utilisateur a l'aide de son ID.
/// \param id   La clé unique d'un utilisateur nous permettant de le retrouver s'il existe.
/// \return     Retourne un pointeur constant vers l'utilisateur ayant cet ID ou un nullptr s'il n'existe pas.
const Utilisateur* GestionnaireUtilisateurs::getUtilisateurParId(const std::string& id) const
{
    auto it = utilisateurs_.find(id);
    if (it != utilisateurs_.end())
    {
        return &it->second;
    }
    return nullptr;
}