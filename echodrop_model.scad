// EchoDrop - clip-on acoustic leak sensor (parametric 3D model, starter)
//
// How to get a detailed 3D model/render from this file:
//   1. Install OpenSCAD (free, openscad.org) and open this file.
//   2. Press F6 to render, then File > Export > Export as STL.
//   3. Open the STL in Blender (free) for a full material/lighting render,
//      or in Tinkercad / Fusion 360 to keep editing it.
// All sizes are in millimetres. Change the values below to reshape it.

$fn = 80;

pipe_d   = 25;    // outer diameter of the pipe it clips onto
wall     = 2.4;   // casing wall thickness
clip_len = 22;    // how much of the pipe length the clamp hugs

body_w   = 46;    // casing width
body_d   = 18;    // casing depth
body_h   = 26;    // casing height

module rbox(w, d, h, r) {
    hull() for (x = [-1, 1], y = [-1, 1])
        translate([x * (w/2 - r), y * (d/2 - r), 0]) cylinder(h = h, r = r);
}

module clamp() {                       // ring the pipe passes through, open underneath so it snaps on
    rotate([0, 90, 0])
    difference() {
        cylinder(h = clip_len, r = pipe_d/2 + wall, center = true);
        cylinder(h = clip_len + 2, r = pipe_d/2, center = true);
        translate([0, -(pipe_d/2 + wall), 0])
            cube([clip_len + 2, pipe_d * 0.55, pipe_d * 0.5], center = true);
    }
}

module body() {
    base_z = pipe_d/2 + 1;
    translate([0, 0, base_z])
    difference() {
        rbox(body_w, body_d, body_h, 5);
        translate([0, 0, wall]) rbox(body_w - 2*wall, body_d - 2*wall, body_h, 4);
        translate([body_w/2 - 9, 0, body_h - 1]) cylinder(h = 4, r = 2.4);   // LED port
    }
}

clamp();
body();
