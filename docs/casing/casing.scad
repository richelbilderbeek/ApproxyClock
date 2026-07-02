// From https://github.com/BelfrySCAD/BOSL2
include <../../../BOSL2/std.scad>
include <../../../BOSL2/threading.scad>
include <../../../BOSL2/std.scad>

// From https://github.com/brodykenrick/text_on_OpenSCAD
use <../../../text_on_OpenSCAD/text_on.scad>

pi = 3.141592653589793238462643383279502884197;
$fn = 20;

params = create_params(
  hole_diameter = 60, // The diameter of the hollow sphere that would fit in the casing
  hole_length = 120, // The length of the lipstick
  wall_thickness = pi, // Thickness of walls
  pitch = 4, // Depth of the threading grooves
  air_gap = 2, // The gap between nut and bolt
  height = 10, // The height of the nut and bolt
  font_size = 7, // The size of the letters on the casing
  wire_hole_diameter = 3.4 + 1.0, // Add uncertainty // The hole for a wire
  speaker_hole_diameter = 2.0 + 1.0, // Add uncertainty // The hole for a speaker
  holes_angle = 70, // The height of the wire and speaker holes
  display_mode = 3 // How the casing is displayed:
  // 0: to print, both
  // 1: to print, lower half, with wire holes
  // 2: to print, upper half, with text
  // 3: assambled
  // 4: assambled, cutout from left to right
  // 5: assambled, cutout from front to back
);

// Some warnings
if (struct_val(params, "wall_thickness") <= 2.0) {
  echo("Warning: a wall thickness below 2 mm results in a fragile casing");
}


function create_params(
  hole_diameter,
  hole_length,
  wall_thickness,
  pitch,
  air_gap,
  height,
  font_size,
  wire_hole_diameter,
  speaker_hole_diameter,
  holes_angle,
  display_mode
) 
  = struct_set(
    [],  
    [
      "hole_diameter", hole_diameter, 
      "hole_length", hole_length,
      "wall_thickness", wall_thickness, 
      "pitch", pitch, 
      "air_gap", air_gap, 
      "height", height,
      "font_size", font_size,
      "wire_hole_diameter", wire_hole_diameter,
      "speaker_hole_diameter", speaker_hole_diameter,
      "holes_angle", holes_angle,
      "display_mode", display_mode,
      // Helpers
      "bolt_thread_inner_diameter", hole_diameter + wall_thickness,
      "bolt_thread_outer_diameter", hole_diameter + wall_thickness + pitch,
      "nut_thread_inner_diameter",  hole_diameter + wall_thickness + pitch + air_gap,
      "nut_thread_outer_diameter",  hole_diameter + wall_thickness + pitch + air_gap + pitch,
      "nut_outer_diameter",         hole_diameter + wall_thickness + pitch + air_gap + pitch + wall_thickness,
      "sphere_diameter",            hole_diameter + wall_thickness + pitch + air_gap + pitch + wall_thickness + wall_thickness,
      "sphere_length",              hole_length + wall_thickness
    ]
  );

module check_params(params) 
{
  assert(is_struct(params), "params must be a struct. Tip: use 'create_params'");

  // Get the variables
  hole_diameter = struct_val(params, "hole_diameter");
  hole_length = struct_val(params, "hole_length");

  wall_thickness = struct_val(params, "wall_thickness");
  pitch = struct_val(params, "pitch");
  air_gap = struct_val(params, "air_gap");
  height = struct_val(params, "height");
  bolt_thread_inner_diameter = struct_val(params, "bolt_thread_inner_diameter");
  bolt_thread_outer_diameter = struct_val(params, "bolt_thread_outer_diameter");
  nut_thread_inner_diameter = struct_val(params, "nut_thread_inner_diameter");
  nut_thread_outer_diameter = struct_val(params, "nut_thread_outer_diameter");
  nut_outer_diameter = struct_val(params, "nut_outer_diameter");
  sphere_diameter = struct_val(params, "sphere_diameter");
  font_size = struct_val(params, "font_size");
  wire_hole_diameter = struct_val(params, "wire_hole_diameter");
  speaker_hole_diameter = struct_val(params, "speaker_hole_diameter");
  holes_angle = struct_val(params, "holes_angle");
  display_mode = struct_val(params, "display_mode");

  // Variables must make sense
  assert(hole_diameter > 0);
  assert(hole_length > 0);
  assert(wall_thickness > 0);
  assert(pitch > 0);
  assert(air_gap > 0); 
  assert(height > 0);
  assert(font_size > 0);
  assert(wire_hole_diameter > 0);
  assert(speaker_hole_diameter > 0);
  assert(holes_angle > 0);
  assert(holes_angle < 90);
  assert(display_mode >= 0);
  assert(display_mode <= 5);

  // Diameters go up
  assert(hole_diameter > 0);
  assert(bolt_thread_inner_diameter > hole_diameter);
  assert(bolt_thread_outer_diameter > bolt_thread_inner_diameter);
  assert(nut_thread_inner_diameter > bolt_thread_outer_diameter);
  assert(nut_thread_outer_diameter > nut_thread_inner_diameter);
  assert(nut_outer_diameter > nut_thread_outer_diameter);
  assert(sphere_diameter > nut_outer_diameter);

  // Other constraints
  assert(air_gap > 1, "An air gap between 0-1 mm is too narrow for the screw to turn"); 
}



// Lower, inner, red, part
// outer_diameter: the outer diameter of the part, where the thread ends
// inner_diameter: the diameter of the hole
// pitch: depth of the threads
//

//              *****       Air
//          ***       ***          
//        **             **        
//       *                 *       
//      *   Thread          *      
//     *                     *     
//    *          ***          *    
//    *        **   **        *    
//    *       *       *       *    
//    *       *       *       *    
//    *       * Hole  *       *    
//    *       *       *       *    
//    *        **   **        *    
//    *          ***          *    
//     *                     *     
//      *                   *      
//       *                 *       
//        **             **        
//          ***       ***          
//              *****          
//    
//           |-------| inner_diameter
//    |----------------------| outer_diameter
module draw_bolt(params)
{
  check_params(params);
  height = struct_val(params, "height");
  pitch = struct_val(params, "pitch");
  hole_diameter = struct_val(params, "hole_diameter");
  bolt_thread_inner_diameter = struct_val(params, "bolt_thread_inner_diameter");
  bolt_thread_outer_diameter = struct_val(params, "bolt_thread_outer_diameter");
  color([1, 0, 0])
    difference() {
      threaded_rod(d = bolt_thread_outer_diameter, l = height, pitch = pitch);
      cylinder(h = height, d = hole_diameter, center = true);
    }
}

// Upper, outer, blue, part
module draw_nut(params)
{
  check_params(params);

  height = struct_val(params, "height");
  pitch = struct_val(params, "pitch");
  nut_thread_inner_diameter = struct_val(params, "nut_thread_inner_diameter");
  nut_thread_outer_diameter = struct_val(params, "nut_thread_outer_diameter");
  nut_outer_diameter = struct_val(params, "nut_outer_diameter");

  color([0, 0, 1])
    intersection() {
      threaded_nut(
        shape = "square", 
        nutwidth = nut_outer_diameter, 
        id = nut_thread_inner_diameter, 
        h = height, 
        pitch = pitch,
        ibevel = false,
        spin = 180
      );  
      cylinder(height, d = nut_outer_diameter, center = true);
    }
}

// Filled egg
module draw_egg(params) 
{
  check_params(params);
  sphere_length = struct_val(params, "sphere_length");
  sphere_diameter = struct_val(params, "sphere_diameter");
  difference() {
    scale([sphere_diameter, sphere_diameter, sphere_length])
      sphere(d = 1.0);
  };
}

// Hollow egg
module draw_hollow_egg(params) 
{
  check_params(params);
  nut_outer_diameter = struct_val(params, "nut_outer_diameter");
  sphere_length = struct_val(params, "sphere_length");
  sphere_diameter = struct_val(params, "sphere_diameter");
  // Outer sphere, lower half cut off
  difference() {
    draw_egg(params);
    scale([nut_outer_diameter, nut_outer_diameter, sphere_length])
      sphere(d = 1.0);
  };
}


module draw_upper_egg(params) 
{
  check_params(params);
  height = struct_val(params, "height");
  hole_diameter = struct_val(params, "hole_diameter");
  nut_outer_diameter = struct_val(params, "nut_outer_diameter");
  sphere_diameter = struct_val(params, "sphere_diameter");
  wire_hole_diameter = struct_val(params, "wire_hole_diameter");
  font_size = struct_val(params, "font_size");
  // Add a hole
  // Outer sphere, lower half cut off
  color([0,1,0])
    difference() {
      draw_hollow_egg(params);
      translate([-(sphere_diameter / 2), -(sphere_diameter / 2), -sphere_diameter - (height / 2)])
        cube(sphere_diameter);
    };
  // Connect egg to nut
  color([0.5,1,0.5])
    // Make the open ring go down directly to prevent scaffolding
    intersection() {
      // Open ring that connects out wall to nut
      difference() {
        translate([0, 0, +(height / 2) + (sphere_diameter / 2)])
          cylinder(h = sphere_diameter, d = nut_outer_diameter, center = true);
        translate([0, 0, +(height / 2) + (sphere_diameter / 2)])
          cylinder(h = sphere_diameter, d = hole_diameter, center = true);
      }
      draw_egg(params);
    };


  text_on_sphere(t = "Approxyclock", r = sphere_diameter / 2, size = font_size);
}

module draw_lower_egg_without_holes(params)
{
  check_params(params);
  height = struct_val(params, "height");
  hole_diameter = struct_val(params, "hole_diameter");
  nut_outer_diameter = struct_val(params, "nut_outer_diameter");
  sphere_diameter = struct_val(params, "sphere_diameter");
  sphere_length = struct_val(params, "sphere_length");
  wall_thickness = struct_val(params, "wall_thickness");
  color([0,1,0])
    difference() {
      draw_hollow_egg(params);
      translate([-(sphere_diameter / 2), -(sphere_diameter / 2), -(height / 2)])
        cube(sphere_diameter);
      translate([-(wall_thickness / 2), -(sphere_diameter / 2),height])
        cube([wall_thickness, sphere_diameter, sphere_diameter]);
    };
  // Connect sphere to bolt
  color([0.5,1,0.5])
    // Make the open ring go down directly to prevent scaffolding
    intersection() {
      // Open ring that connects out wall to inner bolt
      difference() {
        translate([0, 0, -(height / 2) - (sphere_diameter / 2)])
          cylinder(h = sphere_diameter, d = nut_outer_diameter, center = true);
        translate([0, 0, -(height / 2) - (sphere_diameter / 2)])
          cylinder(h = sphere_diameter, d = hole_diameter, center = true);
      }
      draw_egg(params);
    };
}

module draw_lower_egg(params)
{
  check_params(params);
  height = struct_val(params, "height");
  sphere_diameter = struct_val(params, "sphere_diameter");
  wire_hole_diameter = struct_val(params, "wire_hole_diameter");
  speaker_hole_diameter = struct_val(params, "speaker_hole_diameter");
  holes_angle = struct_val(params, "holes_angle");
  // Draw sphere with connector to bolt, then cut holes
  color([0,1,0])
    difference() {
      draw_lower_egg_without_holes(params);
      rotate([holes_angle, 0, 0])
        translate([0, 0, -sphere_diameter])
          cylinder(sphere_diameter, d = wire_hole_diameter);
        rotate([-holes_angle, 0, 0])
          translate([0, 0, -sphere_diameter])
            cylinder(sphere_diameter, d = speaker_hole_diameter);
    }
}

// The upper half has the nut
module draw_upper_half(params) 
{
  check_params(params);
  draw_upper_egg(params);
  draw_nut(params);
}

// The upper half has the bolt
module draw_lower_half(params) 
{
  check_params(params);
  draw_lower_egg(params);
  draw_bolt(params);
}

//-----------------------------------------------------------------------
// Displayal
//-----------------------------------------------------------------------
module display(params)
{
  check_params(params);
  display_mode = struct_val(params, "display_mode");
  if (display_mode == 0 || display_mode == 1 || display_mode == 2) 
  {
    // Make the outsides be up; only scaffolding marks on the inside
    if (display_mode == 0 || display_mode == 2)
    {
      draw_upper_half(params);
    }
    if (display_mode == 0 || display_mode == 1)
    {
      sphere_diameter = struct_val(params, "sphere_diameter");
      translate([sphere_diameter + 1, 0, 0])
      rotate([180, 0, 0])
      draw_lower_half(params);
    }
  }
  else if (display_mode == 3) 
  {
    draw_lower_half(params);
    draw_upper_half(params);
  }
  else if (display_mode == 4) 
  {
    // Show cutout
    difference() {
      draw_lower_half(params);
      translate([-500,0,-500])
        cube([1000, 100, 1000]);
    }
    difference() {
      draw_upper_half(params);
      translate([-500,0,-500])
        cube([1000, 100, 1000]);
    }
  } 
  else if (display_mode == 5) 
  {
    // Show cutout
    difference() {
      draw_lower_half(params);
      translate([0,-500,-500])
        cube([100, 1000, 1000]);
    }
    difference() {
      draw_upper_half(params);
      translate([0,-500,-500])
        cube([100, 1000, 1000]);
    }
  } 

}

//-----------------------------------------------------------------------
// Program starts here
//-----------------------------------------------------------------------
display(params);
