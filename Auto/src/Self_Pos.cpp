/*
 * Self_Pos.cpp
 *
 *  Created on: 7 May 2021
 *
 *@Author: Ayato Yoshida
 *
 *@Purpose_of_this_class:(description_about_thisclass)
 *
 *@Input(value)in(variable)of(functionname)
 *
 *@Output(value)to(where)
 *
 *@Attention_(description)
 *
 *@Usertouch(functionname)&_(variable_name)
 *
 */

#include "main.h"
#include "Self_Pos.hpp"
#include "stdio.h"
#include <math.h>
#include "Gyro.hpp"

double Self_Pos::Self_pos_X = 0;
double Self_Pos::Self_pos_Y = 0;

using x_odm() = encoder_read_2();
using y_odm() = encoder_read_5();

vector Self_Pos::get_self_pos(void)
{
	MPU6050* gyro = new MPU6050();
	vector self_pos;

	double d1 = d1 = ODM_CIR * ( ( double )this -> y_odm() / ( double ) ODM_PPR ); //encoder5_moving distance(mm) 55.5=wheel radius 2048=encoder resolution
	double d2 = d2 = ODM_CIR * ( ( double )this -> x_odm() / ( double ) ODM_PPR ); //encoder5_moving distance(mm) 55.5=wheel radius 2048=encoder resolution


	self_pos.x = this -> Self_Pos_X += d1 * cos( M_PI / 2 + gyro -> get_direction()  ) + d2 * cos( gyro -> get_direction() ); //X_coordinate
	self_pos.y = this -> Self_Pos_Y += d1 * sin( M_PI / 2 + gyro -> get_direction()  ) + d2 * sin( gyro -> get_direction() ); //Y_coordinate

	delete gyro;

	return self_pos;
}
//delete self_pos;
//---------------------------------------
//---------------------------------------
double Self_Pos::encoder_read_5(void)
{
	static int prev_encoder_value = 0;
	uint32_t enc_buff_5 = TIM5->CNT;
	int encoder_value = 0;

	if (enc_buff_5 <= 2147483647) encoder_value = enc_buff_5;
	else if (enc_buff_5 >= 2147483649)  encoder_value = enc_buff_5 - 4294967295;

	int encoder_diff = encoder_value - prev_encoder_value;

	prev_encoder_value = encoder_value;

	return encoder_diff;

}
//------------------------------------------------------
double Self_Pos::encoder_read_2(void)
{

	static double prev_encoder_value = 0;
	uint32_t enc_buff_2 = TIM2->CNT;
	double encoder_value = 0;


	if (enc_buff_2 <= 2147483647) encoder_value = enc_buff_2;
	else if (enc_buff_2 >= 2147483649) encoder_value = enc_buff_2 - 4294967295;

	double encoder_diff = encoder_value - prev_encoder_value;

	prev_encoder_value = encoder_value;

	return encoder_diff;

}


double Self_Pos::rad(double deg)
{
	return ( ( deg ) / ( double )180 ) * ( double )M_PI;
}

