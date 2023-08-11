#ifndef LLINEEDITOR_H
#define LLINEEDITOR_H

#include <QLineEdit>

#include "layers_global.h"
#include "layers_exports.h"

#include "lwidget.h"

LAYERS_NAMESPACE_BEGIN
/*!
	An LLineEditor is a LWidget that allows the user to edit a single line of
	text.
*/
class LAYERS_EXPORT LLineEditor : public LWidget
{
	Q_OBJECT

signals:
	/*!
		This signal is emitted when the line editor's text is changed.
	*/
	void text_edited(const QString& text);

public:
	/*!
		Constructs a line editor.
	*/
	LLineEditor(QWidget* parent = nullptr);

	~LLineEditor();

	/*!
		Returns a pointer to the left-padding attribute of the line editor.
	*/
	LAttribute* left_padding() const;

	/*!
		Sets the default value to be used in the line editor if it is left
		empty after losing focus.
	*/
	void set_default_value(const QString& default_value);

	/*!
		Sets the *size* of the label's font.
	*/
	void set_font_size(int size);

	/*!
		Sets the *size* of the label's font.
	*/
	void set_font_size_f(qreal size);

	/*!
		Sets all margin attributes with one value.
	*/
	void set_margin(int margin);

	/*!
		Sets the margin attributes individually.
	*/
	void set_margin(int left, int top, int right, int bottom);

	/*!
		Sets the line editor's text.
	*/
	void set_text(const QString& text);

	/*!
		Sets the validator to constrain values entered into the line
		editor.
	*/
	void set_validator(const QValidator* validator);

	/*!
		Returns a pointer to the text color attribute of the line editor.
	*/
	LAttribute* text_color() const;

	/*!
		Returns a pointer to the text attribute of the line editor.
	*/
	LAttribute* text() const;

	virtual void update() override;

protected:
	bool eventFilter(QObject* object, QEvent* event) override;

private:
	void init_attributes();

	LAttribute* m_left_padding{ new LAttribute("left_padding", QVariant::fromValue(3.0)) };
	LAttribute* m_text_color{ new LAttribute("Text Color", QColor(Qt::black), this) };
	LAttribute* m_text{ new LAttribute("text", QString("")) };

	QString* m_default_value{ nullptr };

	QLineEdit* m_line_edit{ new QLineEdit(this) };
};
LAYERS_NAMESPACE_END

#endif // LLINEEDITOR_H
