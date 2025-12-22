#ifdef USE_EMBREE

#include "SceneEmbree.hpp"
#include "Operations.hpp"
#include <iostream>
#include <limits>

SceneEmbree::SceneEmbree(const Scene& scene)
    : sceneRef(scene)
{
    device = rtcNewDevice(nullptr);
    if (!device) {
        std::cerr << "[Embree] Error: no se pudo crear el dispositivo\n";
        return;
    }

    rtcScene = rtcNewScene(device);
    if (!rtcScene) {
        std::cerr << "[Embree] Error: no se pudo crear la escena\n";
        return;
    }

    const auto& tris = sceneRef.getTriangles();
    std::cerr << "[Embree] DEBUG: tris.size()=" << tris.size() << "\n";
    if (!tris.empty()) {
        std::cerr << "[Embree] DEBUG: Entrando a crear geometría...\n";
        vertices.reserve(tris.size() * 3);
        indices.reserve(tris.size());
        triMap.primById.reserve(tris.size());

        for (size_t i = 0; i < tris.size(); ++i) {
            const Triangle& t = tris[i];
            vertices.push_back({t.v0.x, t.v0.y, t.v0.z});
            vertices.push_back({t.v1.x, t.v1.y, t.v1.z});
            vertices.push_back({t.v2.x, t.v2.y, t.v2.z});
            indices.push_back({static_cast<unsigned>(i*3), static_cast<unsigned>(i*3+1), static_cast<unsigned>(i*3+2)});
            triMap.primById.push_back(const_cast<Triangle*>(&t));
        }

        RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
        
        // Crear buffers que Embree poseerá
        float* vbuf = (float*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
                                                       3*sizeof(float), vertices.size());
        unsigned* ibuf = (unsigned*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
                                                             3*sizeof(unsigned), indices.size());
        
        // Copiar datos
        for (size_t i = 0; i < vertices.size(); ++i) {
            vbuf[i*3 + 0] = vertices[i].x;
            vbuf[i*3 + 1] = vertices[i].y;
            vbuf[i*3 + 2] = vertices[i].z;
        }
        
        for (size_t i = 0; i < indices.size(); ++i) {
            ibuf[i*3 + 0] = indices[i].v0;
            ibuf[i*3 + 1] = indices[i].v1;
            ibuf[i*3 + 2] = indices[i].v2;
        }
        
        rtcCommitGeometry(geom);
        unsigned geomID = rtcAttachGeometry(rtcScene, geom);
        rtcReleaseGeometry(geom);
        std::cerr << "[Embree] GeomID asignado: " << geomID << ", vertices=" << vertices.size() 
                  << ", triangles=" << indices.size() << "\n";
        
        // Debug: mostrar primeros vértices e índices
        if (vertices.size() >= 3) {
            std::cerr << "[Embree] Primeros 3 vértices:\n";
            for (int i = 0; i < 3 && i < (int)vertices.size(); ++i) {
                std::cerr << "  v[" << i << "]=(" << vertices[i].x << "," << vertices[i].y << "," << vertices[i].z << ")\n";
            }
        }
        if (indices.size() >= 1) {
            std::cerr << "[Embree] Primer triángulo: idx[0]=(" << indices[0].v0 << "," << indices[0].v1 << "," << indices[0].v2 << ")\n";
        }
    }

    // Fallback para primitivas no triangulares (planos, esferas, etc.)
    for (const auto& s : sceneRef.getSpheres()) {
        fallbackPrims.push_back(const_cast<Sphere*>(&s));
    }
    for (const auto& p : sceneRef.getPlanes()) {
        fallbackPrims.push_back(const_cast<Plane*>(&p));
    }

    rtcCommitScene(rtcScene);
    std::cout << "[Embree] Escena construida con " << tris.size() << " triángulos";
    if (!fallbackPrims.empty()) {
        std::cout << " y " << fallbackPrims.size() << " primitivas analíticas (fallback)";
    }
    std::cout << "\n";
}

SceneEmbree::~SceneEmbree() {
    if (rtcScene) rtcReleaseScene(rtcScene);
    if (device) rtcReleaseDevice(device);
}

Interseccion SceneEmbree::intersect(Ray& ray) const {
    Interseccion inter{};
    inter.hayInterseccion = false;
    inter.t = std::numeric_limits<float>::max();
    inter.primitiva = nullptr;

    float bestT = inter.t;
    Primitive* bestPrim = nullptr;
    bool hitEmbree = false;

    if (rtcScene) {
        RTCIntersectArguments args;
        rtcInitIntersectArguments(&args);

        RTCRayHit rh{};
        rh.ray.org_x = ray.origin_.x; rh.ray.org_y = ray.origin_.y; rh.ray.org_z = ray.origin_.z;
        // Usar la misma dirección sin normalizar que el resto del pipeline para paridad con BVH
        rh.ray.dir_x = ray.direction_.x; rh.ray.dir_y = ray.direction_.y; rh.ray.dir_z = ray.direction_.z;
        rh.ray.tnear = 0.001f;
        rh.ray.tfar = 1e6f;
        rh.ray.mask = 0xFFFFFFFF;
        rh.ray.flags = 0;
        rh.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rh.hit.primID = RTC_INVALID_GEOMETRY_ID;

        rtcIntersect1(rtcScene, &rh, &args);

        if (rh.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            hitEmbree = true;
            // Usar t directamente para mantener coherencia con BVH (dir no normalizada)
            bestT = rh.ray.tfar;
            
            static int hitDebug = 0;
            if (hitDebug++ < 3) {
                std::cerr << "[Embree] HIT! geomID=" << rh.hit.geomID << " primID=" << rh.hit.primID 
                          << " t_embree=" << rh.ray.tfar
                          << " u=" << rh.hit.u << " v=" << rh.hit.v << "\n";
            }
            
            if (rh.hit.primID < triMap.primById.size()) {
                bestPrim = triMap.primById[rh.hit.primID];
                // Debug material inmediatamente
                static int matCount = 0;
                if (matCount++ < 5 && bestPrim) {
                    Material mat = bestPrim->getMaterial();
                    std::cerr << "[MAT] Embree tri: kd=(" << mat.kd.R << "," << mat.kd.G << "," << mat.kd.B << ") "
                              << "ks=(" << mat.ks.R << "," << mat.ks.G << "," << mat.ks.B << ")\n";
                }
            } else {
                std::cerr << "[Embree] WARN: primID " << rh.hit.primID << " >= triMap.size " << triMap.primById.size() << "\n";
            }
        }
    }

    // Fallback analítico
    bool hadEmbreeHit = hitEmbree && bestPrim != nullptr;
    for (auto* p : fallbackPrims) {
        float tempT;
        if (p->intersect(ray, tempT) && tempT > 0.001f && tempT < bestT) {
            bestT = tempT;
            bestPrim = p;
            hitEmbree = false;  // Fallback ganó, no fue hit de Embree
        }
    }

    if (bestPrim) {
        inter.hayInterseccion = true;
        inter.t = bestT;
        inter.primitiva = bestPrim;
        // Usar dirección original para mantener coherencia con el resto del pipeline
        Direction dirUsed = ray.direction_.toDirection();
        inter.punto = Operations::add(ray.origin_.toPoint(), Operations::mult(dirUsed, bestT));
        // Usar el mismo cálculo de normal que el resto del pipeline para mantener paridad con BVH
        inter.normal = bestPrim->getPointNormal(inter.punto);
        
        static int ptoDebug = 0;
        if (ptoDebug++ < 10) {
            std::cerr << "[Embree] Final point: (" << inter.punto.x << "," << inter.punto.y << "," << inter.punto.z << ")";
            std::cerr << " normal: (" << inter.normal.x << "," << inter.normal.y << "," << inter.normal.z << ") len=" << Operations::mod(inter.normal) << "\n";
            Material m = bestPrim->getMaterial();
            std::cerr << "[Embree] Mat kd=(" << m.kd.R << "," << m.kd.G << "," << m.kd.B << ") ks=(" << m.ks.R << "," << m.ks.G << "," << m.ks.B << ")\n";
        }
        static int replaceDebug = 0;
        if (replaceDebug++ < 5 && hadEmbreeHit) {
            bool isTri = dynamic_cast<Triangle*>(bestPrim) != nullptr;
            std::cerr << "[Embree] Winner prim: " << (isTri ? "Triangle" : "Fallback")
                      << " t=" << bestT << " hadEmbreeHit=" << (hadEmbreeHit?"yes":"no")
                      << " hitEmbree=" << (hitEmbree?"yes":"no") << "\n";
        }
    }

    static int debugCount = 0;
    if (debugCount++ < 10) {
        std::cerr << "[DEBUG] Ray org=(" << ray.origin_.x << "," << ray.origin_.y << "," << ray.origin_.z << ") "
                  << "dir=(" << ray.direction_.x << "," << ray.direction_.y << "," << ray.direction_.z << ") "
                  << "embree_hit=" << (hitEmbree ? "yes" : "no") << " fallback_hit=" << (bestPrim ? "yes" : "no") << "\n";
    }

    return inter;
}

bool SceneEmbree::intersectShadow(Ray& ray, float maxDist) const {
    float bestT = maxDist;
    Primitive* hit = nullptr;

    if (rtcScene) {
        RTCIntersectArguments args;
        rtcInitIntersectArguments(&args);
        RTCRayHit rh{};
        rh.ray.org_x = ray.origin_.x; rh.ray.org_y = ray.origin_.y; rh.ray.org_z = ray.origin_.z;
        // Mantener la dirección sin normalizar para que t coincida con el resto del pipeline
        rh.ray.dir_x = ray.direction_.x; rh.ray.dir_y = ray.direction_.y; rh.ray.dir_z = ray.direction_.z;
        rh.ray.tnear = 0.001f;
        rh.ray.tfar = maxDist;
        rh.ray.mask = 0xFFFFFFFF;
        rh.ray.flags = 0;
        rh.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        rh.hit.primID = RTC_INVALID_GEOMETRY_ID;

        rtcIntersect1(rtcScene, &rh, &args);

        if (rh.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
            float hitT = rh.ray.tfar;
            if (hitT < bestT) {
                bestT = hitT;
                if (rh.hit.primID < triMap.primById.size()) {
                    hit = triMap.primById[rh.hit.primID];
                } else {
                    std::cerr << "[Embree] WARN: primID " << rh.hit.primID << " >= triMap.size " << triMap.primById.size() << "\n";
                }
            }
        }
    }

    for (auto* p : fallbackPrims) {
        float tempT;
        if (p->intersect(ray, tempT) && tempT > 0.001f && tempT < bestT) {
            bestT = tempT;
            hit = p;
        }
    }

    return hit != nullptr;
}

#endif // USE_EMBREE
