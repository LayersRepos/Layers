#ifndef ATTRIBUTEWIDGET_H
#define ATTRIBUTEWIDGET_H

#include "Button.h"
#include "ColorControl.h"
#include "Combobox.h"
#include "GradientControl.h"
#include "FillControl.h"
#include "Graphic.h"
#include "Label.h"
#include "LineEditor.h"
#include "MiniSlider.h"
#include "ToggleSwitch.h"

namespace Layers
{
	class AttributeWidget : public Widget
	{
		Q_OBJECT

	signals:
		void widget_disabled();

	public:
		AttributeWidget(Entity* entity = nullptr, QWidget* parent = nullptr);
		~AttributeWidget();

		ToggleSwitch* disable_toggle() const;

		bool disabled() const;

		Widget* toggle_label_separator() const;

	public slots:
		virtual void set_current_editting_state(const QString& state);

	protected:
		void init_attributes();

		Entity* m_entity;

		ToggleSwitch* m_disabled_toggle{ new ToggleSwitch };

		Widget* m_toggle_label_separator{ new Widget };
	};

	class StateAW : public AttributeWidget
	{
		Q_OBJECT

	public:
		StateAW(QWidget* parent = nullptr);

		void add_attribute_widget(AttributeWidget* attribute_widget);

		Combobox* state_combobox() const;

		void populate_state_combobox(const QList<QString>& states);

	private:
		void setup_layout();

		Combobox* m_state_combobox{ new Combobox };

		Label* m_label{ new Label("State")};

		QList<AttributeWidget*> m_child_attribute_widgets{ QList<AttributeWidget*>() };

		QVBoxLayout* m_main_layout{ new QVBoxLayout };

		QVBoxLayout* m_widgets_vbox{ new QVBoxLayout };
	};

	class AWGroup : public AttributeWidget
	{
		Q_OBJECT

	public:
		AWGroup(AttributeGroup* attr_group, QWidget* parent = nullptr);

		void add_attribute_widget(AttributeWidget* attribute_widget);

		void set_collapsed(bool collapsed = true);

		//virtual bool disabled() const;

	public slots:
		virtual void set_current_editting_state(const QString& state) override;

	private:
		void setup_layout();

		Button* m_collapse_button{ new Button(new Graphic(":/svgs/collapse_arrow_right.svg", QSize(8, 12)), new Graphic(":/svgs/collapse_arrow_down.svg", QSize(12, 8)), true) };

		bool m_collapsed{ true };

		Label* m_label{ new Label };

		QList<AttributeWidget*> m_child_attribute_widgets{ QList<AttributeWidget*>() };

		QVBoxLayout* m_widgets_vbox{ new QVBoxLayout };
	};

	class CornerRadiiAW : public AWGroup
	{
		Q_OBJECT

	public:
		CornerRadiiAW(CornerRadiiAttributes* linked_corner_radii, QWidget* parent = nullptr);

	public slots:
		void set_current_editting_state(const QString& state);

	private:
		void setup_layout();

		AttributeWidget* m_attribute_widget;

		MiniSlider* m_tl_slider{ new MiniSlider(30.0) };
		MiniSlider* m_tr_slider{ new MiniSlider(30.0) };
		MiniSlider* m_bl_slider{ new MiniSlider(30.0) };
		MiniSlider* m_br_slider{ new MiniSlider(30.0) };

		LineEditor* m_tl_line_editor{ new LineEditor };
		LineEditor* m_tr_line_editor{ new LineEditor };
		LineEditor* m_bl_line_editor{ new LineEditor };
		LineEditor* m_br_line_editor{ new LineEditor };

		QVBoxLayout* m_main_layout{ new QVBoxLayout };

		Widget* m_example_widget{ new Widget };
	};

	class ColorAW : public AttributeWidget
	{
		Q_OBJECT

	public:
		ColorAW(Attribute* attribute, QWidget* parent = nullptr);

		ColorControl* color_control() const;

	public slots:
		void set_current_editting_state(const QString& state);

	private:
		ColorControl* m_color_control{ new ColorControl };

		Label* m_attribute_label{ new Label };
	};

	class GradientAW : public AttributeWidget
	{
		Q_OBJECT

	public:
		GradientAW(const QString& attribute_label_text, Attribute* attribute, QWidget* parent = nullptr);

	private:
		GradientControl* m_gradient_control{ new GradientControl };

		Label* m_attribute_label;
	};

	class FillAW : public AttributeWidget
	{
		Q_OBJECT

	public:
		FillAW(Attribute* attribute, QWidget* parent = nullptr);

		FillControl* fill_control() const;

	public slots:
		virtual void set_current_editting_state(const QString& state) override;

	private:
		FillControl* m_fill_control{ new FillControl };

		Label* m_attribute_label{ new Label };
	};

	class NumberAW : public AttributeWidget
	{
		Q_OBJECT

	public:
		NumberAW(Attribute* attribute, QIntValidator* int_validator, QWidget* parent = nullptr);

		void set_unit_label_text(const QString& unit_string);

	private:
		void setup_layout();

		Label* m_attribute_label{ new Label };
		//Label* m_unit_label{ new Label };

		LineEditor* m_line_editor{ new LineEditor };

		QVBoxLayout* m_main_layout{ new QVBoxLayout };

		MiniSlider* m_slider{ new MiniSlider };
	};
}

#endif // ATTRIBUTEWIDGET_H
