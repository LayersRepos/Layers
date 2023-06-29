#ifndef LTABLEVIEW_H
#define LTABLEVIEW_H

#include <QHeaderView>
#include <QTableView>
#include <QStyledItemDelegate>

#include "layers_global.h"
#include "layers_exports.h"

#include <Layers/lsvgrenderer.h>
#include <Layers/lthemeable.h>

LAYERS_NAMESPACE_BEGIN
/*!
	An LTableView is a QTableView and a LThemeable that displays items from a
	model in the form of a table.
*/
class LAYERS_EXPORT LTableView : public QTableView, public LThemeable
{
	Q_OBJECT

public:
	/*!
		Constructs a table view.
	*/
	LTableView(QWidget* parent = nullptr);

	// TODO: Create a destructor; free the attributes

	virtual QList<LThemeable*> child_themeables(
		Qt::FindChildOptions options = Qt::FindDirectChildrenOnly) override;

	/*!
		Sets the item delegate of the table view.
	*/
	void setItemDelegate(QAbstractItemDelegate* item_delegate);

	/*!
		Updates the height of the table view.
	*/
	void update_height();

private:
	void init_attributes();

	void update_stylesheet();

	LAttribute* m_border_fill{
		new LAttribute("border.fill", QColor(Qt::gray), this) };

	LAttribute* m_border_thickness{
		new LAttribute("border.thickness", 3.0, this) };

	LAttribute* m_corner_radii_bottom_left{
		new LAttribute("corner_radii.bottom_left", 5.0, this) };

	LAttribute* m_corner_radii_bottom_right{
		new LAttribute("corner_radii.bottom_right", 5.0, this) };

	LAttribute* m_corner_radii_top_left{
		new LAttribute("corner_radii.top_left", 5.0, this) };

	LAttribute* m_corner_radii_top_right{
		new LAttribute("corner_radii.top_right", 5.0, this) };

	LAttribute* m_fill{
		new LAttribute("fill", QColor(Qt::white), this) };

	LAttribute* m_item_fill{
		new LAttribute("item_fill", QColor(Qt::white), this) };

	QAbstractItemDelegate* m_table_delegate{ nullptr };

	LAttribute* m_text_color{
		new LAttribute("text_color", QColor(Qt::black), this) };

	int m_visible_row_limit{ 3 };
};
LAYERS_NAMESPACE_END

#endif // LTABLEVIEW_H
