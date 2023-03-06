#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QHBoxLayout>
#include <QLineEdit>
#include <QVBoxLayout>

#include "Label.h"
#include "Widget.h"

namespace Layers
{
	class ComboboxItem : public Widget
	{
		Q_OBJECT

	public:
		ComboboxItem(const QString& item_text, QWidget* parent = nullptr);

		QHBoxLayout* layout() const;

		QString item_text();

		void replace_item_text(const QString& new_item_text);

		void set_font_size(int size);

	protected:
		void init_attributes();

	private:
		void init_layout();

		Label* m_item_label;

		QString m_item_text;

		QHBoxLayout* m_layout{ new QHBoxLayout };
	};

	class Combobox : public Widget
	{
		Q_OBJECT

	signals:
		void current_item_changed(const QString& current_item);
		void item_replaced(const QString& old_item, const QString& new_item);

	public:
		Combobox(QWidget* parent = nullptr);
		~Combobox();

		ComboboxItem* add_item(const QString& item);

		void alphabetize();

		void edit_current_item();

		void enable_alphabetization(bool cond = true);

		void set_current_item(const QString& item);
		void set_disabled(bool cond = true);
		void set_font_size(int size);
		void set_item_renaming_disabled(bool disable = true);
		void set_padding(int left, int top, int right, int bottom);
		void setFixedSize(const QSize& s);
		void setFixedSize(int w, int h);

		QString current_item() const;

		QList<QString> items();

		void update_theme_dependencies();

		Attribute a_line_edit_text_color{ Attribute("line_edit_text_color", QColor(Qt::black)) };

	public slots:
		void line_edit_return_pressed();

	protected:
		virtual bool eventFilter(QObject* object, QEvent* event) override;

		void init_attributes();

	private:
		void setup_layout();

		bool m_alphabetize{ false };
		bool m_disabled{ false };
		bool m_item_renaming_disabled{ true };

		Widget* m_control_drop_down{ new Widget(this) };

		ComboboxItem* m_control_combobox_item{ new ComboboxItem("", m_control_drop_down) };

		ComboboxItem* m_current_combobox_item{ nullptr };

		Widget* m_drop_down{ new Widget };

		Label* m_current_item_label{ new Label(this) };

		QLineEdit* m_line_edit{ new QLineEdit(this) };

		QList<ComboboxItem*> m_combobox_items;

		QVBoxLayout* m_drop_down_layout{ new QVBoxLayout };
	};
}

#endif // COMBOBOX_H
