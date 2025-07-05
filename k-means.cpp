#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <vector>
#include <exception>

/**
* \file k-means.cpp
* \brief Programme pour le projet de C++ du Semestre 5
* \author G. Maxime
* \date 22/01/2024
*/

using namespace std;

class RGB{
    public:
    int R,G,B;
    RGB(); // Constructeur par défaut
    RGB(int r,int g,int b); // Constructeur avec set des valeurs R,G,B
    RGB(const RGB& x); // Constructeur de clonage
    RGB& operator=(const RGB& x); // Opérateur d'affectation
    ~RGB() = default; // Destructeur
    bool operator==(const RGB b); // Opérateur d'égalité
    double distance(const RGB& x); // Calcul distance entre 2 valeurs RGB
};

RGB::RGB(){
    this->R = 0;
    this->G = 0;
    this->B = 0;
}

RGB::RGB(int r,int g,int b){
    this->R = r;
    this->G = g;
    this->B = b;
}

RGB::RGB(const RGB& a){
    this->R = a.R;
    this->G = a.G;
    this->B = a.B;
}

RGB& RGB::operator=(const RGB& a){
    if (this != &a) {
        this->R = a.R;
        this->G = a.G;
        this->B = a.B;
    }
    return *this;
}
bool RGB::operator==(const RGB b)
{
    int tolerance = 1;
    return abs(this->R - b.R) <= tolerance && abs(this->G - b.G) <= tolerance && abs(this->B - b.B) <= tolerance;
}

// Fonction qui calcule la distance entre 2 valeur RGB
double RGB::distance(const RGB&x){
    int R_diff = this->R - x.R;
    int G_diff = this->G - x.G;
    int B_diff = this->B - x.B;
    return (sqrt(pow(R_diff,2)+pow(G_diff,2)+pow(B_diff,2)));
}


class Img{
    public:
    int width,height;
    vector<RGB> tab;
    Img(); // Constructeur par défaut
    Img(int width, int height); // Constructeur avec set de width et height plus création du tableau avec ces dimensions
    Img& operator=(const Img& x); // Constructeur d'affetctaion d'une image 
    Img(const Img& in); // Operateur de clonage
    ~Img() = default; // Destructeur
    Img(int width, int height, vector<RGB> in);
    Img(string filename);
    private: 
    void Resize(int width, int height);
};

void Img::Resize(int width, int height){
    this->width = width;
    this->height = height;
    this->tab.resize(height*width);
}

Img::Img(){
    this->Resize(0,0);
}

Img::Img(int width, int height){
    this->Resize(width,height);
}

Img::Img(int width, int height, vector<RGB> in){
    this->Resize(width,height);
    this->tab = in;
}
Img::Img(const Img& in){
    this->Resize(in.width,in.height);
    this->tab = in.tab;
}

Img& Img::operator=(const Img& x){
    if (this != &x) {
        this->Resize(width,height);
        this->tab = x.tab;
    }
    return *this;
}

class Cluster {
    public:
    RGB barycentre;
    vector<int> l_ind; // Listes des indices d'une Image qui appartient au cluster
    Cluster();
    Cluster(RGB in);
    Cluster(const Cluster& clu); // Operateur de clonage
    ~Cluster() = default; // Destructeur par défaut
    Cluster& operator=(const Cluster& x); // Operateur d'affectation
    void Cluster_upd(const Img &in); // Cluster upd
};

Cluster::Cluster(){
    this->barycentre = RGB();
    this->l_ind.resize(0);
}

Cluster::Cluster(RGB in){
    this->barycentre = in;
    this->l_ind.resize(0);
}

Cluster::Cluster(const Cluster& clu){
    this->barycentre = clu.barycentre;
    this->l_ind = clu.l_ind;
}
Cluster& Cluster::operator=(const Cluster& x){
    if (this != &x){
        this->barycentre = x.barycentre;
        this->l_ind = x.l_ind;
    }
    return *this;
}

// Fonction qui renvoie l'index du cluster dont le barycentre est le plus proche
int ind_nearestBarycentre(RGB test, vector<Cluster>& list){
    double min_dist = test.distance(list[0].barycentre);
    double current_dist = 0.0;
    int index =0;
    int index_Cluster=0;
    for (vector<Cluster>::iterator c = list.begin(); c != list.end(); c++){
        current_dist = test.distance(c->barycentre);
        if (current_dist < min_dist){
            min_dist = current_dist;
            index_Cluster=index;
        }
        index++;
    }
    return index_Cluster;
}

// Fonction qui calcule la nouvelle valuer du barycentre en faisant la moyenne de toutes les couleurs comprises à l'intérieur de ce dernier
void Cluster::Cluster_upd(const Img &in){
    double r = 0, g = 0, b = 0;
    int size = this->l_ind.size();
    for(vector<int>::iterator a = this->l_ind.begin(); a != this->l_ind.end(); ++a){
        r += in.tab[*a].R ;
        g += in.tab[*a].G ;
        b += in.tab[*a].B ;
    }
    if (size != 0) {
        r = r / static_cast<double>(size);
        g = g / static_cast<double>(size);
        b = b / static_cast<double>(size);
        this->barycentre = RGB(static_cast<int>(r),static_cast<int>(g),static_cast<int>(b));
    }else{
        printf("Erreur taille de cluster nulle");
    }
    
}

// Fonction qui applique l'algorithme de k-means sur un objet de la classe Img avec en entrée le nombre de couleurs qu'on souhaite à la fin
vector<Cluster> k_means(const Img& im, int k){
    vector<Cluster> res(0), temp(0);
    Cluster tmp;
    int m =0;
    int compt = 0 ;
    for (int i = 0; i < k ; i++){
        tmp = Cluster(im.tab[rand()%im.tab.size()]);
        res.emplace_back(tmp);
    }
    do{
        for(int l = 0 ; l < k ; l++){
            res[l].l_ind.resize(0);
        }
        for (int n = 0; n < (im.width*im.height); n++){
            m = ind_nearestBarycentre(im.tab[n],res);
            res[m].l_ind.emplace_back(n);
        }
        temp = res;
        for(int t = 0; t < (int)res.size(); t++){
            (res[t]).Cluster_upd(im);
        }
        compt = 0;
        for(int z = 0; z < k ; z++){
            if(res[z].barycentre == temp[z].barycentre){
                compt++;
            }
        }
  
    }while(compt != k);

    return res;
}

// Fonction pour renplir un objet de la classe Img avec les résultats de k-means qui sont stocké dans un vector de clusters
Img fill_Img_res_kmeans(Img& in,vector<Cluster>& rkmeans){
    Img result(in.width,in.height);
    for(vector<Cluster>::iterator a = rkmeans.begin() ; a != rkmeans.end() ; ++a){
        for(vector<int>::iterator n = a->l_ind.begin(); n != a->l_ind.end(); ++n){
            result.tab[*n].R = a->barycentre.R;
            result.tab[*n].G = a->barycentre.G;
            result.tab[*n].B = a->barycentre.B;
        }
    }
    return result;
}

// Fonction pour récupérer les données d'un fichier PPM et les stocké dans un objet de la classe Img
Img::Img(string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Error reading file");
    }

    string magicNumber, comment;
    int width, height, maxValue;
    file >> magicNumber >> comment;

    if (magicNumber != "P3")
    {
        throw std::runtime_error("File is not of P3 magic number");
    }

    file >> width >> height >> maxValue;
    this->width = width;
    this->height = height;
    this->tab.resize(width * height);

    for (int i = 0; i < width * height; i++)
    {
        int r, g, b;
        if (!(file >> r >> g >> b))
        {
            throw std::runtime_error("Error reading pixel data");
        }
        this->tab[i].R = r;
        this->tab[i].G = g;
        this->tab[i].B = b;
    }

    file.close();

    if (this->tab.size() == 0)
    {
        throw std::runtime_error("Empty pixel data set from input stream.");
    }
}


// Fonction qui écrit un fichier PPM en utilisant un objet de la classe Img pour récupérer le résultat du k-means
void writePPM(const Img& img, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Cannot open file " << filename << "\n";
        return;
    }
    file << "P3\n#_png_write\n" << img.width << " " << img.height << "\n255\n";
    for (int i = 0; i < img.width * img.height; i++) {
        file << static_cast<int>(img.tab[i].R) << " ";
        file << static_cast<int>(img.tab[i].G) << " ";
        file << static_cast<int>(img.tab[i].B) << "\n";
    }
    file.close();
}

/* 
Lors de ce projet de C++ j'ai essayé de mettre en place l'ensemble des notions vues en cours.
Je n'ai juste pas encapsulé mon programme en mettant des attributs en private et des fonctions getter et setter pour chaque attributs de classes
J'ai eu beaucoup de problèmes lors de la mise en place de ce programme,
j'ai rencontrés plusieurs erreurs de memoires et de récupération des données dans le fichier PPM
Finalement, j'ai réussi à faire une implémentation de l'algorithme k-means.
Je pense que mon programme est loin d'être le plus optimisé mais il est fonctionnel.
J'ai essayé de faire au mieux avec mes compétences minimes en C++
Pour ce programme j'ai beaucoup utiliser la documentation à disposition sur cppreference.com

Temps passé : environ 15 heures
*/

int main(){
    Img inputImage("plage.ppm");
    int numClusters = 6;
    vector<Cluster> clusters = k_means(inputImage, numClusters);
    Img resultImage = fill_Img_res_kmeans(inputImage, clusters);
    writePPM(resultImage, "result.ppm");
    return 0;
}