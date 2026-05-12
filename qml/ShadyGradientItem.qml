/**
 * ShadyGradientItem.qml
 *
 * Pure QML integration of the waterPlane cosmic aurora gradient.
 * Uses ShadyGradientEffect (QQuickFramebufferObject) registered as a QML type.
 *
 * Usage:
 *   import ShaderQt 1.0
 *
 *   ShadyGradientItem {
 *       anchors.fill: parent
 *       speed: 0.3
 *       color1: "#00C8FF"
 *       color2: "#B400FF"
 *       color3: "#001450"
 *   }
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import ShaderQt 1.0

Item {
    id: root

    // ---- Public API ----
    property alias speed:         gradient.speed
    property alias noiseDensity:  gradient.noiseDensity
    property alias noiseStrength: gradient.noiseStrength
    property alias color1:        gradient.color1
    property alias color2:        gradient.color2
    property alias color3:        gradient.color3

    // ---- Rendered gradient ----
    ShadyGradientEffect {
        id: gradient
        anchors.fill: parent

        speed:         0.3
        noiseDensity:  1.5
        noiseStrength: 0.8
        color1:        "#00C8FF"
        color2:        "#B400FF"
        color3:        "#001450"
    }

    // ---- Optional: child content overlaid on top ----
    // default property alias content: overlay.data
    // Item { id: overlay; anchors.fill: parent }
}
