/*************************************************************************/
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "sg_rigid_body_2d.h"
#include "sg_character_body_2d.h"

#define DELTA fixed(2185)
#define HALF fixed(32768)
#define ONE fixed(65536)
#define TWO fixed(65536 * 2)

using namespace godot;

fixed rk4(fixed integrated_forces_axis, fixed x0, fixed y0, fixed v0, fixed _x,
          int n);

SGRigidBody2D::SGRigidBody2D() {
  gravity = ClassDB::instantiate("SGFixedVector2");
}

void SGRigidBody2D::_bind_methods() {
  ClassDB::bind_method(D_METHOD("get_angular_velocity"),
                       &SGRigidBody2D::get_angular_velocity);
  ClassDB::bind_method(D_METHOD("set_angular_velocity", "p_angular_velocity"),
                       &SGRigidBody2D::set_angular_velocity);
  ADD_PROPERTY(
      PropertyInfo(Variant::INT, "angular_velocity", PROPERTY_HINT_NONE),
      "set_angular_velocity", "get_angular_velocity");
  ClassDB::bind_method(D_METHOD("get_gravity_x"),
                       &SGRigidBody2D::get_gravity_x);
  ClassDB::bind_method(D_METHOD("set_gravity_x", "p_gravity"),
                       &SGRigidBody2D::set_gravity_x);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "gravity_x", PROPERTY_HINT_NONE),
               "set_gravity_x", "get_gravity_x");
  ClassDB::bind_method(D_METHOD("get_gravity_y"),
                       &SGRigidBody2D::get_gravity_y);
  ClassDB::bind_method(D_METHOD("set_gravity_y", "p_gravity"),
                       &SGRigidBody2D::set_gravity_y);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "gravity_y", PROPERTY_HINT_NONE),
               "set_gravity_y", "get_gravity_y");
  ClassDB::bind_method(D_METHOD("get_mass"), &SGRigidBody2D::get_mass);
  ClassDB::bind_method(D_METHOD("set_mass", "p_mass"),
                       &SGRigidBody2D::set_mass);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "mass", PROPERTY_HINT_NONE),
               "set_mass", "get_mass");
  ClassDB::bind_method(D_METHOD("get_bounciness"),
                       &SGRigidBody2D::get_bounciness);
  ClassDB::bind_method(D_METHOD("set_bounciness", "p_bounciness"),
                       &SGRigidBody2D::set_bounciness);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "bounciness", PROPERTY_HINT_NONE),
               "set_bounciness", "get_bounciness");
  ClassDB::bind_method(D_METHOD("get_linear_damping"),
                       &SGRigidBody2D::get_linear_damping);
  ClassDB::bind_method(D_METHOD("set_linear_damping", "p_linear_damping"),
                       &SGRigidBody2D::set_linear_damping);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "linear_damping", PROPERTY_HINT_NONE),
               "set_linear_damping", "get_linear_damping");
  ClassDB::bind_method(D_METHOD("get_angular_damping"),
                       &SGRigidBody2D::get_angular_damping);
  ClassDB::bind_method(D_METHOD("set_angular_damping", "p_angular_damping"),
                       &SGRigidBody2D::set_angular_damping);
  ADD_PROPERTY(
      PropertyInfo(Variant::INT, "angular_damping", PROPERTY_HINT_NONE),
      "set_angular_damping", "get_angular_damping");
  ClassDB::bind_method(D_METHOD("get_gravity_scale"),
                       &SGRigidBody2D::get_gravity_scale);
  ClassDB::bind_method(D_METHOD("set_gravity_scale", "p_gravity_scale"),
                       &SGRigidBody2D::set_gravity_scale);
  ADD_PROPERTY(PropertyInfo(Variant::INT, "gravity_scale", PROPERTY_HINT_NONE),
               "set_gravity_scale", "get_gravity_scale");
  ClassDB::bind_method(D_METHOD("get_freeze"), &SGRigidBody2D::get_freeze);
  ClassDB::bind_method(D_METHOD("set_freeze", "p_freeze"),
                       &SGRigidBody2D::set_freeze);
  ADD_PROPERTY(PropertyInfo(Variant::BOOL, "freeze", PROPERTY_HINT_NONE),
               "set_freeze", "get_freeze");

  ClassDB::bind_method(D_METHOD("clear_forces"), &SGRigidBody2D::clear_forces);
  ClassDB::bind_method(D_METHOD("apply_central_force", "force"),
                       &SGRigidBody2D::apply_central_force);
  ClassDB::bind_method(D_METHOD("apply_central_impulse", "force"),
                       &SGRigidBody2D::apply_central_impulse);
  ClassDB::bind_method(D_METHOD("apply_impulse", "force", "position"),
                       &SGRigidBody2D::apply_impulse);
  ClassDB::bind_method(D_METHOD("apply_force", "force", "position"),
                       &SGRigidBody2D::apply_force);
  ClassDB::bind_method(D_METHOD("move_and_slide"),
                       &SGRigidBody2D::move_and_slide);
}

// Properties
void SGRigidBody2D::set_angular_velocity(int p_angular_velocity) {
  angular_velocity = fixed(p_angular_velocity);
}
int SGRigidBody2D::get_angular_velocity() { return angular_velocity.value; }
void SGRigidBody2D::set_gravity_x(int p_gravity) { gravity->set_x(p_gravity); }
int SGRigidBody2D::get_gravity_x() { return gravity->get_x(); }
void SGRigidBody2D::set_gravity_y(int p_gravity) { gravity->set_y(p_gravity); }
int SGRigidBody2D::get_gravity_y() { return gravity->get_y(); }
void SGRigidBody2D::set_mass(int p_mass) { mass = fixed(p_mass); }
int SGRigidBody2D::get_mass() { return mass.value; }
void SGRigidBody2D::set_bounciness(int p_bounciness) {
  bounciness = fixed(p_bounciness);
}
int SGRigidBody2D::get_bounciness() { return bounciness.value; }
void SGRigidBody2D::set_linear_damping(int p_linear_damping) {
  linear_damping = fixed(p_linear_damping);
}
int SGRigidBody2D::get_linear_damping() { return linear_damping.value; }
void SGRigidBody2D::set_gravity_scale(int p_gravity_scale) {
  gravity_scale = p_gravity_scale;
}
int SGRigidBody2D::get_gravity_scale() { return gravity_scale; }
void SGRigidBody2D::set_angular_damping(int p_angular_damping) {
  angular_damping = fixed(p_angular_damping);
}
int SGRigidBody2D::get_angular_damping() { return angular_damping.value; }
void SGRigidBody2D::set_freeze(bool p_freeze) { freeze = p_freeze; }
bool SGRigidBody2D::get_freeze() { return freeze; }

bool SGRigidBody2D::move_and_slide() {
  if (freeze) {
    return false;
  }

  Ref<SGFixedVector2> g = ClassDB::instantiate("SGFixedVector2");
  g->set_x((gravity->get_x() * gravity_scale) >> 16);
  g->set_y((gravity->get_y() * gravity_scale) >> 16);
  apply_central_force(g);

  fixed prev_velocity_x = fixed(velocity->get_x());
  fixed prev_velocity_y = fixed(velocity->get_y());

  integrate_forces();
  call("_integrate_forces");

  bool move_and_slide_output = SGCharacterBody2D::move_and_slide();

  collide(prev_velocity_x, prev_velocity_y);

  clear_forces();

  return move_and_slide_output;
}

void SGRigidBody2D::clear_forces() {
  forces.clear();
  impulses.clear();
  angular_impulses.clear();
}

void SGRigidBody2D::apply_force(Ref<SGFixedVector2> force,
                                Ref<SGFixedVector2> position) {
  forces.push_back(force);
}

void SGRigidBody2D::apply_impulse(Ref<SGFixedVector2> force,
                                  Ref<SGFixedVector2> position) {
  if (position->length() > 0) {
    apply_angular_impulse(force, position);
  }
  impulses.push_back(force);
}

void SGRigidBody2D::apply_angular_impulse(Ref<SGFixedVector2> force,
                                          Ref<SGFixedVector2> position) {
  fixed moi = mass * fixed(position->length() * position->length());
  if (moi < fixed(100)) {
    return;
  }
  fixed torque = fixed(position->get_y() * force->get_x() -
                       position->get_x() * force->get_y());
  fixed angular_acceleration = torque / moi;
  angular_impulses.push_back(angular_acceleration);
}

void SGRigidBody2D::apply_central_impulse(Ref<SGFixedVector2> force) {
  Ref<SGFixedVector2> v = ClassDB::instantiate("SGFixedVector2");
  v->set_x(0);
  v->set_y(0);
  apply_impulse(force, v);
}

void SGRigidBody2D::apply_central_force(Ref<SGFixedVector2> force) {
  Ref<SGFixedVector2> v = ClassDB::instantiate("SGFixedVector2");
  v->set_x(0);
  v->set_y(0);
  apply_force(force, v);
}

void SGRigidBody2D::integrate_forces() {
  Ref<SGFixedVector2> integrated_forces =
      ClassDB::instantiate("SGFixedVector2");

  for (int i = 0; i < forces.size(); i++) {
    integrated_forces->set_x(integrated_forces->get_x() + forces[i]->get_x());
    integrated_forces->set_y(integrated_forces->get_y() + forces[i]->get_y());
  }

  fixed x_approximation =
      rk4(fixed(integrated_forces->get_x()), fixed(0), fixed(0),
          fixed(velocity->get_x() + integrated_forces->get_x()), DELTA, 4);
  fixed y_approximation =
      rk4(fixed(integrated_forces->get_y()), fixed(0), fixed(0),
          fixed(velocity->get_y() + integrated_forces->get_y()), DELTA, 4);

  for (int i = 0; i < impulses.size(); i++) {
    x_approximation += fixed(impulses[i]->get_x());
    y_approximation += fixed(impulses[i]->get_y());
  }

  fixed angular_velocity_sum = fixed(0);

  for (int i = 0; i < angular_impulses.size(); i++) {
    angular_velocity_sum = angular_velocity_sum + angular_impulses[i];
  }

  fixed velocity_x = (x_approximation - x_approximation * linear_damping);
  fixed velocity_y = (y_approximation - y_approximation * linear_damping);

  if (velocity_x > fixed(1966080)) {
    velocity_x = fixed(1966080);
  }
  if (velocity_x < fixed(-1966080)) {
    velocity_x = fixed(-1966080);
  }
  if (velocity_y > fixed(1966080)) {
    velocity_y = fixed(1966080);
  }
  if (velocity_y < fixed(-1966080)) {
    velocity_y = fixed(-1966080);
  }

  velocity->set_x(velocity_x.value);
  velocity->set_y(velocity_y.value);
}

void SGRigidBody2D::collide(fixed prev_velocity_x, fixed prev_velocity_y) {
  int slide_coll_count = get_slide_count();

  // Skip initializing objects and computations if there is no need
  if (slide_coll_count == 0) {
    return;
  }

  fixed x_force = fixed(0);
  fixed y_force = fixed(0);

  Ref<SGFixedVector2> prev_velocity = ClassDB::instantiate("SGFixedVector2");
  prev_velocity->set_x(prev_velocity_x.value);
  prev_velocity->set_y(prev_velocity_y.value);

  for (int i = 0; i < slide_coll_count; i++) {
    Ref<SGKinematicCollision2D> c = get_slide_collision(i);
    fixed dot_c = fixed(prev_velocity->dot(c->get_normal()));
    Ref<SGFixedVector2> UP = ClassDB::instantiate("SGFixedVector2");
    UP->set_x(0);
    UP->set_y(ONE.value);
    fixed c_up = fixed(c->get_normal()->dot(UP));
    fixed body_up = fixed(prev_velocity->normalized()->dot(UP));
    fixed d = (bounciness + ONE) * dot_c;
    x_force = x_force + d * fixed(c->get_normal()->get_x());
    y_force = y_force + d * fixed(c->get_normal()->get_y());
  }

  velocity->set_x((fixed(prev_velocity->get_x()) - x_force).value);
  velocity->set_y((fixed(prev_velocity->get_y()) - y_force).value);
}

fixed rk4(fixed integrated_forces_axis, fixed x0, fixed y0, fixed v0, fixed _x,
          int n) {
  fixed h = DELTA / (fixed(n) << 16);
  fixed y = y0;
  fixed v = v0;

  for (int i = 0; i < n; i++) {
    fixed k1_x = h * v;
    fixed k1_v = h * integrated_forces_axis;
    fixed k2_x = h * (v + (HALF * k1_v));
    fixed k2_v = h * integrated_forces_axis;
    fixed k3_x = h * (v + (HALF * k2_v));
    fixed k3_v = h * integrated_forces_axis;
    fixed k4_x = h * (v + k3_v);
    fixed k4_v = h * integrated_forces_axis;

    y += fixed(10923) * (k1_x + TWO * k2_x + TWO * k3_x + k4_x);
    v += fixed(10923) * (k1_v + TWO * k2_v + TWO * k3_v + k4_v);

    x0 += h;
  }

  return v;
}

// Deconstructor
SGRigidBody2D::~SGRigidBody2D() {}

