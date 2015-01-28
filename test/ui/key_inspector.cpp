
#include <Beard/utility.hpp>
#include <Beard/Error.hpp>
#include <Beard/keys.hpp>
#include <Beard/tty/Defs.hpp>
#include <Beard/tty/Ops.hpp>
#include <Beard/tty/Terminal.hpp>
#include <Beard/ui/Defs.hpp>
#include <Beard/ui/Context.hpp>
#include <Beard/ui/Root.hpp>
#include <Beard/ui/Container.hpp>
#include <Beard/ui/Widget/Defs.hpp>
#include <Beard/ui/Widget/Base.hpp>

#include <ceformat/Format.hpp>
#include <ceformat/print.hpp>

#include <duct/IO/memstream.hpp>

#include <cassert>
#include <utility>
#include <iostream>

#include "../common/common.hpp"

using namespace Beard;

enum class CustomWidgetType : unsigned {
	BASE = enum_cast(ui::Widget::Type::USERSPACE_BASE) + 0,
	KeyInspector,
};

static txt::Sequence const
s_key_code_names[]{
	"none",
	"esc",
	"backspace",
	"enter",
	"insert",
	"del",
	"home",
	"end",
	"pgup",
	"pgdn",
	"up",
	"down",
	"left",
	"right",
	"tab",
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"f11",
	"f12"
};

class KeyInspector final
	: public ui::Widget::Base
{
private:
	using base_type = ui::Widget::Base;

public:
	using SPtr = aux::shared_ptr<KeyInspector>;

private:
	enum class ctor_priv {};

	KeyInputData m_kid{};

	KeyInspector() noexcept = delete;
	KeyInspector(KeyInspector const&) = delete;
	KeyInspector& operator=(KeyInspector const&) = delete;

	bool
	handle_event_impl(
		ui::Event const& event
	) noexcept override;

	void
	render_impl(
		ui::Widget::RenderData& rd
	) noexcept override;

public:
	~KeyInspector() noexcept override = default;

	KeyInspector(
		ctor_priv const,
		ui::group_hash_type const group,
		ui::RootWPtr&& root,
		ui::Widget::WPtr&& parent
	) noexcept
		: base_type(
			static_cast<ui::Widget::Type>(
				CustomWidgetType::KeyInspector
			),
			ui::Widget::Flags::visible,
			group,
			{{1, 1}, false, Axis::both, Axis::both},
			std::move(root),
			std::move(parent)
		)
	{}

	static KeyInspector::SPtr
	make(
		ui::RootWPtr root,
		ui::group_hash_type const group = ui::group_label,
		ui::Widget::WPtr parent = ui::Widget::WPtr()
	) {
		auto p = aux::make_shared<KeyInspector>(
			ctor_priv{},
			group,
			std::move(root),
			std::move(parent)
		);
		return p;
	}

	KeyInspector(KeyInspector&&) = default;
	KeyInspector& operator=(KeyInspector&&) = default;

	void
	set_text(
		String text
	);
};

bool
KeyInspector::handle_event_impl(
	ui::Event const& event
) noexcept {
	if (event.type != ui::EventType::key_input) {
		return false;
	}
	m_kid = event.key_input;
	enqueue_actions(ui::UpdateActions::render);
	return true;
}

static constexpr ceformat::Format const
s_cp_format{"%08x '%s'"};

void
KeyInspector::render_impl(
	ui::Widget::RenderData& rd
) noexcept {
	auto const& frame = geometry().frame();

	bool const has_alt = enum_cast(m_kid.mod & KeyMod::esc);
	bool const has_ctrl = enum_cast(m_kid.mod & KeyMod::ctrl);
	bool const has_shift = enum_cast(m_kid.mod & KeyMod::shift);

	tty::attr_type const
		inactive_fg = rd.attr(ui::property_content_fg_inactive),
		inactive_bg = rd.attr(ui::property_content_bg_inactive),
		active_fg = rd.attr(ui::property_content_fg_selected),
		active_bg = rd.attr(ui::property_content_bg_selected)
	;

	rd.terminal.put_sequence(
		frame.pos.x + 0, frame.pos.y,
		txt::Sequence{"alt"}, 3,
		has_alt ? active_fg : inactive_fg,
		has_alt ? active_bg : inactive_bg
	);
	rd.terminal.put_sequence(
		frame.pos.x + 3 + 1, frame.pos.y,
		txt::Sequence{"shift"}, 5,
		has_shift ? active_fg : inactive_fg,
		has_shift ? active_bg : inactive_bg
	);
	rd.terminal.put_sequence(
		frame.pos.x + 3 + 1 + 5 + 1, frame.pos.y,
		txt::Sequence{"ctrl"}, 4,
		has_ctrl ? active_fg : inactive_fg,
		has_ctrl ? active_bg : inactive_bg
	);

	auto const& kc_seq = s_key_code_names[enum_cast(m_kid.code)];
	rd.terminal.put_sequence(
		frame.pos.x + 3 + 1 + 5 + 1 + 4 + 1, frame.pos.y,
		kc_seq, kc_seq.size,
		inactive_fg,
		inactive_bg
	);

	char cp_info[32];
	duct::IO::omemstream format_stream{cp_info, sizeof(cp_info)};
	txt::UTF8Block cp_units{m_kid.cp};
	ceformat::write<s_cp_format>(
		format_stream,
		m_kid.cp,
		m_kid.cp == codepoint_none
		? String{}
		: String{cp_units.units, cp_units.size()}
	);
	txt::Sequence cp_seq{cp_info, static_cast<unsigned>(format_stream.tellp())};
	rd.terminal.put_sequence(
		frame.pos.x + 3 + 1 + 5 + 1 + 4 + 1 + kc_seq.size + 1, frame.pos.y,
		cp_seq, cp_seq.size,
		inactive_fg,
		inactive_bg
	);
}

signed
main(
	signed argc,
	char* argv[]
) {
	if (2 > argc || 3 < argc) {
		std::cerr <<
			"invalid arguments\n"
			"usage: dynamic_focus terminfo-file-path [tty-path]\n"
		;
		return -1;
	}

	ui::Context ctx;
	tty::Terminal& term = ctx.terminal();

	char const* const info_path = argv[1];
	if (!load_term_info(term.info(), info_path)) {
		return -2;
	}
	term.update_cache();

	bool use_sigwinch = false;
	String tty_path{};
	if (2 < argc) {
		tty_path.assign(argv[2]);
	} else {
		tty_path.assign(tty::this_path());
		use_sigwinch = true;
	}

	try {
		ctx.open(tty_path, use_sigwinch);
	} catch (Error const& ex) {
		report_error(ex);
		return 1;
	}

	auto root = ui::Root::make(ctx, Axis::vertical);
	ctx.set_root(root);

	auto const key_inspector = KeyInspector::make(root);
	root->push_back(key_inspector);
	root->set_focus(key_inspector);

	auto& pmap = ctx.property_map().find(ui::group_default)->second;
	pmap.find(ui::property_frame_debug_enabled)->second.set_boolean(false);

	ctx.render(true);
	while (true) {
		ctx.update(15u);
		auto const& event = ctx.last_event();
		if (ui::EventType::key_input != event.type) {
			continue;
		} else if (key_input_match(event.key_input, s_kim_c)) {
			break;
		}
	}
	ctx.close();
	return 0;
}
