#pragma once
#include "Anim.h"
#include "SimpleMath.h"
using namespace DirectX::SimpleMath;

namespace onut
{
    class Texture;
    class ParticleSystem;
    struct sEmitterDesc;

    template<typename Ttype>
    struct sParticleRange
    {
        Ttype from;
        Ttype to;
        Ttype value;

        void update(float t)
        {
            value = lerp(from, to, t);
        }
    };

    class Particle
    {
    public:
        void update();

        bool isAlive() const { return life > 0.f; }

        Particle*               pNext = nullptr;
        sEmitterDesc*           pDesc;
        float                   life;
        float                   delta;

        Vector3                 position;
        Vector3                 velocity;

        sParticleRange<Color>           color;
        sParticleRange<float>           angle;
        sParticleRange<float>           size;
        sParticleRange<unsigned int>    image_index;

        Texture*                        pTexture = nullptr;
    };
}
