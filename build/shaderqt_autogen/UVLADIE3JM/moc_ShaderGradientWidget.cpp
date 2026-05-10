/****************************************************************************
** Meta object code from reading C++ file 'ShaderGradientWidget.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ShaderGradientWidget.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ShaderGradientWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN20ShaderGradientWidgetE_t {};
} // unnamed namespace

template <> constexpr inline auto ShaderGradientWidget::qt_create_metaobjectdata<qt_meta_tag_ZN20ShaderGradientWidgetE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ShaderGradientWidget",
        "speedChanged",
        "",
        "v",
        "noiseDensityChanged",
        "noiseStrengthChanged",
        "color1Changed",
        "QColor",
        "c",
        "color2Changed",
        "color3Changed",
        "speed",
        "noiseDensity",
        "noiseStrength",
        "color1",
        "color2",
        "color3"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'speedChanged'
        QtMocHelpers::SignalData<void(float)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 3 },
        }}),
        // Signal 'noiseDensityChanged'
        QtMocHelpers::SignalData<void(float)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 3 },
        }}),
        // Signal 'noiseStrengthChanged'
        QtMocHelpers::SignalData<void(float)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 3 },
        }}),
        // Signal 'color1Changed'
        QtMocHelpers::SignalData<void(const QColor &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'color2Changed'
        QtMocHelpers::SignalData<void(const QColor &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
        // Signal 'color3Changed'
        QtMocHelpers::SignalData<void(const QColor &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'speed'
        QtMocHelpers::PropertyData<float>(11, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'noiseDensity'
        QtMocHelpers::PropertyData<float>(12, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'noiseStrength'
        QtMocHelpers::PropertyData<float>(13, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'color1'
        QtMocHelpers::PropertyData<QColor>(14, 0x80000000 | 7, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 3),
        // property 'color2'
        QtMocHelpers::PropertyData<QColor>(15, 0x80000000 | 7, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 4),
        // property 'color3'
        QtMocHelpers::PropertyData<QColor>(16, 0x80000000 | 7, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 5),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<ShaderGradientWidget, qt_meta_tag_ZN20ShaderGradientWidgetE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ShaderGradientWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ShaderGradientWidgetE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ShaderGradientWidgetE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20ShaderGradientWidgetE_t>.metaTypes,
    nullptr
} };

void ShaderGradientWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ShaderGradientWidget *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->speedChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 1: _t->noiseDensityChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 2: _t->noiseStrengthChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 3: _t->color1Changed((*reinterpret_cast<std::add_pointer_t<QColor>>(_a[1]))); break;
        case 4: _t->color2Changed((*reinterpret_cast<std::add_pointer_t<QColor>>(_a[1]))); break;
        case 5: _t->color3Changed((*reinterpret_cast<std::add_pointer_t<QColor>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientWidget::*)(float )>(_a, &ShaderGradientWidget::speedChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientWidget::*)(float )>(_a, &ShaderGradientWidget::noiseDensityChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientWidget::*)(float )>(_a, &ShaderGradientWidget::noiseStrengthChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientWidget::*)(const QColor & )>(_a, &ShaderGradientWidget::color1Changed, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientWidget::*)(const QColor & )>(_a, &ShaderGradientWidget::color2Changed, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientWidget::*)(const QColor & )>(_a, &ShaderGradientWidget::color3Changed, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<float*>(_v) = _t->speed(); break;
        case 1: *reinterpret_cast<float*>(_v) = _t->noiseDensity(); break;
        case 2: *reinterpret_cast<float*>(_v) = _t->noiseStrength(); break;
        case 3: *reinterpret_cast<QColor*>(_v) = _t->color1(); break;
        case 4: *reinterpret_cast<QColor*>(_v) = _t->color2(); break;
        case 5: *reinterpret_cast<QColor*>(_v) = _t->color3(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setSpeed(*reinterpret_cast<float*>(_v)); break;
        case 1: _t->setNoiseDensity(*reinterpret_cast<float*>(_v)); break;
        case 2: _t->setNoiseStrength(*reinterpret_cast<float*>(_v)); break;
        case 3: _t->setColor1(*reinterpret_cast<QColor*>(_v)); break;
        case 4: _t->setColor2(*reinterpret_cast<QColor*>(_v)); break;
        case 5: _t->setColor3(*reinterpret_cast<QColor*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *ShaderGradientWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShaderGradientWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ShaderGradientWidgetE_t>.strings))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QOpenGLFunctions_3_3_Core"))
        return static_cast< QOpenGLFunctions_3_3_Core*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int ShaderGradientWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void ShaderGradientWidget::speedChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ShaderGradientWidget::noiseDensityChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ShaderGradientWidget::noiseStrengthChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ShaderGradientWidget::color1Changed(const QColor & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ShaderGradientWidget::color2Changed(const QColor & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ShaderGradientWidget::color3Changed(const QColor & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
