#include "../../../include/AttributeWidgets.h"
#include "../../../include/CustomizePanel.h"
#include "../../../include/Combobox.h"

#include <QEvent>
#include <QIntValidator>

using Layers::ComboboxItem;

ComboboxItem::ComboboxItem(const QString& item_text, QWidget* parent) :
	m_item_label{ new Label(item_text) }, m_item_text{ item_text }, Widget(parent)
{
	init_child_themeable_reference_list();
	init_attributes();

	set_name("combobox_item");

	m_item_label->setParent(this);
	m_item_label->set_name("label");
	m_item_label->set_proper_name("Item Label");
	m_item_label->set_padding(0, 7, 0, 0);
}

void ComboboxItem::init_attributes()
{
	a_corner_radius_tl.init_state_variant_map({
			{ "Single", 10 },
			{ "Top", 10 },
			{ "Middle", 0 },
			{ "Bottom", 0 }
		});

	a_corner_radius_tr.init_state_variant_map({
			{ "Single", 10 },
			{ "Top", 10 },
			{ "Middle", 0 },
			{ "Bottom", 0 }
		});

	a_corner_radius_bl.init_state_variant_map({
			{ "Single", 10 },
			{ "Top", 0 },
			{ "Middle", 0 },
			{ "Bottom", 10 }
		});

	a_corner_radius_br.init_state_variant_map({
			{ "Single", 10 },
			{ "Top", 0 },
			{ "Middle", 0 },
			{ "Bottom", 10 }
		});

	a_hover_fill.set_disabled(false);
}

void ComboboxItem::init_child_themeable_reference_list()
{
	store_child_themeable_pointer(m_item_label);
}

QString ComboboxItem::item_text()
{
	return m_item_text;
}

void ComboboxItem::replace_all_attributes_with(ComboboxItem* cb_item)
{
	Widget::replace_all_attributes_with(cb_item);

	m_item_label->replace_all_attributes_with(cb_item->m_item_label);
}

void ComboboxItem::replace_item_text(const QString& new_item_text)
{
	m_item_text = new_item_text;

	m_item_label->setText(new_item_text);
}

void ComboboxItem::set_font_size(int size)
{
	m_item_label->set_font_size(size);

	m_item_label->move(width() * 0.09, height() / 2 - m_item_label->height() / 2);
}

void ComboboxItem::setFixedSize(const QSize& s)
{
	Widget::setFixedSize(s);

	m_item_label->move(width() * 0.09, height() / 2 - m_item_label->height() / 2);
}

void ComboboxItem::setFixedSize(int w, int h)
{
	setFixedSize(QSize(w, h));
}
