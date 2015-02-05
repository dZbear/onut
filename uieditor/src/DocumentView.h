#pragma once
#include "onut.h"

enum class eDocumentState : uint8_t
{
    IDLE,
    MOVING_GIZO,
    MOVING_HANDLE
};

class DocumentView
{
public:
    DocumentView();
    virtual ~DocumentView();

    void setSelected(onut::UIControl* in_pSelected, bool bUpdateSceneGraph = true);

    void update();
    void render();

    onut::UIContext* pUIContext = nullptr;
    onut::UIControl* pUIScreen = nullptr;
    onut::UIControl* pSelected = nullptr;

    void onGizmoStart(onut::UIControl* pControl, const onut::UIMouseEvent& mouseEvent);
    void onGizmoEnd(onut::UIControl* pControl, const onut::UIMouseEvent& mouseEvent);

    void controlCreated(onut::UIControl* pControl, onut::UIControl* pParent);
    void updateSelectedGizmoRect();
    void updateInspector();

private:
    void onGizmoHandleStart(onut::UIControl* pControl, const onut::UIMouseEvent& mouseEvent);
    void onGizmoHandleEnd(onut::UIControl* pControl, const onut::UIMouseEvent& mouseEvent);

    void updateSelectionWithRect(const onut::sUIRect& rect);
    void updateGizmoRect();
    void updateMovingHandle();
    void updateMovingGizmo();
    void deleteSelection();

    bool getXAutoGuide(const onut::sUIRect& rect, float& x, bool& side);
    bool getYAutoGuide(const onut::sUIRect& rect, float& y, bool& side);
    void xAutoGuideAgainst(const onut::sUIRect& otherRect, bool& found, const onut::sUIRect& rect, float& x, bool& side, float& closest);
    void yAutoGuideAgainst(const onut::sUIRect& otherRect, bool& found, const onut::sUIRect& rect, float& y, bool& side, float& closest);
    onut::sUIRect applyAnchorsToRect(onut::UIControl* pControl, onut::sUIRect& rect);
    onut::sUIRect revertAnchorsFromRect(onut::UIControl* pControl, onut::sUIRect& rect);

    onut::UIPanel*      m_pGizmo = nullptr;
    onut::UIControl*    m_gizmoHandles[8];
    onut::UIControl*    m_guides[2];
    onut::UIControl*    m_pCurrentHandle = nullptr;
    onut::UITreeView*   m_pSceneGraph = nullptr;
    Vector2             m_mousePosOnDown;
    onut::sUIRect       m_controlRectOnDown;
    eDocumentState      m_state = eDocumentState::IDLE;
    bool                m_autoGuide = true;
    float               m_autoPadding = 8.f;
};
