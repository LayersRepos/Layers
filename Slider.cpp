#include "Layers.h"

using Layers::Slider;

Slider::Slider(int range_start, int range_end, QWidget* parent) :
	m_range_start{ range_start }, m_range_end{ range_end },
	Widget(parent)
{
	init_attributes();
	init_child_themeable_reference_list();

	installEventFilter(this);
	setFixedSize(244, 45);

	m_bar->setFixedHeight(5);
	m_bar->set_name("bar");
	m_bar->set_margin(23, 0, 23, 0);

	m_handle->setFixedSize(45, 45);
	m_handle->set_name("handle");
	m_handle->set_margin(15, 5, 15, 5);

	setup_layout();
}

void Slider::init_attributes()
{
	set_attribute_value("background_disabled", true);
	add_attribute("value", 0);

	m_bar->set_attribute_value("corner_radius_tl", 2);
	m_bar->set_attribute_value("corner_radius_tr", 2);
	m_bar->set_attribute_value("corner_radius_bl", 2);
	m_bar->set_attribute_value("corner_radius_br", 2);

	m_handle->set_attribute_value("corner_radius_tl", 3);
	m_handle->set_attribute_value("corner_radius_tr", 3);
	m_handle->set_attribute_value("corner_radius_bl", 3);
	m_handle->set_attribute_value("corner_radius_br", 3);
}

void Slider::init_child_themeable_reference_list()
{
	add_child_themeable_reference(m_bar);
	add_child_themeable_reference(m_handle);
}

int Slider::range_difference()
{
	return m_range_end - m_range_start;
}

void Slider::update_handle_pos()
{
	double drag_increment = double(width() - m_handle->width()) / double(range_difference());

	m_handle->move(drag_increment * attributes()["value"].value().value<int>(), m_handle->y());
}

void Slider::update_theme_dependencies()
{
	update_handle_pos();
}

bool Slider::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress && m_handle->underMouse())
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

		if (mouse_event->button() & Qt::LeftButton)
		{
			m_dragging_handle = true;

			m_mouse_click_position = mouse_event->pos();
			m_value_on_click = attributes()["value"].value().value<int>();
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease && m_handle->underMouse())
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

		if (mouse_event->button() & Qt::LeftButton)
		{
			m_dragging_handle = false;
		}
	}

	if (event->type() == QEvent::MouseMove && m_dragging_handle == true)
	{
		QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

		QPoint delta = mouse_event->pos() - m_mouse_click_position;

		double drag_increment = double(m_bar->width() - m_handle->width()) / double(range_difference());

		int new_value = m_value_on_click + int(delta.x() / drag_increment);

		if (new_value < m_range_start)
		{
			if (attributes()["value"].value().value<int>() != m_range_start)
			{
				set_attribute_value("value", m_range_start);
				update_handle_pos();
				share_attributes();
			}
		}
		else if (new_value > m_range_end)
		{
			if (attributes()["value"].value().value<int>() != m_range_end)
			{
				set_attribute_value("value", m_range_end);
				update_handle_pos();
				share_attributes();
			}
		}
		else
		{
			set_attribute_value("value", new_value);
			update_handle_pos();
			share_attributes();
		}
	}

	return false;
}

void Slider::setup_layout()
{
	QVBoxLayout* main_layout = new QVBoxLayout;

	main_layout->setMargin(0);
	main_layout->setSpacing(0);
	main_layout->addWidget(m_bar);

	setLayout(main_layout);

	m_handle->raise();
}
