#ifndef PHOTONMAPPING_HPP
#define PHOTONMAPPING_HPP

#include "lightSource.hpp"
#include "scene.hpp"
#include "kdtree.h"
#include "image.hpp"
#include <list>
#include "photon.hpp"

enum KERNEL {
    GAUSSIAN,
    BOX,
    CONE
};

struct PhotonAxisPosition{
    float operator()(const Photon& p, std::size_t i) const{
        return p.Position(i);
    }
};


using PhotonMap = nn::KDTree<Photon, 3ULL, PhotonAxisPosition>;

class PhotonMapping{
    int Nwalks;
    Image image;
    KERNEL kernel;
    public:
        PhotonMapping(int _Nwalks, Image _image, KERNEL _kernel = KERNEL::BOX);
        void selectLightSource(Scene scene);
        void randomWalkRec(PointLight* light, std::list<Photon>& photonsList,Intersection lastInter,int walk,Scene scene,RGB prevFlux,Ray r0);
        void first_intersection(vector<Ray> RGBsrays, Scene scene, RGB &color,const PhotonMap& map, const KERNEL kernel);
        std::list<Photon> randomWalk(Scene scene);
        std::list<Photon> randomWalk2(Scene scene);
        PhotonMap generation_of_photon_map(Scene scene);
        Image getImage();
        void setImage(int _width, int _height, std::vector<RGB> _RGBs);
        void parallelization(Scene scene);
};


#endif