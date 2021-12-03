/*
 * Self_Pos.hpp
 *
 *  Created on: 7 May 2021
 *
 *@Author: Ayato Yoshida
 *
 *@Purpose_of_this_class:(get self position, direction)
 *
 *@Input(address of hi2c(e.g. &hi2c1))in(I2C_HandleTypeDef* hi2c_device)of(void BNO055_Init_I2C)
 *@Input(address of hi2c(e.g. &hi2c1))in(I2C_HandleTypeDef* hi2c_device)of(void BNO055_update_gravity_direction)
 *@Input(robot name in enum_class E_robot_name)in(E_robot_name)of(void set_initial_direction())
 *@Input(value)in(variable)of(functionname)
 *@Input(value)in(variable)of(functionname)
 *
 *@Output(gravity value(usually 9.8))to(float gravity)
 *@Output(direction value(0 to 360))to(int16_t direction)
 *
 *@Attention_(you have to use Gyro::BNO_Init_I2C function in main function before diving into while(1){}.)
 *@Attention_(you have to initialise before using void get_direction())
 *
 *@Usertouch(Gyro::BNO_Init_I2C())
 *@Usertouch(Gyro::BNO_update_gravity())
 *@Usertouch(Gyro::get_gravity())
 *@Usertouch(Gyro::get_direction())
 *@Usertouch(Gyro::set_direction(E_robot_name robot))
 *
 */
#ifndef INC_SELF_POS_HPP_
#define INC_SELF_POS_HPP_

#include "main.h"

typedef struct
{
	double x = 0,
	double y = 0,
}vector;

#define ODM_RADIUS
#define ODM_PPR
#deifine ODM_CIR 2 * ODM_RADIUS * M_PI

class Self_Pos
{
public:
	vector get_self_pos(void);
private:
	double encoder_read_5(void);
	double encoder_read_2(void);


	static double Self_Pos_X;
	static double Self_Pos_Y;
};



#endif /* INC_SELF_POS_HPP_ */


