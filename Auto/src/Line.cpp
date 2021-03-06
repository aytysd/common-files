/*
 * Line.hpp
 *
 *  Created on: 2021/05/08
 *
 *@Author: nakakubo hiroto
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
#include "Line.hpp"
#include "math.h"
#include "PWM.hpp"
#include "Self_Pos.hpp"
#include <stdio.h>
#include "Gyro.hpp"
#include "General_command.hpp"
#include "MPU6050.hpp"
#include "Debug.hpp"
#include "usart.h"

double Line::integral_diff = 0;

int16_t Line::A_pos_x = 0;
int16_t Line::A_pos_y = 0;

int Line::AftX = 0;
int Line::AftY = 0;

int Line::BefX = 0;
int Line::BefY = 0;

E_Line_status Line::judge = E_Line_status::STOP;
bool Line::through = false;
bool Line::Jump = false;

double direction = 0;

bool Line::Enable_line = false;

void Line::set(int befX, int befY, int tgX, int tgY)
{
	this -> a = -(tgY - befY);
	this -> b = tgX - befX;
	this -> c = -a*tgX - b*tgY;

	this -> sqrtAABB = sqrt((long double)(a*a + b*b));
}

double Line::distance(int x, int y, int tgX, int tgY)
{
	if(sqrtAABB == 0)
	{
		sqrtAABB = 1;
	}
	return ((a*x) + (b*y) + c) / (int)sqrtAABB;
}

double Line::TGdistance(int x, int y, int tgX, int tgY)
{
	if(sqrtAABB == 0)
	{
		sqrtAABB = 1;
	}
	return ((-b*x) + (a*y) - (((-b)*tgX) + (a*tgY))) /(int) sqrtAABB;
}

void Line::MoveLine(void)
{
	if( Line::judge != E_Line_status::STOP)
	{
		int befX = Line::BefX;
		int befY = Line::BefY;
		int tgX = Line::AftX;
		int tgY = Line::AftY;
		bool through = Line::through;

		this -> set(befX, befY, tgX, tgY);

		Self_Pos* self_pos = new Self_Pos();
		this -> now_X = self_pos -> get_Self_Pos_X();
		this -> now_Y = self_pos -> get_Self_Pos_Y();
		delete self_pos;

		this -> devX = this -> distance(this -> now_X, this -> now_Y, tgX, tgY);
		double FM_devX;
//		FM_devX = this -> distance(this -> now_X, this -> now_Y, tgX, tgY);
//		FM_devX *= -1;
//		Debug::TTO_val((int16_t)devX, "devX:", &huart2);
		this -> devY = this -> TGdistance(this -> now_X, this -> now_Y, tgX, tgY);
		this -> devX = fabs(devX);
		this -> devY = fabs(devY);

		this -> devTG = sqrt((long double)(this -> devX*this -> devX + this -> devY*this -> devY));

		if((tgX == now_X) && (tgY > now_Y))
		{
			TG_r = 90;
		}
		else if((tgX == now_X) && (tgY < now_Y))
		{
			TG_r = 270;
		}
		else
		{
			if((tgX - this -> now_X) == 0)
			{
				tgX = 1;
			}
			this -> TG_r = atan(((long double)(tgY - this -> now_Y) / (long double)(tgX - this -> now_X)));
			this -> TG_r = ((long double)TG_r * (long double)180) / M_PI;
			if(tgX < this -> now_X)
			{
				TG_r = TG_r + 180;
			}
			else if(this -> TG_r < 0)
			{
				TG_r = TG_r + 360;
			}
			else if(this -> TG_r >= 360)
			{
				TG_r = TG_r - 360;
			}
		}

		if(( befX == tgX ) && ( befY < tgY ))
		{
			Line_r = 90;
		}
		else if(( befX == tgX ) && ( befY > tgY ))
		{
			Line_r = 270;
		}
		else
		{
			if( ( tgX - befX ) == 0 )
			{
				tgX = 1;
			}
			this -> Line_r = atan(((long double)( tgY - befY ) / (long double)( tgX - befX )));
			this -> Line_r = ((long double)Line_r * (long double)180) / M_PI;
			if( tgX < befX )
			{
				Line_r = Line_r + 180;
			}
			else if(this -> Line_r < 0)
			{
				Line_r = Line_r + 360;
			}
			else if(this -> Line_r >= 360)
			{
				Line_r = Line_r - 360;
			}
		}

//		dev_r = Line_r - TG_r ;
//		if( ( 270 < dev_r ) && ( dev_r < 360 ) )
//		{
//			dev_r = dev_r - 360;
//		}
//		dev_r *= (long double)0.3;
//		TG_r = TG_r - dev_r;

		MPU6050* gyro = new MPU6050();
		this -> now_r = (double)gyro -> get_direction();
		delete gyro;

//		direction = this -> now_r;
//		direction = TG_r - direction; //+ -> 3,4 down  //- -> 1,2 down

		bool arrive = false;

		if(through == true)
		{
			if(this -> devY <= 500)
			{
				arrive = true;
			}
		}
		else if( through == false )
		{
			bool inX = abs(tgX - now_X) <= 100;
			bool inY = abs(tgY - now_Y) <= 100;

			if(inX && inY)
			{
				arrive = true;
			}
			else
			{
				arrive = false;
			}
		}

		this -> TG_v = this -> devTG * 0.2;
		if( through != true )
		{
			if(this -> TG_v > 600)
			{
				this -> TG_v = 600;
			}
			else if( this -> TG_v < 500 )
			{
				this -> TG_v = 500;
			}
		}
		if(through == true)
		{
			this -> TG_v += 400;
			if( this -> TG_v > 600 )
			{
				this  -> TG_v = 600;
			}
			else if( this -> TG_v < 500 )
			{
				this -> TG_v = 500;
			}
		}


		if( Line::Jump == true )
			Line::TG_v = 1000;

//		Debug::TTO_val((uint16_t)TG_r, "TG_r", &huart2);

		if(arrive == true)
		{
			if(through == true)
			{
				PWM* pwm = new PWM();

//				pwm -> Front_Move( 600, (uint16_t)this -> TG_r, (uint16_t)this -> now_r, FM_devX, E_move_status::MOVE );
			    pwm -> V_output(600, (uint16_t)this -> TG_r, 0, (uint16_t)this -> now_r, E_move_status::MOVE);
				judge = E_Line_status::THROUGHING;
//				Line::Enable_line = false;

				delete pwm;

			}
			else
			{
				PWM* pwm = new PWM();

//				pwm -> Front_Move( this -> TG_v, (uint16_t)this -> TG_r, (uint16_t)this -> now_r, FM_devX, E_move_status::STOP);
			    pwm -> V_output(this -> TG_v, (uint16_t)this -> TG_r, 0, (uint16_t)this -> now_r, E_move_status::STOP);
				judge = E_Line_status::STOP;
//				Line::Enable_line = false;

				delete pwm;
			}
		}
		else if( arrive == false )
		{
			PWM* pwm = new PWM();

//			pwm -> Front_Move( this -> TG_v, (uint16_t)this -> TG_r, (uint16_t)this -> now_r, FM_devX, E_move_status::MOVE);
			pwm -> V_output(this -> TG_v, (uint16_t)this -> TG_r, 0, (uint16_t)this -> now_r, E_move_status::MOVE);
			judge = E_Line_status::MOVING;

			delete pwm;
		}
	}
}

void Line::Line_driver(int befX, int befY, int tgX, int tgY, bool through, bool Jump )
{
	Line::judge = E_Line_status::MOVING;
	Line::BefX = befX;
	Line::BefY = befY;
	Line::AftX = tgX;
	Line::AftY = tgY;
	Line::through = through;
	Line::Jump = Jump;
	Line::Enable_line = true;

	Line::Enable_line = true;

	Line::integral_diff = 0;
}

void Line::Line_init(int init_pos_X, int init_pos_Y)
{
	Line::BefX = init_pos_X;
	Line::BefY = init_pos_Y;
	Line::AftX = init_pos_X;
	Line::AftY = init_pos_Y;
	Line::judge = E_Line_status::STOP;
	Line::Jump = false;
	Line::through = false;
	Line::Enable_line = true;
}

double Line::speed_PID( vector robot_A, vector self_pos )
{
	return this -> P( robot_A, self_pos ) + this -> I( robot_A, self_pos ) - this -> D( robot_A, self_pos );
}

double Line::P( vector robot_A, vector self_pos )
{
	return Kp * this -> get_distance( robot_A, self_pos );
}

double Line::I( vector robot_A, vector self_pos )
{

		Line::integral_diff += this -> get_distance( robot_A, self_pos );
		return Ki * Line::integral_diff;


}

double Line::D( vector robot_A, vector self_pos )
{
	static double prev_diff;

	double dev = this -> get_distance( robot_A, self_pos ) - prev_diff;
	prev_diff = this -> get_distance( robot_A, self_pos );

	return Kd * dev;

}

double Line::get_distance( vector A, vector B )
{

    double distance = sqrt( ( ( A.X - B.X ) *  ( A.X - B.X ) ) + ( ( A.Y - B.Y ) * ( A.Y - B.Y ) ) );
    return distance;

}


