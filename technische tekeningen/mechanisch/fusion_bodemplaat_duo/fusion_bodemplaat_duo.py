# -*- coding: utf-8 -*-
"""
Fusion 360 script - LineFollower DUO bodemplaat (2 sensoren achter elkaar)
==========================================================================
Bouwt de 3D-printbare bodemplaat volledig op in een nieuwe component:
  - plaat 150 x 96 x 4 mm
  - sensorbalk VOORAAN (vooruitkijken) op X = 0..20  -> sensor 120 mm voor de wielas
  - smalle hals X = 20..50 (30 mm breed) met de ball caster eronder
  - sensorbalk STUREN op X = 50..70                    -> sensor  70 mm voor de wielas
  - schuine overgang, breed middenstuk (ESP32), achterstuk met wieluitsparingen
  - gleuf (1,5 x 44 mm, 1 mm diep) in beide sensorbalken
  - M3-gaten (3,2 mm) voor sensoren, ball caster en bevestiging
  - schets "onderdelen" met de posities van ESP32, DRV8833, MT3608, sensoren en LiPo

Gebruik:
  1. Fusion 360 -> Utilities -> Scripts and Add-Ins (Shift+S) -> tabblad Scripts
  2. "+" (Create of Add script) -> kies deze map "fusion_bodemplaat_duo"
  3. selecteer het script -> Run
  4. STL voor de 3D-printer: rechtsklik op de body -> Save as Mesh -> STL

Coordinaten: X = afstand vanaf de voorkant (mm), Y = zijwaarts (0 = midden).
De Fusion API werkt in cm: alle maten worden hieronder door 10 gedeeld.
"""
import traceback

import adsk.core
import adsk.fusion

# ---------------------------------------------------------------- maten (mm)
THICKNESS = 4.0          # dikte plaat
AXLE_X = 130.0           # wielas
HOLE_D = 3.2             # M3

OUTLINE = [              # omtrek van de plaat, wijzerzin
    (0, 24), (0, -24),                     # voorkant sensorbalk VOORAAN
    (20, -24), (20, -15),                  # einde sensorbalk -> hals
    (50, -15), (50, -24),                  # einde hals -> sensorbalk STUREN
    (70, -24), (90, -48),                  # schuine overgang
    (110, -48), (110, -30),                # breed middenstuk -> wieluitsparing
    (150, -30), (150, 30),                 # achterkant
    (110, 30), (110, 48),                  # wieluitsparing
    (90, 48), (70, 24),                    # schuine overgang
    (50, 24), (50, 15),                    # sensorbalk STUREN -> hals
    (20, 15), (20, 24),                    # hals -> sensorbalk VOORAAN
]

HOLES = [
    (10, 15), (10, -15),     # sensor VOORAAN
    (60, 15), (60, -15),     # sensor STUREN
    (35, 8), (35, -8),       # ball caster (onder de hals)
    (100, 38), (100, -38),   # bevestiging / extra steun
    (140, 0),                # achteraan (TP4056 / schakelaar)
]

SLOTS = [                    # (x1, y1, x2, y2) gleuf in de sensorbalken
    (8.5, -22, 10.0, 22),
    (58.5, -22, 60.0, 22),
]
SLOT_DEPTH = 1.0

PARTS = [                    # (naam, x1, y1, x2, y2) bovenaanzicht
    ("ESP32 DevKit", 77.3, -26, 106.3, 26),
    ("DRV8833", 115.3, 0.4, 133.3, 16.4),
    ("MT3608", 111.2, -19.3, 133.2, -2.3),
    ("sensor VOORAAN (onder)", 4, -38, 16, 38),
    ("sensor STUREN (onder)", 54, -38, 66, 38),
    ("LiPo 50x60 (onder)", 75, -30, 125, 30),
]


def P(x, y, z=0.0):
    """mm -> Fusion Point3D in cm"""
    return adsk.core.Point3D.create(x / 10.0, y / 10.0, z / 10.0)


def rect(sketch, x1, y1, x2, y2):
    sketch.sketchCurves.sketchLines.addTwoPointRectangle(P(x1, y1), P(x2, y2))


def run(context):
    ui = None
    try:
        app = adsk.core.Application.get()
        ui = app.userInterface
        design = adsk.fusion.Design.cast(app.activeProduct)
        if not design:
            ui.messageBox("Open eerst een ontwerp in de Design-werkruimte.")
            return

        root = design.rootComponent
        occ = root.occurrences.addNewComponent(adsk.core.Matrix3D.create())
        comp = occ.component
        comp.name = "Bodemplaat DUO"

        # ---------------------------------------------- 1. omtrek + gaten
        sk = comp.sketches.add(comp.xYConstructionPlane)
        sk.name = "omtrek"
        lines = sk.sketchCurves.sketchLines
        n = len(OUTLINE)
        for i in range(n):
            x1, y1 = OUTLINE[i]
            x2, y2 = OUTLINE[(i + 1) % n]
            lines.addByTwoPoints(P(x1, y1), P(x2, y2))
        for (x, y) in HOLES:
            sk.sketchCurves.sketchCircles.addByCenterRadius(P(x, y), HOLE_D / 20.0)

        # het grootste profiel = de plaat met de gaten eruit
        plate_profile = None
        best = 0.0
        for i in range(sk.profiles.count):
            prof = sk.profiles.item(i)
            area = prof.areaProperties().area
            if area > best:
                best = area
                plate_profile = prof

        extrudes = comp.features.extrudeFeatures
        ext_in = extrudes.createInput(plate_profile,
                                      adsk.fusion.FeatureOperations.NewBodyFeatureOperation)
        ext_in.setDistanceExtent(False, adsk.core.ValueInput.createByReal(THICKNESS / 10.0))
        plate = extrudes.add(ext_in)
        plate.bodies.item(0).name = "bodemplaat"

        # ---------------------------------------------- 2. gleuven bovenaan
        planes = comp.constructionPlanes
        pl_in = planes.createInput()
        pl_in.setByOffset(comp.xYConstructionPlane,
                          adsk.core.ValueInput.createByReal(THICKNESS / 10.0))
        top = planes.add(pl_in)
        top.name = "bovenkant plaat"

        sk_slot = comp.sketches.add(top)
        sk_slot.name = "gleuven"
        for (x1, y1, x2, y2) in SLOTS:
            rect(sk_slot, x1, y1, x2, y2)
        slot_profiles = adsk.core.ObjectCollection.create()
        for i in range(sk_slot.profiles.count):
            slot_profiles.add(sk_slot.profiles.item(i))
        cut_in = extrudes.createInput(slot_profiles,
                                      adsk.fusion.FeatureOperations.CutFeatureOperation)
        cut_in.setDistanceExtent(False, adsk.core.ValueInput.createByReal(-SLOT_DEPTH / 10.0))
        extrudes.add(cut_in)

        # ---------------------------------------------- 3. onderdelen (referentie)
        sk_parts = comp.sketches.add(top)
        sk_parts.name = "onderdelen"
        for (_, x1, y1, x2, y2) in PARTS:
            rect(sk_parts, x1, y1, x2, y2)
        # wielas
        sk_parts.sketchCurves.sketchLines.addByTwoPoints(P(AXLE_X, -56), P(AXLE_X, 56))
        for i in range(sk_parts.sketchCurves.count):
            sk_parts.sketchCurves.item(i).isConstruction = True

        ui.messageBox(
            "Bodemplaat DUO aangemaakt: 150 x 96 x 4 mm.\n\n"
            "Sensor VOORAAN: 120 mm voor de wielas\n"
            "Sensor STUREN: 70 mm voor de wielas\n\n"
            "STL: rechtsklik op de body 'bodemplaat' -> Save as Mesh."
        )
    except Exception:
        if ui:
            ui.messageBox("Fout:\n{}".format(traceback.format_exc()))
