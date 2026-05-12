/**
 * shaderqt-preview — standalone preview application for ShadyGradientWidget
 *
 * Displays the waterPlane cosmic aurora shader with a side panel of controls
 * for all shader uniforms (speed, noise density, noise strength, 3 gradient colors).
 */

#include <shaderqt/ShadyGradientWidget.h>
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
#include <QComboBox>
#include <QSurfaceFormat>
#include <QFont>
#include <QFontDatabase>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

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
    app.setApplicationName("ShadyGradient");
    app.setOrganizationName("Collective");

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
    win.setWindowTitle("ShadyGradient");
    win.resize(1200, 720);

    QWidget *central = new QWidget;
    QHBoxLayout *root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    win.setCentralWidget(central);

    // ---- Shader widget (left, fills most of window) ----
    auto *sg = new ShadyGradientWidget;
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
    auto *titleLbl = new QLabel("ShadyGradient");
    titleLbl->setObjectName("titleLabel");
    pLayout->addWidget(titleLbl);

    // Type Selector
    auto *typeCombo = new QComboBox;
    typeCombo->addItem("WaterPlane", QVariant::fromValue(ShadyGradientWidget::Type::WaterPlane));
    typeCombo->addItem("Sphere", QVariant::fromValue(ShadyGradientWidget::Type::Sphere));
    typeCombo->setStyleSheet("QComboBox { background-color: #1a1a2e; border: 1px solid #333; border-radius: 4px; padding: 4px; }");
    pLayout->addWidget(typeCombo);

    // ---- Helper: labelled slider ----
    auto addSlider = [&](QVBoxLayout *parent, const QString &label,
                         int min, int max, int val,
                         std::function<void(int)> onChange) -> QSlider* {
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
        return slider;
    };

    // ---- Animation group ----
    {
        auto *grp = new QGroupBox("Animation");
        auto *gl  = new QVBoxLayout(grp);

        // Speed  — 0..100 → 0.0..2.0
        auto *speedSlider = addSlider(gl, "Speed", 0, 100, 30, [sg](int v) {
            sg->setSpeed(v / 50.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::speedChanged, [speedSlider](float v) {
            speedSlider->setValue(qRound(v * 50.0f));
        });

        pLayout->addWidget(grp);

        // Update when type changes
        QObject::connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
            auto type = typeCombo->itemData(index).value<ShadyGradientWidget::Type>();
            sg->setType(type);
            if (type == ShadyGradientWidget::Type::Sphere) {
                sg->setColor1(QColor("#809bd6"));
                sg->setColor2(QColor("#910aff"));
                sg->setColor3(QColor("#af38ff"));
                sg->setSpeed(0.3f);
                sg->setNoiseDensity(0.8f);
                sg->setNoiseStrength(0.4f);
                sg->setSpiral(2.0f);
            } else {
                sg->setColor1(QColor(0, 200, 255));
                sg->setColor2(QColor(180, 0, 255));
                sg->setColor3(QColor(0, 20, 80));
                sg->setSpeed(0.3f);
                sg->setNoiseDensity(1.5f);
                sg->setNoiseStrength(0.8f);
                sg->setSpiral(2.0f);
            }
        });
    }

    // ---- Surface group ----
    {
        auto *grp = new QGroupBox("Surface");
        auto *gl  = new QVBoxLayout(grp);

        // Noise Density — 0..700 → 0.0..7.0
        auto *densitySlider = addSlider(gl, "Noise Density", 0, 700, 150, [sg](int v) {
            sg->setNoiseDensity(v / 100.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::noiseDensityChanged, [densitySlider](float v) {
            densitySlider->setValue(qRound(v * 100.0f));
        });

        // Noise Strength — 0..1000 → 0.0..10.0
        auto *strengthSlider = addSlider(gl, "Noise Strength", 0, 1000, 80, [sg](int v) {
            sg->setNoiseStrength(v / 100.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::noiseStrengthChanged, [strengthSlider](float v) {
            strengthSlider->setValue(qRound(v * 100.0f));
        });

        // Spiral — 0..700 → 0.0..7.0
        auto *spiralSlider = addSlider(gl, "Spiral", 0, 100, 20, [sg](int v) {
            sg->setSpiral(v / 10.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::spiralChanged, [spiralSlider](float v) {
            spiralSlider->setValue(qRound(v * 10.0f));
        });

        pLayout->addWidget(grp);
    }

    // ---- Colors group ----
    {
        auto *grp = new QGroupBox("Gradient Colors");
        auto *gl  = new QVBoxLayout(grp);
        gl->setSpacing(10);

        struct ColorEntry { QString label; QColor *prop; void (ShadyGradientWidget::*setter)(const QColor&); };

        auto *pixelDensitySlider = addSlider(gl, "Pixel Density", 1, 100, 10, [sg](int v) {
            sg->setPixelDensity(v / 10.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::pixelDensityChanged, [pixelDensitySlider](float v) {
            pixelDensitySlider->setValue(qRound(v * 10.0f));
        });

        // We need stable pointers — store colors on the heap
        static QColor c1(  0, 200, 255), c2(180,   0, 255), c3(  0,  20,  80);

        auto makeColorRow = [&](const QString &lbl, QColor &col,
                                void (ShadyGradientWidget::*setter)(const QColor&),
                                void (ShadyGradientWidget::*signal)(const QColor&)) {
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

            QObject::connect(sg, signal, [&, btn](const QColor &newCol) {
                col = newCol;
                btn->setStyleSheet(colorButtonStyle(newCol));
            });
        };

        makeColorRow("Color 1 (A)",  c1, &ShadyGradientWidget::setColor1, &ShadyGradientWidget::color1Changed);
        makeColorRow("Color 2 (B)",  c2, &ShadyGradientWidget::setColor2, &ShadyGradientWidget::color2Changed);
        makeColorRow("Color 3 (C)",  c3, &ShadyGradientWidget::setColor3, &ShadyGradientWidget::color3Changed);

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
            { " Ocean",    QColor(0,150,255), QColor(0,220,200),  QColor(0,20,60),    0.25f, 1.2f, 0.9f },
            { " Universe", QColor(0,200,255), QColor(180,0,255),  QColor(0,20,80),    0.30f, 1.5f, 0.8f },
            { " Sunset",   QColor(255,80,0),  QColor(255,180,0),  QColor(120,0,80),   0.20f, 1.0f, 0.7f },
            { " Forest",   QColor(0,200,80),  QColor(0,100,180),  QColor(0,40,10),    0.15f, 1.8f, 1.0f },
            { " Plasma",   QColor(255,50,0),  QColor(200,0,255),  QColor(40,0,0),     0.50f, 2.0f, 1.2f },
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
        
        // Save Custom Preset Button
        auto *saveBtn = new QPushButton("Save Custom Preset...");
        saveBtn->setStyleSheet("QPushButton { background: #3a2a60; border: 1px solid #7c5af0; "
                               "border-radius: 6px; padding: 6px; text-align: center; font-weight: bold; }"
                               "QPushButton:hover { background: #4a3a70; border-color: #9c7af0; }");
        saveBtn->setMinimumHeight(30);
        QObject::connect(saveBtn, &QPushButton::clicked, [sg, typeCombo, &win]() {
            // Save as predetermined file name for auto-loading
            QString defaultFileName = QDir::currentPath() + "/shady_gradient_preset.json";
            QString fileName = QFileDialog::getSaveFileName(&win, "Save Custom Preset", defaultFileName, "JSON Files (*.json);;Text Files (*.txt)");
            if (fileName.isEmpty()) return;

            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QMessageBox::warning(&win, "Error", "Could not open file for writing.");
                return;
            }

            QString typeName = typeCombo->currentText();
            QString typeEnum = typeName == "WaterPlane" ? "ShadyGradientWidget::Type::WaterPlane" : "ShadyGradientWidget::Type::Sphere";
            QString qmlTypeEnum = typeName == "WaterPlane" ? "ShadyGradientItem.WaterPlane" : "ShadyGradientItem.Sphere";

            QTextStream out(&file);

            if (fileName.endsWith(".json", Qt::CaseInsensitive)) {
                QJsonObject rootObj;
                rootObj["type"] = typeName;
                rootObj["speed"] = sg->speed();
                rootObj["noiseDensity"] = sg->noiseDensity();
                rootObj["noiseStrength"] = sg->noiseStrength();
                rootObj["spiral"] = sg->spiral();
                rootObj["pixelDensity"] = sg->pixelDensity();
                rootObj["color1"] = sg->color1().name();
                rootObj["color2"] = sg->color2().name();
                rootObj["color3"] = sg->color3().name();
                QJsonDocument doc(rootObj);
                out << doc.toJson(QJsonDocument::Indented);
            } else {
                // Default to a text file with copy-pasteable snippets
                out << "========== JSON CONFIG ==========\n";
                out << "{\n";
                out << "  \"type\": \"" << typeName << "\",\n";
                out << "  \"speed\": " << sg->speed() << ",\n";
                out << "  \"noiseDensity\": " << sg->noiseDensity() << ",\n";
                out << "  \"noiseStrength\": " << sg->noiseStrength() << ",\n";
                out << "  \"spiral\": " << sg->spiral() << ",\n";
                out << "  \"pixelDensity\": " << sg->pixelDensity() << ",\n";
                out << "  \"color1\": \"" << sg->color1().name() << "\",\n";
                out << "  \"color2\": \"" << sg->color2().name() << "\",\n";
                out << "  \"color3\": \"" << sg->color3().name() << "\"\n";
                out << "}\n\n";

                out << "========== C++ SNIPPET ==========\n";
                out << "sg->setType(" << typeEnum << ");\n";
                out << "sg->setSpeed(" << sg->speed() << "f);\n";
                out << "sg->setNoiseDensity(" << sg->noiseDensity() << "f);\n";
                out << "sg->setNoiseStrength(" << sg->noiseStrength() << "f);\n";
                out << "sg->setSpiral(" << sg->spiral() << "f);\n";
                out << "sg->setPixelDensity(" << sg->pixelDensity() << "f);\n";
                out << "sg->setColor1(QColor(\"" << sg->color1().name() << "\"));\n";
                out << "sg->setColor2(QColor(\"" << sg->color2().name() << "\"));\n";
                out << "sg->setColor3(QColor(\"" << sg->color3().name() << "\"));\n\n";

                out << "========== QML SNIPPET ==========\n";
                out << "ShadyGradientItem {\n";
                out << "    type: " << qmlTypeEnum << "\n";
                out << "    speed: " << sg->speed() << "\n";
                out << "    noiseDensity: " << sg->noiseDensity() << "\n";
                out << "    noiseStrength: " << sg->noiseStrength() << "\n";
                out << "    spiral: " << sg->spiral() << "\n";
                out << "    pixelDensity: " << sg->pixelDensity() << "\n";
                out << "    color1: \"" << sg->color1().name() << "\"\n";
                out << "    color2: \"" << sg->color2().name() << "\"\n";
                out << "    color3: \"" << sg->color3().name() << "\"\n";
                out << "}\n";
            }
            file.close();
            QMessageBox::information(&win, "Success", "Preset saved successfully.");
        });
        gl->addWidget(saveBtn);

        pLayout->addWidget(grp);
    }

    pLayout->addStretch();
    root->addWidget(panel);
    win.show();
    return app.exec();
}
