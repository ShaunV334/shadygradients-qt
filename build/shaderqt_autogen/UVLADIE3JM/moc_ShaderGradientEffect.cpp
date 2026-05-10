/****************************************************************************
** Meta object code from reading C++ file 'ShaderGradientEffect.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ShaderGradientEffect.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ShaderGradientEffect.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN20ShaderGradientEffectE_t {};
} // unnamed namespace

template <> constexpr inline auto ShaderGradientEffect::qt_create_metaobjectdata<qt_meta_tag_ZN20ShaderGradientEffectE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ShaderGradientEffect",
        "QML.Element",
        "auto",
        "typeChanged",
        "",
        "Type",
        "t",
        "speedChanged",
        "v",
        "noiseDensityChanged",
        "noiseStrengthChanged",
        "spiralChanged",
        "pixelDensityChanged",
        "color1Changed",
        "QColor",
        "c",
        "color2Changed",
        "color3Changed",
        "type",
        "speed",
        "noiseDensity",
        "noiseStrength",
        "spiral",
        "pixelDensity",
        "color1",
        "color2",
        "color3",
        "WaterPlane",
        "Sphere"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'typeChanged'
        QtMocHelpers::SignalData<void(enum Type)>(3, 4, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 5, 6 },
        }}),
        // Signal 'speedChanged'
        QtMocHelpers::SignalData<void(float)>(7, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 8 },
        }}),
        // Signal 'noiseDensityChanged'
        QtMocHelpers::SignalData<void(float)>(9, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 8 },
        }}),
        // Signal 'noiseStrengthChanged'
        QtMocHelpers::SignalData<void(float)>(10, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 8 },
        }}),
        // Signal 'spiralChanged'
        QtMocHelpers::SignalData<void(float)>(11, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 8 },
        }}),
        // Signal 'pixelDensityChanged'
        QtMocHelpers::SignalData<void(float)>(12, 4, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Float, 8 },
        }}),
        // Signal 'color1Changed'
        QtMocHelpers::SignalData<void(const QColor &)>(13, 4, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Signal 'color2Changed'
        QtMocHelpers::SignalData<void(const QColor &)>(16, 4, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
        // Signal 'color3Changed'
        QtMocHelpers::SignalData<void(const QColor &)>(17, 4, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 14, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'type'
        QtMocHelpers::PropertyData<enum Type>(18, 0x80000000 | 5, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 0),
        // property 'speed'
        QtMocHelpers::PropertyData<float>(19, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'noiseDensity'
        QtMocHelpers::PropertyData<float>(20, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'noiseStrength'
        QtMocHelpers::PropertyData<float>(21, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'spiral'
        QtMocHelpers::PropertyData<float>(22, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'pixelDensity'
        QtMocHelpers::PropertyData<float>(23, QMetaType::Float, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'color1'
        QtMocHelpers::PropertyData<QColor>(24, 0x80000000 | 14, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 6),
        // property 'color2'
        QtMocHelpers::PropertyData<QColor>(25, 0x80000000 | 14, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 7),
        // property 'color3'
        QtMocHelpers::PropertyData<QColor>(26, 0x80000000 | 14, QMC::DefaultPropertyFlags | QMC::Writable | QMC::EnumOrFlag | QMC::StdCppSet, 8),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'Type'
        QtMocHelpers::EnumData<enum Type>(5, 5, QMC::EnumIsScoped).add({
            {   27, Type::WaterPlane },
            {   28, Type::Sphere },
        }),
    };
    QtMocHelpers::UintData qt_constructors {};
    QtMocHelpers::ClassInfos qt_classinfo({
            {    1,    2 },
    });
    return QtMocHelpers::metaObjectData<ShaderGradientEffect, void>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums, qt_constructors, qt_classinfo);
}
Q_CONSTINIT const QMetaObject ShaderGradientEffect::staticMetaObject = { {
    QMetaObject::SuperData::link<QQuickFramebufferObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ShaderGradientEffectE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ShaderGradientEffectE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20ShaderGradientEffectE_t>.metaTypes,
    nullptr
} };

void ShaderGradientEffect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ShaderGradientEffect *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->typeChanged((*reinterpret_cast<std::add_pointer_t<enum Type>>(_a[1]))); break;
        case 1: _t->speedChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 2: _t->noiseDensityChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 3: _t->noiseStrengthChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 4: _t->spiralChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 5: _t->pixelDensityChanged((*reinterpret_cast<std::add_pointer_t<float>>(_a[1]))); break;
        case 6: _t->color1Changed((*reinterpret_cast<std::add_pointer_t<QColor>>(_a[1]))); break;
        case 7: _t->color2Changed((*reinterpret_cast<std::add_pointer_t<QColor>>(_a[1]))); break;
        case 8: _t->color3Changed((*reinterpret_cast<std::add_pointer_t<QColor>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(Type )>(_a, &ShaderGradientEffect::typeChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(float )>(_a, &ShaderGradientEffect::speedChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(float )>(_a, &ShaderGradientEffect::noiseDensityChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(float )>(_a, &ShaderGradientEffect::noiseStrengthChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(float )>(_a, &ShaderGradientEffect::spiralChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(float )>(_a, &ShaderGradientEffect::pixelDensityChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(const QColor & )>(_a, &ShaderGradientEffect::color1Changed, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(const QColor & )>(_a, &ShaderGradientEffect::color2Changed, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (ShaderGradientEffect::*)(const QColor & )>(_a, &ShaderGradientEffect::color3Changed, 8))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<enum Type*>(_v) = _t->type(); break;
        case 1: *reinterpret_cast<float*>(_v) = _t->speed(); break;
        case 2: *reinterpret_cast<float*>(_v) = _t->noiseDensity(); break;
        case 3: *reinterpret_cast<float*>(_v) = _t->noiseStrength(); break;
        case 4: *reinterpret_cast<float*>(_v) = _t->spiral(); break;
        case 5: *reinterpret_cast<float*>(_v) = _t->pixelDensity(); break;
        case 6: *reinterpret_cast<QColor*>(_v) = _t->color1(); break;
        case 7: *reinterpret_cast<QColor*>(_v) = _t->color2(); break;
        case 8: *reinterpret_cast<QColor*>(_v) = _t->color3(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setType(*reinterpret_cast<enum Type*>(_v)); break;
        case 1: _t->setSpeed(*reinterpret_cast<float*>(_v)); break;
        case 2: _t->setNoiseDensity(*reinterpret_cast<float*>(_v)); break;
        case 3: _t->setNoiseStrength(*reinterpret_cast<float*>(_v)); break;
        case 4: _t->setSpiral(*reinterpret_cast<float*>(_v)); break;
        case 5: _t->setPixelDensity(*reinterpret_cast<float*>(_v)); break;
        case 6: _t->setColor1(*reinterpret_cast<QColor*>(_v)); break;
        case 7: _t->setColor2(*reinterpret_cast<QColor*>(_v)); break;
        case 8: _t->setColor3(*reinterpret_cast<QColor*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *ShaderGradientEffect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ShaderGradientEffect::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20ShaderGradientEffectE_t>.strings))
        return static_cast<void*>(this);
    return QQuickFramebufferObject::qt_metacast(_clname);
}

int ShaderGradientEffect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickFramebufferObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ShaderGradientEffect::typeChanged(Type _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void ShaderGradientEffect::speedChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void ShaderGradientEffect::noiseDensityChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ShaderGradientEffect::noiseStrengthChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void ShaderGradientEffect::spiralChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void ShaderGradientEffect::pixelDensityChanged(float _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void ShaderGradientEffect::color1Changed(const QColor & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void ShaderGradientEffect::color2Changed(const QColor & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void ShaderGradientEffect::color3Changed(const QColor & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
