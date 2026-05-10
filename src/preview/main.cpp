/**
 * shaderqt-preview — standalone preview application for ShaderGradientWidget
 *
 * Displays the waterPlane cosmic aurora shader with a side panel of controls
 * for all shader uniforms (speed, noise density, noise strength, 3 gradient colors).
 */

#include "../ShaderGradientWidget.h"

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QSurfaceFormat>
#include <QFont>
#include <QFontDatabase>

// ---- Helpers ----

static QSlider *makeSlider(int min, int max, int val, Qt::Orientation o = Qt::Horizontal) {
    auto *s = new QSlider(o);
    s->setRange(min, max);
    s->setValue(val);
    return s;
}

static QString colorButtonStyle(const QColor &c) {
    return QString("QPushButton { background-color: %1; border: 2px solid #444; "
                   "border-radius: 6px; min-height: 32px; }"
                   "QPushButton:hover { border-color: #aaa; }").arg(c.name());
}

// ────────────────────────────────────────────────
// Main window
// ────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    // Request OpenGL 3.3 Core globally
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setDepthBufferSize(24);
    fmt.setSamples(4);
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);
    app.setApplicationName("ShaderGradient Qt Preview");
    app.setOrganizationName("shaderqt");

    // Dark style sheet
    app.setStyleSheet(R"(
        QMainWindow, QWidget {
            background-color: #0d0d14;
            color: #e0e0f0;
            font-family: 'Inter', 'Segoe UI', sans-serif;
            font-size: 13px;
        }
        QGroupBox {
            border: 1px solid #2a2a40;
            border-radius: 8px;
            margin-top: 12px;
            padding: 8px;
            font-weight: bold;
            color: #a0a0c0;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px;
        }
        QSlider::groove:horizontal {
            height: 4px;
            background: #2a2a44;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #7c5af0;
            border: none;
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border-radius: 7px;
        }
        QSlider::sub-page:horizontal {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #4040c0, stop:1 #a050ff);
            border-radius: 2px;
        }
        QLabel { color: #c0c0e0; }
        QLabel#valueLabel { color: #7c5af0; font-weight: bold; min-width: 40px; }
        QPushButton#colorBtn {
            border-radius: 6px;
            min-height: 32px;
            border: 2px solid #333;
        }
        QLabel#titleLabel {
            font-size: 18px;
            font-weight: bold;
            color: #ffffff;
            padding: 8px 0;
        }
        QLabel#subtitleLabel {
            font-size: 11px;
            color: #6060a0;
            padding-bottom: 12px;
        }
    )");

    // ---- Window layout ----
    QMainWindow win;
    win.setWindowTitle("ShaderGradient Qt — waterPlane Preview");
    win.resize(1200, 720);

    QWidget *central = new QWidget;
    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    win.setCentralWidget(central);

    // ---- Shader widget (left, fills most of window) ----
    auto *sg = new ShaderGradientWidget;
    sg->setMinimumSize(600, 400);
    root->addWidget(sg, 1);

    // ---- Control panel (right side) ----
    QWidget *panel = new QWidget;
    panel->setFixedWidth(260);
    panel->setStyleSheet("QWidget { background-color: #0a0a12; border-left: 1px solid #1a1a2e; }");
    QVBoxLayout *pLayout = new QVBoxLayout(panel);
    pLayout->setContentsMargins(16, 16, 16, 16);
    pLayout->setSpacing(8);

    // Title
    auto *titleLbl = new QLabel("ShaderGradient");
    titleLbl->setObjectName("titleLabel");
    auto *subtitleLbl = new QLabel("waterPlane · Cosmic Aurora");
    subtitleLbl->setObjectName("subtitleLabel");
    pLayout->addWidget(titleLbl);
    pLayout->addWidget(subtitleLbl);

    // ---- Helper: labelled slider ----
    auto addSlider = [&](QVBoxLayout *parent, const QString &label,
                         int min, int max, int val,
                         std::function<void(int)> onChange) {
        auto *row = new QHBoxLayout;
        auto *lbl = new QLabel(label);
        auto *valLbl = new QLabel(QString::number(val));
        valLbl->setObjectName("valueLabel");
        valLbl->setFixedWidth(36);
        row->addWidget(lbl);
        row->addStretch();
        row->addWidget(valLbl);
        parent->addLayout(row);

        auto *slider = makeSlider(min, max, val);
        parent->addWidget(slider);

        QObject::connect(slider, &QSlider::valueChanged, [=](int v) {
            valLbl->setText(QString::number(v));
            onChange(v);
        });
    };

    // ---- Animation group ----
    {
        auto *grp = new QGroupBox("Animation");
        auto *gl  = new QVBoxLayout(grp);

        // Speed  — 0..100 → 0.0..2.0
        addSlider(gl, "Speed", 0, 100, 30, [sg](int v) {
            sg->setSpeed(v / 50.0f);
        });

        pLayout->addWidget(grp);
    }

    // ---- Surface group ----
    {
        auto *grp = new QGroupBox("Surface");
        auto *gl  = new QVBoxLayout(grp);

        // Noise Density — 10..400 → 0.1..4.0
        addSlider(gl, "Noise Density", 10, 400, 150, [sg](int v) {
            sg->setNoiseDensity(v / 100.0f);
        });

        // Noise Strength — 0..200 → 0.0..2.0
        addSlider(gl, "Noise Strength", 0, 200, 80, [sg](int v) {
            sg->setNoiseStrength(v / 100.0f);
        });

        pLayout->addWidget(grp);
    }

    // ---- Colors group ----
    {
        auto *grp = new QGroupBox("Gradient Colors");
        auto *gl  = new QVBoxLayout(grp);
        gl->setSpacing(10);

        struct ColorEntry { QString label; QColor *prop; void (ShaderGradientWidget::*setter)(const QColor&); };

        // We need stable pointers — store colors on the heap
        static QColor c1(  0, 200, 255), c2(180,   0, 255), c3(  0,  20,  80);

        auto makeColorRow = [&](const QString &lbl, QColor &col,
                                void (ShaderGradientWidget::*setter)(const QColor&)) {
            auto *rowLayout = new QHBoxLayout;
            auto *nameLbl   = new QLabel(lbl);
            auto *btn       = new QPushButton;
            btn->setObjectName("colorBtn");
            btn->setStyleSheet(colorButtonStyle(col));
            btn->setFixedHeight(30);

            rowLayout->addWidget(nameLbl);
            rowLayout->addWidget(btn);
            gl->addLayout(rowLayout);

            QObject::connect(btn, &QPushButton::clicked, [&, btn, setter]() {
                QColor chosen = QColorDialog::getColor(col, &win, "Pick Gradient Color",
                                                       QColorDialog::ShowAlphaChannel);
                if (chosen.isValid()) {
                    col = chosen;
                    btn->setStyleSheet(colorButtonStyle(col));
                    (sg->*setter)(col);
                }
            });
        };

        makeColorRow("Color 1 (A)",  c1, &ShaderGradientWidget::setColor1);
        makeColorRow("Color 2 (B)",  c2, &ShaderGradientWidget::setColor2);
        makeColorRow("Color 3 (C)",  c3, &ShaderGradientWidget::setColor3);

        pLayout->addWidget(grp);
    }

    // ---- Presets ----
    {
        auto *grp = new QGroupBox("Presets");
        auto *gl  = new QVBoxLayout(grp);

        struct Preset {
            QString name;
            QColor c1, c2, c3;
            float speed, density, strength;
        };

        const QList<Preset> presets = {
            { "🌊 Ocean",    QColor(0,150,255), QColor(0,220,200),  QColor(0,20,60),    0.25f, 1.2f, 0.9f },
            { "🔮 Universe", QColor(0,200,255), QColor(180,0,255),  QColor(0,20,80),    0.30f, 1.5f, 0.8f },
            { "🌅 Sunset",   QColor(255,80,0),  QColor(255,180,0),  QColor(120,0,80),   0.20f, 1.0f, 0.7f },
            { "🌲 Forest",   QColor(0,200,80),  QColor(0,100,180),  QColor(0,40,10),    0.15f, 1.8f, 1.0f },
            { "🔥 Plasma",   QColor(255,50,0),  QColor(200,0,255),  QColor(40,0,0),     0.50f, 2.0f, 1.2f },
        };

        for (const Preset &p : presets) {
            auto *btn = new QPushButton(p.name);
            btn->setStyleSheet("QPushButton { background: #1a1a2e; border: 1px solid #333; "
                               "border-radius: 6px; padding: 6px; text-align: left; }"
                               "QPushButton:hover { background: #252540; border-color: #7c5af0; }");
            QObject::connect(btn, &QPushButton::clicked, [sg, p]() {
                sg->setColor1(p.c1);
                sg->setColor2(p.c2);
                sg->setColor3(p.c3);
                sg->setSpeed(p.speed);
                sg->setNoiseDensity(p.density);
                sg->setNoiseStrength(p.strength);
            });
            gl->addWidget(btn);
        }
        pLayout->addWidget(grp);
    }

    pLayout->addStretch();
    root->addWidget(panel);
    win.show();
    return app.exec();
}
