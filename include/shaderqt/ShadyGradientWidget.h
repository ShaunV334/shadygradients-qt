#pragma once

#include <shaderqt/shaderqt_global.h>

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QElapsedTimer>
#include <QColor>
#include <QPoint>
#include <QTimer>
#include <memory>

class QMouseEvent;
class QWheelEvent;

class SHADERQT_EXPORT ShadyGradientWidget
    : public QOpenGLWidget,
      protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

    // ---- Qt properties (usable from QML or Qt Designer) ----
    Q_PROPERTY(Type type            READ type           WRITE setType           NOTIFY typeChanged)
    Q_PROPERTY(float speed          READ speed          WRITE setSpeed          NOTIFY speedChanged)
    Q_PROPERTY(float noiseDensity   READ noiseDensity   WRITE setNoiseDensity   NOTIFY noiseDensityChanged)
    Q_PROPERTY(float noiseStrength  READ noiseStrength  WRITE setNoiseStrength  NOTIFY noiseStrengthChanged)
    Q_PROPERTY(float spiral         READ spiral         WRITE setSpiral         NOTIFY spiralChanged)
    Q_PROPERTY(float pixelDensity   READ pixelDensity   WRITE setPixelDensity   NOTIFY pixelDensityChanged)
    Q_PROPERTY(QColor color1        READ color1         WRITE setColor1         NOTIFY color1Changed)
    Q_PROPERTY(QColor color2        READ color2         WRITE setColor2         NOTIFY color2Changed)
    Q_PROPERTY(QColor color3        READ color3         WRITE setColor3         NOTIFY color3Changed)
    Q_PROPERTY(float cameraDistance READ cameraDistance WRITE setCameraDistance NOTIFY cameraDistanceChanged)
    Q_PROPERTY(float cameraAzimuth  READ cameraAzimuth  WRITE setCameraAzimuth  NOTIFY cameraAzimuthChanged)
    Q_PROPERTY(float cameraPolar    READ cameraPolar    WRITE setCameraPolar    NOTIFY cameraPolarChanged)
    Q_PROPERTY(float objectPosX     READ objectPosX     WRITE setObjectPosX     NOTIFY objectPositionChanged)
    Q_PROPERTY(float objectPosY     READ objectPosY     WRITE setObjectPosY     NOTIFY objectPositionChanged)
    Q_PROPERTY(float objectPosZ     READ objectPosZ     WRITE setObjectPosZ     NOTIFY objectPositionChanged)
    Q_PROPERTY(float objectRotX     READ objectRotX     WRITE setObjectRotX     NOTIFY objectRotationChanged)
    Q_PROPERTY(float objectRotY     READ objectRotY     WRITE setObjectRotY     NOTIFY objectRotationChanged)
    Q_PROPERTY(float objectRotZ     READ objectRotZ     WRITE setObjectRotZ     NOTIFY objectRotationChanged)
    Q_PROPERTY(float fieldOfView    READ fieldOfView    WRITE setFieldOfView    NOTIFY fieldOfViewChanged)

public:
    enum class Type {
        WaterPlane,
        Sphere,
        Plane
    };
    Q_ENUM(Type)

    explicit ShadyGradientWidget(QWidget *parent = nullptr);
    ~ShadyGradientWidget() override;

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

    // Camera / object controls
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

protected:
    void initializeGL()  override;
    void resizeGL(int w, int h) override;
    void paintGL()       override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void buildMesh();
    void setupShaders();

    // GL objects
    QOpenGLShaderProgram           m_program;
    QOpenGLVertexArrayObject       m_vao;
    QOpenGLBuffer                  m_vbo;
    QOpenGLBuffer                  m_ibo;

    int   m_indexCount = 0;

    // Uniform locations (cached)
    int m_locModelView     = -1;
    int m_locProjection    = -1;
    int m_locNormalMatrix  = -1;
    int m_locTime          = -1;
    int m_locSpeed         = -1;
    int m_locLoadingTime   = -1;
    int m_locNoiseDensity  = -1;
    int m_locNoiseStrength = -1;
    int m_locSpiral        = -1;
    int m_locC1r = -1, m_locC1g = -1, m_locC1b = -1;
    int m_locC2r = -1, m_locC2g = -1, m_locC2b = -1;
    int m_locC3r = -1, m_locC3g = -1, m_locC3b = -1;

    // Animation
    QElapsedTimer m_elapsed;
    QTimer        m_timer;
    float         m_loadingTime = 0.0f; // ramp 0→1

    // Parameters
    Type   m_type          = Type::WaterPlane;
    float  m_speed         = 0.1f;
    float  m_noiseDensity  = 1.1f;
    float  m_noiseStrength = 2.4f;
    float  m_spiral        = 2.0f;
    float  m_pixelDensity  = 1.0f;
    QColor m_color1        = QColor(0x56, 0x06, 0xff);
    QColor m_color2        = QColor(0xfe, 0x89, 0x89);
    QColor m_color3        = QColor(0x00, 0x00, 0x00);

    // Interactive orbit controls for the preview widget
    bool   m_dragging        = false;
    QPoint m_lastMousePos;
    float  m_orbitYaw        = 0.0f;
    float  m_orbitPitch      = 0.0f;
    float  m_cameraDistance  = 0.0f;
    // Base camera orientation used for initial view (azimuth/polar)
    float  m_baseAzimuth     = 0.0f;
    float  m_basePolar       = 0.0f;
    float  m_baseRotationZ   = 0.0f;

    // Object transform overrides
    float  m_objectPosX      = 0.0f;
    float  m_objectPosY      = 0.0f;
    float  m_objectPosZ      = 0.0f;
    float  m_objectRotX      = 0.0f;
    float  m_objectRotY      = 0.0f;
    float  m_objectRotZ      = 0.0f;

    // Projection
    float  m_fieldOfView     = 45.0f;
};
