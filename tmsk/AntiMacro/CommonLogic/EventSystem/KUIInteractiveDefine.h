#ifndef _K_UI_INTERACTIVE_DEFINE_H_
#define _K_UI_INTERACTIVE_DEFINE_H_

const int ui_target_player = 1;
const int ui_target_activity = 2;
const int ui_target_player_world = 3;

template <int x> void _export_ui_interactive_to_lua()
{
	export_enum_to_lua(ui_target_player);
	export_enum_to_lua(ui_target_activity);
	export_enum_to_lua(ui_target_player_world);
}
#define export_ui_interactive_to_lua() _export_ui_interactive_to_lua<0>()

#endif
