/*
 * Copyright (C) 2023 The Layers Project
 *
 * This file is part of Layers.
 *
 * Layers is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Layers is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Layers. If not, see <https://www.gnu.org/licenses/>.
 */

#include <Layers/lthemecomboboxitemdelegate.h>

#include <QMouseEvent>
#include <QPainter>

#include <Layers/lapplication.h>

using Layers::LThemeComboBoxItemDelegate;

bool LThemeComboBoxItemDelegate::editorEvent(
	QEvent* event,
	QAbstractItemModel* model,
	const QStyleOptionViewItem& option,
	const QModelIndex& index)
{
	if (event->type() == QMouseEvent::MouseMove)
		emit mouse_moved(QCursor::pos());

	return false;
}

void LThemeComboBoxItemDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	LTheme* theme = index.data(Qt::UserRole).value<LTheme*>();

	QColor fill_color = (option.state & QStyle::State_HasFocus) ?
		m_fill->as<QColor>({ "Selected" }) : m_fill->as<QColor>({ "Unselected" });

	QFont uuid_font(option.font.family(), 8);

	const QFontMetrics& item_font_metrics = option.fontMetrics;
	const QFontMetrics& uuid_font_metrics = QFontMetrics(uuid_font);

	QPainterPath item_text_path;
	QPainterPath uuid_text_path;

	QString item_text = theme->name();
	QString uuid_text;

	if (!theme->uuid().isNull())
		uuid_text = theme->uuid().toString(QUuid::WithoutBraces);

	painter->setRenderHint(QPainter::Antialiasing);

	// Draw background
	painter->fillPath(background_path(option, index), fill_color);

	if (!uuid_text.isEmpty())
	{
		// Draw item text and uuid text

		item_text_path.addText(
			QPoint(10, option.rect.center().y() + (item_font_metrics.height() / 2) - 12),
			option.font,
			item_text
		);

		uuid_text_path.addText(
			QPoint(10, option.rect.center().y() + (uuid_font_metrics.height() / 2) + 8),
			uuid_font,
			uuid_text
		);
	}
	else
	{
		// Draw item text only

		item_text_path.addText(
			QPoint(10, option.rect.center().y() + (item_font_metrics.height() / 2) - 2),
			option.font,
			item_text
		);
	}

	painter->fillPath(item_text_path, m_text_color->as<QColor>());
	painter->fillPath(uuid_text_path, m_text_color->as<QColor>());

	if (!theme->has_implementation(layersApp->app_identifier()))
	{
		QPoint caution_image_location = QPoint(
			option.rect.right() - m_caution_image.width() - 10,
			option.rect.center().y() - (m_caution_image.height() / 2));

		painter->drawImage(caution_image_location, m_caution_image);
	}
}
