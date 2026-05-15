#include <shaderqt/ShadyGradientEffect.h>

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <QQuickWindow>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <vector>

// ────────────────────────────────────────────────
// Internal FBO Renderer
// ────────────────────────────────────────────────

class ShadyGradientRenderer
    : public QQuickFramebufferObject::Renderer
    , protected QOpenGLFunctions_3_3_Core
{
public:
    ShadyGradientRenderer() : m_vbo(QOpenGLBuffer::VertexBuffer), m_ibo(QOpenGLBuffer::IndexBuffer) {}
    ~ShadyGradientRenderer() override {
        m_vao.destroy();
        m_vbo.destroy();
        m_ibo.destroy();
    }

    void render() override;
    void synchronize(QQuickFramebufferObject *item) override;
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;

private:
    void init();
    void buildMesh();

    bool m_initialised = false;

    QOpenGLShaderProgram     m_program;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer            m_vbo;
    QOpenGLBuffer            m_ibo;
    int                      m_indexCount = 0;
    float                    m_pixelDensity = 1.0f;

    // Cached uniforms
    int m_locMV = -1, m_locProj = -1, m_locNM = -1;
    int m_locTime = -1, m_locSpeed = -1, m_locLoading = -1;
    int m_locND = -1, m_locNS = -1, m_locSpiral = -1;
    int m_locC1r=-1,m_locC1g=-1,m_locC1b=-1;
    int m_locC2r=-1,m_locC2g=-1,m_locC2b=-1;
    int m_locC3r=-1,m_locC3g=-1,m_locC3b=-1;

    // Sync'd state from the QQuickItem
    ShadyGradientEffect::RenderState m_state;
    QSize m_size;
};

// ────────────────────────────────────────────────
// ShadyGradientEffect
// ────────────────────────────────────────────────

ShadyGradientEffect::ShadyGradientEffect(QQuickItem *parent)
    : QQuickFramebufferObject(parent)
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
            
            // View / Camera
            if (obj.contains("cameraDistance")) m_cameraDistance = obj["cameraDistance"].toDouble();
            if (obj.contains("cameraAzimuth")) m_baseAzimuth = obj["cameraAzimuth"].toDouble();
            if (obj.contains("cameraPolar")) m_basePolar = obj["cameraPolar"].toDouble();
            if (obj.contains("objectPosX")) m_objectPosX = obj["objectPosX"].toDouble();
            if (obj.contains("objectPosY")) m_objectPosY = obj["objectPosY"].toDouble();
            if (obj.contains("objectPosZ")) m_objectPosZ = obj["objectPosZ"].toDouble();
            if (obj.contains("objectRotX")) m_objectRotX = obj["objectRotX"].toDouble();
            if (obj.contains("objectRotY")) m_objectRotY = obj["objectRotY"].toDouble();
            if (obj.contains("objectRotZ")) m_objectRotZ = obj["objectRotZ"].toDouble();
            if (obj.contains("fieldOfView")) m_fieldOfView = obj["fieldOfView"].toDouble();
        }
        presetFile.close();
    }

    m_elapsed.start();
    setMirrorVertically(true);

    // Trigger re-render each frame
    connect(this, &QQuickItem::windowChanged, this, [this](QQuickWindow *w) {
        if (w) {
            connect(w, &QQuickWindow::beforeSynchronizing, this,
                    &ShadyGradientEffect::update, Qt::DirectConnection);
        }
    });
}

QQuickFramebufferObject::Renderer *ShadyGradientEffect::createRenderer() const
{
    return new ShadyGradientRenderer();
}

ShadyGradientEffect::RenderState ShadyGradientEffect::renderState() const
{
    return {
        m_type, m_speed, m_noiseDensity, m_noiseStrength, m_spiral,
        static_cast<float>(m_elapsed.elapsed()) / 1000.0f,
        m_color1, m_color2, m_color3,
        m_cameraDistance, m_baseAzimuth, m_basePolar,
        m_objectPosX, m_objectPosY, m_objectPosZ,
        m_objectRotX, m_objectRotY, m_objectRotZ,
        m_fieldOfView
    };
}

void ShadyGradientEffect::setType(Type t)          { if (m_type == t) return; m_type = t; emit typeChanged(t); update(); }
void ShadyGradientEffect::setSpeed(float v)        { if (m_speed == v) return; m_speed = v; emit speedChanged(v); update(); }
void ShadyGradientEffect::setNoiseDensity(float v) { if (m_noiseDensity == v) return; m_noiseDensity = v; emit noiseDensityChanged(v); update(); }
void ShadyGradientEffect::setNoiseStrength(float v){ if (m_noiseStrength == v) return; m_noiseStrength = v; emit noiseStrengthChanged(v); update(); }
void ShadyGradientEffect::setSpiral(float v)       { if (m_spiral == v) return; m_spiral = v; emit spiralChanged(v); update(); }
void ShadyGradientEffect::setPixelDensity(float v) { if (m_pixelDensity == v) return; m_pixelDensity = v; emit pixelDensityChanged(v); update(); }
void ShadyGradientEffect::setColor1(const QColor &c){ if (m_color1 == c) return; m_color1 = c; emit color1Changed(c); update(); }
void ShadyGradientEffect::setColor2(const QColor &c){ if (m_color2 == c) return; m_color2 = c; emit color2Changed(c); update(); }
void ShadyGradientEffect::setColor3(const QColor &c){ if (m_color3 == c) return; m_color3 = c; emit color3Changed(c); update(); }

void ShadyGradientEffect::setCameraDistance(float v) { v = std::clamp(v, 0.5f, 60.0f); if (m_cameraDistance == v) return; m_cameraDistance = v; emit cameraDistanceChanged(v); update(); }
void ShadyGradientEffect::setCameraAzimuth(float v)  { if (m_baseAzimuth == v) return; m_baseAzimuth = v; emit cameraAzimuthChanged(v); update(); }
void ShadyGradientEffect::setCameraPolar(float v)    { if (m_basePolar == v) return; m_basePolar = v; emit cameraPolarChanged(v); update(); }

void ShadyGradientEffect::setObjectPosX(float v) { if (m_objectPosX == v) return; m_objectPosX = v; emit objectPositionChanged(); update(); }
void ShadyGradientEffect::setObjectPosY(float v) { if (m_objectPosY == v) return; m_objectPosY = v; emit objectPositionChanged(); update(); }
void ShadyGradientEffect::setObjectPosZ(float v) { if (m_objectPosZ == v) return; m_objectPosZ = v; emit objectPositionChanged(); update(); }

void ShadyGradientEffect::setObjectRotX(float v) { if (m_objectRotX == v) return; m_objectRotX = v; emit objectRotationChanged(); update(); }
void ShadyGradientEffect::setObjectRotY(float v) { if (m_objectRotY == v) return; m_objectRotY = v; emit objectRotationChanged(); update(); }
void ShadyGradientEffect::setObjectRotZ(float v) { if (m_objectRotZ == v) return; m_objectRotZ = v; emit objectRotationChanged(); update(); }

void ShadyGradientEffect::setFieldOfView(float v) { if (m_fieldOfView == v) return; m_fieldOfView = v; emit fieldOfViewChanged(v); update(); }

// ────────────────────────────────────────────────
// Renderer implementation
// ────────────────────────────────────────────────

QOpenGLFramebufferObject *ShadyGradientRenderer::createFramebufferObject(const QSize &size)
{
    m_size = size;
    QOpenGLFramebufferObjectFormat fmt;
    fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fmt.setSamples(4);
    return new QOpenGLFramebufferObject(size, fmt);
}

void ShadyGradientRenderer::synchronize(QQuickFramebufferObject *item)
{
    auto *effect = static_cast<ShadyGradientEffect *>(item);
    auto newState = effect->renderState();
    bool typeChanged = (m_state.type != newState.type);
    m_state = newState;
    if (m_pixelDensity != effect->pixelDensity() || typeChanged) {
        m_pixelDensity = effect->pixelDensity();
        if (typeChanged && m_initialised) {
            init();
        } else if (m_initialised) {
            buildMesh();
        }
    }
}

void ShadyGradientRenderer::render()
{
    if (!m_initialised) {
        init();
        m_initialised = true;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (!m_program.isLinked()) return;

    const float aspect = m_size.width() / static_cast<float>(m_size.height() ? m_size.height() : 1);

    QMatrix4x4 proj;
    proj.perspective(m_state.fieldOfView > 0 ? m_state.fieldOfView : 45.0f, aspect, 0.1f, 100.0f);

    QMatrix4x4 view;
    float baseDist = m_state.cameraDistance;
    float basePolar = m_state.cameraPolar;
    float baseAzimuth = m_state.cameraAzimuth;
    float baseRotZ = 0.0f;
    
    if (baseDist == 0.0f) {
        if (m_state.type == ShadyGradientEffect::Type::WaterPlane || m_state.type == ShadyGradientEffect::Type::Plane) {
            baseDist = 3.9f; basePolar = 115.0f; baseAzimuth = 180.0f; baseRotZ = 235.0f;
        } else {
            baseDist = 12.5f; basePolar = 140.0f; baseAzimuth = 250.0f; baseRotZ = 140.0f;
        }
    } else {
        if (m_state.type == ShadyGradientEffect::Type::WaterPlane || m_state.type == ShadyGradientEffect::Type::Plane) {
            baseRotZ = 235.0f;
        } else {
            baseRotZ = 140.0f;
        }
    }

    view.lookAt({0.0f, 0.0f, baseDist}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

    QMatrix4x4 model;
    model.rotate(basePolar - 90.0f, 1.0f, 0.0f, 0.0f);
    model.rotate(baseAzimuth, 0.0f, 1.0f, 0.0f);
    model.rotate(baseRotZ, 0.0f, 0.0f, 1.0f);
    
    model.rotate(m_state.objectRotX, 1.0f, 0.0f, 0.0f);
    model.rotate(m_state.objectRotY, 0.0f, 1.0f, 0.0f);
    model.rotate(m_state.objectRotZ, 0.0f, 0.0f, 1.0f);
    model.translate(m_state.objectPosX, m_state.objectPosY, m_state.objectPosZ);

    const QMatrix4x4 mv = view * model;
    const QMatrix3x3 nm = mv.normalMatrix();

    m_program.bind();
    m_program.setUniformValue(m_locMV,   mv);
    m_program.setUniformValue(m_locProj, proj);
    m_program.setUniformValue(m_locNM,   nm);
    m_program.setUniformValue(m_locTime,    m_state.time);
    m_program.setUniformValue(m_locSpeed,   m_state.speed);
    m_program.setUniformValue(m_locLoading, 1.0f);
    m_program.setUniformValue(m_locND,      m_state.noiseDensity);
    m_program.setUniformValue(m_locNS,      m_state.noiseStrength);
    m_program.setUniformValue(m_locSpiral,  m_state.spiral);

    auto set3 = [&](int r, int g, int b, const QColor &c) {
        m_program.setUniformValue(r, static_cast<float>(c.redF()));
        m_program.setUniformValue(g, static_cast<float>(c.greenF()));
        m_program.setUniformValue(b, static_cast<float>(c.blueF()));
    };
    set3(m_locC1r, m_locC1g, m_locC1b, m_state.color1);
    set3(m_locC2r, m_locC2g, m_locC2b, m_state.color2);
    set3(m_locC3r, m_locC3g, m_locC3b, m_state.color3);

    m_vao.bind();
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    m_vao.release();
    m_program.release();

    // Tell Qt Quick to keep rendering (animation)
    update();
}

void ShadyGradientRenderer::init()
{
    initializeOpenGLFunctions();

    m_program.removeAllShaders();

    if (m_state.type == ShadyGradientEffect::Type::WaterPlane) {
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/waterplane.vert"))
            qWarning() << "SGEffect vertex:" << m_program.log();
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/waterplane.frag"))
            qWarning() << "SGEffect fragment:" << m_program.log();
    } else if (m_state.type == ShadyGradientEffect::Type::Plane) {
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/plane.vert"))
            qWarning() << "SGEffect vertex:" << m_program.log();
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/plane.frag"))
            qWarning() << "SGEffect fragment:" << m_program.log();
    } else {
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/sphere.vert"))
            qWarning() << "SGEffect vertex:" << m_program.log();
        if (!m_program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/sphere.frag"))
            qWarning() << "SGEffect fragment:" << m_program.log();
    }

    if (!m_program.link())
        qWarning() << "SGEffect link:" << m_program.log();

    m_locMV   = m_program.uniformLocation("u_modelViewMatrix");
    m_locProj = m_program.uniformLocation("u_projectionMatrix");
    m_locNM   = m_program.uniformLocation("u_normalMatrix");
    m_locTime = m_program.uniformLocation("uTime");
    m_locSpeed= m_program.uniformLocation("uSpeed");
    m_locLoading = m_program.uniformLocation("uLoadingTime");
    m_locND   = m_program.uniformLocation("uNoiseDensity");
    m_locNS   = m_program.uniformLocation("uNoiseStrength");
    m_locSpiral = m_program.uniformLocation("uSpiral");
    m_locC1r  = m_program.uniformLocation("uC1r");
    m_locC1g  = m_program.uniformLocation("uC1g");
    m_locC1b  = m_program.uniformLocation("uC1b");
    m_locC2r  = m_program.uniformLocation("uC2r");
    m_locC2g  = m_program.uniformLocation("uC2g");
    m_locC2b  = m_program.uniformLocation("uC2b");
    m_locC3r  = m_program.uniformLocation("uC3r");
    m_locC3g  = m_program.uniformLocation("uC3g");
    m_locC3b  = m_program.uniformLocation("uC3b");

    buildMesh();
}

void ShadyGradientRenderer::buildMesh()
{
    const int   segsX = std::max(1, static_cast<int>(64 * m_pixelDensity));
    const int   segsY = std::max(1, static_cast<int>(64 * m_pixelDensity));
    const int   vertsX = segsX + 1, vertsY = segsY + 1;

    std::vector<float> vdata;
    vdata.reserve(vertsX * vertsY * 8);

    if (m_state.type == ShadyGradientEffect::Type::WaterPlane || m_state.type == ShadyGradientEffect::Type::Plane) {
        const float sizeX = 10.0f, sizeY = 10.0f;
        for (int iy = 0; iy < vertsY; ++iy) {
            float v = static_cast<float>(iy) / segsY;
            float y = (v - 0.5f) * sizeY;
            for (int ix = 0; ix < vertsX; ++ix) {
                float u = static_cast<float>(ix) / segsX;
                float x = (u - 0.5f) * sizeX;
                vdata.insert(vdata.end(), {x, y, 0.0f, 0.0f, 0.0f, 1.0f, u, v});
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

                vdata.insert(vdata.end(), {x, y, z, x/radius, y/radius, z/radius, u, v});
            }
        }
    }

    std::vector<unsigned int> idata;
    idata.reserve(segsX * segsY * 6);
    for (int iy = 0; iy < segsY; ++iy) {
        for (int ix = 0; ix < segsX; ++ix) {
            unsigned int a = iy * vertsX + ix;
            unsigned int b = a + vertsX;
            unsigned int c = b + 1, d = a + 1;
            idata.insert(idata.end(), {a, b, d, b, c, d});
        }
    }
    m_indexCount = static_cast<int>(idata.size());

    if (m_vao.isCreated()) m_vao.destroy();
    if (m_vbo.isCreated()) m_vbo.destroy();
    if (m_ibo.isCreated()) m_ibo.destroy();

    m_vao.create(); m_vao.bind();
    m_vbo.create(); m_vbo.bind();
    m_vbo.allocate(vdata.data(), static_cast<int>(vdata.size() * sizeof(float)));
    m_ibo.create(); m_ibo.bind();
    m_ibo.allocate(idata.data(), static_cast<int>(idata.size() * sizeof(unsigned int)));

    const int stride = 8 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));

    m_vao.release();
    m_vbo.release();
}
