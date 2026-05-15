#pragma once

#include <shaderqt/shaderqt_global.h>

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QElapsedTimer>
#include <QColor>

/**
 * @brief ShadyGradientEffect
 *
 * A QQuickFramebufferObject that renders the shadergradient "waterPlane"
 * cosmic aurora shader for use inside QML.
 *
 * QML usage:
 * @code
 *   import ShaderQt 1.0
 *
 *   ShadyGradientEffect {
 *       anchors.fill: parent
 *       speed: 0.3
 *       noiseDensity: 1.5
 *       noiseStrength: 0.8
 *       color1: "#00C8FF"
 *       color2: "#B400FF"
 *       color3: "#001450"
 *   }
 * @endcode
 */
class SHADERQT_EXPORT ShadyGradientEffect : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(Type   type           READ type           WRITE setType           NOTIFY typeChanged)
    Q_PROPERTY(float  speed          READ speed          WRITE setSpeed          NOTIFY speedChanged)
    Q_PROPERTY(float  noiseDensity   READ noiseDensity   WRITE setNoiseDensity   NOTIFY noiseDensityChanged)
    Q_PROPERTY(float  noiseStrength  READ noiseStrength  WRITE setNoiseStrength  NOTIFY noiseStrengthChanged)
    Q_PROPERTY(float  spiral         READ spiral         WRITE setSpiral         NOTIFY spiralChanged)
    Q_PROPERTY(float  pixelDensity   READ pixelDensity   WRITE setPixelDensity   NOTIFY pixelDensityChanged)
    Q_PROPERTY(QColor color1         READ color1         WRITE setColor1         NOTIFY color1Changed)
    Q_PROPERTY(QColor color2         READ color2         WRITE setColor2         NOTIFY color2Changed)
    Q_PROPERTY(QColor color3         READ color3         WRITE setColor3         NOTIFY color3Changed)
    Q_PROPERTY(float cameraDistance  READ cameraDistance WRITE setCameraDistance NOTIFY cameraDistanceChanged)
    Q_PROPERTY(float cameraAzimuth   READ cameraAzimuth  WRITE setCameraAzimuth  NOTIFY cameraAzimuthChanged)
    Q_PROPERTY(float cameraPolar     READ cameraPolar    WRITE setCameraPolar    NOTIFY cameraPolarChanged)
    Q_PROPERTY(float objectPosX      READ objectPosX     WRITE setObjectPosX     NOTIFY objectPositionChanged)
    Q_PROPERTY(float objectPosY      READ objectPosY     WRITE setObjectPosY     NOTIFY objectPositionChanged)
    Q_PROPERTY(float objectPosZ      READ objectPosZ     WRITE setObjectPosZ     NOTIFY objectPositionChanged)
    Q_PROPERTY(float objectRotX      READ objectRotX     WRITE setObjectRotX     NOTIFY objectRotationChanged)
    Q_PROPERTY(float objectRotY      READ objectRotY     WRITE setObjectRotY     NOTIFY objectRotationChanged)
    Q_PROPERTY(float objectRotZ      READ objectRotZ     WRITE setObjectRotZ     NOTIFY objectRotationChanged)
    Q_PROPERTY(float fieldOfView     READ fieldOfView    WRITE setFieldOfView    NOTIFY fieldOfViewChanged)
public:
    enum class Type {
        WaterPlane,
        Sphere,
        Plane
    };
    Q_ENUM(Type)

    explicit ShadyGradientEffect(QQuickItem *parent = nullptr);

    // QQuickFramebufferObject interface
    Renderer *createRenderer() const override;

    // ---- Getters ----
    Type   type()          const { return m_type; }
    float  speed()         const { return m_speed; }
    float  noiseDensity()  const { return m_noiseDensity; }
    float  noiseStrength() const { return m_noiseStrength; }
    float  spiral()        const { return m_spiral; }
    float  pixelDensity()  const { return m_pixelDensity; }
    QColor color1()        const { return m_color1; }
    QColor color2()        const { return m_color2; }
    QColor color3()        const { return m_color3; }
    
    float  cameraDistance() const { return m_cameraDistance; }
    float  cameraAzimuth()  const { return m_baseAzimuth; }
    float  cameraPolar()    const { return m_basePolar; }
    float  objectPosX()     const { return m_objectPosX; }
    float  objectPosY()     const { return m_objectPosY; }
    float  objectPosZ()     const { return m_objectPosZ; }
    float  objectRotX()     const { return m_objectRotX; }
    float  objectRotY()     const { return m_objectRotY; }
    float  objectRotZ()     const { return m_objectRotZ; }
    float  fieldOfView()    const { return m_fieldOfView; }
    // snapshot of state for the render thread
    struct RenderState {
        Type   type;
        float  speed, noiseDensity, noiseStrength, spiral;
        float  time;
        QColor color1, color2, color3;
        float cameraDistance, cameraAzimuth, cameraPolar;
        float objectPosX, objectPosY, objectPosZ;
        float objectRotX, objectRotY, objectRotZ;
        float fieldOfView;
    };
    RenderState renderState() const;

    // ---- Setters ----
    void setType(Type t);
    void setSpeed(float v);
    void setNoiseDensity(float v);
    void setNoiseStrength(float v);
    void setSpiral(float v);
    void setPixelDensity(float v);
    void setColor1(const QColor &c);
    void setColor2(const QColor &c);
    void setColor3(const QColor &c);
    
    void setCameraDistance(float v);
    void setCameraAzimuth(float v);
    void setCameraPolar(float v);
    void setObjectPosX(float v);
    void setObjectPosY(float v);
    void setObjectPosZ(float v);
    void setObjectRotX(float v);
    void setObjectRotY(float v);
    void setObjectRotZ(float v);
    void setFieldOfView(float v);

signals:
    void typeChanged(Type t);
    void speedChanged(float v);
    void noiseDensityChanged(float v);
    void noiseStrengthChanged(float v);
    void spiralChanged(float v);
    void pixelDensityChanged(float v);
    void color1Changed(const QColor &c);
    void color2Changed(const QColor &c);
    void color3Changed(const QColor &c);
    void cameraDistanceChanged(float v);
    void cameraAzimuthChanged(float v);
    void cameraPolarChanged(float v);
    void objectPositionChanged();
    void objectRotationChanged();
    void fieldOfViewChanged(float v);

private:
    Type   m_type          = Type::WaterPlane;
    float  m_speed         = 0.1f;
    float  m_noiseDensity  = 1.1f;
    float  m_noiseStrength = 2.4f;
    float  m_spiral        = 2.0f;
    float  m_pixelDensity  = 1.0f;
    QColor m_color1        = QColor(0x56, 0x06, 0xff);
    QColor m_color2        = QColor(0xfe, 0x89, 0x89);
    QColor m_color3        = QColor(0x00, 0x00, 0x00);
    
    float  m_cameraDistance  = 3.9f;
    float  m_baseAzimuth     = 0.0f;
    float  m_basePolar       = 0.0f;
    float  m_objectPosX      = 0.0f;
    float  m_objectPosY      = 0.0f;
    float  m_objectPosZ      = 0.0f;
    float  m_objectRotX      = 0.0f;
    float  m_objectRotY      = 0.0f;
    float  m_objectRotZ      = 0.0f;
    float  m_fieldOfView     = 45.0f;

    QElapsedTimer m_elapsed;
};
