#pragma once

enum KErrorCode
{
	err_success = 0,
	err_worldType_not_found,
	err_position_out_of_map,
	err_player_not_found,
	err_world_not_found,
	err_dbagent_not_avail,
	err_add_player_to_game,
	err_load_player_data,
	err_state_refuse_backToCharacter,
	err_worldSever_not_avail,
	err_acct_not_found,
	err_password_mismatch,
	err_already_in_acct_login,
	err_otp_mismatch,
	err_invalid_sql_statement,
	err_execute_sql,
	err_create_character,
};
