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
#include <QPair>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QComboBox>
#include <QScrollArea>
#include <QToolButton>
#include <QStackedWidget>
#include <QSurfaceFormat>
#include <QFont>
#include <QFontDatabase>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QButtonGroup>

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

    // Light theme stylesheet
    app.setStyleSheet(R"(
        QWidget {
            font-family: 'Inter', 'Segoe UI', sans-serif;
            font-size: 13px;
        }
        QMainWindow { background-color: #0d0d14; }
        
        QWidget#bottomDock {
            background-color: #f9f6f2;
            border-radius: 20px;
            border: 1px solid #e5dfd5;
        }
        
        QGroupBox {
            border: none;
            margin-top: 0px;
            padding: 0px;
            font-weight: normal;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 0px;
            padding: 0;
            color: #ff5a17;
            font-weight: bold;
        }
        
        QSlider::groove:horizontal {
            height: 4px;
            background: #f7d5ca;
            border-radius: 2px;
        }
        QSlider::handle:horizontal {
            background: #ff5a17;
            border: none;
            width: 16px;
            height: 16px;
            margin: -6px 0;
            border-radius: 8px;
        }
        QSlider::sub-page:horizontal {
            background: #ff5a17;
            border-radius: 2px;
        }
        
        QLabel { color: #ff5a17; font-weight: bold; font-size: 12px; }
        QLabel#valueLabel { 
            color: #ff5a17; 
            font-weight: bold; 
            background: #f0e4de; 
            border-radius: 6px; 
            padding: 4px; 
            margin-right: 8px;
        }
        
        QPushButton#colorBtn {
            border-radius: 6px;
            min-height: 32px;
            border: 2px solid #ddd;
        }
        
        QTabBar {
            background: transparent;
        }
        QTabBar::tab {
            background: transparent;
            color: #f7cabf;
            padding: 8px 12px;
            margin-right: 4px;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: bold;
        }
        QTabBar::tab:selected {
            color: #ff5a17;
            background: transparent;
        }
        QTabBar::tab:hover:!selected {
            color: #ff8a57;
        }
        
        QScrollArea {
            border: none;
            background: transparent;
        }
        QScrollBar:vertical {
            width: 0px;
            background: transparent;
        }
        
        QPushButton#typeBtn {
            background-color: transparent;
            color: #ff5a17;
            border: none;
            border-radius: 12px;
            padding: 6px 14px;
            font-weight: bold;
            font-size: 12px;
        }
        QPushButton#typeBtn:checked {
            background-color: #f0e4de;
        }
        QPushButton#typeBtn:hover:!checked {
            background-color: #f7efe9;
        }
        
        QPushButton#iconBtn {
            color: #ff5a17;
            border: none;
            background: transparent;
            font-weight: bold;
            font-size: 16px;
            border-radius: 6px;
            padding: 4px;
        }
        QPushButton#iconBtn:hover {
            background: #f0e4de;
        }
    )");

    // ---- Window layout ----
    QMainWindow win;
    win.setWindowTitle("ShadyGradient");
    win.resize(1200, 720);

    QWidget *central = new QWidget;
    QGridLayout *root = new QGridLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    win.setCentralWidget(central);

    // ---- Shader widget (fills window) ----
    auto *sg = new ShadyGradientWidget;
    sg->setMinimumSize(600, 400);
    root->addWidget(sg, 0, 0);

    // ---- Overlay for the pill menu ----
    QVBoxLayout *overlayLayout = new QVBoxLayout;
    overlayLayout->addStretch(1);
    
    QWidget *bottomDock = new QWidget;
    bottomDock->setObjectName("bottomDock");
    bottomDock->setFixedWidth(560);
    
    QVBoxLayout *dockLayout = new QVBoxLayout(bottomDock);
    dockLayout->setContentsMargins(20, 20, 20, 10);
    dockLayout->setSpacing(10);
    dockLayout->setSizeConstraint(QLayout::SetFixedSize);
    
    QWidget *sectionContainer = new QWidget;
    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionContainer);
    sectionLayout->setContentsMargins(0, 0, 0, 0);
    dockLayout->addWidget(sectionContainer);
    
    QList<QWidget*> pages;
    
    // Tabs and icons at the bottom of the pill
    QHBoxLayout *bottomBarLayout = new QHBoxLayout;
    bottomBarLayout->setContentsMargins(0, 8, 0, 0);
    
    QTabBar *sectionTabs = new QTabBar;
    sectionTabs->addTab("Shape");
    sectionTabs->addTab("Colors");
    sectionTabs->addTab("Motion");
    sectionTabs->addTab("View");
    sectionTabs->setDrawBase(false);
    bottomBarLayout->addWidget(sectionTabs);
    
    bottomBarLayout->addStretch();
    dockLayout->addLayout(bottomBarLayout);
    
    overlayLayout->addWidget(bottomDock, 0, Qt::AlignHCenter);
    overlayLayout->addSpacing(30); // Margin from bottom edge
    root->addLayout(overlayLayout, 0, 0);

    // Helper to make pages
    auto makePage = [&](const QString &title) {
        QWidget *content = new QWidget;
        auto *layout = new QVBoxLayout(content);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(12);
        sectionLayout->addWidget(content);
        pages.append(content);
        content->hide(); // hidden initially
        Q_UNUSED(title);
        return layout;
    };

    QVBoxLayout *shapeLayout = makePage("Shape");
    QVBoxLayout *colorsLayout = makePage("Colors");
    QVBoxLayout *motionLayout = makePage("Motion");
    QVBoxLayout *viewLayout = makePage("View");

    QMap<QString, QVBoxLayout*> sectionLayouts;
    sectionLayouts.insert("Shape", shapeLayout);
    sectionLayouts.insert("Colors", colorsLayout);
    sectionLayouts.insert("Motion", motionLayout);
    sectionLayouts.insert("View", viewLayout);

    // ---- Helper: labelled slider ----
    auto addSlider = [&](QVBoxLayout *parent, const QString &label,
                         int min, int max, int val,
                         std::function<void(int)> onChange) -> QSlider* {
        auto *row = new QHBoxLayout;
        auto *lbl = new QLabel(label);
        lbl->setMinimumWidth(100);
        
        auto *valLbl = new QLabel(QString::number(val));
        valLbl->setObjectName("valueLabel");
        valLbl->setFixedSize(36, 24);
        valLbl->setAlignment(Qt::AlignCenter);
        
        auto *slider = makeSlider(min, max, val);
        slider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        
        row->addWidget(lbl);
        row->addStretch(1);
        row->addWidget(valLbl);
        row->addWidget(slider, 3);
        
        // Insert before stretch
        parent->insertLayout(parent->count() - 1, row);

        QObject::connect(slider, &QSlider::valueChanged, [=](int v) {
            valLbl->setText(QString::number(v));
            onChange(v);
        });
        return slider;
    };

    auto addSectionGroup = [&](const QString &section, QGroupBox *grp) {
        auto* l = sectionLayouts.value(section);
        l->insertWidget(l->count() - 1, grp);
    };

    QObject::connect(sectionTabs, &QTabBar::currentChanged, [pages](int index) {
        for(int i = 0; i < pages.size(); ++i) {
            pages[i]->setVisible(i == index);
        }
    });

    // ==========================================
    // SHAPE PAGE
    // ==========================================
    {
        QHBoxLayout *typeLayout = new QHBoxLayout;
        auto *typeLbl = new QLabel("Type");
        typeLbl->setMinimumWidth(100);
        typeLayout->addWidget(typeLbl);
        typeLayout->addStretch(1);
        
        QPushButton *btnPlane = new QPushButton("Plane");
        QPushButton *btnSphere = new QPushButton("Sphere");
        QPushButton *btnWater = new QPushButton("Water");
        btnPlane->setObjectName("typeBtn"); btnPlane->setCheckable(true);
        btnSphere->setObjectName("typeBtn"); btnSphere->setCheckable(true);
        btnWater->setObjectName("typeBtn"); btnWater->setCheckable(true);
        
        QButtonGroup *typeGroup = new QButtonGroup(&win);
        typeGroup->setExclusive(true);
        typeGroup->addButton(btnPlane, 0);
        typeGroup->addButton(btnSphere, 1);
        typeGroup->addButton(btnWater, 2);
        btnWater->setChecked(true); // Default
        
        typeLayout->addWidget(btnPlane);
        typeLayout->addWidget(btnSphere);
        typeLayout->addWidget(btnWater);
        
        shapeLayout->insertLayout(shapeLayout->count() - 1, typeLayout);

        QObject::connect(typeGroup, QOverload<int>::of(&QButtonGroup::idClicked), [=](int id) {
            if (id == 0) sg->setType(ShadyGradientWidget::Type::Plane);
            else if (id == 1) sg->setType(ShadyGradientWidget::Type::Sphere);
            else if (id == 2) sg->setType(ShadyGradientWidget::Type::WaterPlane);
            
            if (id == 1) { // Sphere
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
        
        // Noise Strength — 0..1000 → 0.0..10.0
        auto *strengthSlider = addSlider(shapeLayout, "Noise Strength", 0, 1000, 80, [sg](int v) {
            sg->setNoiseStrength(v / 100.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::noiseStrengthChanged, [strengthSlider](float v) {
            strengthSlider->setValue(qRound(v * 100.0f));
        });

        // Noise Density — 0..700 → 0.0..7.0
        auto *densitySlider = addSlider(shapeLayout, "Noise Density", 0, 700, 150, [sg](int v) {
            sg->setNoiseDensity(v / 100.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::noiseDensityChanged, [densitySlider](float v) {
            densitySlider->setValue(qRound(v * 100.0f));
        });

        // Spiral — 0..700 → 0.0..7.0
        auto *spiralSlider = addSlider(shapeLayout, "Spiral", 0, 100, 20, [sg](int v) {
            sg->setSpiral(v / 10.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::spiralChanged, [spiralSlider](float v) {
            spiralSlider->setValue(qRound(v * 10.0f));
        });
        
        auto *pixelDensitySlider = addSlider(shapeLayout, "Pixel Density", 1, 100, 10, [sg](int v) {
            sg->setPixelDensity(v / 10.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::pixelDensityChanged, [pixelDensitySlider](float v) {
            pixelDensitySlider->setValue(qRound(v * 10.0f));
        });
    }

    // ==========================================
    // COLORS PAGE
    // ==========================================
    {
        auto *grp = new QGroupBox("Gradient Colors");
        auto *gl  = new QVBoxLayout(grp);
        gl->setSpacing(10);

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

        addSectionGroup("Colors", grp);
    }
    
    // ==========================================
    // MOTION PAGE
    // ==========================================
    {
        // Speed  — 0..100 → 0.0..2.0
        auto *speedSlider = addSlider(motionLayout, "Speed", 0, 100, 30, [sg](int v) {
            sg->setSpeed(v / 50.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::speedChanged, [speedSlider](float v) {
            speedSlider->setValue(qRound(v * 50.0f));
        });
    }

    // ==========================================
    // VIEW PAGE
    // ==========================================
    {
        auto *grp = new QGroupBox("");
        auto *gl  = new QVBoxLayout(grp);

        auto *cameraSection = new QLabel("Camera");
        gl->addWidget(cameraSection);

        // Distance: absolute camera distance in the range 0.50 .. 60.00
        auto *distanceSlider = addSlider(gl, "Distance", 50, 6000, qRound(sg->cameraDistance() * 100.0f), [sg](int v) {
            sg->setCameraDistance(v / 100.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::cameraDistanceChanged, [distanceSlider](float v) {
            distanceSlider->setValue(qRound(v * 100.0f));
        });

        // Camera angles
        auto *azSlider = addSlider(gl, "azimuth", 0, 360, qRound(sg->cameraAzimuth()), [sg](int v) {
            sg->setCameraAzimuth(static_cast<float>(v));
        });
        QObject::connect(sg, &ShadyGradientWidget::cameraAzimuthChanged, [azSlider](float v) {
            azSlider->setValue(qRound(v));
        });

        auto *polarSlider = addSlider(gl, "polar", 0, 180, qRound(sg->cameraPolar()), [sg](int v) {
            sg->setCameraPolar(static_cast<float>(v));
        });
        QObject::connect(sg, &ShadyGradientWidget::cameraPolarChanged, [polarSlider](float v) {
            polarSlider->setValue(qRound(v));
        });

        auto *objectPositionSection = new QLabel("Object Position");
        objectPositionSection->setStyleSheet("padding-top: 6px;");
        gl->addWidget(objectPositionSection);

        auto *posXSlider = addSlider(gl, "x", -200, 200, qRound(sg->objectPosX() * 10.0f), [sg](int v) {
            sg->setObjectPosX(v / 10.0f);
        });
        auto *posYSlider = addSlider(gl, "y", -200, 200, qRound(sg->objectPosY() * 10.0f), [sg](int v) {
            sg->setObjectPosY(v / 10.0f);
        });
        auto *posZSlider = addSlider(gl, "z", -200, 200, qRound(sg->objectPosZ() * 10.0f), [sg](int v) {
            sg->setObjectPosZ(v / 10.0f);
        });
        QObject::connect(sg, &ShadyGradientWidget::objectPositionChanged, [=]() {
            posXSlider->setValue(qRound(sg->objectPosX() * 10.0f));
            posYSlider->setValue(qRound(sg->objectPosY() * 10.0f));
            posZSlider->setValue(qRound(sg->objectPosZ() * 10.0f));
        });

        auto *objectRotationSection = new QLabel("Object Rotation");
        objectRotationSection->setStyleSheet("padding-top: 6px;");
        gl->addWidget(objectRotationSection);

        auto *rotXSlider = addSlider(gl, "x", 0, 360, qRound(sg->objectRotX()), [sg](int v) {
            sg->setObjectRotX(static_cast<float>(v));
        });
        auto *rotYSlider = addSlider(gl, "y", 0, 360, qRound(sg->objectRotY()), [sg](int v) {
            sg->setObjectRotY(static_cast<float>(v));
        });
        auto *rotZSlider = addSlider(gl, "z", 0, 360, qRound(sg->objectRotZ()), [sg](int v) {
            sg->setObjectRotZ(static_cast<float>(v));
        });
        QObject::connect(sg, &ShadyGradientWidget::objectRotationChanged, [=]() {
            rotXSlider->setValue(qRound(sg->objectRotX()));
            rotYSlider->setValue(qRound(sg->objectRotY()));
            rotZSlider->setValue(qRound(sg->objectRotZ()));
        });

        // Field of view 10..120
        auto *fovSlider = addSlider(gl, "Field of view", 10, 120, qRound(sg->fieldOfView()), [sg](int v) {
            sg->setFieldOfView(static_cast<float>(v));
        });
        QObject::connect(sg, &ShadyGradientWidget::fieldOfViewChanged, [fovSlider](float v) {
            fovSlider->setValue(qRound(v));
        });

        addSectionGroup("View", grp);
    }

    // Save Custom Preset Button
    auto *saveBtn = new QPushButton("Save Custom Preset");
    saveBtn->setStyleSheet("QPushButton { background: #f0e4de; color: #ff5a17; border-radius: 12px; padding: 8px 16px; font-weight: bold; font-size: 14px; margin-top: 10px; }"
                           "QPushButton:hover { background: #f7efe9; }");
    QObject::connect(saveBtn, &QPushButton::clicked, [sg, &win]() {
        QString fileName = QDir::currentPath() + "/shady_gradient_preset.json";

        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(&win, "Error", "Could not open file for writing.");
            return;
        }

        QString typeName = sg->type() == ShadyGradientWidget::Type::WaterPlane ? "WaterPlane" : 
                           sg->type() == ShadyGradientWidget::Type::Plane ? "Plane" : "Sphere";

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
        
        // Add View properties
        rootObj["cameraDistance"] = sg->cameraDistance();
        rootObj["cameraAzimuth"] = sg->cameraAzimuth();
        rootObj["cameraPolar"] = sg->cameraPolar();
        rootObj["objectPosX"] = sg->objectPosX();
        rootObj["objectPosY"] = sg->objectPosY();
        rootObj["objectPosZ"] = sg->objectPosZ();
        rootObj["objectRotX"] = sg->objectRotX();
        rootObj["objectRotY"] = sg->objectRotY();
        rootObj["objectRotZ"] = sg->objectRotZ();
        rootObj["fieldOfView"] = sg->fieldOfView();

        QJsonDocument doc(rootObj);
        QTextStream out(&file);
        out << doc.toJson(QJsonDocument::Indented);
        file.close();
        
        QMessageBox::information(&win, "Success", "Preset saved successfully.");
    });
    
    // Add to View page
    viewLayout->addWidget(saveBtn);

    // Initialize to default
    sectionTabs->setCurrentIndex(0);
    pages[0]->setVisible(true);

    win.show();
    return app.exec();
}
