#include "GridlineItemDelegate.h"

#include <QPainter>

using Layers::BorderAttributes;
using Layers::GridlineItemDelegate;

GridlineItemDelegate::GridlineItemDelegate(QObject* parent) :
	QStyledItemDelegate(parent)
{
	init_attributes();

	set_name("cells");
	set_proper_name("Cells");
}

BorderAttributes* GridlineItemDelegate::grid() const
{
	return m_grid;
}

void GridlineItemDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	if (!m_grid->fill()->disabled())
	{
		QColor grid_fill = m_grid->fill()->as<QColor>();

		int grid_thickness = m_grid->thickness()->as<int>();

		if (index.row() < index.model()->rowCount() - 1)
		{
			QRect border_rect_bottom = QRect(
				option.rect.x(),
				option.rect.y() + option.rect.height() - grid_thickness,
				option.rect.width(),
				grid_thickness
			);
			painter->fillRect(border_rect_bottom, grid_fill);
		}

		if (index.column() < index.model()->columnCount() - 1)
		{
			QRect border_rect_right = QRect(
				option.rect.x() + option.rect.width() - grid_thickness,
				option.rect.y(),
				grid_thickness,
				option.rect.height()
			);
			painter->fillRect(border_rect_right, grid_fill);
		}
	}
}

void GridlineItemDelegate::init_attributes()
{
	m_attributes.insert({
		{ "grid", m_grid }
		});

	connect(m_grid->fill(), &AbstractAttribute::value_changed, [this] { emit changed(); });
	connect(m_grid->thickness(), &AbstractAttribute::value_changed, [this] { emit changed(); });

	m_grid->fill()->set_value(QColor("#808080"));
	m_grid->thickness()->set_value(3.0);
}