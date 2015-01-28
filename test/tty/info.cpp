
// usage: info <terminfo-file-path> [print-strings]
// P.S. Don't let this output to your terminal...

#include <Beard/config.hpp>
#include <Beard/tty/Caps.hpp>
#include <Beard/tty/TerminalInfo.hpp>

#include <iostream>
#include <iomanip>

#include "../common/common.hpp"

using Beard::tty::CapFlag;
using Beard::tty::CapNumber;
using Beard::tty::CapString;
using Beard::tty::TerminalInfo;

void
cout_flag(
	TerminalInfo const& term_info,
	CapFlag const cap,
	char const name[]
) {
	auto const value = term_info.cap_flag(cap);
	if (value) {
		std::cout
			<< name
			<< ": " << value
			<< '\n'
		;
	}
}

void
cout_number(
	TerminalInfo const& term_info,
	CapNumber const cap,
	char const name[]
) {
	auto const value = term_info.cap_number(cap);
	if (Beard::tty::CAP_NUMBER_NOT_SUPPORTED != value) {
		std::cout
			<< name
			<< ": " << value
			<< '\n'
		;
	}
}

void
cout_string(
	TerminalInfo const& term_info,
	CapString const cap,
	char const name[]
) {
	TerminalInfo::cap_string_map_type::const_iterator it;
	if (term_info.lookup_cap_string(cap, it)) {
		std::cout
			<< name
			<< ": " << it->second
			<< '\n'
		;
	}
}

#define BEARD_TEST_TTY_CAP_FLAG__(__c) \
	cout_flag(term_info, CapFlag:: __c, #__c)
//

#define BEARD_TEST_TTY_CAP_NUMBER__(__c) \
	cout_number(term_info, CapNumber:: __c, #__c)
//

#define BEARD_TEST_TTY_CAP_STRING__(__c) \
	cout_string(term_info, CapString:: __c, #__c)
//

signed
main(
	signed argc,
	char* argv[]
) {
	if (2 > argc || 3 < argc) {
		std::cerr <<
			"invalid arguments\n"
			"usage: info <terminfo-file-path> [print-strings]\n"
		;
		return -1;
	}

	char const* path = argv[1];

	bool print_strings = false;
	if (2 < argc) {
		print_strings = (0 != std::atoi(argv[2]));
	}

	TerminalInfo term_info{};
	if (!load_term_info(term_info, path)) {
		return -2;
	}

	std::cout << "names (" << term_info.names().size() << "):\n";
	for (auto const& name : term_info.names()) {
		std::cout << "  '" << name << "'\n";
	}

// flags
	std::cout << '\n';
	std::cout
		<< std::boolalpha
		<< "flags (" << term_info.cap_flag_count() << "):\n"
	;

	BEARD_TEST_TTY_CAP_FLAG__(auto_right_margin);
	BEARD_TEST_TTY_CAP_FLAG__(auto_left_margin);
	BEARD_TEST_TTY_CAP_FLAG__(auto_right_margin);
	BEARD_TEST_TTY_CAP_FLAG__(no_esc_ctlc);
	BEARD_TEST_TTY_CAP_FLAG__(ceol_standout_glitch);
	BEARD_TEST_TTY_CAP_FLAG__(eat_newline_glitch);
	BEARD_TEST_TTY_CAP_FLAG__(erase_overstrike);
	BEARD_TEST_TTY_CAP_FLAG__(generic_type);
	BEARD_TEST_TTY_CAP_FLAG__(hard_copy);
	BEARD_TEST_TTY_CAP_FLAG__(has_meta_key);
	BEARD_TEST_TTY_CAP_FLAG__(has_status_line);
	BEARD_TEST_TTY_CAP_FLAG__(insert_null_glitch);
	BEARD_TEST_TTY_CAP_FLAG__(memory_above);
	BEARD_TEST_TTY_CAP_FLAG__(memory_below);
	BEARD_TEST_TTY_CAP_FLAG__(move_insert_mode);
	BEARD_TEST_TTY_CAP_FLAG__(move_standout_mode);
	BEARD_TEST_TTY_CAP_FLAG__(over_strike);
	BEARD_TEST_TTY_CAP_FLAG__(status_line_esc_ok);
	BEARD_TEST_TTY_CAP_FLAG__(dest_tabs_magic_smso);
	BEARD_TEST_TTY_CAP_FLAG__(tilde_glitch);
	BEARD_TEST_TTY_CAP_FLAG__(transparent_underline);
	BEARD_TEST_TTY_CAP_FLAG__(xon_xoff);
	BEARD_TEST_TTY_CAP_FLAG__(needs_xon_xoff);
	BEARD_TEST_TTY_CAP_FLAG__(prtr_silent);
	BEARD_TEST_TTY_CAP_FLAG__(hard_cursor);
	BEARD_TEST_TTY_CAP_FLAG__(non_rev_rmcup);
	BEARD_TEST_TTY_CAP_FLAG__(no_pad_char);
	BEARD_TEST_TTY_CAP_FLAG__(non_dest_scroll_region);
	BEARD_TEST_TTY_CAP_FLAG__(can_change);
	BEARD_TEST_TTY_CAP_FLAG__(back_color_erase);
	BEARD_TEST_TTY_CAP_FLAG__(hue_lightness_saturation);
	BEARD_TEST_TTY_CAP_FLAG__(col_addr_glitch);
	BEARD_TEST_TTY_CAP_FLAG__(cr_cancels_micro_mode);
	BEARD_TEST_TTY_CAP_FLAG__(has_print_wheel);
	BEARD_TEST_TTY_CAP_FLAG__(row_addr_glitch);
	BEARD_TEST_TTY_CAP_FLAG__(semi_auto_right_margin);
	BEARD_TEST_TTY_CAP_FLAG__(cpi_changes_res);
	BEARD_TEST_TTY_CAP_FLAG__(lpi_changes_res);
	BEARD_TEST_TTY_CAP_FLAG__(backspaces_with_bs);
	BEARD_TEST_TTY_CAP_FLAG__(crt_no_scrolling);
	BEARD_TEST_TTY_CAP_FLAG__(no_correctly_working_cr);
	BEARD_TEST_TTY_CAP_FLAG__(gnu_has_meta_key);
	BEARD_TEST_TTY_CAP_FLAG__(linefeed_is_newline);
	BEARD_TEST_TTY_CAP_FLAG__(has_hardware_tabs);
	BEARD_TEST_TTY_CAP_FLAG__(return_does_clr_eol);

// numbers
	std::cout << '\n';
	std::cout
		<< std::boolalpha
		<< "numbers (" << term_info.cap_number_count() << "):\n"
	;

	BEARD_TEST_TTY_CAP_NUMBER__(columns);
	BEARD_TEST_TTY_CAP_NUMBER__(init_tabs);
	BEARD_TEST_TTY_CAP_NUMBER__(lines);
	BEARD_TEST_TTY_CAP_NUMBER__(lines_of_memory);
	BEARD_TEST_TTY_CAP_NUMBER__(magic_cookie_glitch);
	BEARD_TEST_TTY_CAP_NUMBER__(padding_baud_rate);
	BEARD_TEST_TTY_CAP_NUMBER__(virtual_terminal);
	BEARD_TEST_TTY_CAP_NUMBER__(width_status_line);
	BEARD_TEST_TTY_CAP_NUMBER__(num_labels);
	BEARD_TEST_TTY_CAP_NUMBER__(label_height);
	BEARD_TEST_TTY_CAP_NUMBER__(label_width);
	BEARD_TEST_TTY_CAP_NUMBER__(max_attributes);
	BEARD_TEST_TTY_CAP_NUMBER__(maximum_windows);
	BEARD_TEST_TTY_CAP_NUMBER__(max_colors);
	BEARD_TEST_TTY_CAP_NUMBER__(max_pairs);
	BEARD_TEST_TTY_CAP_NUMBER__(no_color_video);
	BEARD_TEST_TTY_CAP_NUMBER__(buffer_capacity);
	BEARD_TEST_TTY_CAP_NUMBER__(dot_vert_spacing);
	BEARD_TEST_TTY_CAP_NUMBER__(dot_horz_spacing);
	BEARD_TEST_TTY_CAP_NUMBER__(max_micro_address);
	BEARD_TEST_TTY_CAP_NUMBER__(max_micro_jump);
	BEARD_TEST_TTY_CAP_NUMBER__(micro_col_size);
	BEARD_TEST_TTY_CAP_NUMBER__(micro_line_size);
	BEARD_TEST_TTY_CAP_NUMBER__(number_of_pins);
	BEARD_TEST_TTY_CAP_NUMBER__(output_res_char);
	BEARD_TEST_TTY_CAP_NUMBER__(output_res_line);
	BEARD_TEST_TTY_CAP_NUMBER__(output_res_horz_inch);
	BEARD_TEST_TTY_CAP_NUMBER__(output_res_vert_inch);
	BEARD_TEST_TTY_CAP_NUMBER__(print_rate);
	BEARD_TEST_TTY_CAP_NUMBER__(wide_char_size);
	BEARD_TEST_TTY_CAP_NUMBER__(buttons);
	BEARD_TEST_TTY_CAP_NUMBER__(bit_image_entwining);
	BEARD_TEST_TTY_CAP_NUMBER__(bit_image_type);
	BEARD_TEST_TTY_CAP_NUMBER__(magic_cookie_glitch_ul);
	BEARD_TEST_TTY_CAP_NUMBER__(carriage_return_delay);
	BEARD_TEST_TTY_CAP_NUMBER__(new_line_delay);
	BEARD_TEST_TTY_CAP_NUMBER__(backspace_delay);
	BEARD_TEST_TTY_CAP_NUMBER__(horizontal_tab_delay);
	BEARD_TEST_TTY_CAP_NUMBER__(number_of_function_keys);

// strings
	std::cout << '\n';
	std::cout
		<< std::boolalpha
		<< "strings (" << term_info.cap_string_count() << "):\n"
	;

	if (!print_strings) {
		std::cout << "(skipping)\n";
	} else {
		BEARD_TEST_TTY_CAP_STRING__(back_tab);
		BEARD_TEST_TTY_CAP_STRING__(bell);
		BEARD_TEST_TTY_CAP_STRING__(carriage_return);
		BEARD_TEST_TTY_CAP_STRING__(change_scroll_region);
		BEARD_TEST_TTY_CAP_STRING__(clear_all_tabs);
		BEARD_TEST_TTY_CAP_STRING__(clear_screen);
		BEARD_TEST_TTY_CAP_STRING__(clr_eol);
		BEARD_TEST_TTY_CAP_STRING__(clr_eos);
		BEARD_TEST_TTY_CAP_STRING__(column_address);
		BEARD_TEST_TTY_CAP_STRING__(command_character);
		BEARD_TEST_TTY_CAP_STRING__(cursor_address);
		BEARD_TEST_TTY_CAP_STRING__(cursor_down);
		BEARD_TEST_TTY_CAP_STRING__(cursor_home);
		BEARD_TEST_TTY_CAP_STRING__(cursor_invisible);
		BEARD_TEST_TTY_CAP_STRING__(cursor_left);
		BEARD_TEST_TTY_CAP_STRING__(cursor_mem_address);
		BEARD_TEST_TTY_CAP_STRING__(cursor_normal);
		BEARD_TEST_TTY_CAP_STRING__(cursor_right);
		BEARD_TEST_TTY_CAP_STRING__(cursor_to_ll);
		BEARD_TEST_TTY_CAP_STRING__(cursor_up);
		BEARD_TEST_TTY_CAP_STRING__(cursor_visible);
		BEARD_TEST_TTY_CAP_STRING__(delete_character);
		BEARD_TEST_TTY_CAP_STRING__(delete_line);
		BEARD_TEST_TTY_CAP_STRING__(dis_status_line);
		BEARD_TEST_TTY_CAP_STRING__(down_half_line);
		BEARD_TEST_TTY_CAP_STRING__(enter_alt_charset_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_blink_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_bold_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_ca_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_delete_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_dim_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_insert_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_secure_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_protected_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_reverse_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_standout_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_underline_mode);
		BEARD_TEST_TTY_CAP_STRING__(erase_chars);
		BEARD_TEST_TTY_CAP_STRING__(exit_alt_charset_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_attribute_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_ca_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_delete_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_insert_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_standout_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_underline_mode);
		BEARD_TEST_TTY_CAP_STRING__(flash_screen);
		BEARD_TEST_TTY_CAP_STRING__(form_feed);
		BEARD_TEST_TTY_CAP_STRING__(from_status_line);
		BEARD_TEST_TTY_CAP_STRING__(init_1string);
		BEARD_TEST_TTY_CAP_STRING__(init_2string);
		BEARD_TEST_TTY_CAP_STRING__(init_3string);
		BEARD_TEST_TTY_CAP_STRING__(init_file);
		BEARD_TEST_TTY_CAP_STRING__(insert_character);
		BEARD_TEST_TTY_CAP_STRING__(insert_line);
		BEARD_TEST_TTY_CAP_STRING__(insert_padding);
		BEARD_TEST_TTY_CAP_STRING__(key_backspace);
		BEARD_TEST_TTY_CAP_STRING__(key_catab);
		BEARD_TEST_TTY_CAP_STRING__(key_clear);
		BEARD_TEST_TTY_CAP_STRING__(key_ctab);
		BEARD_TEST_TTY_CAP_STRING__(key_dc);
		BEARD_TEST_TTY_CAP_STRING__(key_dl);
		BEARD_TEST_TTY_CAP_STRING__(key_down);
		BEARD_TEST_TTY_CAP_STRING__(key_eic);
		BEARD_TEST_TTY_CAP_STRING__(key_eol);
		BEARD_TEST_TTY_CAP_STRING__(key_eos);
		BEARD_TEST_TTY_CAP_STRING__(key_f0);
		BEARD_TEST_TTY_CAP_STRING__(key_f1);
		BEARD_TEST_TTY_CAP_STRING__(key_f10);
		BEARD_TEST_TTY_CAP_STRING__(key_f2);
		BEARD_TEST_TTY_CAP_STRING__(key_f3);
		BEARD_TEST_TTY_CAP_STRING__(key_f4);
		BEARD_TEST_TTY_CAP_STRING__(key_f5);
		BEARD_TEST_TTY_CAP_STRING__(key_f6);
		BEARD_TEST_TTY_CAP_STRING__(key_f7);
		BEARD_TEST_TTY_CAP_STRING__(key_f8);
		BEARD_TEST_TTY_CAP_STRING__(key_f9);
		BEARD_TEST_TTY_CAP_STRING__(key_home);
		BEARD_TEST_TTY_CAP_STRING__(key_ic);
		BEARD_TEST_TTY_CAP_STRING__(key_il);
		BEARD_TEST_TTY_CAP_STRING__(key_left);
		BEARD_TEST_TTY_CAP_STRING__(key_ll);
		BEARD_TEST_TTY_CAP_STRING__(key_npage);
		BEARD_TEST_TTY_CAP_STRING__(key_ppage);
		BEARD_TEST_TTY_CAP_STRING__(key_right);
		BEARD_TEST_TTY_CAP_STRING__(key_sf);
		BEARD_TEST_TTY_CAP_STRING__(key_sr);
		BEARD_TEST_TTY_CAP_STRING__(key_stab);
		BEARD_TEST_TTY_CAP_STRING__(key_up);
		BEARD_TEST_TTY_CAP_STRING__(keypad_local);
		BEARD_TEST_TTY_CAP_STRING__(keypad_xmit);
		BEARD_TEST_TTY_CAP_STRING__(lab_f0);
		BEARD_TEST_TTY_CAP_STRING__(lab_f1);
		BEARD_TEST_TTY_CAP_STRING__(lab_f10);
		BEARD_TEST_TTY_CAP_STRING__(lab_f2);
		BEARD_TEST_TTY_CAP_STRING__(lab_f3);
		BEARD_TEST_TTY_CAP_STRING__(lab_f4);
		BEARD_TEST_TTY_CAP_STRING__(lab_f5);
		BEARD_TEST_TTY_CAP_STRING__(lab_f6);
		BEARD_TEST_TTY_CAP_STRING__(lab_f7);
		BEARD_TEST_TTY_CAP_STRING__(lab_f8);
		BEARD_TEST_TTY_CAP_STRING__(lab_f9);
		BEARD_TEST_TTY_CAP_STRING__(meta_off);
		BEARD_TEST_TTY_CAP_STRING__(meta_on);
		BEARD_TEST_TTY_CAP_STRING__(newline);
		BEARD_TEST_TTY_CAP_STRING__(pad_char);
		BEARD_TEST_TTY_CAP_STRING__(parm_dch);
		BEARD_TEST_TTY_CAP_STRING__(parm_delete_line);
		BEARD_TEST_TTY_CAP_STRING__(parm_down_cursor);
		BEARD_TEST_TTY_CAP_STRING__(parm_ich);
		BEARD_TEST_TTY_CAP_STRING__(parm_index);
		BEARD_TEST_TTY_CAP_STRING__(parm_insert_line);
		BEARD_TEST_TTY_CAP_STRING__(parm_left_cursor);
		BEARD_TEST_TTY_CAP_STRING__(parm_right_cursor);
		BEARD_TEST_TTY_CAP_STRING__(parm_rindex);
		BEARD_TEST_TTY_CAP_STRING__(parm_up_cursor);
		BEARD_TEST_TTY_CAP_STRING__(pkey_key);
		BEARD_TEST_TTY_CAP_STRING__(pkey_local);
		BEARD_TEST_TTY_CAP_STRING__(pkey_xmit);
		BEARD_TEST_TTY_CAP_STRING__(print_screen);
		BEARD_TEST_TTY_CAP_STRING__(prtr_off);
		BEARD_TEST_TTY_CAP_STRING__(prtr_on);
		BEARD_TEST_TTY_CAP_STRING__(repeat_char);
		BEARD_TEST_TTY_CAP_STRING__(reset_1string);
		BEARD_TEST_TTY_CAP_STRING__(reset_2string);
		BEARD_TEST_TTY_CAP_STRING__(reset_3string);
		BEARD_TEST_TTY_CAP_STRING__(reset_file);
		BEARD_TEST_TTY_CAP_STRING__(restore_cursor);
		BEARD_TEST_TTY_CAP_STRING__(row_address);
		BEARD_TEST_TTY_CAP_STRING__(save_cursor);
		BEARD_TEST_TTY_CAP_STRING__(scroll_forward);
		BEARD_TEST_TTY_CAP_STRING__(scroll_reverse);
		BEARD_TEST_TTY_CAP_STRING__(set_attributes);
		BEARD_TEST_TTY_CAP_STRING__(set_tab);
		BEARD_TEST_TTY_CAP_STRING__(set_window);
		BEARD_TEST_TTY_CAP_STRING__(tab);
		BEARD_TEST_TTY_CAP_STRING__(to_status_line);
		BEARD_TEST_TTY_CAP_STRING__(underline_char);
		BEARD_TEST_TTY_CAP_STRING__(up_half_line);
		BEARD_TEST_TTY_CAP_STRING__(init_prog);
		BEARD_TEST_TTY_CAP_STRING__(key_a1);
		BEARD_TEST_TTY_CAP_STRING__(key_a3);
		BEARD_TEST_TTY_CAP_STRING__(key_b2);
		BEARD_TEST_TTY_CAP_STRING__(key_c1);
		BEARD_TEST_TTY_CAP_STRING__(key_c3);
		BEARD_TEST_TTY_CAP_STRING__(prtr_non);
		BEARD_TEST_TTY_CAP_STRING__(char_padding);
		BEARD_TEST_TTY_CAP_STRING__(acs_chars);
		BEARD_TEST_TTY_CAP_STRING__(plab_norm);
		BEARD_TEST_TTY_CAP_STRING__(key_btab);
		BEARD_TEST_TTY_CAP_STRING__(enter_xon_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_xon_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_am_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_am_mode);
		BEARD_TEST_TTY_CAP_STRING__(xon_character);
		BEARD_TEST_TTY_CAP_STRING__(xoff_character);
		BEARD_TEST_TTY_CAP_STRING__(ena_acs);
		BEARD_TEST_TTY_CAP_STRING__(label_on);
		BEARD_TEST_TTY_CAP_STRING__(label_off);
		BEARD_TEST_TTY_CAP_STRING__(key_beg);
		BEARD_TEST_TTY_CAP_STRING__(key_cancel);
		BEARD_TEST_TTY_CAP_STRING__(key_close);
		BEARD_TEST_TTY_CAP_STRING__(key_command);
		BEARD_TEST_TTY_CAP_STRING__(key_copy);
		BEARD_TEST_TTY_CAP_STRING__(key_create);
		BEARD_TEST_TTY_CAP_STRING__(key_end);
		BEARD_TEST_TTY_CAP_STRING__(key_enter);
		BEARD_TEST_TTY_CAP_STRING__(key_exit);
		BEARD_TEST_TTY_CAP_STRING__(key_find);
		BEARD_TEST_TTY_CAP_STRING__(key_help);
		BEARD_TEST_TTY_CAP_STRING__(key_mark);
		BEARD_TEST_TTY_CAP_STRING__(key_message);
		BEARD_TEST_TTY_CAP_STRING__(key_move);
		BEARD_TEST_TTY_CAP_STRING__(key_next);
		BEARD_TEST_TTY_CAP_STRING__(key_open);
		BEARD_TEST_TTY_CAP_STRING__(key_options);
		BEARD_TEST_TTY_CAP_STRING__(key_previous);
		BEARD_TEST_TTY_CAP_STRING__(key_print);
		BEARD_TEST_TTY_CAP_STRING__(key_redo);
		BEARD_TEST_TTY_CAP_STRING__(key_reference);
		BEARD_TEST_TTY_CAP_STRING__(key_refresh);
		BEARD_TEST_TTY_CAP_STRING__(key_replace);
		BEARD_TEST_TTY_CAP_STRING__(key_restart);
		BEARD_TEST_TTY_CAP_STRING__(key_resume);
		BEARD_TEST_TTY_CAP_STRING__(key_save);
		BEARD_TEST_TTY_CAP_STRING__(key_suspend);
		BEARD_TEST_TTY_CAP_STRING__(key_undo);
		BEARD_TEST_TTY_CAP_STRING__(key_sbeg);
		BEARD_TEST_TTY_CAP_STRING__(key_scancel);
		BEARD_TEST_TTY_CAP_STRING__(key_scommand);
		BEARD_TEST_TTY_CAP_STRING__(key_scopy);
		BEARD_TEST_TTY_CAP_STRING__(key_screate);
		BEARD_TEST_TTY_CAP_STRING__(key_sdc);
		BEARD_TEST_TTY_CAP_STRING__(key_sdl);
		BEARD_TEST_TTY_CAP_STRING__(key_select);
		BEARD_TEST_TTY_CAP_STRING__(key_send);
		BEARD_TEST_TTY_CAP_STRING__(key_seol);
		BEARD_TEST_TTY_CAP_STRING__(key_sexit);
		BEARD_TEST_TTY_CAP_STRING__(key_sfind);
		BEARD_TEST_TTY_CAP_STRING__(key_shelp);
		BEARD_TEST_TTY_CAP_STRING__(key_shome);
		BEARD_TEST_TTY_CAP_STRING__(key_sic);
		BEARD_TEST_TTY_CAP_STRING__(key_sleft);
		BEARD_TEST_TTY_CAP_STRING__(key_smessage);
		BEARD_TEST_TTY_CAP_STRING__(key_smove);
		BEARD_TEST_TTY_CAP_STRING__(key_snext);
		BEARD_TEST_TTY_CAP_STRING__(key_soptions);
		BEARD_TEST_TTY_CAP_STRING__(key_sprevious);
		BEARD_TEST_TTY_CAP_STRING__(key_sprint);
		BEARD_TEST_TTY_CAP_STRING__(key_sredo);
		BEARD_TEST_TTY_CAP_STRING__(key_sreplace);
		BEARD_TEST_TTY_CAP_STRING__(key_sright);
		BEARD_TEST_TTY_CAP_STRING__(key_srsume);
		BEARD_TEST_TTY_CAP_STRING__(key_ssave);
		BEARD_TEST_TTY_CAP_STRING__(key_ssuspend);
		BEARD_TEST_TTY_CAP_STRING__(key_sundo);
		BEARD_TEST_TTY_CAP_STRING__(req_for_input);
		BEARD_TEST_TTY_CAP_STRING__(key_f11);
		BEARD_TEST_TTY_CAP_STRING__(key_f12);
		BEARD_TEST_TTY_CAP_STRING__(key_f13);
		BEARD_TEST_TTY_CAP_STRING__(key_f14);
		BEARD_TEST_TTY_CAP_STRING__(key_f15);
		BEARD_TEST_TTY_CAP_STRING__(key_f16);
		BEARD_TEST_TTY_CAP_STRING__(key_f17);
		BEARD_TEST_TTY_CAP_STRING__(key_f18);
		BEARD_TEST_TTY_CAP_STRING__(key_f19);
		BEARD_TEST_TTY_CAP_STRING__(key_f20);
		BEARD_TEST_TTY_CAP_STRING__(key_f21);
		BEARD_TEST_TTY_CAP_STRING__(key_f22);
		BEARD_TEST_TTY_CAP_STRING__(key_f23);
		BEARD_TEST_TTY_CAP_STRING__(key_f24);
		BEARD_TEST_TTY_CAP_STRING__(key_f25);
		BEARD_TEST_TTY_CAP_STRING__(key_f26);
		BEARD_TEST_TTY_CAP_STRING__(key_f27);
		BEARD_TEST_TTY_CAP_STRING__(key_f28);
		BEARD_TEST_TTY_CAP_STRING__(key_f29);
		BEARD_TEST_TTY_CAP_STRING__(key_f30);
		BEARD_TEST_TTY_CAP_STRING__(key_f31);
		BEARD_TEST_TTY_CAP_STRING__(key_f32);
		BEARD_TEST_TTY_CAP_STRING__(key_f33);
		BEARD_TEST_TTY_CAP_STRING__(key_f34);
		BEARD_TEST_TTY_CAP_STRING__(key_f35);
		BEARD_TEST_TTY_CAP_STRING__(key_f36);
		BEARD_TEST_TTY_CAP_STRING__(key_f37);
		BEARD_TEST_TTY_CAP_STRING__(key_f38);
		BEARD_TEST_TTY_CAP_STRING__(key_f39);
		BEARD_TEST_TTY_CAP_STRING__(key_f40);
		BEARD_TEST_TTY_CAP_STRING__(key_f41);
		BEARD_TEST_TTY_CAP_STRING__(key_f42);
		BEARD_TEST_TTY_CAP_STRING__(key_f43);
		BEARD_TEST_TTY_CAP_STRING__(key_f44);
		BEARD_TEST_TTY_CAP_STRING__(key_f45);
		BEARD_TEST_TTY_CAP_STRING__(key_f46);
		BEARD_TEST_TTY_CAP_STRING__(key_f47);
		BEARD_TEST_TTY_CAP_STRING__(key_f48);
		BEARD_TEST_TTY_CAP_STRING__(key_f49);
		BEARD_TEST_TTY_CAP_STRING__(key_f50);
		BEARD_TEST_TTY_CAP_STRING__(key_f51);
		BEARD_TEST_TTY_CAP_STRING__(key_f52);
		BEARD_TEST_TTY_CAP_STRING__(key_f53);
		BEARD_TEST_TTY_CAP_STRING__(key_f54);
		BEARD_TEST_TTY_CAP_STRING__(key_f55);
		BEARD_TEST_TTY_CAP_STRING__(key_f56);
		BEARD_TEST_TTY_CAP_STRING__(key_f57);
		BEARD_TEST_TTY_CAP_STRING__(key_f58);
		BEARD_TEST_TTY_CAP_STRING__(key_f59);
		BEARD_TEST_TTY_CAP_STRING__(key_f60);
		BEARD_TEST_TTY_CAP_STRING__(key_f61);
		BEARD_TEST_TTY_CAP_STRING__(key_f62);
		BEARD_TEST_TTY_CAP_STRING__(key_f63);
		BEARD_TEST_TTY_CAP_STRING__(clr_bol);
		BEARD_TEST_TTY_CAP_STRING__(clear_margins);
		BEARD_TEST_TTY_CAP_STRING__(set_left_margin);
		BEARD_TEST_TTY_CAP_STRING__(set_right_margin);
		BEARD_TEST_TTY_CAP_STRING__(label_format);
		BEARD_TEST_TTY_CAP_STRING__(set_clock);
		BEARD_TEST_TTY_CAP_STRING__(display_clock);
		BEARD_TEST_TTY_CAP_STRING__(remove_clock);
		BEARD_TEST_TTY_CAP_STRING__(create_window);
		BEARD_TEST_TTY_CAP_STRING__(goto_window);
		BEARD_TEST_TTY_CAP_STRING__(hangup);
		BEARD_TEST_TTY_CAP_STRING__(dial_phone);
		BEARD_TEST_TTY_CAP_STRING__(quick_dial);
		BEARD_TEST_TTY_CAP_STRING__(tone);
		BEARD_TEST_TTY_CAP_STRING__(pulse);
		BEARD_TEST_TTY_CAP_STRING__(flash_hook);
		BEARD_TEST_TTY_CAP_STRING__(fixed_pause);
		BEARD_TEST_TTY_CAP_STRING__(wait_tone);
		BEARD_TEST_TTY_CAP_STRING__(user0);
		BEARD_TEST_TTY_CAP_STRING__(user1);
		BEARD_TEST_TTY_CAP_STRING__(user2);
		BEARD_TEST_TTY_CAP_STRING__(user3);
		BEARD_TEST_TTY_CAP_STRING__(user4);
		BEARD_TEST_TTY_CAP_STRING__(user5);
		BEARD_TEST_TTY_CAP_STRING__(user6);
		BEARD_TEST_TTY_CAP_STRING__(user7);
		BEARD_TEST_TTY_CAP_STRING__(user8);
		BEARD_TEST_TTY_CAP_STRING__(user9);
		BEARD_TEST_TTY_CAP_STRING__(orig_pair);
		BEARD_TEST_TTY_CAP_STRING__(orig_colors);
		BEARD_TEST_TTY_CAP_STRING__(initialize_color);
		BEARD_TEST_TTY_CAP_STRING__(initialize_pair);
		BEARD_TEST_TTY_CAP_STRING__(set_color_pair);
		BEARD_TEST_TTY_CAP_STRING__(set_foreground);
		BEARD_TEST_TTY_CAP_STRING__(set_background);
		BEARD_TEST_TTY_CAP_STRING__(change_char_pitch);
		BEARD_TEST_TTY_CAP_STRING__(change_line_pitch);
		BEARD_TEST_TTY_CAP_STRING__(change_res_horz);
		BEARD_TEST_TTY_CAP_STRING__(change_res_vert);
		BEARD_TEST_TTY_CAP_STRING__(define_char);
		BEARD_TEST_TTY_CAP_STRING__(enter_doublewide_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_draft_quality);
		BEARD_TEST_TTY_CAP_STRING__(enter_italics_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_leftward_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_micro_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_near_letter_quality);
		BEARD_TEST_TTY_CAP_STRING__(enter_normal_quality);
		BEARD_TEST_TTY_CAP_STRING__(enter_shadow_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_subscript_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_superscript_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_upward_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_doublewide_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_italics_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_leftward_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_micro_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_shadow_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_subscript_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_superscript_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_upward_mode);
		BEARD_TEST_TTY_CAP_STRING__(micro_column_address);
		BEARD_TEST_TTY_CAP_STRING__(micro_down);
		BEARD_TEST_TTY_CAP_STRING__(micro_left);
		BEARD_TEST_TTY_CAP_STRING__(micro_right);
		BEARD_TEST_TTY_CAP_STRING__(micro_row_address);
		BEARD_TEST_TTY_CAP_STRING__(micro_up);
		BEARD_TEST_TTY_CAP_STRING__(order_of_pins);
		BEARD_TEST_TTY_CAP_STRING__(parm_down_micro);
		BEARD_TEST_TTY_CAP_STRING__(parm_left_micro);
		BEARD_TEST_TTY_CAP_STRING__(parm_right_micro);
		BEARD_TEST_TTY_CAP_STRING__(parm_up_micro);
		BEARD_TEST_TTY_CAP_STRING__(select_char_set);
		BEARD_TEST_TTY_CAP_STRING__(set_bottom_margin);
		BEARD_TEST_TTY_CAP_STRING__(set_bottom_margin_parm);
		BEARD_TEST_TTY_CAP_STRING__(set_left_margin_parm);
		BEARD_TEST_TTY_CAP_STRING__(set_right_margin_parm);
		BEARD_TEST_TTY_CAP_STRING__(set_top_margin);
		BEARD_TEST_TTY_CAP_STRING__(set_top_margin_parm);
		BEARD_TEST_TTY_CAP_STRING__(start_bit_image);
		BEARD_TEST_TTY_CAP_STRING__(start_char_set_def);
		BEARD_TEST_TTY_CAP_STRING__(stop_bit_image);
		BEARD_TEST_TTY_CAP_STRING__(stop_char_set_def);
		BEARD_TEST_TTY_CAP_STRING__(subscript_characters);
		BEARD_TEST_TTY_CAP_STRING__(superscript_characters);
		BEARD_TEST_TTY_CAP_STRING__(these_cause_cr);
		BEARD_TEST_TTY_CAP_STRING__(zero_motion);
		BEARD_TEST_TTY_CAP_STRING__(char_set_names);
		BEARD_TEST_TTY_CAP_STRING__(key_mouse);
		BEARD_TEST_TTY_CAP_STRING__(mouse_info);
		BEARD_TEST_TTY_CAP_STRING__(req_mouse_pos);
		BEARD_TEST_TTY_CAP_STRING__(get_mouse);
		BEARD_TEST_TTY_CAP_STRING__(set_a_foreground);
		BEARD_TEST_TTY_CAP_STRING__(set_a_background);
		BEARD_TEST_TTY_CAP_STRING__(pkey_plab);
		BEARD_TEST_TTY_CAP_STRING__(device_type);
		BEARD_TEST_TTY_CAP_STRING__(code_set_init);
		BEARD_TEST_TTY_CAP_STRING__(set0_des_seq);
		BEARD_TEST_TTY_CAP_STRING__(set1_des_seq);
		BEARD_TEST_TTY_CAP_STRING__(set2_des_seq);
		BEARD_TEST_TTY_CAP_STRING__(set3_des_seq);
		BEARD_TEST_TTY_CAP_STRING__(set_lr_margin);
		BEARD_TEST_TTY_CAP_STRING__(set_tb_margin);
		BEARD_TEST_TTY_CAP_STRING__(bit_image_repeat);
		BEARD_TEST_TTY_CAP_STRING__(bit_image_newline);
		BEARD_TEST_TTY_CAP_STRING__(bit_image_carriage_return);
		BEARD_TEST_TTY_CAP_STRING__(color_names);
		BEARD_TEST_TTY_CAP_STRING__(define_bit_image_region);
		BEARD_TEST_TTY_CAP_STRING__(end_bit_image_region);
		BEARD_TEST_TTY_CAP_STRING__(set_color_band);
		BEARD_TEST_TTY_CAP_STRING__(set_page_length);
		BEARD_TEST_TTY_CAP_STRING__(display_pc_char);
		BEARD_TEST_TTY_CAP_STRING__(enter_pc_charset_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_pc_charset_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_scancode_mode);
		BEARD_TEST_TTY_CAP_STRING__(exit_scancode_mode);
		BEARD_TEST_TTY_CAP_STRING__(pc_term_options);
		BEARD_TEST_TTY_CAP_STRING__(scancode_escape);
		BEARD_TEST_TTY_CAP_STRING__(alt_scancode_esc);
		BEARD_TEST_TTY_CAP_STRING__(enter_horizontal_hl_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_left_hl_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_low_hl_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_right_hl_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_top_hl_mode);
		BEARD_TEST_TTY_CAP_STRING__(enter_vertical_hl_mode);
		BEARD_TEST_TTY_CAP_STRING__(set_a_attributes);
		BEARD_TEST_TTY_CAP_STRING__(set_pglen_inch);
		BEARD_TEST_TTY_CAP_STRING__(termcap_init2);
		BEARD_TEST_TTY_CAP_STRING__(termcap_reset);
		BEARD_TEST_TTY_CAP_STRING__(linefeed_if_not_lf);
		BEARD_TEST_TTY_CAP_STRING__(backspace_if_not_bs);
		BEARD_TEST_TTY_CAP_STRING__(other_non_function_keys);
		BEARD_TEST_TTY_CAP_STRING__(arrow_key_map);
		BEARD_TEST_TTY_CAP_STRING__(acs_ulcorner);
		BEARD_TEST_TTY_CAP_STRING__(acs_llcorner);
		BEARD_TEST_TTY_CAP_STRING__(acs_urcorner);
		BEARD_TEST_TTY_CAP_STRING__(acs_lrcorner);
		BEARD_TEST_TTY_CAP_STRING__(acs_ltee);
		BEARD_TEST_TTY_CAP_STRING__(acs_rtee);
		BEARD_TEST_TTY_CAP_STRING__(acs_btee);
		BEARD_TEST_TTY_CAP_STRING__(acs_ttee);
		BEARD_TEST_TTY_CAP_STRING__(acs_hline);
		BEARD_TEST_TTY_CAP_STRING__(acs_vline);
		BEARD_TEST_TTY_CAP_STRING__(acs_plus);
		BEARD_TEST_TTY_CAP_STRING__(memory_lock);
		BEARD_TEST_TTY_CAP_STRING__(memory_unlock);
		BEARD_TEST_TTY_CAP_STRING__(box_chars_1);
	}

//
	return 0;
}
