#include "lsettingsmenu.h"

#include <QMouseEvent>

#include "lsettingstab.h"
#include "lthemeswidget.h"

using Layers::LGraphic;
using Layers::LSettingsMenu;
using Layers::LThemesWidget;

LSettingsMenu::LSettingsMenu(QWidget* parent) :
	m_themes_widget{ new LThemesWidget },
	LWidget(parent)
{
	init_layout();
	setMouseTracking(true);
	set_icon(LGraphic(":/images/settings_animated.svg", QSize(24, 24)));
	set_name("Settings Menu");

	//m_themes_settings_panel->hide();
	m_themes_widget->setMouseTracking(true);
	
	// Add Themes Settings Tab
	add_settings_tab(LGraphic(":/images/themes_icon.svg", QSize(25, 25)), "Themes");
	m_settings_tabs.last()->set_name("Themes Settings Tab");
	//connect(m_settings_tabs.last(), &LSettingsTab::clicked, [this] { m_app_preferences_settings_panel->hide(); m_themes_settings_panel->show(); });

	m_settings_tabs.first()->select_states()->set_state("Selected");

	m_sidebar->setFixedWidth(recommended_minimum_tab_width());
	m_sidebar->setMouseTracking(true);
	m_sidebar->set_icon(LGraphic(":/images/sidebar_icon.svg", QSize(20, 20)));
	m_sidebar->set_name("Sidebar");
	m_sidebar->fill()->set_value(QColor(Qt::lightGray));
}

void LSettingsMenu::add_settings_tab(const LGraphic& icon, const QString& label_text)
{
	LSettingsTab* settings_tab = new LSettingsTab(icon, label_text);

	for (LSettingsTab* st : m_settings_tabs)
	{
		connect(st, &LSettingsTab::clicked, [settings_tab]
		{
			settings_tab->select_states()->set_state("Unselected");
			settings_tab->LThemeable::update();
		});
		connect(settings_tab, &LSettingsTab::clicked, [st]
		{
			st->select_states()->set_state("Unselected");
			st->LThemeable::update();
		});
	}

	m_settings_tabs.append(settings_tab);

	int active_tab_index = m_settings_tabs.indexOf(settings_tab);

	m_sidebar_layout->insertWidget(m_sidebar_layout->count() - 1, settings_tab);
}

int LSettingsMenu::largest_tab_index() const
{
	int tab_index = -1;

	for (LSettingsTab* st : m_settings_tabs)
	{
		int tab_rec_min_width = st->recommended_minimum_width();
		if (tab_rec_min_width > tab_index)
		{
			tab_index = m_settings_tabs.indexOf(st);
		}
	}

	return tab_index;
}

int LSettingsMenu::recommended_minimum_tab_width() const
{
	int tab_width = 0;

	for (LSettingsTab* st : m_settings_tabs)
	{
		int tab_rec_min_width = st->recommended_minimum_width();
		if (tab_rec_min_width > tab_width)
		{
			tab_width = tab_rec_min_width;
		}
	}

	return tab_width;
}

LThemesWidget* LSettingsMenu::themes_widget() const
{
	return m_themes_widget;
}

void LSettingsMenu::init_layout()
{
	// Sidebar

	m_sidebar_layout->setContentsMargins(0, 0, 0, 0);
	m_sidebar_layout->setSpacing(0);
	m_sidebar_layout->addStretch();

	m_sidebar->setLayout(m_sidebar_layout);

	// Main Layout

	QHBoxLayout* main_layout = new QHBoxLayout;

	main_layout->setContentsMargins(0, 0, 0, 0);
	main_layout->setSpacing(0);
	main_layout->addWidget(m_sidebar);
	main_layout->addWidget(m_themes_widget);

	setLayout(main_layout);
}