/*
 * Rope.hpp
 *
 *  Created on: 8 Jul 2021
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
#ifndef INC_ROPE_HPP_
#define INC_ROPE_HPP_

#define RMN 1

class Rope
{
public:
	static int over_flow_cnt_3;

	int encoder_read_3( bool rope );
	void stop_rope(uint8_t motor_number);
	void rotate_rope(uint8_t motor_number, uint8_t direction, uint16_t down_speed, uint16_t up_speed);

	void Encoder_val_TX( void );
	void Encoder_val_RX( int* rope, uint8_t* received_data );

	static bool rope_enable;

private:

	static bool run_C;

};

#endif /* INC_ROPE_HPP_ */
