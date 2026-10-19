/*************************************************************************/
/* Copyright (c) 2021-2022 David Snopek                                  */
/*                                                                       */
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

#ifndef SG_RIGID_BODY_2D_H
#define SG_RIGID_BODY_2D_H

#include <godot_cpp/templates/vector.hpp>

#include "sg_character_body_2d.h"
#include "sg_physics_body_2d.h"

using namespace godot;

class SGRigidBody2D : public SGCharacterBody2D {
	GDCLASS(SGRigidBody2D, SGCharacterBody2D);

    fixed angular_velocity;
	fixed mass;
	fixed bounciness;
	fixed linear_damping;
	fixed angular_damping;
	int gravity_scale;
	Ref<SGFixedVector2> gravity;
	fixed delta = (fixed) 2185;
	bool freeze = false;
    std::vector<Ref<SGFixedVector2>> forces;
    std::vector<Ref<SGFixedVector2>> impulses;
    std::vector<fixed> angular_impulses;

    void integrate_forces();
    void collide(fixed prev_velocit_x, fixed prev_velocit_y);

protected:
	static void _bind_methods();

public:
    void set_angular_velocity(int angular_velocity);
    int get_angular_velocity();
    void set_mass(int mass);
    int get_mass();
    void set_bounciness(int bounciness);
    int get_bounciness();
    void set_linear_damping(int linear_damping);
    int get_linear_damping();
    void set_angular_damping(int angular_damping);
    int get_angular_damping();
    void set_gravity_scale(int gravity_scale);
    int get_gravity_scale();
    void set_gravity_x(int gravity);
    int get_gravity_x();
    void set_gravity_y(int gravity);
    int get_gravity_y();
    void set_freeze(bool freeze);
    bool get_freeze();
    
    bool move_and_slide();
    void clear_forces();

    void apply_force(Ref<SGFixedVector2> force, Ref<SGFixedVector2> position);
    void apply_impulse(Ref<SGFixedVector2> force, Ref<SGFixedVector2> position);
    void apply_angular_impulse(Ref<SGFixedVector2> force, Ref<SGFixedVector2> position);
    void apply_central_force(Ref<SGFixedVector2> force);
    void apply_central_impulse(Ref<SGFixedVector2> force);
    
	SGRigidBody2D();
	~SGRigidBody2D();
};

#endif
