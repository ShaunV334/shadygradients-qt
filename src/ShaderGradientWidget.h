#pragma once

#include "shaderqt_global.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QElapsedTimer>
#include <QColor>
#include <QTimer>
#include <memory>

/**
 * @brief ShaderGradientWidget
 *
 * A QOpenGLWidget that renders the shadergradient "waterPlane" cosmic aurora
 * shader gradient. Drop it into any QWidget layout like any other widget.
 *
 * Properties can be set at construction or via setters at any time —
 * changes take effect on the next rendered frame.
 *
 * Example:
 * @code
 *   auto *sg = new ShaderGradientWidget(this);
 *   sg->setSpeed(0.3f);
 *   sg->setColor1(QColor(0, 200, 255));   // cyan
 *   sg->setColor2(QColor(180, 0, 255));   // violet
 *   sg->setColor3(QColor(0, 20, 80));     // deep blue
 *   layout->addWidget(sg);
 * @endcode
 */
class SHADERQT_EXPORT ShaderGradientWidget
    : public QOpenGLWidget,
      protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

    // ---- Qt properties (usable from QML or Qt Designer) ----
    Q_PROPERTY(float speed          READ speed          WRITE setSpeed          NOTIFY speedChanged)
    Q_PROPERTY(float noiseDensity   READ noiseDensity   WRITE setNoiseDensity   NOTIFY noiseDensityChanged)
    Q_PROPERTY(float noiseStrength  READ noiseStrength  WRITE setNoiseStrength  NOTIFY noiseStrengthChanged)
    Q_PROPERTY(QColor color1        READ color1         WRITE setColor1         NOTIFY color1Changed)
    Q_PROPERTY(QColor color2        READ color2         WRITE setColor2         NOTIFY color2Changed)
    Q_PROPERTY(QColor color3        READ color3         WRITE setColor3         NOTIFY color3Changed)

public:
    explicit ShaderGradientWidget(QWidget *parent = nullptr);
    ~ShaderGradientWidget() override;

    // ---- Getters ----
    float  speed()         const { return m_speed; }
    float  noiseDensity()  const { return m_noiseDensity; }
    float  noiseStrength() const { return m_noiseStrength; }
    QColor color1()        const { return m_color1; }
    QColor color2()        const { return m_color2; }
    QColor color3()        const { return m_color3; }

    // ---- Setters ----
    void setSpeed(float v);
    void setNoiseDensity(float v);
    void setNoiseStrength(float v);
    void setColor1(const QColor &c);
    void setColor2(const QColor &c);
    void setColor3(const QColor &c);

signals:
    void speedChanged(float v);
    void noiseDensityChanged(float v);
    void noiseStrengthChanged(float v);
    void color1Changed(const QColor &c);
    void color2Changed(const QColor &c);
    void color3Changed(const QColor &c);

protected:
    void initializeGL()  override;
    void resizeGL(int w, int h) override;
    void paintGL()       override;

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
    int m_locC1r = -1, m_locC1g = -1, m_locC1b = -1;
    int m_locC2r = -1, m_locC2g = -1, m_locC2b = -1;
    int m_locC3r = -1, m_locC3g = -1, m_locC3b = -1;

    // Animation
    QElapsedTimer m_elapsed;
    QTimer        m_timer;
    float         m_loadingTime = 0.0f; // ramp 0→1

    // Parameters
    float  m_speed         = 0.1f;
    float  m_noiseDensity  = 1.1f;
    float  m_noiseStrength = 2.4f;
    QColor m_color1        = QColor(0x56, 0x06, 0xff);
    QColor m_color2        = QColor(0xfe, 0x89, 0x89);
    QColor m_color3        = QColor(0x00, 0x00, 0x00);
};
