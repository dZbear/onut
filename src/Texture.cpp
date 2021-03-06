#include "LodePNG.h"
#include "onut.h"
#include "Texture.h"

#include <cassert>
#include <vector>

namespace onut
{
    Texture* Texture::createRenderTarget(const sSize& size)
    {
#ifdef EASY_GRAPHIX
        auto pRet = new Texture();
        pRet->m_pTextureView = egCreateTexture2D(static_cast<uint32_t>(size.x),
                                                 static_cast<uint32_t>(size.y),
                                                 nullptr, 0, EG_RENDER_TARGET);
        pRet->m_size = size;
        return pRet;
#else
        auto pDevice = ORenderer->getDevice();

        auto pRet = new Texture();

        D3D11_TEXTURE2D_DESC textureDesc = {0};
        HRESULT result;
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
        memset(&renderTargetViewDesc, 0, sizeof(renderTargetViewDesc));
        memset(&shaderResourceViewDesc, 0, sizeof(shaderResourceViewDesc));

        pRet->m_size = size;

        // Setup the render target texture description.
        textureDesc.Width = size.x;
        textureDesc.Height = size.y;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;

        // Create the render target texture.
        result = pDevice->CreateTexture2D(&textureDesc, NULL, &pRet->m_pTexture);
        if (result != S_OK)
        {
            assert(false && "Failed CreateTexture2D");
            return nullptr;
        }

        // Setup the description of the render target view.
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;

        // Create the render target view.
        result = pDevice->CreateRenderTargetView(pRet->m_pTexture, &renderTargetViewDesc, &pRet->m_pRenderTargetView);
        if (result != S_OK)
        {
            assert(false && "Failed CreateRenderTargetView");
            return nullptr;
        }

        // Setup the description of the shader resource view.
        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;

        // Create the shader resource view.
        result = pDevice->CreateShaderResourceView(pRet->m_pTexture, &shaderResourceViewDesc, &pRet->m_pTextureView);
        if (result != S_OK)
        {
            assert(false && "Failed CreateShaderResourceView");
            return nullptr;
        }

        return pRet;
#endif /* !EASY_GRAPHIX */
    }

    Texture* Texture::createDynamic(const sSize& size)
    {
#ifdef EASY_GRAPHIX
        return nullptr;
#else /* EASY_GRAPHIX */
        ID3D11Texture2D* pTexture = NULL;
        ID3D11ShaderResourceView* pTextureView = NULL;
        auto pRet = new Texture();

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = size.x;
        desc.Height = size.y;
        desc.MipLevels = desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

        auto pDevice = ORenderer->getDevice();
        auto ret = pDevice->CreateTexture2D(&desc, NULL, &pTexture);
        assert(ret == S_OK);
        ret = pDevice->CreateShaderResourceView(pTexture, NULL, &pTextureView);
        assert(ret == S_OK);

        pRet->m_size = size;
        pRet->m_pTextureView = pTextureView;
        pRet->m_pTexture = pTexture;

        return pRet;
#endif /* !EASY_GRAPHIX */
    }

    void Texture::setData(const uint8_t *in_pData)
    {
        auto pDeviceContext = ORenderer->getDeviceContext();

        D3D11_MAPPED_SUBRESOURCE data;
        pDeviceContext->Map(m_pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
        memcpy(data.pData, in_pData, m_size.x * m_size.y * 4);
        pDeviceContext->Unmap(m_pTexture, 0);
    }

    Texture* Texture::createFromFile(const std::string& filename, bool generateMipmaps)
    {
        Texture* pRet = NULL;

        std::vector<unsigned char> image; //the raw pixels (holy crap that must be slow)
        unsigned int w, h;
        auto ret = lodepng::decode(image, w, h, filename);
        assert(!ret);
        sSize size{w, h};
        byte* pData = &(image[0]);
        ULONG len = size.x * size.y;

        // Pre multiplied
        for (ULONG i = 0; i < len; ++i, pData += 4)
        {
            pData[0] = pData[0] * pData[3] / 255;
            pData[1] = pData[1] * pData[3] / 255;
            pData[2] = pData[2] * pData[3] / 255;
        }

        return createFromData(size, &(image[0]), generateMipmaps);
    }

    Texture* Texture::createFromFileData(const unsigned char* in_pData, uint32_t in_size, bool in_generateMipmaps)
    {
        Texture* pRet = NULL;

        std::vector<unsigned char> image; //the raw pixels (holy crap that must be slow)
        unsigned int w, h;
        lodepng::State state;
        auto ret = lodepng::decode(image, w, h, state, in_pData, in_size);
        assert(!ret);
        sSize size{w, h};
        byte* pData = &(image[0]);
        ULONG len = size.x * size.y;

        // Pre multiplied
        for (ULONG i = 0; i < len; ++i, pData += 4)
        {
            pData[0] = pData[0] * pData[3] / 255;
            pData[1] = pData[1] * pData[3] / 255;
            pData[2] = pData[2] * pData[3] / 255;
        }

        return createFromData(size, &(image[0]), in_generateMipmaps);
    }

    Texture* Texture::createFromData(const sSize& size, const unsigned char* in_pData, bool in_generateMipmaps)
    {
#ifdef EASY_GRAPHIX
        auto pRet = new Texture();
        pRet->m_pTextureView = egCreateTexture2D(static_cast<uint32_t>(size.x),
                                                 static_cast<uint32_t>(size.y),
                                                 in_pData, EG_U8 | EG_RGBA, 
                                                 in_generateMipmaps ? EG_GENERATE_MIPMAPS : static_cast<EG_TEXTURE_FLAGS>(0));
        pRet->m_size = size;
        return pRet;
#else /* EASY_GRAPHIX */
        ID3D11Texture2D* pTexture = NULL;
        ID3D11ShaderResourceView* pTextureView = NULL;
        auto pRet = new Texture();

        // Manually generate mip levels
        bool allowMipMaps = true;
        UINT w2 = 1;
        UINT h2 = 1;
        while (w2 < (UINT)size.x) w2 *= 2;
        if (size.x != w2) allowMipMaps = false;
        while (h2 < (UINT)size.y) h2 *= 2;
        if (size.y != h2) allowMipMaps = false;
        unsigned char* pMipMaps = NULL;
        int mipLevels = 1;
        D3D11_SUBRESOURCE_DATA* mipsData = NULL;
        allowMipMaps = allowMipMaps && in_generateMipmaps;
        if (allowMipMaps)
        {
            UINT biggest = max(w2, h2);
            UINT w2t = w2;
            UINT h2t = h2;
            UINT totalSize = w2t * h2t * 4;
            while (!(w2t == 1 && h2t == 1))
            {
                ++mipLevels;
                w2t /= 2;
                if (w2t < 1) w2t = 1;
                h2t /= 2;
                if (h2t < 1) h2t = 1;
                totalSize += w2t * h2t * 4;
            }
            pMipMaps = new byte[totalSize];
            memcpy(pMipMaps, in_pData, size.x * size.y * 4);

            mipsData = new D3D11_SUBRESOURCE_DATA[mipLevels];

            w2t = w2;
            h2t = h2;
            totalSize = 0;
            int mipTarget = mipLevels;
            mipLevels = 0;
            byte* prev;
            byte* cur;
            while (mipLevels != mipTarget)
            {
                prev = pMipMaps + totalSize;
                mipsData[mipLevels].pSysMem = prev;
                mipsData[mipLevels].SysMemPitch = w2t * 4;
                mipsData[mipLevels].SysMemSlicePitch = 0;
                totalSize += w2t * h2t * 4;
                cur = pMipMaps + totalSize;
                w2t /= 2;
                if (w2t < 1) w2t = 1;
                h2t /= 2;
                if (h2t < 1) h2t = 1;
                ++mipLevels;
                if (mipLevels == mipTarget) break;
                int accum;

                // Generate the mips
                int multX = w2 / w2t;
                int multY = h2 / h2t;
                for (UINT y = 0; y < h2t; ++y)
                {
                    for (UINT x = 0; x < w2t; ++x)
                    {
                        for (UINT k = 0; k < 4; ++k)
                        {
                            accum = 0;
                            accum += prev[(y * multY * w2 + x * multX) * 4 + k];
                            accum += prev[(y * multY * w2 + (x + multX / 2) * multX) * 4 + k];
                            accum += prev[((y + multY / 2) * multY * w2 + x * multX) * 4 + k];
                            accum += prev[((y + multY / 2) * multY * w2 + (x + multX / 2) * multX) * 4 + k];
                            cur[(y * w2t + x) * 4 + k] = accum / 4;
                        }
                    }
                }

                w2 = w2t;
                h2 = h2t;
            }
        }

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = size.x;
        desc.Height = size.y;
        desc.MipLevels = mipLevels;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = (pMipMaps) ? pMipMaps : in_pData;
        data.SysMemPitch = size.x * 4;
        data.SysMemSlicePitch = 0;

        auto pDevice = ORenderer->getDevice();
        auto ret = pDevice->CreateTexture2D(&desc, (mipsData) ? mipsData : &data, &pTexture);
        assert(ret == S_OK);
        ret = pDevice->CreateShaderResourceView(pTexture, NULL, &pTextureView);
        assert(ret == S_OK);

        pTexture->Release();
        if (pMipMaps) delete[] pMipMaps;
        if (mipsData) delete[] mipsData;

        pRet->m_size = size;
        pRet->m_pTextureView = pTextureView;

        return pRet;
#endif /* EASY_GRAPHIX */
    }

    Texture::~Texture()
    {
#ifdef EASY_GRAPHIX
        egDestroyTexture(&m_pTextureView);
#else
        if (m_pTextureView) m_pTextureView->Release();
        if (m_pTexture) m_pTexture->Release();
        if (m_pRenderTargetView) m_pRenderTargetView->Release();
#endif
    }

    void Texture::bind(int slot)
    {
#ifdef EASY_GRAPHIX
        switch (slot)
        {
            case 0:
                egBindDiffuse(m_pTextureView);
                break;
            case 1:
                egBindNormal(m_pTextureView);
                break;
            case 2:
                egBindMaterial(m_pTextureView);
                break;
        }
#else
        ORenderer->getDeviceContext()->PSSetShaderResources(slot, 1, &m_pTextureView);
#endif
    }

    void Texture::bindRenderTarget()
    {
        if (m_pRenderTargetView)
        {
            ORenderer->getDeviceContext()->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
        }
    }

    void Texture::unbindRenderTarget()
    {
        ORenderer->bindRenderTarget(nullptr);
    }

    void Texture::clearRenderTarget(const Color& color)
    {
        ORenderer->getDeviceContext()->ClearRenderTargetView(m_pRenderTargetView, &color.x);
    }
}
