#include <LayersWidgets/lcoloreditor.h>

using Layers::LAttribute;
using Layers::LColorEditor;
using Layers::LColorControl;
using Layers::LTheme;

LColorEditor::LColorEditor(LAttribute* attribute, QWidget* parent) :
	LAttributeEditor(attribute, parent)
{
	// Setup Attribute Label
	m_attribute_label->set_name("Label");
	m_attribute_label->set_font_size(14);
	if (attribute)
		m_attribute_label->setText(attribute->capitalized_name());

	connect(m_disabled_toggle, &LToggleSwitch::toggled_event, [this, attribute] {
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

	if (attribute)
	{
		if (!attribute->disabled()) m_disabled_toggle->toggle();
		else m_color_control->hide();

		// Setup Color Control
		m_color_control->fill()->entangle_with(*attribute);
	}

	// Setup Layout
	QHBoxLayout* hbox = new QHBoxLayout;
	hbox->setContentsMargins(8, 0, 8, 0);
	hbox->setSpacing(0);
	hbox->addWidget(m_attribute_label);
	hbox->addSpacing(14);
	hbox->addWidget(m_disabled_toggle);
	hbox->addSpacing(6);
	hbox->addWidget(m_color_control);
	hbox->addStretch();
	setLayout(hbox);
}

LColorControl* LColorEditor::color_control() const
{
	return m_color_control;
}

void LColorEditor::set_current_editing_state(const QString& state)
{
	m_color_control->set_current_editing_state(state);
}