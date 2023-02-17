#include "../../include/ScrollArea.h"

#include <QEvent>

using Layers::ScrollArea;
using Layers::ScrollBar;

ScrollArea::ScrollArea(QWidget* parent) : Widget(parent)
{
	installEventFilter(this);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	m_scroll_area->setWidgetResizable(true);
	m_scroll_area->setStyleSheet("QScrollArea { background-color:transparent; border:none; }");
	m_scroll_area->setHorizontalScrollBar(m_horizontal_scrollbar);
	m_scroll_area->setVerticalScrollBar(m_vertical_scrollbar);

	m_control_horizontal_scrollbar->hide();
	m_control_horizontal_scrollbar->set_name("horizontal_scrollbar");
	m_control_horizontal_scrollbar->set_proper_name("Horizontal Scrollbar");
	
	m_control_vertical_scrollbar->hide();
	m_control_vertical_scrollbar->set_name("vertical_scrollbar");
	m_control_vertical_scrollbar->set_proper_name("Vertical Scrollbar");
	
	m_horizontal_scrollbar->entangle_with(m_control_horizontal_scrollbar);
	m_vertical_scrollbar->entangle_with(m_control_vertical_scrollbar);
}

//ScrollArea::~ScrollArea()
//{
//	//if (!m_horizontal_scrollbar->parentWidget())
//	//{
//	delete m_horizontal_scrollbar;
//	m_horizontal_scrollbar = nullptr;
//	//}
//
//	//if (!m_vertical_scrollbar->parentWidget())
//	//{
//	delete m_vertical_scrollbar;
//	m_vertical_scrollbar = nullptr;
//	//}
//}

ScrollBar* ScrollArea::horizontal_scrollbar() const
{
	return m_horizontal_scrollbar;
}

void ScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
	m_scroll_area->setHorizontalScrollBarPolicy(policy);
}

void ScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarPolicy policy)
{
	m_scroll_area->setVerticalScrollBarPolicy(policy);
}

void ScrollArea::setWidget(QWidget* widget)
{
	m_scroll_area->setWidget(widget);

	widget->setStyleSheet("background-color:transparent;");
}

QWidget* ScrollArea::takeWidget() const
{
	return m_scroll_area->takeWidget();
}

ScrollBar* ScrollArea::vertical_scrollbar() const
{
	return m_vertical_scrollbar;
}

QWidget* ScrollArea::widget() const
{
	return m_scroll_area->widget();
}

bool ScrollArea::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::Resize)
		m_scroll_area->setFixedSize(size());

	return false;
}
