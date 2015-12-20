// Required for WinMain
#include <Windows.h>

// Oak Nut include
#include "onut.h"

void init();
void update();
void render();

float g_spriteAngle = 0.f;
OAnim<Matrix> batchTransform;

// Main
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
    OSettings->setGameName("Sprites Sample");
    ORun(init, update, render);
}

void init()
{
    batchTransform.startKeyframed(
        Matrix::Identity,
        {
            {
                Matrix::Identity,
                2.f,
                OTeleport
            },
            {
                Matrix::CreateTranslation(-OScreenCenterXf, -OScreenCenterYf, 0) *
                Matrix::CreateRotationZ(DirectX::XMConvertToRadians(45.f)) *
                Matrix::CreateTranslation(OScreenCenterXf, OScreenCenterYf, 0),
                1.0f,
                OLinear
            },
            {
                Matrix::CreateTranslation(-OScreenCenterXf, -OScreenCenterYf, 0) *
                Matrix::CreateRotationZ(DirectX::XMConvertToRadians(-45.f)) *
                Matrix::CreateTranslation(OScreenCenterXf, OScreenCenterYf, 0),
                2.0f,
                OLinear
            },
            {
                Matrix::Identity,
                1.f,
                OLinear
            }
        }, OLoop);
}

void update()
{
    g_spriteAngle += ODT * 45.f;
}

void render()
{
    auto pNutTexture = OGetTexture("onutLogo.png");
    auto pFrameTexture = OGetTexture("frameSmall.png");
    auto pChainTexture = OGetTexture("chain.png");

    // Clear
    ORenderer->clear(OColorHex(1d232d));

    // Begin a batch
    OSpriteBatch->begin(batchTransform);

    // Opaque
    OSpriteBatch->drawRect(nullptr, Rect(0, 0, 64, 64));
    OSpriteBatch->drawRect(nullptr, Rect(64, 0, 64, 64), Color(1, 0, 0, 1)); // Red
    OSpriteBatch->drawInclinedRect(nullptr, Rect(192, 0, 64, 64), -.5f); // Inclined

    // Textured
    OSpriteBatch->drawRect(pNutTexture, Rect(0, 64, 64, 64));
    OSpriteBatch->drawRect(pNutTexture, Rect(64, 64, 64, 64), Color(1, 0, 0, 1)); // Tinted

    // Per vertex coloring
    OSpriteBatch->drawRectWithColors(nullptr, Rect(0, 128, 64, 64), {Color(1, 0, 0, 1), Color(1, 1, 0, 1), Color(0, 1, 0, 1), Color(0, 0, 1, 1)});

    // Custom UVs
    OSpriteBatch->drawRectWithUVs(pNutTexture, Rect(64, 128, 64, 64), Vector4(0, 0, .5f, .5f));

    // Split the texture in 4 corner (Good for corsairs)
    OSpriteBatch->draw4Corner(pNutTexture, Rect(128, 64, 160, 160));

    // Scale 9, 9 Patch, 9 slice (whatever you guys call it)
    OSpriteBatch->drawRectScaled9(pFrameTexture, Rect(0, 256, 160, 160), Vector4(48, 48, 48, 48));
    OSpriteBatch->drawRectScaled9(pFrameTexture, Rect(160, 256, 256, 160), Vector4(48, 48, 48, 48));
    OSpriteBatch->drawRectScaled9RepeatCenters(pFrameTexture, Rect(416, 256, 256, 160), Vector4(48, 48, 48, 48));

    // Sprites
    OSpriteBatch->drawSprite(pNutTexture, Vector2(64, 480));
    OSpriteBatch->drawSprite(pNutTexture, Vector2(192, 480), Color::White, g_spriteAngle);
    OSpriteBatch->drawSprite(pNutTexture, Vector2(320, 480), Color::White, g_spriteAngle, .5f);

    // With a custom matrix
    Matrix customTransform = Matrix::Identity;
    customTransform *= Matrix::CreateRotationZ(DirectX::XMConvertToRadians(g_spriteAngle));
    customTransform *= Matrix::CreateScale(4.f, 0.5f, 1.f);
    customTransform *= Matrix::CreateTranslation(520, 120, 0);
    OSpriteBatch->drawSprite(pNutTexture, customTransform, Color::White);

    // Lines
    OSpriteBatch->drawBeam(pChainTexture, Vector2(448, 480), Vector2(648, 440), 32.0f, Color::White);
    OSpriteBatch->drawBeam(pChainTexture, Vector2(448, 520), Vector2(648, 480), 32.0f, Color::White, g_spriteAngle * .1f);

    OSpriteBatch->changeBlendMode(onut::SpriteBatch::eBlendMode::Add);
    OSpriteBatch->drawSprite(pNutTexture, Vector2(400, 50), Color::White, 0, .25f);
    OSpriteBatch->changeBlendMode(onut::SpriteBatch::eBlendMode::Alpha);
    OSpriteBatch->drawSprite(pNutTexture, Vector2(450, 50), Color::White, 0, .25f);
    OSpriteBatch->changeBlendMode(onut::SpriteBatch::eBlendMode::ForceWrite);
    OSpriteBatch->drawSprite(pNutTexture, Vector2(500, 50), Color::White, 0, .25f);
    OSpriteBatch->changeBlendMode(onut::SpriteBatch::eBlendMode::Multiplied);
    OSpriteBatch->drawSprite(pNutTexture, Vector2(550, 50), Color::White, 0, .25f);
    OSpriteBatch->changeBlendMode(onut::SpriteBatch::eBlendMode::Opaque);
    OSpriteBatch->drawSprite(pNutTexture, Vector2(600, 50), Color::White, 0, .25f);
    OSpriteBatch->changeBlendMode(onut::SpriteBatch::eBlendMode::PreMultiplied);
    OSpriteBatch->drawSprite(pNutTexture, Vector2(650, 50), Color::White, 0, .25f);

    // End and flush the batch
    OSpriteBatch->end();
}
