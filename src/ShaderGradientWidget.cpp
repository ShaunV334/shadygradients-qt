#include "ShaderGradientWidget.h"

#include <QOpenGLContext>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QDebug>
#include <cmath>
#include <vector>

// ────────────────────────────────────────────────
// Constructor / Destructor
// ────────────────────────────────────────────────

ShaderGradientWidget::ShaderGradientWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_vbo(QOpenGLBuffer::VertexBuffer)
    , m_ibo(QOpenGLBuffer::IndexBuffer)
{
    // Request OpenGL 3.3 Core
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);   // MSAA x4
    setFormat(fmt);

    // Drive animation at ~60 fps
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        // Ramp up loadingTime from 0 → 1 over ~1.5 s
        if (m_loadingTime < 1.0f)
            m_loadingTime = qMin(1.0f, static_cast<float>(m_elapsed.elapsed()) / 1500.0f);
        update();
    });
    m_elapsed.start();
    m_timer.start(16); // ~62.5 fps
}

ShaderGradientWidget::~ShaderGradientWidget()
{
    makeCurrent();
    m_vao.destroy();
    m_vbo.destroy();
    m_ibo.destroy();
    doneCurrent();
}

// ────────────────────────────────────────────────
// Property setters
// ────────────────────────────────────────────────

void ShaderGradientWidget::setSpeed(float v)        { if (m_speed == v) return; m_speed = v; emit speedChanged(v); }
void ShaderGradientWidget::setNoiseDensity(float v) { if (m_noiseDensity == v) return; m_noiseDensity = v; emit noiseDensityChanged(v); }
void ShaderGradientWidget::setNoiseStrength(float v){ if (m_noiseStrength == v) return; m_noiseStrength = v; emit noiseStrengthChanged(v); }
void ShaderGradientWidget::setColor1(const QColor &c){ if (m_color1 == c) return; m_color1 = c; emit color1Changed(c); }
void ShaderGradientWidget::setColor2(const QColor &c){ if (m_color2 == c) return; m_color2 = c; emit color2Changed(c); }
void ShaderGradientWidget::setColor3(const QColor &c){ if (m_color3 == c) return; m_color3 = c; emit color3Changed(c); }

// ────────────────────────────────────────────────
// GL Initialisation
// ────────────────────────────────────────────────

void ShaderGradientWidget::initializeGL()
{
    if (!initializeOpenGLFunctions()) {
        qFatal("Failed to initialize OpenGL 3.3 Core functions");
        return;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    setupShaders();
    buildMesh();
}

void ShaderGradientWidget::setupShaders()
{
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/waterplane.vert")) {
        qWarning() << "Vertex shader compile error:" << m_program.log();
    }
    if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/waterplane.frag")) {
        qWarning() << "Fragment shader compile error:" << m_program.log();
    }
    if (!m_program.link()) {
        qWarning() << "Shader link error:" << m_program.log();
    }

    // Cache uniform locations
    m_locModelView     = m_program.uniformLocation("u_modelViewMatrix");
    m_locProjection    = m_program.uniformLocation("u_projectionMatrix");
    m_locNormalMatrix  = m_program.uniformLocation("u_normalMatrix");
    m_locTime          = m_program.uniformLocation("uTime");
    m_locSpeed         = m_program.uniformLocation("uSpeed");
    m_locLoadingTime   = m_program.uniformLocation("uLoadingTime");
    m_locNoiseDensity  = m_program.uniformLocation("uNoiseDensity");
    m_locNoiseStrength = m_program.uniformLocation("uNoiseStrength");
    m_locC1r = m_program.uniformLocation("uC1r");
    m_locC1g = m_program.uniformLocation("uC1g");
    m_locC1b = m_program.uniformLocation("uC1b");
    m_locC2r = m_program.uniformLocation("uC2r");
    m_locC2g = m_program.uniformLocation("uC2g");
    m_locC2b = m_program.uniformLocation("uC2b");
    m_locC3r = m_program.uniformLocation("uC3r");
    m_locC3g = m_program.uniformLocation("uC3g");
    m_locC3b = m_program.uniformLocation("uC3b");
}

// ────────────────────────────────────────────────
// Mesh Generation — Subdivided Plane
//   Matches Three.js PlaneGeometry(10, 10, 64, 64)
// ────────────────────────────────────────────────

void ShaderGradientWidget::buildMesh()
{
    const int   segsX = 64, segsY = 64;
    const float sizeX = 10.0f, sizeY = 10.0f;
    const int   vertsX = segsX + 1, vertsY = segsY + 1;

    // Interleaved: position(3) + normal(3) + uv(2) = 8 floats per vertex
    std::vector<float> vdata;
    vdata.reserve(vertsX * vertsY * 8);

    for (int iy = 0; iy < vertsY; ++iy) {
        float v = static_cast<float>(iy) / segsY;
        float y = (v - 0.5f) * sizeY;

        for (int ix = 0; ix < vertsX; ++ix) {
            float u = static_cast<float>(ix) / segsX;
            float x = (u - 0.5f) * sizeX;

            // position
            vdata.push_back(x);
            vdata.push_back(y);
            vdata.push_back(0.0f);
            // normal (flat plane points +Z)
            vdata.push_back(0.0f);
            vdata.push_back(0.0f);
            vdata.push_back(1.0f);
            // uv
            vdata.push_back(u);
            vdata.push_back(v);
        }
    }

    std::vector<unsigned int> idata;
    idata.reserve(segsX * segsY * 6);

    for (int iy = 0; iy < segsY; ++iy) {
        for (int ix = 0; ix < segsX; ++ix) {
            unsigned int a = iy * vertsX + ix;
            unsigned int b = a + vertsX;
            unsigned int c = b + 1;
            unsigned int d = a + 1;
            idata.push_back(a); idata.push_back(b); idata.push_back(d);
            idata.push_back(b); idata.push_back(c); idata.push_back(d);
        }
    }
    m_indexCount = static_cast<int>(idata.size());

    // Upload to GPU
    m_vao.create();
    m_vao.bind();

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vdata.data(), static_cast<int>(vdata.size() * sizeof(float)));

    m_ibo.create();
    m_ibo.bind();
    m_ibo.allocate(idata.data(), static_cast<int>(idata.size() * sizeof(unsigned int)));

    const int stride = 8 * sizeof(float);
    // location 0: position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    // location 1: normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    // location 2: uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));

    m_vao.release();
    m_vbo.release();
    // Note: IBO stays bound to VAO — do NOT release IBO while VAO is bound
}

// ────────────────────────────────────────────────
// Resize
// ────────────────────────────────────────────────

void ShaderGradientWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

// ────────────────────────────────────────────────
// Paint
// ────────────────────────────────────────────────

void ShaderGradientWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_program.isLinked())
        return;

    // ---- Camera / Transform ----
    const float aspect = static_cast<float>(width()) / static_cast<float>(height() ? height() : 1);

    QMatrix4x4 proj;
    proj.perspective(45.0f, aspect, 0.1f, 100.0f);

    QMatrix4x4 view;
    // Distance = 3.9, polar angle = 115 deg, azimuth angle = 180 deg
    // We can simulate the camera orbit by keeping lookAt simple and rotating the model.
    view.lookAt({0.0f, 0.0f, 3.9f},  // eye
                {0.0f, 0.0f, 0.0f},  // center
                {0.0f, 1.0f, 0.0f}); // up

    QMatrix4x4 model;
    model.rotate(115.0f - 90.0f, 1.0f, 0.0f, 0.0f); // cPolarAngle
    model.rotate(180.0f, 0.0f, 1.0f, 0.0f);         // cAzimuthAngle
    model.rotate(235.0f, 0.0f, 0.0f, 1.0f);         // rotationZ

    const QMatrix4x4 mv = view * model;
    const QMatrix3x3 normalMat = mv.normalMatrix();

    // ---- Set uniforms ----
    m_program.bind();

    m_program.setUniformValue(m_locModelView,    mv);
    m_program.setUniformValue(m_locProjection,   proj);
    m_program.setUniformValue(m_locNormalMatrix, normalMat);

    const float t = static_cast<float>(m_elapsed.elapsed()) / 1000.0f;
    m_program.setUniformValue(m_locTime,          t);
    m_program.setUniformValue(m_locSpeed,         m_speed);
    m_program.setUniformValue(m_locLoadingTime,   m_loadingTime);
    m_program.setUniformValue(m_locNoiseDensity,  m_noiseDensity);
    m_program.setUniformValue(m_locNoiseStrength, m_noiseStrength);

    // Colors — normalise QColor (0–255) → float (0.0–1.0)
    auto set3 = [&](int r, int g, int b, const QColor &c) {
        m_program.setUniformValue(r, static_cast<float>(c.redF()));
        m_program.setUniformValue(g, static_cast<float>(c.greenF()));
        m_program.setUniformValue(b, static_cast<float>(c.blueF()));
    };
    set3(m_locC1r, m_locC1g, m_locC1b, m_color1);
    set3(m_locC2r, m_locC2g, m_locC2b, m_color2);
    set3(m_locC3r, m_locC3g, m_locC3b, m_color3);

    // ---- Draw ----
    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    m_vao.release();

    m_program.release();
}
