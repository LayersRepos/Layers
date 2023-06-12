#include "lsettingstab.h"

#include <QIntValidator>
#include <QMouseEvent>

#include <LayersWidgets/llabel.h>

using Layers::LGraphic;
using Layers::LSettingsTab;

LSettingsTab::LSettingsTab(const LGraphic& icon, const QString& label_text, QWidget* parent) :
	m_icon_label{ new LLabel(icon) }, m_text_label { new LLabel(label_text) }, LWidget(parent)
{
    init_attributes();

	connect(this, &LSettingsTab::clicked, [this] { if (state() != "Selected") set_state("Selected"); });

    installEventFilter(this);
    setMouseTracking(true);
	setFixedHeight(60);

    set_icon(LGraphic(":/images/settings_tab_icon.svg", QSize(20, 9)));
    set_name("Setting Tab");

    m_icon_label->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_icon_label->set_icon(icon);
	m_icon_label->set_name("Icon Label");

    m_text_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_text_label->set_name("Text Label");
    m_text_label->set_font_size(14);

    init_layout();

    set_state("Unselected");
}

void LSettingsTab::init_attributes()
{
    m_corner_radii->top_left()->set_value(5.0);
	m_corner_radii->bottom_left()->set_value(5.0);
    m_fill->init_variant_map({
        { "Unselected", QColor(Qt::lightGray) },
        { "Selected", QColor(Qt::white) }
        });

    m_icon_label->graphic()->svg_renderer()->common_color()->init_variant_map({
		{ "Unselected", QColor(Qt::gray) },
		{ "Selected", QColor(Qt::black) }
		});

	m_text_label->text_color()->init_variant_map({
		{ "Unselected", QColor(Qt::gray) },
		{ "Selected", QColor(Qt::black) }
		});
}

int LSettingsTab::recommended_minimum_width()
{
    return 18 + m_icon_label->sizeHint().width() + 12 + m_text_label->sizeHint().width() + 18;
}

void LSettingsTab::set_disabled(bool cond)
{
    m_disabled = cond;
}

bool LSettingsTab::eventFilter(QObject* object, QEvent* event)
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

//void LSettingsTab::resizeEvent(QResizeEvent* event)
//{
//    int minimum_width = 18 + m_icon_label->width() + 12 + m_text_label->width() + 18;
//
//    if (width() < minimum_width)
//        emit under_minimum_width();
//    else
//        emit over_minimum_width();
//}

void LSettingsTab::init_layout()
{
    main_layout->setContentsMargins(18, 0, 18, 0);
    main_layout->setSpacing(12);
    main_layout->addWidget(m_icon_label);
    main_layout->addWidget(m_text_label);

    setLayout(main_layout);
}
