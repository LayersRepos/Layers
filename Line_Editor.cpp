#include "Layers.h"

using Layers::Line_Editor;

Line_Editor::Line_Editor(QWidget* parent) : Widget(parent)
{
	init_attributes();

    setFixedSize(45, 45);
    set_margin(7, 7, 8, 8);

    m_line_edit->installEventFilter(this);
    m_line_edit->setStyleSheet("QLineEdit { border: none; background: transparent; padding-left: " + QString::number(attributes()["left_padding"].value().value<int>() + attributes()["margin_left"].value().value<int>()) + "px; padding-bottom: 1px; }");

    connect(m_line_edit, &QLineEdit::textEdited, [this] {
        if (m_line_edit->text().startsWith("0")) m_line_edit->setText("0");
        else if (m_line_edit->hasAcceptableInput() || m_line_edit->text() == "")
        {
            set_attribute_value("text", m_line_edit->text());
            share_attributes();
        }
        else m_line_edit->setText(attributes()["text"].value().value<QString>());

		emit text_edited(m_line_edit->text());
        });
}

void Line_Editor::init_attributes()
{
    set_attribute_value("background_color", QColor(Qt::lightGray));
    set_attribute_value("corner_radius_tl", 5);
    set_attribute_value("corner_radius_tr", 5);
    set_attribute_value("corner_radius_bl", 5);
    set_attribute_value("corner_radius_br", 5);
    add_attribute("left_padding", 3);
    add_attribute("text_color", QColor(Qt::black));
    add_attribute("text", QString(""));
}

void Line_Editor::set_default_value(const QString& default_value)
{
    if (m_default_value) delete m_default_value;

    m_default_value = new QString(default_value);
}

void Line_Editor::set_disabled(bool cond)
{
    m_disabled = cond;
}

void Line_Editor::set_font_size(int size)
{
    QFont line_edit_font = m_line_edit->font();

    line_edit_font.setPointSize(size);

    m_line_edit->setFont(line_edit_font);

    update_theme_dependencies();
}

void Line_Editor::set_margin(int margin)
{
    Widget::set_margin(margin);

    update_theme_dependencies();
}

void Line_Editor::set_margin(int left, int top, int right, int bottom)
{
    Widget::set_margin(left, top, right, bottom);

    update_theme_dependencies();
}

void Line_Editor::set_text(const QString& text)
{
    m_line_edit->setText(text);

    set_attribute_value("text", text);

	share_attributes();
}

void Line_Editor::set_validator(const QValidator* validator)
{
    m_line_edit->setValidator(validator);
}

void Line_Editor::setFixedSize(int w, int h)
{
	QWidget::setFixedSize(w, h);

	m_line_edit->setFixedSize(w, h);
}

QString Line_Editor::text()
{
	return m_line_edit->text();
}

void Line_Editor::update_theme_dependencies()
{
    m_line_edit->setStyleSheet("QLineEdit { border: none; background: transparent; color: " + attributes()["text_color"].value().value<QColor>().name() + "; padding-left: " + QString::number(attributes()["left_padding"].value().value<int>() + attributes()["margin_left"].value().value<int>()) + "px; padding-bottom: 2px; }");
    
    if (m_line_edit->text() != attributes()["text"].value().value<QString>()) m_line_edit->setText(attributes()["text"].value().value<QString>());
}

bool Line_Editor::eventFilter(QObject* object, QEvent* event)
{
    if (object == m_line_edit)
    {
        if (event->type() == QEvent::FocusOut)
        {
            if (m_line_edit->text() == "" && m_default_value) m_line_edit->setText(*m_default_value);
        }
    }

    return false;
}
