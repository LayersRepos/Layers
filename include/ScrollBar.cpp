#include "ScrollBar.h"

using Layers::ScrollBar;

ScrollBar::ScrollBar(QWidget* parent) : QScrollBar(parent)
{
	init_attributes();

	set_name("scrollbar");
	set_proper_name("ScrollBar");

	update_theme_dependencies();

	//if (orientation() == Qt::Vertical)
	//	setFixedWidth(45);
	//else if (orientation() == Qt::Horizontal)
	//	setFixedHeight(45);
}

void ScrollBar::apply_theme_attributes(QMap<QString, AttributeType*>& theme_attrs)
{
	a_background_color.copy_value_from(*dynamic_cast<Attribute*>(theme_attrs["background_color"]));
	corner_radii.copy_from(*dynamic_cast<AttributeGroup*>(theme_attrs["corner_radii"]));
	handle_corner_radii.copy_from(*dynamic_cast<AttributeGroup*>(theme_attrs["handle_corner_radii"]));
	a_handle_color.copy_value_from(*dynamic_cast<Attribute*>(theme_attrs["handle_color"]));
}

void ScrollBar::update_theme_dependencies()
{
	setStyleSheet(build_stylesheet());
}

QString ScrollBar::build_stylesheet()
{
	return
		/* VERTICAL */
		"QScrollBar:vertical {"
		//"border-left: 1px solid black;"
		"background: " + a_background_color.as<QColor>().name() + ";"
		"width: 45px;"
		"border-top-left-radius: " + corner_radii.top_left.as<QString>() + "px;"
		"border-top-right-radius: " + corner_radii.top_right.as<QString>() + "px;"
		"border-bottom-left-radius: " + corner_radii.bottom_left.as<QString>() + "px;"
		"border-bottom-right-radius: " + corner_radii.bottom_right.as<QString>() + "px;"
		"margin: 0px 0px 0px 0px;"
		"}"

		"QScrollBar::handle:vertical {"
		"background: " + a_handle_color.as<QColor>().name() + ";"
		"min-height: 26px;"
		"border-top-left-radius: " + handle_corner_radii.top_left.as<QString>() + "px;"
		"border-top-right-radius: " + handle_corner_radii.top_right.as<QString>() + "px;"
		"border-bottom-left-radius: " + handle_corner_radii.bottom_left.as<QString>() + "px;"
		"border-bottom-right-radius: " + handle_corner_radii.bottom_right.as<QString>() + "px;"
		"margin: 10px 10px 10px 10px;"
		"}"

		"QScrollBar::add-line:vertical {"
		"border: none;"
		"background: none;"
		"}"

		"QScrollBar::sub-line:vertical {"
		"height: 0px;"
		"width: 0px;"
		"border: none;"
		"background: none;"
		"}"

		"QScrollBar:up-arrow:vertical, QScrollBar::down-arrow:vertical {"
		"border: none;"
		"background: none;"
		"color: none;"
		"}"

		"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
		"background: none;"
		"}"

		/* HORIZONTAL */
		"QScrollBar:horizontal {"
		//"border-top: 1px solid black;"
		"background: " + a_background_color.as<QColor>().name() + ";"
		"height: 45px;"
		"border-top-left-radius: " + corner_radii.top_left.as<QString>() + "px;"
		"border-top-right-radius: " + corner_radii.top_right.as<QString>() + "px;"
		"border-bottom-left-radius: " + corner_radii.bottom_left.as<QString>() + "px;"
		"border-bottom-right-radius: " + corner_radii.bottom_right.as<QString>() + "px;"
		"margin: 0px 0px 0px 0px;"
		"}"

		"QScrollBar::handle:horizontal {"
		"background: " + a_handle_color.as<QColor>().name() + ";"
		"min-width: 26px;"
		"border-top-left-radius: " + handle_corner_radii.top_left.as<QString>() + "px;"
		"border-top-right-radius: " + handle_corner_radii.top_right.as<QString>() + "px;"
		"border-bottom-left-radius: " + handle_corner_radii.bottom_left.as<QString>() + "px;"
		"border-bottom-right-radius: " + handle_corner_radii.bottom_right.as<QString>() + "px;"
		"margin: 10px 10px 10px 10px;"
		"}"

		"QScrollBar::add-line:horizontal {"
		"border: none;"
		"background: none;"
		"}"

		"QScrollBar::sub-line:horizontal {"
		"height: 0px;"
		"width: 0px;"
		"border: none;"
		"background: none;"
		"}"

		"QScrollBar:up-arrow:horizontal, QScrollBar::down-arrow:horizontal {"
		"border: none;"
		"background: none;"
		"color: none;"
		"}"

		"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
		"background: none;"
		"}";
}

void ScrollBar::init_attributes()
{
	corner_radii.bottom_right.set_value(10.0);

	handle_corner_radii.top_left.set_value(5.0);
	handle_corner_radii.top_right.set_value(5.0);
	handle_corner_radii.bottom_left.set_value(5.0);
	handle_corner_radii.bottom_right.set_value(5.0);

	m_attributes.insert({
		{ "background_color", &a_background_color },
		{ "corner_radii", &corner_radii },
		{ "handle_corner_radii", &handle_corner_radii },
		{ "handle_color", &a_handle_color }
		});

	m_attribute_layout.append(&a_background_color);
	m_attribute_layout.append(&corner_radii);
	m_attribute_layout.append(&a_handle_color);
	m_attribute_layout.append(&handle_corner_radii);

	connect(&a_background_color, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&corner_radii, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&corner_radii.top_left, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&corner_radii.top_right, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&corner_radii.bottom_left, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&corner_radii.bottom_right, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&handle_corner_radii, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&handle_corner_radii.top_left, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&handle_corner_radii.top_right, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&handle_corner_radii.bottom_left, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&handle_corner_radii.bottom_right, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
	connect(&a_handle_color, &AttributeType::value_changed, [this] { update_theme_dependencies(); });
}