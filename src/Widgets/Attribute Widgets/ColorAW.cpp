#include "../../../include/AttributeWidgets.h"

using Layers::Attribute;
using Layers::ColorAW;
using Layers::ColorControl;
using Layers::Theme;

ColorAW::ColorAW(Attribute* attribute, QWidget* parent) :
	m_attribute_label{ new Label(attribute->capitalized_name()) },
	AttributeWidget(attribute, parent)
{
	init_child_themeable_reference_list();

	//m_attribute = attribute;

	// Setup Attribute Label
	m_attribute_label->set_name("label");
	m_attribute_label->set_proper_name("Label");
	m_attribute_label->set_font_size(14);
	m_attribute_label->set_padding(0, 7, 0, 0);

	// Setup Left Stretch
	m_left_stretch->a_fill.set_disabled();
	m_left_stretch->hide();

	// Setup Right Stretch
	m_right_stretch->a_fill.set_disabled();

	connect(m_disabled_toggle, &ToggleSwitch::toggled_event, [this, attribute] {
		if (m_disabled_toggle->toggled())
		{
			m_color_control->show();

			attribute->set_disabled(false);
		}
		else
		{
			m_color_control->hide();

			attribute->set_disabled();
		}
	});

	if (!attribute->disabled()) m_disabled_toggle->toggle();
	else m_color_control->hide();

	// Setup Color Control
	m_color_control->a_fill.get_data_from(*attribute);

	// Setup Layout
	QHBoxLayout* hbox = new QHBoxLayout;
	hbox->setContentsMargins(8, 0, 8, 0);
	hbox->setSpacing(0);
	hbox->addWidget(m_left_stretch);
	hbox->addWidget(m_attribute_label);
	hbox->addSpacing(14);
	hbox->addWidget(m_disabled_toggle);
	hbox->addSpacing(6);
	hbox->addWidget(m_color_control);
	hbox->addWidget(m_right_stretch);
	setLayout(hbox);
}

ColorControl* ColorAW::color_control() const
{
	return m_color_control;
}

void ColorAW::set_centered(bool centered)
{
	if (m_centered != centered)
	{
		m_centered = centered;

		if (m_centered) m_left_stretch->show();
		else m_left_stretch->hide();
	}
}

void ColorAW::set_current_editting_state(const QString& state)
{
	m_color_control->set_current_editting_state(state);
}

void ColorAW::init_child_themeable_reference_list()
{
	store_child_themeable_pointer(m_attribute_label);
	if (m_disabled_toggle) store_child_themeable_pointer(m_disabled_toggle);
}
