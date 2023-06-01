#include "HeaderView.h"

#include <QPainter>

using Layers::BorderAttributes;
using Layers::HeaderView;

HeaderView::HeaderView(Qt::Orientation orientation, QWidget* parent) :
	QHeaderView(orientation, parent)
{
	init_attributes();
	update_stylesheet();

	if (orientation == Qt::Horizontal)
		set_name("Horizontal Header");
	else
		set_name("Vertical Header");
}

void HeaderView::init_attributes()
{
	m_attributes.insert({
		{ "border", m_border },
		{ "corner_radii", m_corner_radii },
		{ "fill", m_fill },
		{ "text_color", m_text_color }
		});

	connect(m_border->fill(), &AbstractAttribute::changed, [this] { update_stylesheet(); });
	connect(m_border->thickness(), &AbstractAttribute::changed, [this] { update_stylesheet(); });
	connect(m_corner_radii->top_left(), &AbstractAttribute::changed, [this] { update_stylesheet(); });
	connect(m_corner_radii->top_right(), &AbstractAttribute::changed, [this] { update_stylesheet(); });
	connect(m_corner_radii->bottom_left(), &AbstractAttribute::changed, [this] { update_stylesheet(); });
	connect(m_corner_radii->bottom_right(), &AbstractAttribute::changed, [this] { update_stylesheet(); });
	connect(m_fill, &AbstractAttribute::changed, [this] { update_stylesheet(); });
	connect(m_text_color, &AbstractAttribute::changed, [this] { update_stylesheet(); });

	m_corner_radii->top_left()->set_value(2.0);
	m_corner_radii->top_right()->set_value(2.0);
	m_fill->set_value(QColor("#808080"));
}

void HeaderView::update_stylesheet()
{
	setStyleSheet(
		"QHeaderView {"
		"background: " + m_fill->as<QColor>().name() + ";"
		"border-top-left-radius: " + m_corner_radii->top_left()->as<QString>() + "px;"
		"border-top-right-radius: " + m_corner_radii->top_right()->as<QString>() + "px;"
		"border-bottom-left-radius: " + m_corner_radii->bottom_left()->as<QString>() + "px;"
		"border-bottom-right-radius: " + m_corner_radii->bottom_right()->as<QString>() + "px;"
		"color: " + m_text_color->as<QColor>().name() + ";"
		"}"

		"QHeaderView::section {"
		"background: transparent;"
		"}"
	);

	update();
}
