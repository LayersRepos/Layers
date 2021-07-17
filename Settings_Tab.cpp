#include "Layers.h"

using Layers::Graphic_Widget;
using Layers::Settings_Tab;

Settings_Tab::Settings_Tab(Graphic_Widget* icon, const QString& label_text, QWidget* parent) :
	m_tab_icon{ icon }, m_text_label { new Label(label_text) }, Widget(parent)
{
	init_child_themeable_reference_list();
	init_attributes();

	connect(this, &Settings_Tab::clicked, [this] { if (state() != "Selected") set_state("Selected"); });

    installEventFilter(this);
    setMouseTracking(true);
	setFixedHeight(60);

    set_icon(new Graphic_Widget(":/svgs/settings_tab_icon.svg", QSize(20, 9)));
    set_name("settings_tab");
    set_proper_name(label_text + " Tab");

    m_tab_icon->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_tab_icon->set_icon(new Graphic_Widget(*m_tab_icon));
    m_tab_icon->set_name("icon");
	m_tab_icon->set_proper_name("Icon");

    m_text_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_text_label->set_name("text_label");
    m_text_label->set_proper_name("Text Label");
    m_text_label->set_font_size(14);
    m_text_label->set_padding(0, 8, 0, 0);

    m_spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_spacer->setFixedSize(12, 12);
    m_spacer->set_attribute_value("background_disabled", true);

    m_stretch_widget->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_stretch_widget->set_attribute_value("background_disabled", true);
    m_stretch_widget->hide();

    m_stretch_widget2->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_stretch_widget2->set_attribute_value("background_disabled", true);

    setup_layout();
}

void Settings_Tab::expand()
{
    m_spacer->show();
    m_text_label->show();
    m_stretch_widget->hide();

    main_layout->setContentsMargins(18, 0, 0, 0);
}

void Settings_Tab::shrink()
{
    m_spacer->hide();
    m_text_label->hide();
    m_stretch_widget->show();

    main_layout->setContentsMargins(0, 0, 0, 0);
}

void Settings_Tab::init_attributes()
{
	set_attribute_value("corner_radius_tl", 5);
	set_attribute_value("corner_radius_bl", 5);

	make_attribute_stateful("background_color", {
		{ "Unselected", QColor(Qt::lightGray) },
		{ "Selected", QColor(Qt::white) }
		});

	m_tab_icon->make_attribute_stateful("common_color", {
		{ "Unselected", QColor(Qt::gray) },
		{ "Selected", QColor(Qt::black) }
		});

	m_text_label->make_attribute_stateful("color", {
		{ "Unselected", QColor(Qt::gray) },
		{ "Selected", QColor(Qt::black) }
		});

	set_state("Unselected");
}

void Settings_Tab::init_attribute_widgets()
{
	if (m_customize_panel)
	{
		Attribute_Widget_Container* border_awc = new Attribute_Widget_Container("Border", false);
		Attribute_Widget_Container* corner_radii_awc = new Attribute_Widget_Container("Corner Radii", true);
		Color_Attribute_Widget* background_caw = new Color_Attribute_Widget("Background", m_attributes["background_color"], m_attributes["background_disabled"], true);
		Color_Attribute_Widget* hover_background_caw = new Color_Attribute_Widget("Hover Background", m_attributes["background_color_hover"], m_attributes["background_color_hover_disabled"], false);
		Color_Attribute_Widget* outline_caw = new Color_Attribute_Widget("Outline", m_attributes["outline_color"], m_attributes["outline_color_disabled"], false);
		Color_Attribute_Widget* corner_color_caw = new Color_Attribute_Widget("Corner Color", m_attributes["corner_color"], m_attributes["corner_color_disabled"], false);
		Color_Attribute_Widget* border_caw = new Color_Attribute_Widget("Color", m_attributes["border_color"], true);
		Gradient_Attribute_Widget* border_gaw = new Gradient_Attribute_Widget("Gradient", m_attributes["border_gradient_stops"], true);
		Number_Attribute_Widget* border_thickness_naw = new Number_Attribute_Widget("Thickness", m_attributes["border_thickness"], new QIntValidator(0, 30), true);
		Number_Attribute_Widget* corner_radius_tl_naw = new Number_Attribute_Widget("Top-Left", m_attributes["corner_radius_tl"], new QIntValidator(0, 30), true);
		Number_Attribute_Widget* corner_radius_tr_naw = new Number_Attribute_Widget("Top-Right", m_attributes["corner_radius_tr"], new QIntValidator(0, 30), true);
		Number_Attribute_Widget* corner_radius_bl_naw = new Number_Attribute_Widget("Bottom-Left", m_attributes["corner_radius_bl"], new QIntValidator(0, 30), true);
		Number_Attribute_Widget* corner_radius_br_naw = new Number_Attribute_Widget("Bottom-Right", m_attributes["corner_radius_br"], new QIntValidator(0, 30), true);
		Switch_Attribute_Widget* border_color_saw = new Switch_Attribute_Widget(
			"Solid", border_caw,
			"Gradient", border_gaw,
			m_attributes["border_gradient_disabled"], true);

		border_caw->set_centered();
		border_gaw->set_centered();

		border_thickness_naw->set_centered();
		border_thickness_naw->set_unit_label_text("px");
		border_thickness_naw->enable_silder();

		border_awc->add_attribute_widget(border_color_saw);
		border_awc->add_attribute_widget(border_thickness_naw);

		corner_radius_tl_naw->set_centered();
		corner_radius_tl_naw->enable_silder();

		corner_radius_tr_naw->set_centered();
		corner_radius_tr_naw->enable_silder();

		corner_radius_bl_naw->set_centered();
		corner_radius_bl_naw->enable_silder();

		corner_radius_br_naw->set_centered();
		corner_radius_br_naw->enable_silder();

		corner_radii_awc->add_attribute_widget(corner_radius_tl_naw);
		corner_radii_awc->add_attribute_widget(corner_radius_tr_naw);
		corner_radii_awc->add_attribute_widget(corner_radius_bl_naw);
		corner_radii_awc->add_attribute_widget(corner_radius_br_naw);

		m_customize_panel->add_attribute_widget(background_caw);
		m_customize_panel->add_attribute_widget(hover_background_caw);
		m_customize_panel->add_attribute_widget(outline_caw);
		m_customize_panel->add_attribute_widget(corner_color_caw);
		m_customize_panel->add_attribute_widget(border_awc);
		m_customize_panel->add_attribute_widget(corner_radii_awc);

		// Add attribute widget references

		m_attribute_widgets["border_awc"] = border_awc;
		m_attribute_widgets["corner_radii_awc"] = corner_radii_awc;
		m_attribute_widgets["background_caw"] = background_caw;
		m_attribute_widgets["hover_background_caw"] = hover_background_caw;
		m_attribute_widgets["outline_caw"] = outline_caw;
		m_attribute_widgets["corner_color_caw"] = corner_color_caw;
		m_attribute_widgets["border_caw"] = border_caw;
		m_attribute_widgets["border_gaw"] = border_gaw;
		m_attribute_widgets["border_thickness_naw"] = border_thickness_naw;
		m_attribute_widgets["corner_radius_tl_naw"] = corner_radius_tl_naw;
		m_attribute_widgets["corner_radius_tr_naw"] = corner_radius_tr_naw;
		m_attribute_widgets["corner_radius_bl_naw"] = corner_radius_bl_naw;
		m_attribute_widgets["corner_radius_br_naw"] = corner_radius_br_naw;
		m_attribute_widgets["border_color_saw"] = border_color_saw;

		// TODO: Consider an implicit way to get this done
		m_customize_panel->add_child_themeable_reference(border_color_saw);
		m_customize_panel->add_child_themeable_reference(border_caw);
		m_customize_panel->add_child_themeable_reference(border_gaw);
		m_customize_panel->add_child_themeable_reference(border_thickness_naw);
		m_customize_panel->add_child_themeable_reference(corner_radius_tl_naw);
		m_customize_panel->add_child_themeable_reference(corner_radius_tr_naw);
		m_customize_panel->add_child_themeable_reference(corner_radius_bl_naw);
		m_customize_panel->add_child_themeable_reference(corner_radius_br_naw);
	}
}

void Settings_Tab::init_child_themeable_reference_list()
{
    add_child_themeable_reference(m_tab_icon);
    add_child_themeable_reference(m_text_label);
}

int Settings_Tab::recommended_minimum_width()
{
    return 18 + m_tab_icon->width() + 12 + m_text_label->width() + 18;
}

void Settings_Tab::set_disabled(bool cond)
{
    m_disabled = cond;
}

bool Settings_Tab::eventFilter(QObject* object, QEvent* event)
{
    if (!m_disabled)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = static_cast<QMouseEvent*>(event);

            if (mouse_event->button() & Qt::LeftButton)
            {
                emit clicked();
            }
        }
    }

    return false;
}

void Settings_Tab::resizeEvent(QResizeEvent* event)
{
    int minimum_width = 18 + m_tab_icon->width() + 12 + m_text_label->width() + 18;

    if (width() < minimum_width) emit under_minimum_width();
    else emit over_minimum_width();
}

void Settings_Tab::setup_layout()
{
    main_layout->setContentsMargins(18, 0, 0, 0);
    main_layout->setSpacing(0);
    main_layout->addWidget(m_stretch_widget);
    main_layout->addWidget(m_tab_icon);
    main_layout->addWidget(m_spacer);
    main_layout->addWidget(m_text_label);
    main_layout->addWidget(m_stretch_widget2);

    setLayout(main_layout);
}
