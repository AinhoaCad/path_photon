#include "photonMapping.hpp"
#include <random>
#include "photon.hpp"
#include <transformations.hpp>
#include <list>
#include <thread>
// Generador y distribución definidos globalmente
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0);
#define M_PI 3.14159265358979323846

#define N 100000
#define MAX_BOUNCES 10
using PhotonMap = nn::KDTree<Photon,3,PhotonAxisPosition>;

static int method = 1;


PhotonMapping::PhotonMapping(int _Nwalks, Image _image, KERNEL kernel):Nwalks(_Nwalks),image(_image),kernel(kernel)
{
}

float probability(const RGB& ki) {
    float max_val = ki[0];
    
    if (ki[1] > max_val) {
        max_val = ki[1];
    }
    if (ki[2] > max_val) {
        max_val = ki[2];
    }
    
    return max_val;
}


void createBasis(Direction &d1, Direction &d2, Direction &d3){
    d1 = d1.normalization();
    Direction aux = Direction(0.0,0.0,1.0);
    if (d1 == aux || d1 == aux*(-1)){
        aux = Direction(1.0,0.0,0.0);
    }
    d3 = d1.cross_product(aux);
    d3=d3.normalization();
    d2= d1.cross_product(d3);
    d2=d2.normalization();

}

Direction randomW(Intersection inter) {

    // Crear generador y distribución aleatoria
    float randomA = (float)acos(-1.0 + 2.0*dis(gen));
    // Generar ángulo azimutal (entre 0 y 2π)
    float randomL = 2.0 * M_PI * dis(gen);
    //cout <<"A: " +to_string(randomA)+" l:"+to_string(randomL)+"\n";
    Direction dUniverse = Direction(sin(randomA)*cos(randomL),sin(randomA)*sin(randomL),cos(randomA));
    Direction d2=Direction(0.0,0.0,0.0);
    Direction d3=Direction(0.0,0.0,0.0);
    Direction n = inter.getNormal();
        
    createBasis(n, d2, d3);
    //cout <<"d1" << n << "   "<< "d2"<<d2<<"   d3"<<d3<<" homcoed" <<dUniverse.homCoord()[0]<<dUniverse.homCoord()[1]<<dUniverse.homCoord()[2]<<dUniverse.homCoord()[3]<<endl;
    //Direction random = Direction(Transformation::ChangeBasis(n, d2, d3, inter.getPoint())*dUniverse.homCoord());
    Matrix changeBasisMatrix =Transformation::ChangeBasis(d2, d3, n, inter.getPoint());
    Direction random = Direction(changeBasisMatrix*dUniverse.homCoord());
    //cout << random<<endl;
    return random.normalization();
}

bool isAShadowPoint(Ray r, Scene scene){
    bool intersection=false;
    int index=0;
     float distanceToPixel=r.getDirection().modulus();
    for (Primitive* p: scene.getPrimitives()){
        Intersection i;
        index++;
        bool inter=p->doesIntersect(r,i);
        if( inter && (i.getDistance()<distanceToPixel) && i.getDistance()>10e-10){
            intersection =true;
        }
    }
    return intersection;
}

RGB NextEventEstimation(Scene scene,Intersection inter) {
    RGB color=RGB(0.0,0.0,0.0);
    for (LightSource* l:scene.getLightSources()) {
        if (!l->isAreaLight()){
            Direction raytoLight=l->getCenter()-inter.getPoint();
            if (!isAShadowPoint(Ray(raytoLight.normalization(),inter.getPoint()), scene)) {
                //raytoLight=raytoLight.normalization();
                float n = abs(inter.getNormal().dot_product(raytoLight.normalization()));
                RGB fr;
                fr = inter.getK_d()*(1.0/(float)M_PI);
                color = color+fr*n*(l->getLightIntensity()*(1.0/(float)(raytoLight.modulus()*raytoLight.modulus())));
            }
        }
    }
    return color;
}

bool intRayWithPrimitives(Ray r, Scene scene, Intersection &inter){
    bool thereIsIntersection = false;
    float min = std::numeric_limits<float>::infinity();
    for (Primitive* p: scene.getPrimitives()){
        Intersection i;
        bool intersection= p->doesIntersect(r,i);
        if (intersection && (i.getDistance() < min)){
            min=i.getDistance();
            inter.setDistance(i.getDistance());
            inter.setK_d(i.getK_d());
            inter.setK_s(i.getK_s());
            inter.setK_t(i.getK_t());
            inter.setK_e(i.getK_e());
            inter.setRefractionIndex(i.getRefractionIndex());
            inter.setNormal(i.getNormal());
            inter.setPoint(i.getPoint());
            thereIsIntersection=true;
        }
    }
    return thereIsIntersection;
}




void PhotonMapping::selectLightSource(Scene scene)
{
    RGB totalP=RGB(0.0,0.0,0.0);
    for (auto l: scene.getLightSources()){
        if (!l->isAreaLight()){
            totalP = totalP + l->getLightIntensity();
        }
    }
    for (auto l: scene.getLightSources()){
        if (!l->isAreaLight()){
            PointLight* p = static_cast <PointLight*>(l);
            RGB n = p->getLightIntensity()*N*(1.0/totalP);
            //cout << to_string(n[0])+" "+to_string(n[1])+" "+to_string(n[2])+"\n";
            p->setPhotons(n);
        }
    }
}

void PhotonMapping::randomWalkRec(PointLight* light, std::list<Photon>& photonsList,Intersection lastInter,int walk,Scene scene, RGB prevFlux, Ray r0){
    Direction w0 = r0.getDirection();
    float RussianRoulette = (float)dis(gen);
    Direction wi;
    bool i=true;
    Ray newR=Ray(Direction(0,0,0),Point(0,0,0));
    RGB flux=prevFlux;
    Intersection newInter;
    walk = walk +1;
    if (RussianRoulette - probability(lastInter.getK_d()) < 0){
        // acabo de llegar a una superficie difusa
        wi = randomW(lastInter);
        newR = Ray(wi,lastInter.getPoint());
        newR.setRefractionIndex(lastInter.getRefractionIndex());
        i = intRayWithPrimitives(newR,scene,newInter);
        Point position=newInter.getPoint();
        flux = prevFlux * lastInter.getK_d()*(1.0/probability(lastInter.getK_d()));
        Photon p = Photon(lastInter.getPoint(), w0, prevFlux);
        photonsList.push_back(p);

    } else if (RussianRoulette - probability(lastInter.getK_d())-probability(lastInter.getK_s())< 0.0){
        flux = prevFlux * lastInter.getK_s()*(1.0/probability(lastInter.getK_s()));
        wi = w0 + lastInter.getNormal()*(-2.0f)*w0.dot_product(lastInter.getNormal());
        newR = Ray(wi.normalization(), lastInter.getPoint() + wi.normalization() * 0.01);
        newR.setRefractionIndex(lastInter.getRefractionIndex());
        i = intRayWithPrimitives(newR,scene,newInter);

    } else if ((RussianRoulette - probability(lastInter.getK_d())-probability(lastInter.getK_s())-probability(lastInter.getK_t()) < 0.0)){
        Direction _n = lastInter.getNormal();
        flux = prevFlux * lastInter.getK_t()*(1.0/probability(lastInter.getK_t()));
        float ior_in = 1.0f;
        float ior_out = lastInter.getRefractionIndex();
        float n_w0 = w0.dot_product(_n);
        if (n_w0 < 0) {
            // fuera de la superficie, cos(theta) es positivo
            n_w0 = -n_w0;
        }
        else {
            _n = _n * -1;
            float temp = ior_in;
            ior_in = ior_out;
            ior_out = temp;
        }
        float eta = ior_in / ior_out;
        float d = 1 - eta * eta * (1 - n_w0 * n_w0);
            if (d < 0) {
            // total internal reflection
            // hay que multiplicar por -1 la normal
            wi = w0 + lastInter.getNormal()*(-1)*(-2)*w0.dot_product(lastInter.getNormal());
        }
            else {
            wi = w0 * eta + _n * (eta * n_w0 - sqrt(d));
        }
        newR = Ray(wi.normalization(), lastInter.getPoint() + wi.normalization()* 0.0001, ior_out);
        newR.setRefractionIndex(lastInter.getRefractionIndex());
        i = intRayWithPrimitives(newR,scene,newInter);
    } else {
        return;
    }
    //cout << to_string(walk)+"\n";
    if(walk < Nwalks){
        if (i){
            randomWalkRec(light,photonsList,newInter,walk,scene,flux,newR);
        }
    }
}

std::list<Photon> PhotonMapping::randomWalk(Scene scene){
    selectLightSource(scene);
    std::list<Photon> photonsList ={};
    for (auto l: scene.getLightSources()){
        if (!l->isAreaLight()){
            PointLight* p = static_cast <PointLight*>(l);
            RGB photons = p->getPhotons();
            int nPhtons = probability(photons);
            // << nPhtons <<endl;
            for (int i=0;i<nPhtons;i++){
                float randomA = (float)acos(-1.0 + 2.0*dis(gen));           
                // Generar ángulo azimutal (entre 0 y 2π)
                float randomL = 2.0 * M_PI * dis(gen);
                //cout <<"A: " +to_string(randomA)+" l:"+to_string(randomL)+"\n";
                Direction dRandom = Direction(sin(randomA)*cos(randomL),sin(randomA)*sin(randomL),cos(randomA));
                Ray r = Ray(dRandom,l->getCenter());
                Intersection inter;
                int walk=1;
                bool intPrimitives=intRayWithPrimitives(r,scene,inter);
                if (intPrimitives){
                    RGB flux = l->getLightIntensity()*(1.0/(float)nPhtons)*4*M_PI;
                    Photon photon = Photon(inter.getPoint(),dRandom,flux);
                    if (method == 1){
                        photonsList.push_back(photon);
                    }
                    randomWalkRec(p,photonsList,inter,walk,scene,flux,r);
                }
            }
        }
    }
    return photonsList;
}



PhotonMap PhotonMapping::generation_of_photon_map(Scene scene){
    std::list<Photon> photons;
    photons = randomWalk(scene);
    PhotonMap map =PhotonMap(photons,PhotonAxisPosition());
    cout << photons.size();
    return map;
}

void search_nearest(const PhotonMap& map, vector<const Photon *> &nearest,Point interPoint){
    Point query_position=interPoint;
    unsigned long nphtons_estimate=1000;
    float radius_estimate=0.1;
    nearest = map.nearest_neighbors(query_position,nphtons_estimate,radius_estimate);
}

bool findDifussePrimitive(Intersection &inter, Scene scene, int bounces, Ray r0){
    Direction w0 = r0.getDirection();
    Ray r = Ray(Direction(0,0,0),Point(0,0,0));
    if (bounces <= MAX_BOUNCES) {
        Direction wi;
        float RussianRoulette = (float)dis(gen);
        if (RussianRoulette - probability(inter.getK_d()) < 0.0 && probability(inter.getK_d()) > 0.0){
            // found a diffuse surface, return
            return true;

        } else if(RussianRoulette - probability(inter.getK_d())-probability(inter.getK_s()) < 0.0 ){
            wi = w0 + inter.getNormal()*(-2.0f)*w0.dot_product(inter.getNormal());
            r = Ray(wi.normalization(), inter.getPoint() + wi.normalization()* 0.0001, inter.getRefractionIndex());

        } else if (RussianRoulette - probability(inter.getK_d())-probability(inter.getK_s())-probability(inter.getK_t()) < 0.0){
            Direction _n = inter.getNormal();
            //float ior_in = r0.getRefractionIndex();
            float ior_in = 1.0f;
            float ior_out = inter.getRefractionIndex();
            float n_wi = w0.dot_product(_n);
            
            if (n_wi < 0) {
                // fuera de la superficie, cos(theta) es positivo
                //n_wi = -n_wi;
            }
            //float cos_theta_i = w0.dot_product(_n);
            /*if (cos_theta_i < 0.0f) {
                cos_theta_i = -cos_theta_i;
                
            }*/else {
                _n = _n * -1;
                float temp = ior_in;
                ior_in = ior_out;
                ior_out = temp;
            }
            
            //cout <<eta<<endl;
            float eta = ior_in / ior_out;
            float d = 1 - eta * eta * (1 - n_wi * n_wi);
            /*
            if (ior_in > ior_out) {
                wi = w0 + inter.getNormal()*(-1)*(-2)*w0.dot_product(inter.getNormal());
            }
            else{
            */
                //float sin_theta_t_2 = eta*eta*(1-cos_theta_i*cos_theta_i);
                //wi = w0 * eta + _n*(eta * cos_theta_i -sqrtf(1.0 - sin_theta_t_2));
                
            //}
            if (d < 0) { //angulo critico 
                wi = w0 + inter.getNormal()*(-1)*(-2)*w0.dot_product(inter.getNormal());
            }else{
                wi = w0 * eta + _n * (eta * n_wi - sqrt(d));
            }
            r = Ray(wi.normalization(), inter.getPoint() + wi.normalization() * 0.001 , ior_out);

        }else{
            // dead event
            return false;
        }
        //Direction wi = randomW(inter);
        bool i = intRayWithPrimitives(r, scene, inter);
        if (i) {
            return findDifussePrimitive(inter, scene, bounces+1, r);
        }
        else {
            return false;
        }
    }
    return false;
}



void first_intersection(vector<Ray> RGBsrays, Scene scene, RGB &color,const PhotonMap& map, const KERNEL kernel) {
    int index=0;
    float radius=0.1; //?????????????
    RGB avg=RGB(0.0,0.0,0.0);
    for (Ray r : RGBsrays){
        Intersection inter;
        bool i = intRayWithPrimitives(r,scene,inter);
        if (i) {
            if (findDifussePrimitive(inter, scene, 1, r)) {
                RGB aux=RGB(0.0,0.0,0.0);
                vector<const Photon *> nearest;
                search_nearest(map, nearest, inter.getPoint());
                
                switch (kernel) {
                    case BOX:
                    {
                        for (auto p: nearest){
                            aux = aux + p->getFlux()*(1.0/(float)(M_PI*radius*radius));
                        }
                        break;
                    }
                    
                    case CONE:
                    {
                        float k = 2.0;
                        float wpc, dp;
                        for (auto p: nearest){
                            dp = (p->getPosition()-inter.getPoint()).modulus();
                            wpc = 1.0 - dp/(k*radius);
                            aux = aux + p->getFlux() * wpc;
                        }
                        aux = aux * (1.0f / ((M_PI * radius * radius) * (1.0f - (2.0f/(3.0f*k)))));
                        break;
                    }

                    case GAUSSIAN:
                    {
                        float alpha = 0.918;
                        float beta = 1.953;
                        float wpg, dp;
                        for (auto p: nearest){
                            dp = (p->getPosition()-inter.getPoint()).modulus();
                            wpg = alpha * (1.0f - ((1.0f - exp(-beta * ((dp * dp) / 2.0f * radius * radius))) / (1.0f - exp(-beta))));
                            aux = aux + p->getFlux() * wpg;
                        }
                        aux = aux * (1.0f / (M_PI * radius * radius));
                        break;
                    }
                    
                }
                RGB fr = inter.getK_d();
                aux = aux * fr;
                if (method == 1){
                    avg = avg + aux;
                }else{
                    avg = avg + aux + NextEventEstimation(scene,inter);
                }
            }
        }
    }
    color = avg*(1.0/(float)RGBsrays.size());
}

Direction randomPointinPixel(Image &image, Scene scene, float xmax, float ymax, float xmin, float ymin) {
    Camera cam=scene.getCamera();
    // Generar i dentro de [xmin, xmax] asegurando que no sobrepase image.width
    float i = ((float)dis(gen)) * (xmax - xmin) + xmin;
    //if (i > image.width) i = image.width;  // Limitar i a image.width
    if (i < -1.0f) i = -1.0f;  // Evitar que sea negativo
    if (i > 1.0f) i = 1.0f;  // Evitar que sea negativo

    // Generar row dentro de [ymin, ymax] asegurando que no sobrepase image.height
    float row = ((float)dis(gen)) * (ymax - ymin) + ymin;
    //if (row > image.height) row = image.height;  // Limitar row a image.height
    if (row < -1.0f) i = -1.0f;  // Evitar que sea negativo
    if (row > 1.0f) i = 1.0f;  // Evitar que sea negativo

    Direction d = cam.getf() 
           + cam.getl() * i
           + cam.getu() * row;
   // cout <<d;

    return d;
}



void section(Image &image,Scene scene,int  start, int end,int row,const PhotonMap& map, KERNEL kernel){
    int n_rays=512;
    Point origin=scene.getCamera().getOrigin();
    Camera cam=scene.getCamera();
    for (int i=start;i <end;i++){
        vector<Ray> RGBsrays;
        Direction d = cam.getf() 
            + cam.getl() * (1 - ((i % image.width) / (float)(image.width / 2))) 
            + cam.getu() * (1 - ((row % image.height) / (float)(image.height / 2)));
        Ray r = Ray(d,origin);
        RGBsrays.emplace_back(r);
        //cout << "DIRECTION" <<d << endl;
        if ((i!=end)&&(row!=image.width)){
            for (int j=0;j<n_rays-1;j++){
                Ray randomray= Ray(randomPointinPixel(image,scene,
                (1 - ((i % image.width) / (float)(image.width / 2))),
                (1 - ((row % image.height) / (float)(image.height / 2))),
                (1 - (((i+1) % image.width) / (float)(image.width / 2))),
                (1 - (((row+1) % image.height) / (float)(image.height / 2)))),origin);
                RGBsrays.emplace_back(randomray);
            }
            //cout << to_string(i)+"\n";
        }
        RGB p;
        Intersection inter;
        first_intersection(RGBsrays,scene,p, map, kernel);       
        image.RGBes[i]=p;
    }
    
}

void PhotonMapping::setImage(int _width, int _height, std::vector<RGB> _RGBs)
{
    image=Image(_RGBs,_width,_height);

}


void PhotonMapping::parallelization(Scene scene) {
    int num_threads = 8;  // Fixed number of threads
    PhotonMap map = generation_of_photon_map(scene);
    vector<thread> threads;

    int rows_per_thread = image.height / num_threads;  // Rows each thread will process
    int remainder = image.height % num_threads;       // Remaining rows to distribute

    // Launch threads
    for (int t = 0; t < num_threads; ++t) {
        int start_row = t * rows_per_thread;
        int end_row = start_row + rows_per_thread;

        // Distribute the remainder rows among the first few threads
        if (t < remainder) {
            end_row++;
        }

        // Launch a thread for the assigned row range
        threads.emplace_back([=, &scene, &map]() {
            for (int row = start_row; row < end_row; ++row) {
                int start = row * image.width;  // Start pixel index for this row
                int end = start + image.width; // End pixel index for this row
                section(image, scene, start, end, row, map, this->kernel);
            }
        });
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    cout << "Fin de parallelization\n";
}


Image PhotonMapping::getImage()
{
    return image;
}
