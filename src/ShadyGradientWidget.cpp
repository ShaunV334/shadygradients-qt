#include <shaderqt/ShadyGradientWidget.h>

#include <QOpenGLContext>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <cmath>
#include <vector>

// ────────────────────────────────────────────────
// Constructor / Destructor
// ────────────────────────────────────────────────

ShadyGradientWidget::ShadyGradientWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_vbo(QOpenGLBuffer::VertexBuffer)
    , m_ibo(QOpenGLBuffer::IndexBuffer)
{
    // Load default preset if it exists
    QFile presetFile;
    // 1. Prioritize reading from the local directory (allows rapid previewing / tweaking)
    if (QFile::exists(QDir::currentPath() + "/shady_gradient_preset.json")) {
        presetFile.setFileName(QDir::currentPath() + "/shady_gradient_preset.json");
    } 
    // 2. Fall back to the built-in default library preset if no local config exists
    else {
        presetFile.setFileName(":/shaders/shady_gradient_preset.json");
    }

    if (presetFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonDocument doc = QJsonDocument::fromJson(presetFile.readAll());
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("type")) {
                QString tStr = obj["type"].toString();
                if (tStr == "WaterPlane") m_type = Type::WaterPlane;
                else if (tStr == "Plane") m_type = Type::Plane;
                else m_type = Type::Sphere;
            }
            if (obj.contains("speed")) m_speed = obj["speed"].toDouble();
            if (obj.contains("noiseDensity")) m_noiseDensity = obj["noiseDensity"].toDouble();
            if (obj.contains("noiseStrength")) m_noiseStrength = obj["noiseStrength"].toDouble();
            if (obj.contains("spiral")) m_spiral = obj["spiral"].toDouble();
            if (obj.contains("pixelDensity")) m_pixelDensity = obj["pixelDensity"].toDouble();
            if (obj.contains("color1")) m_color1 = QColor(obj["color1"].toString());
            if (obj.contains("color2")) m_color2 = QColor(obj["color2"].toString());
            if (obj.contains("color3")) m_color3 = QColor(obj["color3"].toString());
        }
        presetFile.close();
    }

    // Request OpenGL 3.3 Core
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);   // MSAA x4
    setFormat(fmt);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

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

ShadyGradientWidget::~ShadyGradientWidget()
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

void ShadyGradientWidget::setType(Type t) {
    if (m_type == t) return;
    m_type = t;
    emit typeChanged(t);
    makeCurrent();
    setupShaders();
    buildMesh();
    update();
    doneCurrent();
}

void ShadyGradientWidget::setSpeed(float v)        { if (m_speed == v) return; m_speed = v; emit speedChanged(v); }
void ShadyGradientWidget::setNoiseDensity(float v) { if (m_noiseDensity == v) return; m_noiseDensity = v; emit noiseDensityChanged(v); update(); }
void ShadyGradientWidget::setNoiseStrength(float v){ if (m_noiseStrength == v) return; m_noiseStrength = v; emit noiseStrengthChanged(v); update(); }
void ShadyGradientWidget::setSpiral(float v)       { if (m_spiral == v) return; m_spiral = v; emit spiralChanged(v); update(); }
void ShadyGradientWidget::setPixelDensity(float v) {
    if (m_pixelDensity == v) return;
    m_pixelDensity = v;
    emit pixelDensityChanged(v);
    makeCurrent();
    buildMesh();
    doneCurrent();
}
void ShadyGradientWidget::setColor1(const QColor &c){ if (m_color1 == c) return; m_color1 = c; emit color1Changed(c); }
void ShadyGradientWidget::setColor2(const QColor &c){ if (m_color2 == c) return; m_color2 = c; emit color2Changed(c); }
void ShadyGradientWidget::setColor3(const QColor &c){ if (m_color3 == c) return; m_color3 = c; emit color3Changed(c); }

// ────────────────────────────────────────────────
// GL Initialisation
// ────────────────────────────────────────────────

void ShadyGradientWidget::initializeGL()
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

void ShadyGradientWidget::setupShaders()
{
    m_program.removeAllShaders();

    if (m_type == Type::WaterPlane) {
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/waterplane.vert")) {
            qWarning() << "Vertex shader compile error:" << m_program.log();
        }
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/waterplane.frag")) {
            qWarning() << "Fragment shader compile error:" << m_program.log();
        }
    } else if (m_type == Type::Plane) {
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/plane.vert")) {
            qWarning() << "Vertex shader compile error:" << m_program.log();
        }
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/plane.frag")) {
            qWarning() << "Fragment shader compile error:" << m_program.log();
        }
    } else {
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/sphere.vert")) {
            qWarning() << "Vertex shader compile error:" << m_program.log();
        }
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/sphere.frag")) {
            qWarning() << "Fragment shader compile error:" << m_program.log();
        }
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
    m_locSpiral        = m_program.uniformLocation("uSpiral");
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

void ShadyGradientWidget::buildMesh()
{
    const int segsX = std::max(1, static_cast<int>(64 * m_pixelDensity));
    const int segsY = std::max(1, static_cast<int>(64 * m_pixelDensity));
    const int vertsX = segsX + 1, vertsY = segsY + 1;

    std::vector<float> vdata;
    vdata.reserve(vertsX * vertsY * 8);

    if (m_type == Type::WaterPlane || m_type == Type::Plane) {
        const float sizeX = 10.0f, sizeY = 10.0f;
        for (int iy = 0; iy < vertsY; ++iy) {
            float v = static_cast<float>(iy) / segsY;
            float y = (v - 0.5f) * sizeY;

            for (int ix = 0; ix < vertsX; ++ix) {
                float u = static_cast<float>(ix) / segsX;
                float x = (u - 0.5f) * sizeX;

                vdata.push_back(x); vdata.push_back(y); vdata.push_back(0.0f);
                vdata.push_back(0.0f); vdata.push_back(0.0f); vdata.push_back(1.0f);
                vdata.push_back(u); vdata.push_back(v);
            }
        }
    } else {
        const float radius = 5.0f;
        for (int iy = 0; iy <= segsY; ++iy) {
            float v = static_cast<float>(iy) / segsY;
            float theta = v * M_PI;
            for (int ix = 0; ix <= segsX; ++ix) {
                float u = static_cast<float>(ix) / segsX;
                float phi = u * 2.0f * M_PI;

                float x = -radius * std::cos(phi) * std::sin(theta);
                float y = radius * std::cos(theta);
                float z = radius * std::sin(phi) * std::sin(theta);

                vdata.push_back(x); vdata.push_back(y); vdata.push_back(z);
                float nx = x / radius, ny = y / radius, nz = z / radius;
                vdata.push_back(nx); vdata.push_back(ny); vdata.push_back(nz);
                vdata.push_back(u); vdata.push_back(v);
            }
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
    if (m_vao.isCreated()) m_vao.destroy();
    if (m_vbo.isCreated()) m_vbo.destroy();
    if (m_ibo.isCreated()) m_ibo.destroy();

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

void ShadyGradientWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void ShadyGradientWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    QOpenGLWidget::mousePressEvent(event);
}

void ShadyGradientWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        const QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        m_orbitYaw += static_cast<float>(delta.x()) * 0.5f;
        m_orbitPitch = std::clamp(m_orbitPitch + static_cast<float>(delta.y()) * 0.5f, -80.0f, 80.0f);

        update();
        event->accept();
        return;
    }

    QOpenGLWidget::mouseMoveEvent(event);
}

void ShadyGradientWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
        unsetCursor();
        event->accept();
        return;
    }

    QOpenGLWidget::mouseReleaseEvent(event);
}

void ShadyGradientWidget::wheelEvent(QWheelEvent *event)
{
    const float steps = event->angleDelta().y() / 120.0f;
    if (steps != 0.0f) {
        m_cameraDistance = std::clamp(m_cameraDistance - steps * 0.6f, -8.0f, 16.0f);
        update();
        event->accept();
        return;
    }

    QOpenGLWidget::wheelEvent(event);
}

// ────────────────────────────────────────────────
// Paint
// ────────────────────────────────────────────────

void ShadyGradientWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_program.isLinked())
        return;

    // ---- Camera / Transform ----
    const float aspect = static_cast<float>(width()) / static_cast<float>(height() ? height() : 1);

    QMatrix4x4 proj;
    proj.perspective(45.0f, aspect, 0.1f, 100.0f);

    QMatrix4x4 view;
    const float baseDistance = (m_type == Type::WaterPlane || m_type == Type::Plane) ? 3.9f : 12.5f;
    const float cameraDistance = std::max(0.5f, baseDistance + m_cameraDistance);
    if (m_type == Type::WaterPlane || m_type == Type::Plane) {
        view.lookAt({0.0f, 0.0f, cameraDistance}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    } else {
        view.lookAt({0.0f, 0.0f, cameraDistance}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    }

    QMatrix4x4 baseModel;
    if (m_type == Type::WaterPlane || m_type == Type::Plane) {
        baseModel.rotate(115.0f - 90.0f, 1.0f, 0.0f, 0.0f); // cPolarAngle
        baseModel.rotate(180.0f, 0.0f, 1.0f, 0.0f);         // cAzimuthAngle
        baseModel.rotate(235.0f, 0.0f, 0.0f, 1.0f);         // rotationZ
    } else {
        baseModel.rotate(140.0f - 90.0f, 1.0f, 0.0f, 0.0f); // cPolarAngle=140
        baseModel.rotate(250.0f, 0.0f, 1.0f, 0.0f);         // cAzimuthAngle=250
        baseModel.rotate(140.0f, 0.0f, 0.0f, 1.0f);         // rotationZ=140
    }

    QMatrix4x4 orbit;
    orbit.rotate(m_orbitYaw, 0.0f, 1.0f, 0.0f);
    orbit.rotate(m_orbitPitch, 1.0f, 0.0f, 0.0f);

    const QMatrix4x4 mv = view * orbit * baseModel;
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
    m_program.setUniformValue(m_locSpiral,        m_spiral);

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
