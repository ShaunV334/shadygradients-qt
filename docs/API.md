# ShaderQt API Reference

This document details the public API provided by the ShaderQt library for both C++ (Qt Widgets) and QML (Qt Quick).

---

## C++ API

### `ShadyGradientWidget`

A QWidget that renders the shader using OpenGL. It inherits from `QOpenGLWidget` and `QOpenGLFunctions_3_3_Core`.

#### Header
```cpp
#include <shaderqt/ShadyGradientWidget.h>
```

#### Properties

All properties are accessible via standard Qt getters and setters (e.g., `setSpeed(float)`, `speed()`) and include `NOTIFY` signals for bindings.

| Property | Type | Description |
| --- | --- | --- |
| `type` | `ShadyGradientWidget::Type` | The geometric projection of the shader (`WaterPlane` or `Sphere`). |
| `speed` | `float` | The playback speed of the animation. |
| `noiseDensity` | `float` | The denseness/scale of the underlying noise generation. |
| `noiseStrength` | `float` | The intensity of the noise variation affecting the colors. |
| `spiral` | `float` | Modifies the twist or spiraling effect of the mapping. |
| `pixelDensity` | `float` | Pixel scaling, useful for controlling quality vs performance. |
| `color1` | `QColor` | The primary gradient color. |
| `color2` | `QColor` | The secondary gradient color. |
| `color3` | `QColor` | The tertiary/background gradient color. |

#### Enums
**`ShadyGradientWidget::Type`**
* `WaterPlane` - Renders the gradient as a flat cosmic plane.
* `Sphere` - Renders the gradient wrapped around a 3D sphere.

---

### `ShadyGradientEffect`

A QQuickFramebufferObject designed to be utilized internally by QML to render the OpenGL content within the Qt Quick Scene Graph.

#### Header
```cpp
#include <shaderqt/ShadyGradientEffect.h>
```

---

## QML API

To use the QML components, import the `ShaderQt` module:

```qml
import ShaderQt 1.0
```

### `ShadyGradientItem`

The primary QML Item for embedding the shader in your Qt Quick UI.

#### Properties

| Property | Type | Default | Description |
| --- | --- | --- | --- |
| `type` | `ShadyGradientEffect.Type` | `ShadyGradientEffect.WaterPlane` | The geometric projection of the shader (`WaterPlane` or `Sphere`). Set using e.g., `ShadyGradientEffect.Sphere`. |
| `speed` | `real` | `0.3` | Animation playback multiplier. |
| `noiseDensity` | `real` | `1.5` | The structural scale of the noise algorithm. |
| `noiseStrength` | `real` | `0.8` | How pronounced the noisy blending is between colors. |
| `spiral` | `real` | `2.0` | Modifies the twist or spiraling effect of the mapping. |
| `pixelDensity` | `real` | `1.0` | Pixel scaling, useful for controlling quality vs performance. |
| `color1` | `color` | `"#00C8FF"` | First gradient stop. |
| `color2` | `color` | `"#B400FF"` | Second gradient stop. |
| `color3` | `color` | `"#001450"` | Third gradient stop (background). |

#### Example

```qml
ShadyGradientItem {
    width: 400
    height: 400
    
    // Setting enum properties accesses the core effect type
    type: ShadyGradientEffect.WaterPlane
    
    // Animate color variations over time
    SequentialAnimation on color1 {
        loops: Animation.Infinite
        ColorAnimation { to: "#ff0080"; duration: 2000 }
        ColorAnimation { to: "#00C8FF"; duration: 2000 }
    }
}
```