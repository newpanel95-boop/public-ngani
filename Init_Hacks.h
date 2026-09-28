#pragma once

float SlideRange;

enum TouchPhase {
    Began = 0,
    Moved = 1,
    Stationary = 2,
    Ended = 3,
    Canceled = 4,
};

enum TouchType {
    Direct = 0,
    Indirect = 1,
    Stylus = 2,
};

struct Touch {
    int m_FingerId;
    Vector2 m_Position;
    Vector2 m_RawPosition;
    Vector2 m_PositionDelta;
    float m_TimeDelta;
    int m_TapCount;
    TouchPhase m_Phase;
    TouchType m_Type;
    float m_Pressure;
    float m_maximumPossiblePressure;
    float m_Radius;
    float m_RadiusVariance;
    float m_AltitudeAngle;
    float m_AzimuthAngle;
};

enum LineTarget {
    Top = 0,
    Center = 1,
    Bottom = 2
};

enum EspBoxType {
    Fill = 0,
    Outline = 1,
    Corner = 2,
    ThreeD = 3
};

enum EspHealthPosition {
    HealthTop = 0,
    HealthSide = 1
};

enum CrosshairTarget {
    Normal = 0,
    Circle = 1,
    Cross = 2
};

enum EspStyleTarget {
    EspStyleNone = 0,
    EspStyle3DSphere = 1,
    EspStylePlayerSignal = 2
};

enum EAim {
  Distance = 0,
  Crosshair = 1
};

enum EAimTarget {
    Heads = 0,
    Chests = 1,
    Body = 2
};

enum EAimTrigger {
    None = 0,
    Shooting = 1,
    Scoping = 2
};

struct sConfig {
        float Pline;
		float Bline;
		float PskelLine;
		float BskelLine;

    struct sInitImGui {
        bool clearMousePos = true;
        uintptr_t thiz;
    };
    sInitImGui ImGuiMenu{0};

struct sWeaponAim {
        bool Aimbot360;
        float AimAssistSize;
        bool AimSilent;
        EAimTarget Target;
        EAimTrigger Trigger;
        EAim By;
        float size;
        float Cross;
    };
    sWeaponAim Aim{0};

    struct sESPMenuLineScale {
        float lineSize;
    };
    sESPMenuLineScale sESPMenuLineScale{0};

    struct sESPMenu {
        bool Alert;
        bool Count;
        bool Name;
        bool isPlayerLine;
        LineTarget Target;
        EspBoxType BoxType;
        EspHealthPosition HealthPosition;
        CrosshairTarget CrosshairType;
        EspStyleTarget EspStyle;
        bool Box;
        bool Health;
        bool Distance;
        bool Skeleton;
        bool Crosshair;
        bool Aimline;
    };
    sESPMenu ESPMenu{0};

    struct sColorsESPPLAYER {
    float *LinePLAYER;
    float *BoxPLAYER;
    float *NamePLAYER;
    float *HealthPLAYER;
    float *DistancePLAYER;
    float *SkeletonPLAYER;
};
sColorsESPPLAYER sColorsESPPLAYER{0};

struct sColorsESPBOT {
    float *LineBOT;
    float *BoxBOT;
    float *NameBOT;
    float *HealthBOT;
    float *DistanceBOT;
    float *SkeletonBOT;
};
sColorsESPBOT sColorsESPBOT{0};

    struct sColorsESPOTHERS {
        float *PovOTHERS;
    };
    sColorsESPOTHERS sColorsESPOTHERS{0};

    struct sExtraMenu {
        bool StreamHide;
		bool ClearDisplay;
		bool ResetGuest;
		bool RedWallhack;
    	bool Spread;
    	bool Fire;
    	bool Diving;
        bool Recoil;
        bool Reload;
        bool Shake;
        bool Scope;
        bool Switch;
        bool FastArmorPlate;
        bool Flash;
        bool Hit;
        bool Rpd;
        bool Parachute;
        bool WallHack;
        bool Kinetic;
    };
    sExtraMenu ExtraMenu{0};

};

sConfig Config{0};
