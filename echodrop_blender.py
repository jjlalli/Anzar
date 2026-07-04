"""
EchoDrop - Blender model + render script.

Run it (no need to open Blender's UI):

  macOS:
    "/Applications/Blender.app/Contents/MacOS/Blender" --background --python "echodrop_blender.py"

It builds the device (casing, faceplate, LED, clamp collars, water pipe),
sets materials + 3-point lighting + a camera, renders with Cycles, and writes
'echodrop_render.png' next to this file. Also saves 'echodrop.blend' so you can
open it in Blender's UI and keep editing / re-rendering.
"""
import bpy, math, os

BASE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.join(BASE, "echodrop_render.png")

# fresh empty scene
bpy.ops.wm.read_factory_settings(use_empty=True)
scene = bpy.context.scene

def material(name, color, rough=0.5, metal=0.0, emit=None, emit_strength=6.0):
    m = bpy.data.materials.new(name)
    m.use_nodes = True
    b = m.node_tree.nodes.get("Principled BSDF")
    def setv(key, val):
        if b and key in b.inputs:
            try: b.inputs[key].default_value = val
            except Exception: pass
    setv("Base Color", (*color, 1))
    setv("Roughness", rough)
    setv("Metallic", metal)
    if emit:
        for k in ("Emission Color", "Emission"):
            setv(k, (*emit, 1))
        setv("Emission Strength", emit_strength)
    return m

M_white = material("white", (0.92, 0.92, 0.93), rough=0.35)
M_navy  = material("navy",  (0.02, 0.055, 0.14), rough=0.45)
M_metal = material("metal", (0.72, 0.74, 0.77), rough=0.25, metal=1.0)
M_led   = material("led",   (0.10, 0.35, 0.90), rough=0.2, emit=(0.10, 0.35, 0.90), emit_strength=14.0)
M_floor = material("floor", (0.82, 0.80, 0.76), rough=0.9)

def add(obj_mat):
    o = bpy.context.active_object
    o.data.materials.append(obj_mat)
    bpy.ops.object.shade_smooth()
    return o

# water pipe (axis along X)
bpy.ops.mesh.primitive_cylinder_add(radius=0.5, depth=6, rotation=(0, math.radians(90), 0))
add(M_metal)

# two clamp collars gripping the pipe
for x in (-0.32, 0.32):
    bpy.ops.mesh.primitive_torus_add(major_radius=0.57, minor_radius=0.08,
                                     location=(x, 0, 0), rotation=(0, math.radians(90), 0))
    add(M_navy)

# casing body sitting on top of the pipe
bpy.ops.mesh.primitive_cube_add(size=1, location=(0, 0, 0.80))
body = bpy.context.active_object
body.scale = (0.9, 0.45, 0.5)
bpy.ops.object.transform_apply(scale=True)
bev = body.modifiers.new("bevel", "BEVEL")
bev.width = 0.07; bev.segments = 4
body.data.materials.append(M_navy)
bpy.ops.object.shade_smooth()

# recessed white faceplate on the top face
bpy.ops.mesh.primitive_cube_add(size=1, location=(0, 0, 1.045))
fp = bpy.context.active_object
fp.scale = (0.72, 0.3, 0.03)
bpy.ops.object.transform_apply(scale=True)
fpb = fp.modifiers.new("bevel", "BEVEL"); fpb.width = 0.02; fpb.segments = 3
fp.data.materials.append(M_white)
bpy.ops.object.shade_smooth()

# status LED
bpy.ops.mesh.primitive_cylinder_add(radius=0.05, depth=0.05, location=(0.26, 0, 1.07))
add(M_led)

# floor
bpy.ops.mesh.primitive_plane_add(size=40, location=(0, 0, -0.55))
add(M_floor)

# camera aimed at the device
target = bpy.data.objects.new("target", None); scene.collection.objects.link(target)
target.location = (0, 0, 0.65)
bpy.ops.object.camera_add(location=(3.4, -3.4, 2.5))
cam = bpy.context.active_object
c = cam.constraints.new("TRACK_TO"); c.target = target
c.track_axis = "TRACK_NEGATIVE_Z"; c.up_axis = "UP_Y"
scene.camera = cam

# lighting: key / fill / rim
def area(loc, energy, size):
    bpy.ops.object.light_add(type="AREA", location=loc)
    l = bpy.context.active_object
    l.data.energy = energy; l.data.size = size
    con = l.constraints.new("TRACK_TO"); con.target = target
    con.track_axis = "TRACK_NEGATIVE_Z"; con.up_axis = "UP_Y"
    return l
area((2.5, -2.0, 4.0), 950, 6)
area((-3.5, -1.0, 2.0), 300, 5)
area((0.0, 3.5, 3.0), 480, 5)

# soft grey world
w = bpy.data.worlds.new("w"); scene.world = w; w.use_nodes = True
bg = w.node_tree.nodes.get("Background")
if bg:
    bg.inputs[0].default_value = (0.80, 0.80, 0.83, 1)
    bg.inputs[1].default_value = 0.45

# render
try:
    scene.render.engine = "CYCLES"
    scene.cycles.samples = 128
    try: scene.cycles.device = "CPU"
    except Exception: pass
except Exception:
    pass
scene.render.resolution_x = 1600
scene.render.resolution_y = 1200
scene.render.film_transparent = False
scene.render.filepath = OUT
try: scene.view_settings.exposure = -0.4
except Exception: pass

bpy.ops.wm.save_as_mainfile(filepath=os.path.join(BASE, "echodrop.blend"))
print("Rendering to:", OUT)
bpy.ops.render.render(write_still=True)
print("Done.")
